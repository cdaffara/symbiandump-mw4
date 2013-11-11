/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
* ==============================================================================
*  Name        : cpspublisher.h
*  Part of     : WEBKIT / Browser View
*  Interface   : Implemetation of CCpsPublisher
*  Description :
*  Version     :

*/

#ifndef CPSPUBLISHER_H
#define CPSPUBLISHER_H

#include <e32base.h>
#include <liwcommon.h>

class CFbsBitmap;
class CLiwGenericParamList;
class CLiwServiceHandler;
class MLiwInterface;
class CLiwDefaultMap;


class CCpsPublisher : public CBase,
    private MLiwNotifyCallback 
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CCpsPublisher* NewL();

        /**
        * Destructor.
        */
        virtual ~CCpsPublisher();
        
    private:
        CCpsPublisher();
        void ConstructL();
    private: 
    // Form base class MLiwNotifyCallback.
    /**
    * Callback from LIW framework to process an event that was expected.
    * @see MLiwNotifyCallback::HandleNotifyL
    */
    TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& aInParamList);
    
    public:
        TSize BitmapSize();
        void PublishBitmapL( CFbsBitmap& aBitmap, const TDesC& aBundleName );
        void NetworkConnectionCancelledL();
        void NetworkConnectionAllowedL();
        
    private:
        void GetBitmapSizeL();
        void InitCpsInterfaceL();

        void AddImageHandleL( const TDesC& aBundleId, const TInt& aHandle,
                const TInt& aMaskHandle, const TDesC8& aImageKey );
        void ExecuteCommandL(CLiwDefaultMap* aInFilter, CLiwDefaultMap* aOutDataMap,
                const TDesC16& aRegistry  );
        void ExecuteRegistrationCommandL( TUint aOption );
    private:
    
    CFbsBitmap* iMaskBitmap;
    MLiwInterface* iCpsInterface;
    CLiwServiceHandler* iServiceHandler;
    TSize iSize;

    };

#endif
