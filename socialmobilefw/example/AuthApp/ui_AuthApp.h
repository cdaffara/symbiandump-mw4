/********************************************************************************
** Form generated from reading UI file 'AuthApp.ui'
**
** Created: Wed Sep 22 18:28:00 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHAPP_H
#define UI_AUTHAPP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AuthAppClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *buttonForget;
    QPushButton *pushButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QMainWindow *AuthAppClass)
    {
        if (AuthAppClass->objectName().isEmpty())
            AuthAppClass->setObjectName(QString::fromUtf8("AuthAppClass"));
        AuthAppClass->resize(275, 310);
        centralWidget = new QWidget(AuthAppClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonForget = new QPushButton(centralWidget);
        buttonForget->setObjectName(QString::fromUtf8("buttonForget"));

        verticalLayout->addWidget(buttonForget);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        AuthAppClass->setCentralWidget(centralWidget);

        retranslateUi(AuthAppClass);

        QMetaObject::connectSlotsByName(AuthAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *AuthAppClass)
    {
        AuthAppClass->setWindowTitle(QApplication::translate("AuthAppClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        buttonForget->setText(QApplication::translate("AuthAppClass", "Logout", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("AuthAppClass", "Login to facebook", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AuthAppClass: public Ui_AuthAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHAPP_H
