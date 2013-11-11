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
* 
*/

#ifndef WLANLOGINVIEW_H
#define WLANLOGINVIEW_H

// System includes
#include <QUrl>
#include <HbView>
#include <HbScrollArea>
#include <QGraphicsLinearLayout>

// User includes
#include "wlanloginapplication.h"
#include "wlanloginwebpage.h"
#include "ictsqtwrapper.h"

// Forward declarations
class HbDocumentLoader;
class HbProgressBar;
class WlanLoginWebView;
class QNetworkAccessManager;
class HbWidget;

// External data types

// Constants

// Class declaration
class WlanLoginView : public HbView
    {
    Q_OBJECT
    
public:
    WlanLoginView(WlanLoginMainWindow* mainWindow);
    virtual ~WlanLoginView();
    
    /*!
        Enumeration type for current toolbar action in use
    */ 
    enum ActionType {
        Cancel,     //!< "Cancel" button
        Next       //!< "Next" button
    };
    
private:
    void loadDocml();
    
private slots:

    void handleConnectionReady(QUrl url);    
    void handleOrientationChanged(Qt::Orientation orientation);
    void adjustViewSize();
    
    void handleUrlChanged(const QUrl& newUrl);    
    void handleLoadStarted();
    void handleLoadProgress(int progressValue);
    void handleLoadFinished(bool status );
    void handleFormSubmitted();
    void handleCancelAction();
    void handleNextAction();
    void handleIctsOk();
    
signals:
    void cancelTriggered();
    void nextTriggered();

    void startIcts();
    
private: // Data
    
    // Not owned data
    WlanLoginMainWindow* mMainWindow;  //!< pointer to main window instance

    // Owned data

    QScopedPointer<HbDocumentLoader> mDocLoader; //! Document loader for list view
    
    HbProgressBar* mProgressBar;    //!< pointer to HB progressbar  
    HbWidget* mScrollAreaContent;   //!< pointer to scroll are content
    WlanLoginWebView* mWebView;     //!< pointer to web view
    HbAction* mCancelAction;        //!< pointer to "cancel" action 
    HbAction* mNextAction;          //!< pointer to "next" action
    bool mFirstIctsOkResult;         //!< Variable for controlling automatic background sending

    };

#endif // WLANLOGINWEBVIEW_H
