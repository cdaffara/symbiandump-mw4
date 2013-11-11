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
// Description: CFileSystemImage declaration
// 
// 

#ifndef FILESYSTEMIMAGE_H
#define FILESYSTEMIMAGE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION

/**
 *  CFileSystemImage
 * 
 */
class CFileSystemImage : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Two-phased constructor.
	 */
	static CFileSystemImage* NewL( const TDesC& aFileName );

	/**
	 * Destructor.
	 */
	~CFileSystemImage();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CFileSystemImage();

	/**
	 * Default constructor for performing 2nd stage construction
	 */
	void ConstructL( const TDesC& aFileName );

public:
	TInt Open();
	TInt Close();
	TInt Read( const TInt64& aPos, TInt aLength, TDes8& aBuf );
	TInt Write( const TInt64& aPos, TDesC8& aBuf );
	TInt64 Size();

private:
	RFs	  iFs;
	RFile iFile;
	HBufC* iFileName;
	TBool iIsOpened;
	};

#endif // FILESYSTEMIMAGE_H
