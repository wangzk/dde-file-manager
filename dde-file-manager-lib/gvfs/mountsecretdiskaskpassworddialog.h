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

#ifndef MOUNTSECRETDISKASKPASSWORDDIALOG_H
#define MOUNTSECRETDISKASKPASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonObject>
#include <ddialog.h>
#include <dpasswordedit.h>
#include <dpasswordedit.h>

DWIDGET_USE_NAMESPACE



class MountSecretDiskAskPasswordDialog : public DDialog
{
    Q_OBJECT
public:
    explicit MountSecretDiskAskPasswordDialog(const QString& tipMessage, QWidget *parent = 0);
    ~MountSecretDiskAskPasswordDialog();

    void initUI();
    void initConnect();

    QString password() const;

    int passwordSaveMode() const;

public slots:
    void handleRadioButtonClicked(int index);
    void handleButtonClicked(int index, QString text);

protected:
    void showEvent(QShowEvent* event);

private:
    QString m_descriptionMessage = "";
    QLabel* m_titleLabel = nullptr;
    QLabel* m_descriptionLabel = nullptr;
    DPasswordEdit* m_passwordLineEdit = nullptr;

    QRadioButton* m_neverRadioCheckBox = nullptr;
    QRadioButton* m_sessionRadioCheckBox = nullptr;
    QRadioButton* m_forerverRadioCheckBox = nullptr;

    QButtonGroup* m_passwordButtonGroup = nullptr;


    QString m_password = "";
    int m_passwordSaveMode = 0;
};

#endif // MOUNTSECRETDISKASKPASSWORDDIALOG_H
