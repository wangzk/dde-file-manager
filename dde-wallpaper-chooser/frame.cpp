/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "frame.h"
#include "constants.h"
#include "wallpaperlist.h"
#include "wallpaperitem.h"
#include "dbus/appearancedaemon_interface.h"
#include "dbus/deepin_wm.h"
#include "thumbnailmanager.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QScreen>

Frame::Frame(QFrame *parent)
    : DBlurEffectWidget(parent),
      m_wallpaperList(new WallpaperList(this)),
      m_closeButton(new DImageButton(":/images/close_round_normal.svg",
                                 ":/images/close_round_hover.svg",
                                 ":/images/close_round_press.svg", this)),
      m_dbusAppearance(new AppearanceDaemonInterface(AppearanceServ,
                                                     AppearancePath,
                                                     QDBusConnection::sessionBus(),
                                                     this)),
      m_dbusDeepinWM(new DeepinWM(DeepinWMServ,
                                  DeepinWMPath,
                                  QDBusConnection::sessionBus(),
                                  this)),
      m_mouseArea(new DRegionMonitor(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setBlendMode(DBlurEffectWidget::BehindWindowBlend);
    setMaskColor(DBlurEffectWidget::DarkColor);

    initSize();

    connect(m_mouseArea, &DRegionMonitor::buttonPress, [this](const QPoint &p, const int button){
        if (button == 4) {
            m_wallpaperList->prevPage();
        } else if (button == 5) {
            m_wallpaperList->nextPage();
        } else {
            qDebug() << "button pressed on blank area, quit.";

            if (!rect().contains(p.x() - this->x(), p.y() - this->y())) {
                hide();
            }
        }
    });

    m_closeButton->hide();
    connect(m_wallpaperList, &WallpaperList::needCloseButton,
            this, &Frame::handleNeedCloseButton);
    connect(m_wallpaperList, &WallpaperList::needPreviewWallpaper,
            m_dbusDeepinWM, &DeepinWM::SetTransientBackground);

    QTimer::singleShot(0, this, &Frame::initListView);
}

Frame::~Frame()
{

}

void Frame::show()
{
    m_dbusDeepinWM->RequestHideWindows();

    m_mouseArea->registerRegion();

    DBlurEffectWidget::show();
}

void Frame::handleNeedCloseButton(QString path, QPoint pos)
{
    if (!path.isEmpty()) {
        m_closeButton->move(pos.x() - 10, pos.y() - 10);
        m_closeButton->show();
        m_closeButton->disconnect();

        connect(m_closeButton, &DImageButton::clicked, this, [this, path] {
            m_dbusAppearance->Delete("background", path);
            m_wallpaperList->removeWallpaper(path);
            m_closeButton->hide();
        }, Qt::UniqueConnection);
    } else {
        m_closeButton->hide();
    }
}

void Frame::showEvent(QShowEvent * event)
{
    activateWindow();

    QTimer::singleShot(1, this, &Frame::refreshList);

    DBlurEffectWidget::showEvent(event);
}

void Frame::hideEvent(QHideEvent *event)
{
    DBlurEffectWidget::hideEvent(event);

    m_dbusDeepinWM->CancelHideWindows();

    m_mouseArea->unregisterRegion();

    if (!m_wallpaperList->desktopWallpaper().isEmpty())
        m_dbusAppearance->Set("background", m_wallpaperList->desktopWallpaper());
    else
        m_dbusDeepinWM->SetTransientBackground("");

    if (!m_wallpaperList->lockWallpaper().isEmpty())
        m_dbusAppearance->Set("greeterbackground", m_wallpaperList->lockWallpaper());

    ThumbnailManager *manager = ThumbnailManager::instance();
    manager->stop();

    emit done();
}

void Frame::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        qDebug() << "escape key pressed, quit.";
        hide();
    }

    DBlurEffectWidget::keyPressEvent(event);
}

void Frame::initSize()
{
    const QRect primaryRect = qApp->primaryScreen()->geometry();

    setFixedSize(primaryRect.width(), FrameHeight);
    qDebug() << "move befor: " << this->geometry() << m_wallpaperList->geometry();
    move(primaryRect.x(), primaryRect.y() + primaryRect.height() - FrameHeight);
    qDebug() << "this move : " << this->geometry() << m_wallpaperList->geometry();
    m_wallpaperList->setFixedSize(primaryRect.width(), ListHeight);
    m_wallpaperList->move(0, (FrameHeight - ListHeight) / 2);
    qDebug() << "m_wallpaperList move: " << this->geometry() << m_wallpaperList->geometry();
}

void Frame::initListView()
{

}

void Frame::refreshList()
{
    m_wallpaperList->clear();
    m_wallpaperList->hide();

    QDBusPendingCall call = m_dbusAppearance->List("background");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call] {
        if (call.isError()) {
            qWarning() << "failed to get all backgrounds: " << call.error().message();
        } else {
            QDBusReply<QString> reply = call.reply();
            QString value = reply.value();
            QStringList strings = processListReply(value);

            foreach (QString path, strings) {
                WallpaperItem * item = m_wallpaperList->addWallpaper(path);
                item->setDeletable(m_deletableInfo.value(path));
            }

            m_wallpaperList->setFixedWidth(width());
            m_wallpaperList->updateItemThumb();
            m_wallpaperList->show();
        }
    });
}

QStringList Frame::processListReply(const QString &reply)
{
    QStringList result;

    QJsonDocument doc = QJsonDocument::fromJson(reply.toUtf8());
    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        foreach (QJsonValue val, arr) {
            QJsonObject obj = val.toObject();
            QString id = obj["Id"].toString();
            result.append(id);
            m_deletableInfo[id] = obj["Deletable"].toBool();
        }
    }

    return result;
}
