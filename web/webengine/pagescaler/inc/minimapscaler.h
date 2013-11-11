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



#ifndef CMINIMAPSCALER_H
#define CMINIMAPSCALER_H

//  INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CBitmapScaler;
class CFbsBitmap;

// CLASS DECLARATION

class MMinimapScalerCallback
    {
    public:        
        virtual void ScalingCompletedL(CFbsBitmap& aResult, const TRect& aTargetRect) = 0;
    };


// CLASS DECLARATION

/**
*  
*  
*  @lib pagescaler.dll
*  @since 3.1
*/
class CMinimapScaler : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMinimapScaler* NewL(MMinimapScalerCallback& aCallback);
        
        /**
        * Destructor.
        */
        virtual ~CMinimapScaler();

    public: // New functions
        
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void StartScalingL(CFbsBitmap &aSource, const TRect& aTargetRect);
        

    private: // Functions from CActive
    
        void RunL();
        
        void DoCancel();
        

    private:

        /**
        * C++ constructor.
        */
        CMinimapScaler(MMinimapScalerCallback& aCallback);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void DeleteResultBitmap();

    private:    // Data
    
        MMinimapScalerCallback* iCallback;
        
        CFbsBitmap* iResultBitmap; // owned                
        CBitmapScaler* iScaler; // owned
        
        TRect iTargetRect;

    };

#endif      // CMINIMAPSCALER_H   
            
// End of File
