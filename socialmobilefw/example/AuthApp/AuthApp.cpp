/**
* Copyright (c) 2010 Sasken Communication Technologies Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "{License}"
* which accompanies  this distribution, and is available
* at the URL "{LicenseUrl}".
*
* Initial Contributors:
* Narasimhulu Kavadapu, Sasken Communication Technologies Ltd - Initial contribution
*
* Contributors:
*
* Description:
* UI Class of Authentication Application
*/

#include "AuthApp.h"
#include <QDebug>
#include <QMessageBox>

#include "keys.h"

AuthApp::AuthApp(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::AuthAppClass)
{
	qDebug()<<"Inside AuthApp::AuthApp()";
	ui->setupUi(this);
    //iFBSession = FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab", "3b86a756f77967dea4674f080fa5d345", QString());
    //iFBSession = FBSession::sessionForApplication("ed6d6d36813f0fbae8061776beb68daf", "fb10f1c758fc285b2a6113344ef4c893", QString());		//NArasimha's APP
	iFBSession = FBSession::sessionForApplication(kApiKey,kApiSecret, QString());		//Nalina's App
	//iFBSession = FBSession::sessionForApplication("114354298600019", "fb10f1c758fc285b2a6113344ef4c893", QString());//Sid's App
	//iFBSession = FBSession::sessionForApplication("283d4e964b2754505cbbcf69c1d68f3c", "eb8723a3b5921250df442f9695e6bd29", QString());//Sid's App
	connect (iFBSession,SIGNAL(sessionDidLogin(QString)), this, SLOT(sessionDidLogin(QString)));
    connect (iFBSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));
    
    //CheckforLogin();
}

AuthApp::~AuthApp()
{
	delete iFBSession;
	delete ui;
}

void AuthApp::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void AuthApp::CheckforLogin()
{
	qDebug()<<"Inside AuthApp::CheckforLogin()";
	iLoginDialog = NULL;
	if ( !( iFBSession->resume() ) ){
		ui->buttonForget->setDisabled(true);
		ui->pushButton->setEnabled(true);
	}
}
void AuthApp::on_pushButton_clicked()
{
	qDebug()<<"Inside AuthApp::on_pushButton_clicked()";
	iLoginDialog = new FBLoginDialog();
	iLoginDialog->show();
}
void AuthApp::sessionDidLogin(QString sessionkey)
{
	qDebug()<<"Inside AuthApp::sessionDidLogin()";
    if (iLoginDialog )
    {
		QMessageBox msgbox;
		QString msg ("Authorization completes!!");
		msgbox.setText(msg);
		msgbox.exec();
		
        iLoginDialog->deleteLater();;
        iLoginDialog = NULL;
    }else{
		QMessageBox msgbox;
		QString msg ("your already authorized,Please logout and Login again for new authorization!!");
		msgbox.setText(msg);
		msgbox.exec();
    }
    
    ui->pushButton->setDisabled(true);
    ui->buttonForget->setEnabled(true);
}

void AuthApp::sessionDidLogout()
{
	qDebug()<<"Inside AuthApp::sessionDidLogout()";
    QMessageBox msgbox;
    msgbox.setText("logged out successfully!!");
    msgbox.exec();
    
    ui->pushButton->setEnabled(true);
    ui->buttonForget->setDisabled(true);
    
}
void AuthApp::requestFailedWithFacebookError ( const FBError& aError )
{
    qDebug() << "facebook error is " << aError.code() << " - " << aError.description();
}

void AuthApp::on_buttonForget_clicked()
{
	qDebug()<<"Inside AuthApp::on_buttonForget_clicked()";
    iFBSession->logout();
}
