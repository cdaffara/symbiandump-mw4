/********************************************************************************
** Form generated from reading UI file 'progressbar.ui'
**
** Created: Wed Sep 22 18:27:56 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSBAR_H
#define UI_PROGRESSBAR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_progressbarClass
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QProgressBar *progressBar;

    void setupUi(QWidget *progressbarClass)
    {
        if (progressbarClass->objectName().isEmpty())
            progressbarClass->setObjectName(QString::fromUtf8("progressbarClass"));
        verticalLayout = new QVBoxLayout(progressbarClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(progressbarClass);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        progressBar = new QProgressBar(progressbarClass);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);


        retranslateUi(progressbarClass);

        QMetaObject::connectSlotsByName(progressbarClass);
    } // setupUi

    void retranslateUi(QWidget *progressbarClass)
    {
        progressbarClass->setWindowTitle(QApplication::translate("progressbarClass", "progressbar", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("progressbarClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Loading,Please wait.....</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class progressbarClass: public Ui_progressbarClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSBAR_H
