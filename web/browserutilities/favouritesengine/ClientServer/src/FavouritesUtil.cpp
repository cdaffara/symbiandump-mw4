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
*      Implementation of class FavouritesUtil
*      
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "FavouritesUtil.h"

#include <e32property.h>
#include <connect/sbdefs.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// FavouritesUtil::ExternalizeL
// ---------------------------------------------------------
//
void FavouritesUtil::ExternalizeL
( const CArrayFix<TInt>& aArray, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aArray.Count() );
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        aStream.WriteInt32L( aArray[ i ] );
        }
    }

// ---------------------------------------------------------
// FavouritesUtil::InternalizeL
// ---------------------------------------------------------
//
void FavouritesUtil::InternalizeL
( CArrayFix<TInt>& aArray, RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        aArray.AppendL( aStream.ReadInt32L() );
        }
    }

// ---------------------------------------------------------
// FavouritesUtil::IsBackupInProgressL
// ---------------------------------------------------------
//
TBool FavouritesUtil::IsBackupInProgressL()
    {
    TInt backupRestoreStatus;
    TInt err = RProperty::Get(KUidSystemCategory, 
        conn::KUidBackupRestoreKey,
        backupRestoreStatus);

	if(err != KErrNone)
		{
		if(err == KErrNotFound)
			{
			return EFalse;
			}
	    else
			{
    		User::Leave(err);
			}
		}

    if( backupRestoreStatus == conn::EBURUnset || 
        backupRestoreStatus & conn::EBURNormal ||
        (backupRestoreStatus & conn::KBackupIncTypeMask) == conn::ENoBackup)
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

//  End of File  
