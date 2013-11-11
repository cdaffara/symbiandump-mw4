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
* Description:  
*
*/

#include "HttpUiCallbacks.h"
#include "Page.h"
#include <../bidi.h>
#include <StringLoader.h>
#include <uri16.h>
#include <es_enum.h>
#include <Webkit.rsg>
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "BrCtl.h"
#include "SettingsContainer.h"
#include "BrCtlSpecialLoadObserver.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HttpDefs.h"
#include "HttpConnection.h"
#include "cdbcols.h"

using namespace WebCore;

// CONSTANTS
_LIT (KEmptyTitle, "");

static CBrCtl* brctl()
{
    return StaticObjectsContainer::instance()->brctl();
}

HttpUiCallbacks::HttpUiCallbacks()
{
    m_error = KErrNone;
    m_scheduler = NULL;
    m_brctl = NULL;
}

HttpUiCallbacks::~HttpUiCallbacks()
{
    if (m_scheduler) { 
        m_scheduler->Cancel();
        delete m_scheduler;
    }
    m_brctl = NULL;
}

void HttpUiCallbacks::SetBrowserControl(CBrCtl* aBrctl)
{
    m_brctl = aBrctl;        
}

TInt HttpUiCallbacks::handleErrorCb(TAny* aPtr)
{
    HttpUiCallbacks* self = static_cast<HttpUiCallbacks*>(aPtr);
    self->handleError();
    return KErrNone;
}

void HttpUiCallbacks::handleError()
{
    m_scheduler->Cancel();
    delete m_scheduler;
    m_scheduler = NULL;
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->handleError(m_error);
}


TInt HttpUiCallbacks::CreateConnection(TInt* aConnectionPtr, TInt* aSockSvrHandle,
                                       TBool* aNewConn, TApBearerType* aBearerType)
{
    TInt error = KErrNone;
    if( aConnectionPtr && aSockSvrHandle && aNewConn ){
        if(m_brctl && m_brctl->webView()->widgetExtension()){
            TRAP(error, m_brctl->brCtlSpecialLoadObserver()->NetworkConnectionNeededL(aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType));        
            }
        else 
            {
            TRAP(error, brctl()->brCtlSpecialLoadObserver()->NetworkConnectionNeededL(aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType));        
            }     
 
        if( error == KErrNone && *aConnectionPtr ) {
            RConnection* connPtr = REINTERPRET_CAST( RConnection*, *aConnectionPtr );
            TConnectionInfoBuf connInfoBuf;
            TUint conns( 0 );
            connPtr->EnumerateConnections( conns );
            if (conns > 0  && connPtr->GetConnectionInfo(1, connInfoBuf) == KErrNone )  
            {  
                TUint32 iapId;  
                TBuf<20> query;  
                
                //Get the IAP id used by browser from the RConnection object.  
                query.Format( _L("%s\\%s"), IAP, COMMDB_ID );  
                if( connPtr->GetIntSetting( query, iapId ) == KErrNone)  
                {  
                    // Set the access point Id to BrCrl  
                    brctl()->webView()->setAccessPointId( iapId );
                    
                }  
                //If IAP is not avilable from connection Object then use the first connection.  
                else  
                {  
                    brctl()->webView()->setAccessPointId( connInfoBuf().iIapId );
                }
            }
            //pass the connection to download manager        
            // let the dm know about the connection
            HttpDownload* httpDownload = StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->httpDownload();
            if(httpDownload)
                {
                TName name;
                connPtr->Name( name );
                httpDownload->connect( name );                    
                }
        }
  }
    // no connection, cancel transactions asynchronously
    if( error != KErrNone ) {
        m_error = error;
        if (m_scheduler == NULL) {
            m_scheduler = CPeriodic::NewL( CActive::EPriorityStandard );
            m_scheduler->Start( 0, 0, TCallBack( &handleErrorCb, this ) );
        }
    }
    return error;
}

void HttpUiCallbacks::CoverageEvent( TInt aError )
{
    TInt error = (aError == EErrNetUnreach) ? KErrNetUnreach : 
        ((aError == EErrWaitTimeout ) ? KBrowserWaitTimeout : KErrNone);
    TRAP_IGNORE(brctl()->brCtlDialogsProvider()->DialogNotifyErrorL(error));
    StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager()->handleError(error);
    // the download manager handles coverage event in the Dl Mgr Server (KShutDownTimer = 30 sec).  
}

int HttpUiCallbacks::aboutToLoadPage(CBrCtl* brctl, int stateType)
{
    int ret = KErrNone;
    TRAPD(err, ret = aboutToLoadPageL(brctl, stateType));
    return (err == KErrNone) ? ret : err;
}

int HttpUiCallbacks::aboutToLoadPageL(CBrCtl* brctl, int stateType)
{
    TBool result ( ETrue );
    TEnterStatus enterStatus ( (TEnterStatus)stateType );
    TInt resId ( 0 ); // don't display a dialog,
    // event = 0 means, don't send an event. NOTE: TBrCtlDefs::EEventNone = 0
    TBrCtlDefs::TBrCtlLoadEvent event ( TBrCtlDefs::EEventNone ) ;
    // Map TEnterStatus to a corresponding dialog string
  // resource IDs and browser load event
    switch (enterStatus)
    {
        case EExitingSecurePage:
            {
                resId =  R_HTTPSEC_LEAVE_SECURE_SITE;
                event = TBrCtlDefs::EEventExitingSecurePage;
                break;
            }
        case EEnteringSecurePage:
            {
                resId = R_HTTPSEC_ENTER_SECURE_SITE;
                event = TBrCtlDefs::EEventEnteringSecurePage;
                break;
            }
        case ESomeItemsNotSecure:
            {
                resId = R_HTTPSEC_SOME_ITEMS_NOT_SECURE;
                event = TBrCtlDefs::EEventSomeItemsNotSecure;
                break;
            }
        case ESubmittingToNonSecurePage:
            {
                resId = R_HTTPSEC_SUBMITTING_TO_NON_SECURE_PAGE;
                event = TBrCtlDefs::EEventExitingSecurePage;
                break;
            }
        case ERedirectConfirmation:
            {
                resId = R_HTTP_REDIRECT_POST_CONFIRMATION;
                event = TBrCtlDefs::EEventRedirectConfirmation;
                break;
            }
        case ERepostConfirmation:
            {
                resId = R_HTTP_REPOST_CONFIRMATION;
                event = TBrCtlDefs::EEventRepostConfirmation;
                break;
            }
        case ESecureItemInNonSecurePage:
            {
                resId = 0;
                event = TBrCtlDefs::EEventSecureItemInNonSecurePage;
                break;
            }
        case EReEnteringSecurePage:
            {
                resId = 0;
                event = TBrCtlDefs::EEventEnteringSecurePage;
                break;
            }
        default:
            {
                resId = 0;
                event = TBrCtlDefs::EEventNone;
                break;
            }
    }   // end of switch
    TUint retVal = brctl->settings()->brctlSetting(TBrCtlDefs::ESettingsSecurityWarnings);
    // Show dialogs only if warning dialogs are enabled in the preferences, and
    if (retVal && resId)
    {
        HBufC* message = StringLoader::LoadLC(resId);
        HBufC* leftMessage = NULL;
        HBufC* rightMessage = NULL;
        leftMessage = StringLoader::LoadLC(R_TEXT_SOFTKEY_YES);
        rightMessage = StringLoader::LoadLC(R_TEXT_SOFTKEY_NO);
        MBrCtlDialogsProvider* dialogsProvider = brctl->brCtlDialogsProvider();
        TRAPD(err, result = dialogsProvider->DialogConfirmL(KEmptyTitle,
            *message,
            *leftMessage,
            *rightMessage));
        // Clean up the memory we allocated, before returning
        CleanupStack::PopAndDestroy(3); // message, leftMessage, rightMessage
        // Check to see if DialogConfirmL leaves
        if (err != KErrNone)
        {
            return KErrNoMemory;
        }
        // If the user selected "No" or "Cancel", we are cancelling the load
        if ( !result )
        {
            brctl->HandleBrowserLoadEventL(TBrCtlDefs::EEventContentFinished,0,0);
            return KErrCancel;
        }
    }   // end of if
    if(result && event != TBrCtlDefs::EEventNone)
        // Send the BrowserLoadEvent to the registered listeners
    {
        brctl->HandleBrowserLoadEventL(event, 0, 0);
    }
    
    // The user selected "Yes" or "Continue" or we aren't displaying a dialog
    return KErrNone;
}

// -----------------------------------------------------------------------------
// HttpUiCallbacks::AuthenticationNeeded
// Public Class Method
// UI Call back to inform authentication is required
// -----------------------------------------------------------------------------
//
void HttpUiCallbacks::AuthenticationRequest(
    HttpConnection* aHttpConn,
    TPtr& aUri,
    TPtr& aUsername,
    TPtr& aRealm,
    bool aProxy,
    bool aDataExists,
    TPtr& aPassword,
    bool aBasicAuthentication )
    {
    TPtr usernamePtr(NULL, 0);
    TPtr passwordPtr(NULL, 0);
    TUint16* retUsername = NULL;
    TUint16* retPassword = NULL;
    int status( KErrNone );
    int error( KErrNone );
    // Check if authentication data exists
    // If Yes:
    if( !aDataExists )
        {
    //Get user authentication data
       TRAP(status, GetUserAuthenticationDataL(aHttpConn, aUri.Ptr(),
            aUsername.Ptr(), aRealm.Ptr(), &retUsername,
            &retPassword, aBasicAuthentication));
        switch (status)
            {
            case KErrNone:
                {
                if (retUsername )
                    {
                    usernamePtr.Set(retUsername, User::StringLength(retUsername),
                                    User::StringLength(retUsername));
                    }
                if (retPassword != NULL)
                    {
                    passwordPtr.Set(retPassword, User::StringLength(retPassword),
                                    User::StringLength(retPassword));
                    }
                break;
            }
            case KErrCancel:
                {
                error = KErrCancel;
                break;
                }
            case KErrNoMemory:
                {
                error = KErrNoMemory;
                break;
                }
            default:
                {
                error = KErrGeneral;
                break;
                }
            }
        }
    else
    // if aDataExists take the existing  uname/pwd again, because
    // they were ok, but nonce was expired.
    // Make a copy of the original uname/pwd.
        {
        retPassword = new TUint16 [aPassword.Length() + 1];
        if ( retPassword )
            {
            passwordPtr.Set(retPassword, aPassword.Length() + 1, aPassword.Length() + 1);
            passwordPtr.Copy(aPassword);
            passwordPtr.ZeroTerminate();
            retUsername = new TUint16 [aUsername.Length() + 1];
            if ( retUsername )
                {
                usernamePtr.Set(retUsername, aUsername.Length() + 1, aUsername.Length() + 1);
                usernamePtr.Copy(aUsername);
                usernamePtr.ZeroTerminate();
                }
            else
                {
                passwordPtr.Set( NULL, 0, 0 );
                delete [] retPassword;
                retPassword = NULL;
                error = KErrNoMemory;
                }
            }
        else
            {
            error = KErrNoMemory;
            }
        }
    // send the credentials
    aHttpConn->AuthenticationResponse( usernamePtr, passwordPtr, aProxy, error);
    }

// -----------------------------------------------------------------------------
// HttpUiCallbacks::NextUriL
// Private Class Method
// -----------------------------------------------------------------------------
void HttpUiCallbacks::NextUriL(const TDesC8& aUri, CBrCtl* brCtl)
{
    HBufC16* urlDes = HBufC16::NewLC(aUri.Length());
    urlDes->Des().Copy(aUri);
    if (brCtl)
        brCtl->LoadUrlL (*urlDes, -1, TBrCtlDefs::ECacheModeNormal);
    CleanupStack::Pop(urlDes);
}

// -----------------------------------------------------------------------------
// HttpUiCallbacks::GetUserAuthenticationDataL
// Private Class Method
// -----------------------------------------------------------------------------
void HttpUiCallbacks::GetUserAuthenticationDataL(
    HttpConnection* aHttpConn,
    const TUint16* aUri,
    const TUint16* aUsername,
    const TUint16* aRealm,
    TUint16** aRetUsername,
    TUint16** aRetPassword,
    bool aBasicAuthentication)
    {
    TBool bRet( EFalse );
    HBufC* returnedUserName = NULL;
    HBufC* returnedPasswd = NULL;
    // Initialize our return values
    *aRetUsername = NULL;
    *aRetPassword = NULL;
    TInt needPop ( 0 );
    
    CBrCtl* brCtl = control(aHttpConn->frame());
    MBrCtlDialogsProvider* dialogsProvider = brCtl->brCtlDialogsProvider();

    // Display the User Authentication Dialog
    bRet = dialogsProvider->DialogUserAuthenticationLC(TPtrC(aUri),
                                                       TPtrC(aRealm),
                                                       TPtrC(aUsername),
                                                       returnedUserName,
                                                       returnedPasswd,
                                                       aBasicAuthentication);

    TUint retVal ( 0 );
    TRAP_IGNORE( retVal = brCtl->BrowserSettingL(TBrCtlDefs::ESettingsSecurityWarnings));

    TUriParser uriParser;
    TBool secureScheme(EFalse);
    if(uriParser.Parse( TPtrC(aUri) ) == KErrNone)
        {
        if( uriParser.Extract( EUriScheme ).FindF( _L("https") ) == 0 )
            {
            secureScheme = ETrue;
            }
        }

    // Display the Basic Authentication dialog, if needed.
    if (bRet && aBasicAuthentication && retVal && !secureScheme)
        {
        // Our memory pointers for the basic authentication dialog
        HBufC* message = NULL;
        HBufC* okMsg = NULL;
        HBufC* cancelMsg = NULL;
        // Set up the message text
        message = StringLoader::LoadLC(R_BROWSER_QUERY_BASIC_AUTHENTICATION);
        // Set up the OK button text
        okMsg = StringLoader::LoadLC(R_WML_OK_BUTTON);
        // Set up the CANCEL button text
        cancelMsg = StringLoader::LoadLC(R_WML_CANCEL_BUTTON);
        // Display the basic authentication dialog
        if (message && okMsg && cancelMsg)
            {
            bRet = dialogsProvider->DialogConfirmL(KEmptyTitle, *message, *okMsg, *cancelMsg);
            }
        // Clean up the basic authentication dialog memory
        CleanupStack::PopAndDestroy(3); // message, okMsg, cancelMsg
        }   // end of if (bRet && basic authentication && display security warnings)
    // The user selected "yes" or "continue", from the dialog(s).
    // Return the username and password.
    if (bRet)
        {
        // Set up the returned user name
        if (!returnedUserName)
            {
            *aRetUsername = NULL;
            }
        else
            {
            *aRetUsername = new(ELeave) TUint16[returnedUserName->Length() + 1];
            CleanupStack::PushL(*aRetUsername);
            needPop ++;

            TPtr usernamePtr(*aRetUsername, returnedUserName->Length() + 1);
            usernamePtr.Copy(*returnedUserName);
            usernamePtr.ZeroTerminate();
            }

        // Set up the returned password
        if (!returnedPasswd)
            {
            *aRetPassword = NULL;
            }
        else
            {
            *aRetPassword = new(ELeave) TUint16[returnedPasswd->Length() + 1];
            TPtr passwordPtr(*aRetPassword, returnedPasswd->Length() + 1);
            passwordPtr.Copy(*returnedPasswd);
            passwordPtr.ZeroTerminate();
            }
        }
    else
        {
        TRAP_IGNORE(brCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventAuthenticationFailed, 0, 0));
        }
    CleanupStack::Pop(needPop); // *aRetUsername

    // Clean up the memory from User Authentication dialog
    if (returnedUserName)
        {
        CleanupStack::PopAndDestroy(); // returnedUserName
        }
    if (returnedPasswd)
        {
        CleanupStack::PopAndDestroy(); // returnedPasswd
        }
    if (!bRet)
        {
        User::Leave(KErrCancel);
        }
    }

// -----------------------------------------------------------------------------
// HttpUiCallbacks::reportError
// -----------------------------------------------------------------------------
void HttpUiCallbacks::reportError(int error)
{
    MBrCtlDialogsProvider* dialogsProvider = brctl()->brCtlDialogsProvider();
    if ( dialogsProvider )
    {
        TRAP_IGNORE(dialogsProvider->DialogNotifyErrorL(error));
    }
}

void HttpUiCallbacks::reportHttpError(int error, const TDesC& aText )
{
    MBrCtlDialogsProvider* dialogsProvider = brctl()->brCtlDialogsProvider();
    if ( dialogsProvider )
    {
        TRAP_IGNORE(dialogsProvider->DialogNotifyHttpErrorL( error, aText ) );
    }
}

// end of file
