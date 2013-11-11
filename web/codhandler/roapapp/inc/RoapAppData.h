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
*      Declaration of class CRoapAppData.   
*      
*
*/


#ifndef ROAP_APP_DATA_H
#define ROAP_APP_DATA_H

// INCLUDES

#include <e32base.h>
#include <RoapEng.h>

// CLASS DECLARATION

/**
* ROAP data container.
*/
class CRoapAppData: public CBase
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The created model.
        */
        static CRoapAppData* NewL();

        /**
        * Destructor.
        */
        virtual ~CRoapAppData();

    private:    // Constructors

        /**
        * Constructor.
        */
        CRoapAppData();

    public:     // data

        Roap::TTriggerType iType;                   ///< Trigger type.
        Roap::TRiContextStatus iContextStatus;      ///< Context status.
        Roap::TDomainOperation iDomainOperation;    ///< Domain operation.
        RPointerArray<HBufC8> iContentIdList;       ///< Content id list.
        HBufC8* iRiAlias;                           ///< RI alias or NULL. Own.
    };

#endif /* def ROAP_APP_DATA_H */
