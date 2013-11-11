/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class MCodLoadObserver.   
*      
*
*/


#ifndef COD_LOAD_OBSERVER_H
#define COD_LOAD_OBSERVER_H

// FORWARD DECLARATION

class CRoapData;

// INCLUDES

#include <e32std.h>

// CLASS DECLARATION

/**
* Mixin class to observe COD handling (progress, result etc.)
*/
class MCodLoadObserver
    {
    public:     // COD handling progress.

        /**
        * Get download root path.
        * Leave with KErrCancel if query is cancelled.
        * @param aRootPath Root path returned here.
        */
        virtual void GetRootPathL( TDes& aRootPath ) = 0;

        /**
        * Starting load (fetch content or error-notify).
        * @param aStatusCode HTTP status code.
        * - 900 (Success) indicates start of content load.
        * - Other values indicate start of error report.
        */
        virtual void StartLoadL( TInt aStatusCode ) = 0;

        /**
        * Cancelling content load started. Not called if cancelling error
        * report.
        */
        virtual void StartCancelL() = 0;

        /**
        * Load progress update.
        * @param aFinalValue Final progress value.
        * @param aCurrentValue Current progress value.
        */
        virtual void ProgressL( TInt aFinalValue, TInt aCurrentValue ) = 0;

        /**
        * Processing ended.
        * Note that if processing does not include loading, this method can be
        * called without a preceding StartLoadL call.
        * @param aStatusCode HTTP status code (aError mapped to HTTP status
        * code).
        * @param aError Error code of result. (The Set(), Accept() or Reject()
        * request to CodEng will also complete with this value.)
        */
        virtual void Done( TInt aStatusCode, TInt aError ) = 0;

        /**
        * Set Connection Error
        * @param aError Connection Error .
        */
		virtual void SetConnError( TInt aError) = 0;

        /**
        * Connection Error
        * return: Connection Error .
        */
		virtual TInt ConnError() = 0;

        /**
        * Progresive download is available
		*/
		virtual void PdPlayAvailable() = 0;
		
		
        /**
        * Set ActiveDownload is available
		*/
		virtual void SetActiveDownload( ) = 0;


    public:     // HTTP & OTA related progress.

        /**
        * Get confirmation for creating network connection.
        * @return ETrue if connection can be created.
        */
        virtual TBool ConfirmConnectL() = 0;

        /**
        * Opening a connection.
        * @param aIap AP to be used for creating the connection.
        */
        virtual void StartConnect( TUint32 aIap ) = 0;

        /**
        * Connected.
        */
        virtual void EndConnect() = 0;

        /**
        * Authenticate user.
        * @param aHost Host.
        * @param aRealm Realm.
        * @param aProxyAuth ETrue if this is a proxy authentication.
        * @param aUsername Return username here.
        * @param aPassword Return password here.
        * @return ETrue if auth is OK, EFalse if cancelled.
        */
        virtual TBool UserAuthL
            (
            const TDesC& aHost,
            const TDesC& aRealm,
            TBool aProxyAuth,
            TDes& aUsername,
            TDes& aPassword
            ) = 0;

        /**
        * Displays Info message "Waiting for license" 
        */
        virtual void WaitForLicenseL() = 0;
            
    public:     // ROAP related.
            
        /**
        * ROAP Trigger parsed (success or error).
        * @param aData ROAP Trigger data.
        */
        virtual void RoapTriggerParsedL( const CRoapData& aData ) = 0;
        
    public:
            
        /**
        * To indicate the download is paused
        */
        virtual void DownloadPaused() = 0;
        
        /**
        * To indicate the download is resumed and ready for PD
        */
        virtual void DownloadResumedPdAvailable() = 0;
        
        /**
        * To update the server about whether or not download is pausable
        */
        virtual void UpdatePausable( TBool aPausable ) = 0;
        
        /**
        * To update the server about updated media info
        */
        virtual void UpdateMediaInfoL() = 0;
        
        /**
        * To update the server about updated track
        */
        virtual void UpdateTrackInfoL() = 0;
        
        /**
        * 
        * To handle PostResponseUrlL as part of metering response
        * @param aPrUrl: The PostResponseURL
        * @return void
        *  
        */
        virtual void HandleRoapPostResponseUrlL( const TDesC8& aPrUrl ) = 0;
 
        /**
        * To update the server about next media object being downloaded.
        */        
        virtual void MediaObjectNameChanged() = 0;
        
        /**
        * To update the server about content type.
        */        
        virtual void ContentTypeChanged() = 0;
    };

#endif /* def COD_LOAD_OBSERVER_H */
