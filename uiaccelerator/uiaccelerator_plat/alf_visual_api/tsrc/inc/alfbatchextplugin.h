/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Batch extension plugin.
*
*/


#ifndef T_ALFBATCHEXTPLUGIN_H
#define T_ALFBATCHEXTPLUGIN_H

#include <alf/alfextensionfactory.h>

class TAlfBatchExtPlugin : public MAlfExtensionFactory
    {
public:
    MAlfExtension* CreateExtensionL(
        const TInt aObjectId, 
        const TDesC8& aInitialParams, 
        MAlfInterfaceProvider& aResolver );
    void Release();
    };

#endif // T_ALFBATCHEXTPLUGIN_H
