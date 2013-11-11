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

#ifndef __HTTPUICALLBACKS_H__
#define __HTTPUICALLBACKS_H__

#include <e32base.h>
#include <ApEngineConsts.h>
#include <mconnectioncallback.h>
#include <BrCtlDownloadObserver.h>
#include "HttpConnection.h"

class RHTTPTransaction;
class CBrCtl;

class HttpUiCallbacks : public MConnectionCallback, public MBrCtlDownloadObserver
{
public:

enum TEnterStatus
    {
    EEnteringSecurePage,
    EExitingSecurePage,
    ESomeItemsNotSecure,
    ESubmittingToNonSecurePage,
    ERedirectConfirmation,
    ERepostConfirmation,
    ESecureItemInNonSecurePage,
    EReEnteringSecurePage,
    // add new items here        
    EEnterStatusNone
    };

    HttpUiCallbacks();
    ~HttpUiCallbacks();
    static TInt handleErrorCb(TAny* aPtr);

public: // from MConnectionCallback
    /**
    * This function Creates connection if it doesn't exist. Returns status of created connection.
    * @param A reference to RConnection, typecasted as TInt
    * @param A reference to he socket server, typecasted as TInt
    * @param A reference to flag of a new connection
    * @param A reference to bearer type
    * @return TInt symbian wide error code
    **/
    TInt CreateConnection( TInt* aConnectionPtr, TInt* aSockSvrHandle,
                           TBool* aNewConn, TApBearerType* aBearerType );

    /**
    * This function cancel outstanding transactions and notify a user with the message "Out of coverage"
    **/
    void CoverageEvent( TInt aError );

	void reportError(int error);

	void reportHttpError(int error, const TDesC& aText );

public:
    int aboutToLoadPage(CBrCtl* brctl, int stateType);
    /**
    * Called when the authentication dialog box is needed.
    * @since 3.X
    * @param aHttpConn Transaction to be used for sending the response.
    * @param aUri The requset uri.
    * @param aUsername A username that was previously entered for this url.
    * @param aRealm The real requirering user authentication.
    * @param aProxy A flag if a server or a proxy is requiring authentication.
    * @param aPassword Password is valid only if stale is true
    * @return void
    */
    void AuthenticationRequest( HttpConnection* aHttpConn,
                                TPtr& aUri, TPtr& aUsername, TPtr& aRealm,
                                bool aProxy, 
                                bool aDataExists, TPtr& aPassword,
                                bool aBasicAuthentication = EFalse ); 

    void NextUriL(const TDesC8& aUri, CBrCtl* brCtl);

public: // MBrCtlDownloadObserver

        /**
        * Inform the host application that a new download has started using the Download Manager
        * @since 3.0
        * @param aTransactionID The ID of the transaction, it is unique as long as the transaction is on-going
        * @param aFileName Name of the file in which the downloaded content is stored
        * @param aContentType Type of content to be downloaded. For example:
        * Markup, Image, Cascading Style Sheet (CSS), Javascript, Netscape plug-in, Sound
        * @param aUrl The Url of the request to be done in the new window
        * @return ETrue if the file can be displayed or played while it is 
        * downloading (progressive download)
        * EFalse if the file cannot be displayed or played while it is downloading
        */
        virtual TBool NewDownloadL(TUint aTransactionID,
                                   const TDesC& aFileName,
                                   const TDesC& aContentType,
                                   const TDesC& aUrl) {return EFalse;}
                                   
        /**
        * Tells the host application to resume an incomplete download. 
        * After the host application restarts, this method is called 
        * for each file whose download was interrupted when the host application closed.
        * @since 3.0
        * @param aTransactionID ID of the transaction
        * This ID must be unique while the transaction is in progress.
        * @param aLength Length of the content previously downloaded
        * @param aFileName Name of the file in which the downloaded content is stored
        * @param aContentType Type of content downloaded. For example:
        * Markup, Image, Cascading Style Sheet (CSS), Javascript, Netscape plug-in, Sound 
        * @param aUrl URL of the source of the content to be done in the new window
        * @return None
        */
        virtual void ResumeDownloadL(TUint aTransactionID,
                                   TUint aLength,
                                   const TDesC& aFileName,
                                   const TDesC& aContentType,
                                   const TDesC& aUrl) {}

        /**
        * Informs the host application that one of the following download events is in progress:
        * NOTE: All events have the prefix EDownload:
        * EventStarted, EventCompleted, EventProgress, EventCanceled, EventError 
        * EventPaused, EventResumed, EventPausable
        * @since 3.0
        * @param aTransactionID The ID of the transaction, it is unique as long 
        * as the transaction is on-going
        * @param aDownloadEvent Event to be handled Examples:
        * EventStarted, EventCompleted, EventProgress, EventCanceled, EventError 
        * EventPaused, EventResumed, EventPausable
        * @param aValue Value associated with the event. Examples:
        * Total size of the downloaded file
        * Size of that was downloaded so far
        * @return void
        */
        virtual void HandleDownloadEventL(TUint aTransactionID, 
                                          TBrCtlDownloadEvent aDownloadEvent,
                                          TUint aValue) {}     
        void SetBrowserControl(CBrCtl* aBrctl)                                                  ;

private:
    void handleError();

    void GetUserAuthenticationDataL(HttpConnection* aTrans,
                                    const TUint16* aUri,
                                    const TUint16* aUsername,
                                    const TUint16* aRealm, 
                                    TUint16** aRetUsername,
                                    TUint16** aRetPassword,
                                    bool aBasicAuthentication);

    int aboutToLoadPageL(CBrCtl* brctl, int stateType);

private: // data
    CPeriodic* m_scheduler;
    int m_error;
    CBrCtl* m_brctl;

};


#endif // __HTTPUICALLBACKS_H__
// end of file
