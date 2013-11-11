/********************************************************************************
** Form generated from reading UI file 'displaywidget.ui'
**
** Created: Wed Sep 22 18:28:30 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYWIDGET_H
#define UI_DISPLAYWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DisplayWidgetClass
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *comboBox_intf;
    QPushButton *pushButton_intf;
    QLabel *label_2;
    QComboBox *comboBox__SP;
    QPushButton *pushButton_SP;
    QLabel *label_3;
    QComboBox *comboBox_service;
    QPushButton *pushButton_service;
    QListWidget *listWidget;
    QScrollBar *verticalScrollBar_list;

    void setupUi(QWidget *DisplayWidgetClass)
    {
        if (DisplayWidgetClass->objectName().isEmpty())
            DisplayWidgetClass->setObjectName(QString::fromUtf8("DisplayWidgetClass"));
        DisplayWidgetClass->resize(400, 489);
        verticalLayoutWidget = new QWidget(DisplayWidgetClass);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 371, 461));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        comboBox_intf = new QComboBox(verticalLayoutWidget);
        comboBox_intf->setObjectName(QString::fromUtf8("comboBox_intf"));

        verticalLayout->addWidget(comboBox_intf);

        pushButton_intf = new QPushButton(verticalLayoutWidget);
        pushButton_intf->setObjectName(QString::fromUtf8("pushButton_intf"));

        verticalLayout->addWidget(pushButton_intf);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        comboBox__SP = new QComboBox(verticalLayoutWidget);
        comboBox__SP->setObjectName(QString::fromUtf8("comboBox__SP"));

        verticalLayout->addWidget(comboBox__SP);

        pushButton_SP = new QPushButton(verticalLayoutWidget);
        pushButton_SP->setObjectName(QString::fromUtf8("pushButton_SP"));

        verticalLayout->addWidget(pushButton_SP);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        comboBox_service = new QComboBox(verticalLayoutWidget);
        comboBox_service->setObjectName(QString::fromUtf8("comboBox_service"));

        verticalLayout->addWidget(comboBox_service);

        pushButton_service = new QPushButton(verticalLayoutWidget);
        pushButton_service->setObjectName(QString::fromUtf8("pushButton_service"));

        verticalLayout->addWidget(pushButton_service);

        listWidget = new QListWidget(verticalLayoutWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        verticalLayout->addWidget(listWidget);

        verticalScrollBar_list = new QScrollBar(DisplayWidgetClass);
        verticalScrollBar_list->setObjectName(QString::fromUtf8("verticalScrollBar_list"));
        verticalScrollBar_list->setGeometry(QRect(380, 242, 20, 221));
        verticalScrollBar_list->setOrientation(Qt::Vertical);

        retranslateUi(DisplayWidgetClass);

        QMetaObject::connectSlotsByName(DisplayWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *DisplayWidgetClass)
    {
        DisplayWidgetClass->setWindowTitle(QApplication::translate("DisplayWidgetClass", "DisplayWidget", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DisplayWidgetClass", "Select Interface", 0, QApplication::UnicodeUTF8));
        pushButton_intf->setText(QApplication::translate("DisplayWidgetClass", "Ok", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DisplayWidgetClass", "Select Service Provider", 0, QApplication::UnicodeUTF8));
        pushButton_SP->setText(QApplication::translate("DisplayWidgetClass", "Ok", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DisplayWidgetClass", "Select Service", 0, QApplication::UnicodeUTF8));
        pushButton_service->setText(QApplication::translate("DisplayWidgetClass", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DisplayWidgetClass: public Ui_DisplayWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYWIDGET_H
