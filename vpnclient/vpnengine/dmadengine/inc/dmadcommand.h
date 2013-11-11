/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class definition of CDmAdCommand.
*
*/


#ifndef __DMADCOMMAND_H__
#define __DMADCOMMAND_H__

#include <e32base.h>

//------------------------------------------------------------------------------------------------
// CDmAdCommand
//------------------------------------------------------------------------------------------------
class CDmAdCommand : public CBase
    {
public:
    static CDmAdCommand* NewL(const TDesC8& aUri, const TInt aStatusRef, TBool aLeaf);
    static CDmAdCommand* NewLC(const TDesC8& aUri, const TInt aStatusRef, TBool aLeaf);
    ~CDmAdCommand();
    static void CleanupOperationDeleteCArrayPtr(TAny* aPtr);
    
    inline TInt StatusRef(void) const;
    inline TInt Status(void) const;
    inline TBool IsLeaf(void) const;

private:    
    void ConstructL(const TDesC8& aUri);
    CDmAdCommand(const TInt aStatusRef, TBool aLeaf);
    
private:    
    CDmAdCommand();
    void ConstructL(void);
    
private:
    TInt                    iStatusRef;
    TInt                    iStatus;
    TBool                   iLeaf;
    };

#include "dmadcommand.inl"

#endif
