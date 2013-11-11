/********************************************************************************
** Form generated from reading UI file 'posttestui.ui'
**
** Created: Wed Sep 22 18:28:30 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POSTTESTUI_H
#define UI_POSTTESTUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PostTestUiClass
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QComboBox *comboBox;
    QListWidget *listWidget;

    void setupUi(QWidget *PostTestUiClass)
    {
        if (PostTestUiClass->objectName().isEmpty())
            PostTestUiClass->setObjectName(QString::fromUtf8("PostTestUiClass"));
        PostTestUiClass->resize(400, 300);
        verticalLayoutWidget = new QWidget(PostTestUiClass);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 20, 371, 261));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        comboBox = new QComboBox(verticalLayoutWidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 1, 0, 1, 1);

        listWidget = new QListWidget(verticalLayoutWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 2, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);


        retranslateUi(PostTestUiClass);

        QMetaObject::connectSlotsByName(PostTestUiClass);
    } // setupUi

    void retranslateUi(QWidget *PostTestUiClass)
    {
        PostTestUiClass->setWindowTitle(QApplication::translate("PostTestUiClass", "PostTestUi", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("PostTestUiClass", "Retreive Post", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PostTestUiClass", "Post Scrap", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PostTestUiClass", "Post Scrap directed", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class PostTestUiClass: public Ui_PostTestUiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POSTTESTUI_H
