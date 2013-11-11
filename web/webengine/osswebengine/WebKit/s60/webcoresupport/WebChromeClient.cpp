/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of WebChromeClient
*
*/

#include "config.h"
#include "Frame.h"
#include "WebChromeClient.h"
#include "FloatRect.h"
#include "IntRect.h"
#include "PlatformString.h"
#include "DebugStream.h"
#include "BrCtl.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebUtil.h"
#include "FrameLoadRequest.h"
#include "kjs_proxy.h"
#include "kjs_binding.h"
#include <StringLoader.h>
#include <Webkit.rsg>



#include <centralrepository.h>
#include <BrowserUiSDKCRKeys.h>

const TInt KMaxJavaLogMsgLen = 500;
const TInt KMaxJavaLogMsgPaddingLen = 0x200; // for paddings in JS log message such as: log type and labels.
const TInt KMaxJSLogFileSize = 0x100000;

_LIT(KScriptLogDir, "C:\\data\\" );
_LIT(KWidgetScriptLogFile, "jslog_widget.log" );
_LIT(KWebScriptLogFile, "jslog_web.log" );


using namespace WebCore;

WebChromeClient::WebChromeClient(WebView *webView)
{
    m_webView = webView;
    m_visibility = false;
}

void WebChromeClient::chromeDestroyed()
{
    delete this;
}

FloatRect WebChromeClient::windowRect() { notImplemented(); return FloatRect(); }
FloatRect WebChromeClient::pageRect() { notImplemented(); return FloatRect(); }
float WebChromeClient::scaleFactor() { notImplemented(); return 1.0; }

Page* WebChromeClient::createWindow(Frame* frame, const WebCore::FrameLoadRequest& frameRequest)
{
    Page* page = NULL;
    HBufC* name = NULL;
    CBrCtl* newBrCtl = NULL;
    String frameName = frameRequest.frameName();
    if ((frameName.des().Length() && (frameName == "_blank" || frameName == "_new")) || (frameName.des().Length() == 0)){
        name = generateFrameName();
    }
    else { // frame has a name
        name = frameName.des().Alloc();
    }
    if (name) {
    	bool userGesture = frame->scriptProxy()->interpreter()->wasRunByUserGesture();
    	TRAP_IGNORE(
        newBrCtl = m_webView->brCtl()->getWindowL(*name, userGesture);
        );
        delete name;
        if (newBrCtl) {
            page = newBrCtl->webView()->page();
        }
    }
    return page;
}

Page* WebChromeClient::createModalDialog(Frame*, const WebCore::FrameLoadRequest&) { notImplemented(); return 0;}

void WebChromeClient::show()
{
    m_webView->brCtl()->showWindow();
}

bool WebChromeClient::canRunModal() { notImplemented(); return false; }
void WebChromeClient::setToolbarsVisible(bool) { notImplemented(); }
bool WebChromeClient::toolbarsVisible() { notImplemented(); return false; }
void WebChromeClient::setStatusbarVisible(bool) { notImplemented(); }
bool WebChromeClient::statusbarVisible() { notImplemented(); return false; }
void WebChromeClient::setScrollbarsVisible(bool) { notImplemented(); }
bool WebChromeClient::scrollbarsVisible() { notImplemented(); return false; }
void WebChromeClient::setMenubarVisible(bool) { notImplemented(); }
bool WebChromeClient::menubarVisible() { notImplemented(); return false; }
void WebChromeClient::setResizable(bool) { notImplemented(); }

void WebChromeClient::closeWindowSoon()
{
    m_webView->brCtl()->closeWindowSoon();
}

bool WebChromeClient::canTakeFocus(FocusDirection) { notImplemented(); return true; }
void WebChromeClient::takeFocus(FocusDirection) { notImplemented(); }
bool WebChromeClient::canRunBeforeUnloadConfirmPanel() { notImplemented(); return false; }

// ----------------------------------------------------------------------------
// WebChromeClient::GetDateAndTimeL
//
// ----------------------------------------------------------------------------
/* Max length for a TBuf date or time variable.
*/
const TInt KDateOrTimeMaxLength=30;

/* Date format for European style
*/
_LIT(KDateFormat,"%1%/1%2%/2%3\t");

/* 24 hour time format
*/
_LIT(KTimeFormat,"%J%:1%T%:2%S\t");
void WebChromeClient::GetDateAndTimeL(TDes& date, TDes& time) const
    {
    date.SetLength(0);
    time.SetLength(0);

    TTime t;
    t.HomeTime();
    t.FormatL(date, KDateFormat);
    t.FormatL(time, KTimeFormat);
    }

void WebChromeClient::addMessageToConsole(const WebCore::String& message, MessageLevel messageLevel, unsigned int line, const WebCore::String& sourceURL)
{
    TRAP_IGNORE(
    addMessageToConsoleL(message, messageLevel, line, sourceURL);
    );
}

void WebChromeClient::addMessageToConsoleL(const WebCore::String& message, MessageLevel messageLevel, unsigned int line, const WebCore::String& sourceURL)
{

    RFs fsSession;
    User::LeaveIfError( fsSession.Connect());

    TFileName privatePath;
    TPtrC concatPath;
    fsSession.PrivatePath(privatePath);
    TUint16 * privatePathData = (TUint16 * )privatePath.Ptr();
    TInt nextBackSlash = privatePath.Locate((TChar)'\\');
    // replace back slashes to forward slashes & change Private to private.
    for (TInt i = 0; i < privatePath.Length() ; i++ )
    {
        if (*(privatePathData + i) == (TChar)'\\')
            *(privatePathData + i) = (TChar)'/';
        if (*(privatePathData + i + 1) == (TChar)'P')
            *(privatePathData + i + 1) = (TChar)'p';
    }

    TPtrC srcURL = sourceURL.des();
    TInt p = srcURL.Find( privatePath );
    if ( p != KErrNotFound )
    {
        concatPath.Set(srcURL.Right(srcURL.Length() - p - privatePath.Length()));
    }
    else
    {
        concatPath.Set(srcURL);
    }

    TInt scriptLogType;
    CRepository* rep = CRepository::NewL( KCRUidBrowser );
    rep->Get( KBrowserNGScriptLog, scriptLogType );
    delete rep;

    if ( scriptLogType == TBrCtlDefs::EScriptLogDisable )
        return;
    TBool needLogFile = EFalse, needLogConsole = EFalse;

    // construct the log message
    HBufC* log_msg = HBufC::NewLC(KMaxJavaLogMsgPaddingLen + message.length() + srcURL.Length());
    TPtr log_msg_str = log_msg->Des();


    // log time stamp
    TBuf16<KDateOrTimeMaxLength> dateBuf;
    TBuf16<KDateOrTimeMaxLength> timeBuf;
    GetDateAndTimeL(dateBuf,timeBuf);
    log_msg_str.Append(dateBuf);
    log_msg_str.Append(timeBuf);

    // url and line no
    log_msg_str.Append(_L("\r\nURL: "));
    log_msg_str.Append(concatPath);
    log_msg_str.Append(_L(" Line: "));
    log_msg_str.AppendNum(line);


    // log type and log message
    log_msg_str.Append(_L("\r\n"));
    switch ( messageLevel )
        {
    case ErrorMessageLevel:
        log_msg_str.Append(_L("Error: "));
        break;
    case InfoMessageLevel:
        log_msg_str.Append(_L("Info: "));
        break;
    case WarningMessageLevel:
        log_msg_str.Append(_L("Warning: "));
        break;
    case LogMessageLevel:
        log_msg_str.Append(_L("Log: "));
        break;
    case DebugMessageLevel:
        log_msg_str.Append(_L("Debug: "));
        break;
    case AssertMessageLevel:
        log_msg_str.Append(_L("Assert: "));
        break;
        }
    log_msg_str.Append(message);
    log_msg_str.Append(_L("\r\n"));

    switch( scriptLogType )
    {
    case TBrCtlDefs::EScriptLogToFile:
        needLogFile = ETrue;
        break;
    case TBrCtlDefs::EScriptLogToConsole:
        needLogConsole = ETrue;
        break;
    case TBrCtlDefs::EScriptLogToConsoleFile:
        needLogFile = ETrue;
        needLogConsole = ETrue;
        break;
    }

    // log to the log file
    if ( needLogFile )
    {
        User::LeaveIfError( fsSession.SetSessionPath( KScriptLogDir()) );
        HBufC8 * tmpBuf = HBufC8::NewLC( log_msg->Length()*2 );
        tmpBuf->Des().Copy( log_msg_str );
        RFile scriptLogFile;

        if ( KErrNone != scriptLogFile.Open(fsSession,
                                    m_webView->widgetExtension()?  KWidgetScriptLogFile(): KWebScriptLogFile(),
                                    EFileShareExclusive|EFileWrite))
        {
            User::LeaveIfError(scriptLogFile.Create(fsSession,
                                            m_webView->widgetExtension()?  KWidgetScriptLogFile(): KWebScriptLogFile(),
                                            EFileShareExclusive|EFileWrite));
        }
        TInt fileSize;
        scriptLogFile.Size(fileSize);

        // roll over if the size of log file reaches the limit
        if ( fileSize + tmpBuf->Length() > KMaxJSLogFileSize )
            scriptLogFile.SetSize( 0 )
            ;
        scriptLogFile.Write(fileSize, tmpBuf->Des() );
        scriptLogFile.Close();
        CleanupStack::PopAndDestroy( tmpBuf ); // tmpBuf
    }

    // show log as pop up dialog
    if ( needLogConsole )
    {
        HBufC* ok_button = StringLoader::LoadLC( R_WML_OK_BUTTON );
        HBufC* cancel_button = StringLoader::LoadLC( R_WML_CANCEL_BUTTON );
        HBufC* title1 = StringLoader::LoadLC( R_QTN_BROWSER_DIALOG_SCRIPTLOG_T1 );
        HBufC* diable_button = StringLoader::LoadLC( R_QTN_BROWSER_DIALOG_SCRIPTLOG_SK2 );
        HBufC* title2 = StringLoader::LoadLC( R_QTN_BROWSER_DIALOG_SCRIPTLOG_T2 );
        HBufC* disable_msg = StringLoader::LoadLC( R_QTN_BROWSER_DIALOG_SCRIPTLOG_DISABLE_MSG );

        if (!m_webView->brCtl()->brCtlDialogsProvider()->DialogConfirmL(*title1, log_msg_str, *ok_button, *diable_button ) )
        {
            if (m_webView->brCtl()->brCtlDialogsProvider()->DialogConfirmL( *title2, *disable_msg, *ok_button, *cancel_button ) )
            {
                m_webView->brCtl()->SetBrowserSettingL( TBrCtlDefs::ESettingsScriptLog, TBrCtlDefs::EScriptLogDisable );
                m_webView->brCtl()->SetScriptLogMode( TBrCtlDefs::EScriptLogDisable );
            }
        }
        CleanupStack::PopAndDestroy( 6 );   //ok_button, cancel_button, title1, diable_button, title2, disable_msg
    }

    CleanupStack::PopAndDestroy( log_msg ); // log_msg
    fsSession.Close();
}

bool WebChromeClient::runBeforeUnloadConfirmPanel(const WebCore::String&, WebCore::Frame*) { notImplemented(); return false; }

void WebChromeClient::runJavaScriptAlert(Frame* frame, const String& message)
{
    TRAP_IGNORE(
    HBufC* alertHeader = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_ALERT);
    control(frame)->brCtlDialogsProvider()->DialogAlertL(*alertHeader, message.des()) ;
    CleanupStack::PopAndDestroy();
    );

}

bool WebChromeClient::runJavaScriptConfirm(Frame* frame, const String& message)
{
    bool confirmed = false;

    TRAP_IGNORE(
    HBufC* confirmHeader = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_CONFIRM);
    HBufC* leftkey = StringLoader::LoadLC(R_TEXT_SOFTKEY_YES);
    HBufC* rightkey = StringLoader::LoadLC(R_TEXT_SOFTKEY_NO);
    confirmed = control(frame)->brCtlDialogsProvider()->DialogConfirmL(*confirmHeader, message.des(), *leftkey, *rightkey ) ;
    CleanupStack::PopAndDestroy(rightkey);
    CleanupStack::PopAndDestroy(leftkey);
    CleanupStack::PopAndDestroy(confirmHeader);
    );
    return confirmed;
}

bool WebChromeClient::runJavaScriptPrompt(Frame* frame, const String& message, const String& defaultValue, String& result)
{
    bool confirmed = false;
    TRAP_IGNORE(
    HBufC* returningText = NULL;
    HBufC* promptHeader = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_PROMPT);
    confirmed = control(frame)->brCtlDialogsProvider()->DialogPromptLC(*promptHeader, message.des(), defaultValue.des(), returningText);
    if (returningText) {
        result = *returningText;
        CleanupStack::Pop(); // returningText
    } else
        CleanupStack::PopAndDestroy(); // returningText
    //
    CleanupStack::PopAndDestroy(); // promptHeader
    );
    return confirmed;
}

void WebChromeClient::setStatusbarText(const String&){}
bool WebChromeClient::tabsToLinks() const { return false;}
IntRect WebChromeClient::windowResizerRect() const{ return IntRect();}
void WebChromeClient::addToDirtyRegion(const IntRect&){}
void WebChromeClient::scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect){}

void WebChromeClient::focusedElementChanged(Element* element)
{
    m_webView->focusedElementChanged(element);
}

void WebChromeClient::setElementVisibilityChanged(bool visibility) {
    m_visibility = visibility;
    m_visibilityByMouse = m_visibility && m_webView->isMouseEventFired();
    m_visibilityByKey = m_visibility && m_webView->isKeyEventFired();
}
