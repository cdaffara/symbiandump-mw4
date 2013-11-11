/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   factory for alf built-in hitchcock compenents
*
*/



#ifndef __ALFSTATICFACTORY__
#define __ALFSTATICFACTORY__

#include <alf/alfextensionfactory.h> 

NONSHARABLE_CLASS(TAlfFixedFactory):public MAlfExtensionFactory
    {
    MAlfExtension* CreateExtensionL(
            const TInt aObjectId, 
            const TDesC8& aInitialParams, 
            MAlfInterfaceProvider& aResolver );
    
    void Release();
    };

#endif

