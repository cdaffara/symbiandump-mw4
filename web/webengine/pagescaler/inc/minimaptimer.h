/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMINIMAPTIMER_H
#define CMINIMAPTIMER_H

//  INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMinimap;

// DATA TYPES

typedef void (CMinimap::* TMinimapCb)();

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  
*  
*  @lib pagescaler.dll
*  @since 3.1
*/
class CMinimapTimer : public CActive
    {

    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMinimapTimer* NewL(CMinimap& aMinimap, TMinimapCb aCallback, 
            TInt aPriority=CActive::EPriorityStandard);
        
        /**
        * Destructor.
        */
        virtual ~CMinimapTimer();

    public: // New functions
                
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void Start(TTimeIntervalMicroSeconds32 aTime);        

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void StartOrContinue(TTimeIntervalMicroSeconds32 aTime);
        
        void StartOrContinue() { StartOrContinue(iTime); };
                
        
    private:

        /**
        * C++ constructor.
        */
        CMinimapTimer(CMinimap& aMinimap, TMinimapCb aCallback, TInt aPriority);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // from CActive
    
        void RunL();
        void DoCancel();


    private:    // Data
        
        CMinimap* iMinimap;
        TMinimapCb iCb;
        RTimer iTimer;
        TTimeIntervalMicroSeconds32 iTime;
    };

#endif      // CMINIMAPTIMER_H   
            
// End of File
