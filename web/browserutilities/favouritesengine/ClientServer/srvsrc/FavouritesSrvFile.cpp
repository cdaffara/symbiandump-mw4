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
*      Implementation of class CFavouritesSrvFile
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvFile.h"
#include "FavouritesSrvDb.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSrvFile::NewL
// ---------------------------------------------------------
//
CFavouritesSrvFile* CFavouritesSrvFile::NewL()
    {
    CFavouritesSrvFile* file = new (ELeave) CFavouritesSrvFile();
    CleanupStack::PushL( file );
    file->ConstructL();
    CleanupStack::Pop( file );
    return file;
    }

void CFavouritesSrvFile::SetName(TParse aParse)
	{
	iFileName = aParse;
	}
// ---------------------------------------------------------
// CFavouritesSrvFile::OpenL
// ---------------------------------------------------------
//
void CFavouritesSrvFile::OpenL()
    {
    User::LeaveIfError( iFile.Open
        ( iFs, iFileName.FullName(), EFileStream | EFileShareReadersOnly | EFileRead ) );
    }

// ---------------------------------------------------------
// CFavouritesSrvFile::ReplaceL
// ---------------------------------------------------------
//
void CFavouritesSrvFile::ReplaceL()
    {
    User::LeaveIfError( iFile.Replace
        ( iFs, iFileName.FullName(), 
        EFileStream | EFileShareExclusive | EFileWrite ) );
    }

// ---------------------------------------------------------
// CFavouritesSrvFile::TransferToClientL
// ---------------------------------------------------------
//
void CFavouritesSrvFile::TransferToClientL( const RMessage2& aMessage )
    {
    User::LeaveIfError( iFile.TransferToClient( aMessage, 1 ) );
    iFile.Close();
    }
// ---------------------------------------------------------
// CFavouritesSrvFile::ConstructL
// ---------------------------------------------------------
//
void CFavouritesSrvFile::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }


//  End of File  
