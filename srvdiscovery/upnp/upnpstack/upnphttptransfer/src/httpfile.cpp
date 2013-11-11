/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Represents one downloadable file
*
*/


// User include files
#include "httpfile.h"

// Constants
_LIT(KUnderScore, "_");
const TUint KMaxSameFileNames = 1000;
const TUint KSpaceForNumber = 5; // underscore + 4 digits
const TUint KFinishFileCreationLoop = 1001;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpFile::CHttpFile()
// --------------------------------------------------------------------------
//
CHttpFile::CHttpFile( TAny* aKey )
    {
    iKey = aKey;
    iPath = NULL;
    }

// --------------------------------------------------------------------------
// CHttpFile::CHttpFile()
// --------------------------------------------------------------------------
//
CHttpFile::CHttpFile( TAny* aKey, const RFile& aFileHandle )
    {
    iKey = aKey;
    iPath = NULL;
    iFile.Duplicate( aFileHandle );
    }

// --------------------------------------------------------------------------
// CHttpFile::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::ConstructL( const TDesC8& aUri, const TDesC& aPath )
    {
    SetUriL( aUri );

    iPath = aPath.AllocL();
    
    User::LeaveIfError( iFsSession.Connect() );
    }

// --------------------------------------------------------------------------
// CHttpFile::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::ConstructL( const TDesC8& aUri )
    {
    SetUriL( aUri );
    }
// --------------------------------------------------------------------------
// CHttpFile::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpFile::NewL( TAny* aKey, 
                            const TDesC8& aUri, 
                            const TDesC& aPath )
    {
    CHttpFile* self = CHttpFile::NewLC( aKey, aUri, aPath );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpFile::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpFile::NewLC( TAny* aKey, 
                             const TDesC8& aUri, 
                             const TDesC& aPath )
    {
    CHttpFile* self = new( ELeave ) CHttpFile( aKey );
    CleanupStack::PushL( self );
    self->ConstructL( aUri, aPath );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpFile::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpFile::NewL( TAny* aKey, const TDesC8& aUri,
                            const RFile& aFileHandle )
    {
    CHttpFile* self = CHttpFile::NewLC( aKey, aUri, aFileHandle );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpFile::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpFile::NewLC( TAny* aKey, const TDesC8& aUri, 
                             const RFile& aFileHandle )
    {
    CHttpFile* self = new( ELeave ) CHttpFile( aKey, aFileHandle );
    CleanupStack::PushL( self );
    self->ConstructL( aUri );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpFile::~CHttpFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile::~CHttpFile()
    {
    iFile.Close();
    delete iUri;
    delete iPath;
    iHeaderArray.ResetAndDestroy();

    iFsSession.Close();
    }

// --------------------------------------------------------------------------
// CHttpFile::TrackProgress()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::TrackProgress( TBool aValue ) 
    {
    iTrackProgress = aValue;
    }

// --------------------------------------------------------------------------
// CHttpFile::TrackingOn()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TBool CHttpFile::TrackingOn() const
    {
    return iTrackProgress;    
    }

// --------------------------------------------------------------------------
// CHttpFile::Key()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TAny* CHttpFile::Key() const
    {
    return iKey;
    }

// --------------------------------------------------------------------------
// CHttpFile::Uri()
// (See comments in header file)
// --------------------------------------------------------------------------
//
const HBufC8* CHttpFile::Uri() const
    {
    return iUri;
    }

// --------------------------------------------------------------------------
// CHttpFile::SetUriL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::SetUriL( const TDesC8& aUri )
    {
    HBufC8* uri = aUri.AllocL();
    delete iUri;
    iUri = uri;
    }

// --------------------------------------------------------------------------
// CHttpFile::Path()
// (See comments in header file)
// --------------------------------------------------------------------------
//
const HBufC* CHttpFile::Path() const
    {
    return iPath;
    }

// --------------------------------------------------------------------------
// CHttpFile::FileHandle()
// (See comments in header file)
// --------------------------------------------------------------------------
//
RFile CHttpFile::FileHandle() const
    {
    return iFile;
    }

// --------------------------------------------------------------------------
// CHttpFile::SetHeaderL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::SetHeaderL( const TDesC8& aFieldName, 
                            const TDesC8& aFieldValue )
    {
    // if the fieldname already exists ( header already set ) -> leave
    for ( TInt i = 0; i < iHeaderArray.Count(); i++ )
        {
        if ( iHeaderArray[i]->FieldName().CompareF( aFieldName ) == 0 )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

     iHeaderArray.AppendL( CHttpHeader::NewL( aFieldName, aFieldValue ) );
    }

// --------------------------------------------------------------------------
// CHttpFile::Headers()
// (See comments in header file)
// --------------------------------------------------------------------------
//
RPointerArray<CHttpHeader>& CHttpFile::Headers()
    {
    return iHeaderArray;
    }

// --------------------------------------------------------------------------
// CHttpFile::CreateFileInFileSystem()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::CreateFileInFileSystemL()
    {
    TInt ret = KErrNone;

    if ( iPath )
        {
        // Check first that the drive exists
        TInt driveNum;
        TParse p;
		p.Set( *iPath, NULL, NULL ); 
		TPtrC pointer = p.Drive(); 
		TLex lineParser( pointer.Ptr() ); 
		TChar ch = lineParser.Get(); 
		
		User::LeaveIfError( iFsSession.CharToDrive( ch, driveNum) );

		TVolumeInfo volumeInfo;
        User::LeaveIfError( iFsSession.Volume( volumeInfo, driveNum ) );

        // Create the file in the file system
        ret = iFile.Create( iFsSession, *iPath, EFileWrite );

        // Directory not available => create one
        if( KErrPathNotFound == ret )
            {
            // Make directory
            // Creates the directory and ignores the filename
            iFsSession.MkDirAll( *iPath );
            ret = iFile.Create( iFsSession, *iPath, EFileWrite );
            }

        // File already exists
        // File has to be renamed to 
        // <oldfilename>_<number>.<old_file_extension>
        // number will increase if same file is loaded many times
        else if( KErrAlreadyExists  == ret )
            {
            TParse parseFilename;

            parseFilename.Set( *iPath, NULL, NULL );

            HBufC* name = NULL;

            for ( TInt count = 1; count < KMaxSameFileNames; count++ )
                {
                // KSpaceForNumber is meant for underscore and number
                name = HBufC::NewLC( iPath->Length() + 
                                     KSpaceForNumber );

                TBuf<KSpaceForNumber> value;
                value.Num( count );

                name->Des().Append( parseFilename.DriveAndPath() );
                name->Des().Append( parseFilename.Name() );    
                name->Des().Append( KUnderScore() );
                name->Des().Append( value );
                name->Des().Append( parseFilename.Ext() );

                ret = iFile.Create( iFsSession, *name, EFileWrite );

                // If the file already exists make a new loop
                if ( ret == KErrAlreadyExists )
                    {
                    CleanupStack::PopAndDestroy( name );
                    name = NULL;
                    }
                // if file creation succeeded
                else if ( ret == KErrNone )
                    {
                    delete iPath;
                    iPath = name;
                    CleanupStack::Pop( name );
                    
                    count = KFinishFileCreationLoop;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( name );
                    
                    count = KFinishFileCreationLoop;
                    }
                }
            }
        }

    if( ret != KErrNone ) 
        {
        User::Leave( ret );
        }

    }

// --------------------------------------------------------------------------
// CHttpFile::DeleteFileFromFileSystemL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::DeleteFileFromFileSystemL()
    {
    // Close filehandle before delete
    iFile.Close();

    // Delete the file
    if ( iPath )
        {
        CFileMan* fileMan = CFileMan::NewL( iFsSession );
        CleanupStack::PushL( fileMan );
        User::LeaveIfError( fileMan->Delete( iPath->Des() ) );
        CleanupStack::PopAndDestroy( fileMan );
        }
    }

// --------------------------------------------------------------------------
// CHttpFile::CloseFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpFile::CloseFile()
    {
    iFile.Close();
    }

// end of file
