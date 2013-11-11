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
* Main view for the WLAN Login application
* 
*/


#include <QGraphicsLinearLayout>
#include <QWebFrame>

// System includes
#include <HbToolbar>
#include <HbAction>
#include <HbProgressBar>
#include <HbScrollBar>
#include <HbScrollArea>
#include <HbWidget>
#include <HbMainWindow>

// User includes
#include "wlanlogindocumentloader.h"
#include "wlanloginview.h"
#include "wlanloginwebview.h"
#include "wlanloginwebpage.h"
#include "wlanloginmainwindow.h"
#include "wlanloginengine.h"
#include "ictsqtwrapper.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginviewTraces.h"
#endif

/*!
    \class WlanLoginView
    \brief Main view for the WLAN Login application
*/

// External function prototypes

// Local constants
//! WLAN Login view docml file
static const QString wlanLoginViewDocml(":/docml/wlanloginview.docml");

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor       
 */
WlanLoginView::WlanLoginView(WlanLoginMainWindow* mainWindow):
    mMainWindow(mainWindow),
    mDocLoader(new WlanLoginDocumentLoader(mainWindow)),
    mProgressBar(NULL),
    mScrollAreaContent(NULL),
    mWebView(NULL),
    mNextAction(NULL),
    mFirstIctsOkResult(true)
{   
    OstTraceFunctionEntry0(WLANLOGINVIEW_WLANLOGINVIEW_ENTRY);
    
    loadDocml();
    
    setTitleBarVisible(false);
    setStatusBarVisible(false);
    
    // Set white background to content widget
    QPixmap pixmap(10,10);
    pixmap.fill(Qt::white);
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
    mScrollAreaContent->setBackgroundItem(pixmapItem);
    
    mWebView->page()->setNetworkAccessManager(mMainWindow->application()->engine()->networkAccessManager());
    
    bool connectStatus = connect(
        mWebView,
        SIGNAL(loadStarted()),
        this,
        SLOT(handleLoadStarted())); 
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mWebView,
        SIGNAL(loadProgress(int)),
        this,
        SLOT(handleLoadProgress(int))); 
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mWebView,
        SIGNAL(loadFinished(bool)),
        this,
        SLOT(handleLoadFinished(bool)));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
    mWebView,
    SIGNAL(urlChanged(const QUrl&)),
    this,
    SLOT(handleUrlChanged(const QUrl&)));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mWebView->page(),
        SIGNAL(formSubmitted()),
        this,
        SLOT(handleFormSubmitted()));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mCancelAction,
        SIGNAL(triggered()),
        this,
        SLOT(handleCancelAction()));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mNextAction,
        SIGNAL(triggered()),
        this,
        SLOT(handleNextAction()));
    Q_ASSERT(connectStatus == true);
    
    show();
    
    OstTraceFunctionExit0(WLANLOGINVIEW_WLANLOGINVIEW_EXIT);
}

/*!
   Loads widgets and objects from the docml file. 
 */
void WlanLoginView::loadDocml()
{
    OstTraceFunctionEntry0( WLANLOGINVIEW_LOADDOCML_ENTRY );

    setObjectName(QString("wlanLoginView"));
    QObjectList objectList;
    objectList.append(this);
    mDocLoader->setObjectTree(objectList);

    bool ok = false;
    
    mDocLoader->load(wlanLoginViewDocml, &ok);
    Q_ASSERT(ok);
     
    //Fetch pointer for progress bar
    mProgressBar = reinterpret_cast<HbProgressBar *>(
        mDocLoader->findObject("progressBar"));
    Q_ASSERT(mProgressBar);
    
    //Fetch pointer for scroll area content
    mScrollAreaContent = reinterpret_cast<HbWidget *>(
        mDocLoader->findObject("scrollAreaContent"));
    Q_ASSERT(mScrollAreaContent);
    
    //Fetch pointer for Web View
    mWebView = reinterpret_cast<WlanLoginWebView *>(
        mDocLoader->findObject("webView"));
    Q_ASSERT(mWebView);
    
	//Fetch pointer for cancel action
    mCancelAction = qobject_cast<HbAction*> (
        mDocLoader->findObject("cancelAction"));
    Q_ASSERT(mCancelAction != NULL);
 
	//Fetch pointer for next action
    mNextAction = qobject_cast<HbAction*> (
        mDocLoader->findObject("nextAction"));
    Q_ASSERT(mNextAction != NULL);
    
    OstTraceFunctionExit0( WLANLOGINVIEW_LOADDOCML_EXIT );
}

/*!
    Destructor       
 */
WlanLoginView::~WlanLoginView()
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_DESTRUCTOR_ENTRY);
    
    OstTraceFunctionExit0(WLANLOGINVIEW_DESTRUCTOR_EXIT);
}


/*!
    This function handles connectionReady signal from engine and starts loading of
    provided web page
    
    @param [in] url URL to be loaded
 */
void WlanLoginView::handleConnectionReady(QUrl url)
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLECONNECTIONREADY_ENTRY);
    
    mWebView->load(url);
    
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLECONNECTIONREADY_EXIT);
}


/*!
    This function handles orientationChanged signal
    
    @param [in] orientation New Orientation of the screen
 */
void WlanLoginView::handleOrientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    OstTraceFunctionEntry0( WLANLOGINVIEW_ORIENTATIONCHANGED_ENTRY );
    
    adjustViewSize();
    
    OstTraceFunctionEntry0( WLANLOGINVIEW_ORIENTATIONCHANGED_EXIT );
}

void WlanLoginView::adjustViewSize()
{
    OstTraceFunctionEntry0( WLANLOGINVIEW_ADJUSTVIEWSIZE_ENTRY );
    
    //Store current screen size
    QSizeF screenSize = mMainWindow->layoutRect().size();
    
    //Store current content size
    QSize contentSize = mWebView->page()->mainFrame()->contentsSize();
    
    //Set viewPortSize to biggest values of content size or current screen size 
    QSize newViewPortSize;
    if (screenSize.toSize().width() > contentSize.width()) {
        newViewPortSize.setWidth(screenSize.toSize().width());
    } else {    
        newViewPortSize.setWidth(contentSize.width());
    }
    
    if (screenSize.toSize().height() > contentSize.height()) {
        newViewPortSize.setHeight(screenSize.toSize().height());
    } else {    
        newViewPortSize.setHeight(contentSize.height());
    }
    mWebView->page()->setViewportSize(newViewPortSize);
    
    
    //Set Web View size to same size as viewport
    mWebView->setMinimumWidth((qreal)newViewPortSize.width());
    mWebView->setMaximumWidth((qreal)newViewPortSize.width());
    mWebView->setPreferredWidth((qreal)newViewPortSize.width());
    
    mWebView->setMinimumHeight((qreal)newViewPortSize.height());
    mWebView->setMaximumHeight((qreal)newViewPortSize.height());
    mWebView->setPreferredHeight((qreal)newViewPortSize.height());
    
    
    //Set preferred content size to current screen size
    mWebView->page()->setPreferredContentsSize(mMainWindow->layoutRect().size().toSize());
      
    OstTraceFunctionEntry0( WLANLOGINVIEW_ADJUSTVIEWSIZE_EXIT );
}


/*!
    This function handles urlChanged signal from QGraphicsWebView
    
    @param [in] newUrl New URL
 */
void WlanLoginView::handleUrlChanged(const QUrl& newUrl )
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLEURLCHANGED_ENTRY);
 
#ifdef OST_TRACE_COMPILER_IN_USE
    QString url = newUrl.toString();
    TPtrC tmp(url.utf16(),url.length() );
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANLOGINVIEW_HANDLEURLCHANGED_URL_TRACE, 
        "WlanLoginEngine::handleUrlChanged;newUrl=%S",
        tmp);
#endif

    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLEURLCHANGED_EXIT);
}


/*!
    This function handles loadStarted signal from QGraphicsWebView
 */
void WlanLoginView::handleLoadStarted()
{
   OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLELOADSTARTED_ENTRY);
   
    mProgressBar->setVisible(true);
    
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLELOADSTARTED_EXIT);
}


/*!
    This function handles loadProgress signal from QGraphicsWebView

    @param [in] progressValue Indicates page loading progress: 0..100
 */
void WlanLoginView::handleLoadProgress(int progressValue)
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLELOADPROGRESS_ENTRY);
    
    mProgressBar->setProgressValue(progressValue);
    
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLELOADPROGRESS_EXIT);
}


/*!
    This function handles loadfinished signal from QGraphicsWebView

    @param [in] status Success status
 */
void WlanLoginView::handleLoadFinished(bool status)
{
   OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLELOADFINISHED_ENTRY);
   
    if(status)
    {   
        adjustViewSize();
    }
   
    mProgressBar->setVisible(false);
  
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLELOADFINISHED_EXIT);
}


/*!
    This function handles formSumitted signal from WlanLoginWebPage

 */
void WlanLoginView::handleFormSubmitted()
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLEFORMSUBMITTED_ENTRY);
    
    //User has submitted credentials, let's start internet connectivity test
    emit startIcts();       
    
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLEFORMSUBMITTED_EXIT);
}


/*!
    This function handles triggered signal from "cancel" button

 */
void WlanLoginView::handleCancelAction()
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLECANCELACTION_ENTRY);

    mWebView->stop();
    emit cancelTriggered();
               
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLECANCELACTION_EXIT);
}


/*!
    This function handles triggered signal from "next" button

 */
void WlanLoginView::handleNextAction()
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLENEXTACTION_ENTRY);

    emit nextTriggered();
        
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLENEXTACTION_EXIT);
}


/*!
    This function handles ictsOk signal from engine

 */
void WlanLoginView::handleIctsOk()
{
    OstTraceFunctionEntry0(WLANLOGINVIEW_HANDLEICTSRESULT_ENTRY);
            
    mNextAction->setEnabled(true);
    
    //Send application to background automatically only in first successfull login
    //as in other cases WLAN Wizard won't exist in the underneath
    if (mFirstIctsOkResult) {
        mFirstIctsOkResult = false;
        emit nextTriggered();
    }
    
    OstTraceFunctionExit0(WLANLOGINVIEW_HANDLEICTSRESULT_EXIT);
}

