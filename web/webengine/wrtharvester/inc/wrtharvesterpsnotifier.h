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
* Description:  Publish&Subscribe event listener 
*
*/
  

#ifndef C_WRTHARVESTERNOTIFIER_H
#define C_WRTHARVESTERNOTIFIER_H

#include <e32base.h>
#include <e32property.h>
class CWrtHarvester;

// Content Harvester server secure id
const TUid KPropertyCat = { 0x10282E5A };
// Can not conflict with other Content Harvester plugin keys
enum TPropertyKeys { EWidgetUIState = 109, EWidgetRegAltered, EWidgetMMCAltered, EWidgetMassStorageMode };

/**
 * Publish&Subscribe event listener
 *
 * @since S60 v5.0
 */
class CWrtHarvesterPSNotifier : public CActive
    {    
    public:
        /**
         * Creates an instance of CWrtHarvesterPSNotifier implementation.
         *
         * @since S60 v5.0
         * @param aCallBack Reference to notifier interface.
         */
        static CWrtHarvesterPSNotifier* NewL( CWrtHarvester* aHarvester, TPropertyKeys aKey );
        
        /**
         * Destructor.
         */
        ~CWrtHarvesterPSNotifier();
        
        /**
         * Gets key value
         *
         * @since S60 v5.0
         */
        TInt GetValue( TInt& aValue );
        
        /**
         * Sets key value
         *
         * @since S60 v5.0
         */
        TInt SetValue( TInt aValue );
        
    private:
    
        /**
        * 
        * @param aHarvester
        * @param aKey
        */
        CWrtHarvesterPSNotifier( CWrtHarvester* aHarvester, TPropertyKeys aKey );
        
        /**
        * 
        */
        void ConstructL();
        
        /**
        * from CActive
        */
        void RunL();

        /**
        * from CActive
        */
        void DoCancel();
        
        /**
        * from CActive
        */
        TInt RunError(TInt aError);
        
    private:
        /**
         * User side interface to Publish & Subscribe.
         */
        RProperty iProperty;
        
        /**
         * Publish & Subscribe key.
         */
        TPropertyKeys iKey;
        
        /**
         * not own
         */
        CWrtHarvester* iHarvester;
    };

#endif // C_WRTHARVESTERNOTIFIER_H

// End of File
