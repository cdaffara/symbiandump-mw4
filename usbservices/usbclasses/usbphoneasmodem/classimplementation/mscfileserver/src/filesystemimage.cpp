// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: CFileSystemImage implementation
// 
// 

#include "filesystemimage.h"
#include "debug.h"

const TInt KImageFileMode = EFileShareReadersOnly | EFileRead;

CFileSystemImage::CFileSystemImage() :
    iIsOpened( EFalse )
	{
	// No implementation required
	}

CFileSystemImage::~CFileSystemImage()
	{
	// iFile will be closed with the session if it's still open
	iFs.Close();
	delete iFileName;
	}

CFileSystemImage* CFileSystemImage::NewL( const TDesC& aFileName )
	{
	CFileSystemImage* self = new (ELeave) CFileSystemImage();
	CleanupStack::PushL( self );
	self->ConstructL( aFileName );
	CleanupStack::Pop( self );
	return self;
	}

void CFileSystemImage::ConstructL( const TDesC& aFileName )
	{
	TRACE_FUNC
	User::LeaveIfError( iFs.Connect() );
	iFileName = aFileName.AllocL();
	TRACE_INFO(( _L( "Image file is '%S'" ), iFileName ))
	}

TInt CFileSystemImage::Open()
	{
	TRACE_FUNC
	if ( iIsOpened )
	    {
	    TRACE_INFO(( _L( "Image already opened" ) ))
	    return KErrNone;
	    }
	TInt err = iFile.Open( iFs, *iFileName, KImageFileMode );
	if ( err == KErrNone )
	    {
	    iIsOpened = ETrue;
	    }
	TRACE_INFO(( _L( "Image opened with error %d" ), err ))
	return err;
	}

TInt CFileSystemImage::Close()
	{
	if ( iIsOpened )
	    {
	    iFile.Close();
	    iIsOpened = EFalse;
	    }
	return KErrNone;
	}

TInt CFileSystemImage::Read( const TInt64& aPos, TInt aLength, TDes8& aBuf )
	{
	return iFile.Read( aPos, aBuf, aLength );
	}

TInt CFileSystemImage::Write( const TInt64& /*aPos*/, TDesC8& /*aBuf*/ )
	{
	return KErrAccessDenied;
	}

TInt64 CFileSystemImage::Size()
	{
	TInt size = 0;
	iFile.Size( size );
	return size;
	}
