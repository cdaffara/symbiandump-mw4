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

#ifndef AUTHAPP_H
#define AUTHAPP_H

#include <QtGui/QMainWindow>
#include "ui_AuthApp.h"
#include "errorCodes.h"
#include "loginDialog.h"
#include "sessionSP.h"

class AuthApp : public QMainWindow
{
    Q_OBJECT

public:
	AuthApp(QWidget *parent = 0);
    ~AuthApp();
private:
	void CheckforLogin();
private slots:
    void on_buttonForget_clicked();
    void on_pushButton_clicked();

    void sessionDidLogin(QString sessionkey);
    void sessionDidLogout();

    void requestFailedWithFacebookError ( const FBError& aError );

protected:
    void changeEvent(QEvent *e);

private:
    FBSession*      iFBSession;
    FBLoginDialog*  iLoginDialog;
    Ui::AuthAppClass *ui;
};

#endif // AUTHAPP_H
