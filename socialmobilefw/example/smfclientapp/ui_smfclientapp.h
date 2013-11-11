/********************************************************************************
** Form generated from reading UI file 'smfclientapp.ui'
**
** Created: Wed Sep 22 18:28:33 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SMFCLIENTAPP_H
#define UI_SMFCLIENTAPP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SmfClientApp
{
public:
    QListWidget *listWidget;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QLabel *label;
    QPushButton *pushButton_2;

    void setupUi(QWidget *SmfClientApp)
    {
        if (SmfClientApp->objectName().isEmpty())
            SmfClientApp->setObjectName(QString::fromUtf8("SmfClientApp"));
        SmfClientApp->resize(340, 600);
        listWidget = new QListWidget(SmfClientApp);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(20, 230, 256, 271));
        comboBox = new QComboBox(SmfClientApp);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(20, 90, 201, 51));
        pushButton = new QPushButton(SmfClientApp);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(20, 150, 75, 51));
        label = new QLabel(SmfClientApp);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 19, 271, 51));
        pushButton_2 = new QPushButton(SmfClientApp);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(110, 150, 75, 51));

        retranslateUi(SmfClientApp);

        QMetaObject::connectSlotsByName(SmfClientApp);
    } // setupUi

    void retranslateUi(QWidget *SmfClientApp)
    {
        SmfClientApp->setWindowTitle(QApplication::translate("SmfClientApp", "SmfClientApp", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("SmfClientApp", "Friends", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SmfClientApp", "Supported Social Networking Sites", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("SmfClientApp", "Posts", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SmfClientApp: public Ui_SmfClientApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SMFCLIENTAPP_H
