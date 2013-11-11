/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class RFeatMgr.   
*      
*
*/


#ifndef FEAT_MGR_H
#define FEAT_MGR_H

// INCLUDE FILES

#include <e32base.h>

// CLASS DECLARATION

/**
* Feature manager wrapper. Provides leave-safe Feature Manager handling.
*/
NONSHARABLE_CLASS( RFeatMgr )
    {
    public:     // Construct / destruct

        /**
        * Constructor.
        */      
        RFeatMgr();

        /**
        * Close object. Safe to call if not open.
        */
        void Close();

        /**
        * Open object.
        */
        void OpenL();

    public:     // new methods

        /**
        * Query support for feature.
        * @param aFeature Feature id.
        * @return ETrue if feature is supported.
        */
        TBool FeatureSupported( TInt aFeature );

    private:    // data

        TBool iOpen;    ///< ETrue if open.
    };

#endif /* def TIMEOUT_H */
