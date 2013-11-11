/********************************************************************************
** Form generated from reading UI file 'USBSettingsApp.ui'
**
** Created: Fri Mar 5 17:51:29 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USBSETTINGSAPP_H
#define UI_USBSETTINGSAPP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_USBSettingsApp
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *USBSettingsApp)
    {
        if (USBSettingsApp->objectName().isEmpty())
            USBSettingsApp->setObjectName(QString::fromUtf8("USBSettingsApp"));
        USBSettingsApp->resize(800, 600);
        centralwidget = new QWidget(USBSettingsApp);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        USBSettingsApp->setCentralWidget(centralwidget);
        menubar = new QMenuBar(USBSettingsApp);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        USBSettingsApp->setMenuBar(menubar);
        statusbar = new QStatusBar(USBSettingsApp);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        USBSettingsApp->setStatusBar(statusbar);

        retranslateUi(USBSettingsApp);

        QMetaObject::connectSlotsByName(USBSettingsApp);
    } // setupUi

    void retranslateUi(QMainWindow *USBSettingsApp)
    {
        USBSettingsApp->setWindowTitle(QApplication::translate("USBSettingsApp", "USBSettingsApp", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class USBSettingsApp: public Ui_USBSettingsApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USBSETTINGSAPP_H
