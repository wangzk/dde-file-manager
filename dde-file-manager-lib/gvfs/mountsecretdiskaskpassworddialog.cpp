#include "mountsecretdiskaskpassworddialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>


MountSecretDiskAskPasswordDialog::MountSecretDiskAskPasswordDialog(QWidget *parent):
    DDialog(parent)
{
    setModal(true);
    initUI();
    initConnect();
}

MountSecretDiskAskPasswordDialog::~MountSecretDiskAskPasswordDialog()
{

}

void MountSecretDiskAskPasswordDialog::initUI()
{

    QStringList buttonTexts;
    buttonTexts << tr("Cancel") << tr("Connect");

    QFrame* content = new QFrame;

    m_titleLabel = new QLabel(tr("Please input password to decrypt the disk"));
    m_descriptionLabel = new QLabel(tr(""));

    m_passwordLabel = new QLabel(tr("Password"));
    m_passwordLineEdit = new DPasswordEdit;

    QHBoxLayout* passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(m_passwordLabel);
    passwordLayout->addWidget(m_passwordLineEdit);
    passwordLayout->setSpacing(5);
    passwordLayout->setContentsMargins(0, 0, 0, 0);


//    m_neverRadioCheckBox = new QRadioButton(tr("never"));
//    m_sessionRadioCheckBox = new QRadioButton(tr("session"));
//    m_forerverRadioCheckBox = new QRadioButton(tr("forver"));

//    m_passwordButtonGroup = new QButtonGroup;
//    m_passwordButtonGroup->addButton(m_neverRadioCheckBox);
//    m_passwordButtonGroup->addButton(m_sessionRadioCheckBox);
//    m_passwordButtonGroup->addButton(m_forerverRadioCheckBox);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_descriptionLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addSpacing(10);
//    mainLayout->addWidget(m_neverRadioCheckBox, 0);
//    mainLayout->addWidget(m_sessionRadioCheckBox, 1);
//    mainLayout->addWidget(m_forerverRadioCheckBox, 2);


    content->setLayout(mainLayout);

    addContent(content);
    addButtons(buttonTexts);
    setSpacing(10);
    setDefaultButton(1);
}

void MountSecretDiskAskPasswordDialog::initConnect()
{
//    connect(m_passwordButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(handleRadioButtonClicked(int)));
    connect(this, SIGNAL(buttonClicked(int,QString)), this, SLOT(handleButtonClicked(int,QString)));
}

void MountSecretDiskAskPasswordDialog::handleRadioButtonClicked(int index)
{
    m_passwordSaveMode = index;
}

void MountSecretDiskAskPasswordDialog::handleButtonClicked(int index, QString text)
{
    Q_UNUSED(text)
    if (index == 1){
        m_password = m_passwordLineEdit->text();
    }
    accept();
}

void MountSecretDiskAskPasswordDialog::showEvent(QShowEvent *event)
{
    m_passwordLineEdit->setFocus();
    DDialog::showEvent(event);
}

int MountSecretDiskAskPasswordDialog::passwordSaveMode() const
{
    return m_passwordSaveMode;
}

QString MountSecretDiskAskPasswordDialog::password() const
{
    return m_password;
}