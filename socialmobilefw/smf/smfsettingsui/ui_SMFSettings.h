/********************************************************************************
** Form generated from reading UI file 'SMFSettings.ui'
**
** Created: Fri Jul 30 14:06:09 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SMFSETTINGS_H
#define UI_SMFSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SMFSettingsClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *PluginlistboxBox;
    QComboBox *pluginstatusbox;
    QLabel *label_2;
    QComboBox *AuthExplimitbox;
    QLabel *label_3;
    QComboBox *MaxDataTransferlimitBox;
    QLabel *label_4;
    QComboBox *RoamingStatusBox;
    QLabel *label_5;
    QComboBox *Uploadfiletypebox;

    void setupUi(QWidget *SMFSettings)
    {
        if (SMFSettings->objectName().isEmpty())
            SMFSettings->setObjectName(QString::fromUtf8("SMFSettings"));
        SMFSettings->resize(198, 255);
        verticalLayout = new QVBoxLayout(SMFSettings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(SMFSettings);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        PluginlistboxBox = new QComboBox(SMFSettings);
        PluginlistboxBox->setObjectName(QString::fromUtf8("PluginlistboxBox"));

        horizontalLayout->addWidget(PluginlistboxBox);

        pluginstatusbox = new QComboBox(SMFSettings);
        pluginstatusbox->setObjectName(QString::fromUtf8("pluginstatusbox"));

        horizontalLayout->addWidget(pluginstatusbox);


        verticalLayout->addLayout(horizontalLayout);

        label_2 = new QLabel(SMFSettings);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        AuthExplimitbox = new QComboBox(SMFSettings);
        AuthExplimitbox->setObjectName(QString::fromUtf8("AuthExplimitbox"));

        verticalLayout->addWidget(AuthExplimitbox);

        label_3 = new QLabel(SMFSettings);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        MaxDataTransferlimitBox = new QComboBox(SMFSettings);
        MaxDataTransferlimitBox->setObjectName(QString::fromUtf8("MaxDataTransferlimitBox"));

        verticalLayout->addWidget(MaxDataTransferlimitBox);

        label_4 = new QLabel(SMFSettings);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        RoamingStatusBox = new QComboBox(SMFSettings);
        RoamingStatusBox->setObjectName(QString::fromUtf8("RoamingStatusBox"));

        verticalLayout->addWidget(RoamingStatusBox);

        label_5 = new QLabel(SMFSettings);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout->addWidget(label_5);

        Uploadfiletypebox = new QComboBox(SMFSettings);
        Uploadfiletypebox->setObjectName(QString::fromUtf8("Uploadfiletypebox"));

        verticalLayout->addWidget(Uploadfiletypebox);


        retranslateUi(SMFSettings);

        QMetaObject::connectSlotsByName(SMFSettings);
    } // setupUi

    void retranslateUi(QWidget *SMFSettings)
    {
        SMFSettings->setWindowTitle(QApplication::translate("SMFSettingsClass", "SMFSettings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SMFSettingsClass", "Plugins", 0, QApplication::UnicodeUTF8));
        PluginlistboxBox->clear();
        PluginlistboxBox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "Facebook", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Flicker", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Orkut", 0, QApplication::UnicodeUTF8)
        );
        pluginstatusbox->clear();
        pluginstatusbox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "Enable", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Disable", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("SMFSettingsClass", "Auth. Exp. Limit", 0, QApplication::UnicodeUTF8));
        AuthExplimitbox->clear();
        AuthExplimitbox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "7", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "9", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "10", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("SMFSettingsClass", "Max. Data Transfer Limit", 0, QApplication::UnicodeUTF8));
        MaxDataTransferlimitBox->clear();
        MaxDataTransferlimitBox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "100", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "200", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("SMFSettingsClass", "Network access in roaming", 0, QApplication::UnicodeUTF8));
        RoamingStatusBox->clear();
        RoamingStatusBox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "Enable", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Disable", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("SMFSettingsClass", "Upload File Type", 0, QApplication::UnicodeUTF8));
        Uploadfiletypebox->clear();
        Uploadfiletypebox->insertItems(0, QStringList()
         << QApplication::translate("SMFSettingsClass", "Video", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Image", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SMFSettingsClass", "Audio", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class SMFSettingsClass: public Ui_SMFSettingsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SMFSETTINGS_H
