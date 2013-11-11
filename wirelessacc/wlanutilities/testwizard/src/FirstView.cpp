/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Test application for wizards.
*/

#include <qgraphicswidget>
#include <hbpushbutton.h>
#include <hbdocumentloader.h>
#include <hbcombobox.h>
#include <hbcheckbox.h>
#include <hblineedit.h>
#include <hblabel.h>

#include <qdebug.h>

#include "firstview.h"
#include "wlanwizard.h"

FirstView::FirstView() :
    mWizard(NULL)
{
    qDebug("FirstView::FirstView()");
    bool ok = false;
    HbDocumentLoader loader;
    loader.load(":/docml/mainview.docml", &ok);
    Q_ASSERT_X(ok, "EAP Wizard EXE", "Invalid docml file");

    QGraphicsWidget *widget = loader.findWidget("view");
    Q_ASSERT_X(widget != 0, "TestEapWizard", "View not found");

    HbPushButton *action = qobject_cast<HbPushButton *> (loader.findObject("startWizard"));
    Q_ASSERT_X(action != 0, "TestEapWizard", "startWizard not found");
    connect(action, SIGNAL(clicked(bool)), this, SLOT(start(bool)));

    mUseConf = qobject_cast<HbCheckBox *> (loader.findWidget("useConf"));
    Q_ASSERT_X(mUseConf != 0, "TestEapWizard", "useConf not found");

    mSsid = qobject_cast<HbLineEdit *> (loader.findWidget("ssid"));
    Q_ASSERT_X(mSsid != 0, "TestEapWizard", "ssid not found");

    mNetworkMode = qobject_cast<HbComboBox *> (loader.findWidget("networkMode"));
    Q_ASSERT_X(mNetworkMode != 0, "TestEapWizard", "networkMode not found");

    mSecurityMode = qobject_cast<HbComboBox *> (loader.findWidget("securityMode"));
    Q_ASSERT_X(mSecurityMode != 0, "TestEapWizard", "securityMode not found");

    mUseWpaPsk = qobject_cast<HbCheckBox *> (loader.findWidget("useWpaPsk"));
    Q_ASSERT_X(mUseWpaPsk != 0, "TestEapWizard", "useWpaPsk not found");
        
    mUseWps = qobject_cast<HbCheckBox *> (loader.findWidget("useWps"));
    Q_ASSERT_X(mUseConf != 0, "TestEapWizard", "useWps not found");
    
    mStatus = qobject_cast<HbLabel *> (loader.findWidget("labelWizardStatus"));
    Q_ASSERT_X(mStatus != 0, "TestEapWizard", "labelWizardStatus not found");

    setWidget(widget);
    qDebug("FirstView::FirstView() - done %d", action);
}

FirstView::~FirstView()
{
    delete mWizard;
}

void FirstView::start(bool)
{
    qDebug("FirstView::Start");
    if (!mWizard) {
        mWizard = new WlanWizard(mainWindow());

        connect(mWizard, SIGNAL(finished(int, bool)), this, SLOT(finished(int, bool)));
        connect(mWizard, SIGNAL(cancelled()), this, SLOT(cancelled()));
    }

    if (mUseConf->isChecked()) {
        mWizard->setParameters(
            mSsid->text(),
            mSsid->text().toUtf8(),
            mNetworkMode->currentIndex(), 
            1 << mSecurityMode->currentIndex(),
            mUseWpaPsk->isChecked(),
            mUseWps->isChecked());
    }

    mWizard->show();
}

void FirstView::deleteWizard()
{
    qDebug("FirstView::deleteWizard");

    Q_ASSERT(mWizard);
    mWizard->deleteLater();
    mWizard = NULL;

}

void FirstView::finished(int iapId, bool connected)
{
    qDebug("FirstView::complete(), iap id: %d, connected: %d", iapId, connected);
    QString text;
    text.setNum(iapId);
    text.prepend("Finished, IAP ID: ");
    mStatus->setPlainText(text);
    deleteWizard();
}

void FirstView::cancelled()
{
    qDebug("FirstView::cancelled()");
    mStatus->setPlainText("Cancelled");
    deleteWizard();
}
