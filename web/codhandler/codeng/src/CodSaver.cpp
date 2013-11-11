/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class CCodSaver.   
*      
*
*/


// INCLUDE FILES

#include "CodSaver.h"
#include "CodPanic.h"
#include "CodLogger.h"
#include "CodError.h"
#include "FileExt.h"
#include "bautils.h"


// ================= CONSTANTS =======================

/// Response size upper bound multiplier. Used in SetMaxSize().
LOCAL_D const TReal KCodOversizeFactor = 1.4;
/// Response upper bound of response size (10k). Used in SetMaxSize().
LOCAL_D const TInt KCodMinUpperBound = 10240;

const TInt KMaxIndexStringLength = 16;   // max size of index string "(4294967296)"
_LIT( KIndexString, "(%d)" );
const TInt KDownloadNameMaxSize = 120;
#define MIN( a, b ) ( (a) < (b) ? (a) : (b) )


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodSaver::ConstructL()
// ---------------------------------------------------------
//
void CCodSaver::ConstructL()
    {
    iSourceUri = HBufC8::NewL( 0 );
    }

// ---------------------------------------------------------
// CCodSaver::~CCodSaver()
// ---------------------------------------------------------
//
CCodSaver::~CCodSaver()
    {
    delete iSourceUri;
    }

// ---------------------------------------------------------
// CCodSaver::SetObserver()
// ---------------------------------------------------------
//
void CCodSaver::SetObserver( MCodLoadObserver* aObserver )
    {
    CLOG(( ECodEng, 2, _L("CCodSaver::SetObserver(0x%x)"), aObserver ));
//TODO:    __ASSERT_ALWAYS( !(aObserver && iObserver), \
//        CodPanic( ECodObserverAlreadySet ) );
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CCodSaver::CheckMaxSize()
// ---------------------------------------------------------
//
TInt CCodSaver::CheckMaxSize( TInt aDataChunkSize )
    {
    __ASSERT_DEBUG( aDataChunkSize >= 0, CodPanic( ECodInternal ) );
    TInt err( KErrNone );
    if ( iMaxSize && iSize + aDataChunkSize > iMaxSize )
        {
        err = KErrCodAttributeMismatch;
        CLOG(( ECodEng, 2, _L("CCodSaver::CheckMaxSize %d+%d>%d, fail"), \
            iSize, aDataChunkSize, iMaxSize ));
        }
    return err;
    }

// ---------------------------------------------------------
// CCodSaver::SetMaxSize()
// ---------------------------------------------------------
//
void CCodSaver::SetMaxSize( TInt aSize )
    {
    __ASSERT_DEBUG( aSize >= 0, CodPanic( ECodInternal ) );
    // Max size is set with allowance, to compensate transport encodings,
    // packaging, and packaging headers.
    iMaxSize = Max( (TInt)(KCodOversizeFactor * aSize), KCodMinUpperBound );
    }


void CCodSaver::AppendStorageInfoL(TPtr8& aBuf)const
    {
    AppendBufL(aBuf,iSourceUri);
    APPEND_BUF_INT(aBuf,iFname);
    APPEND_BUF_INT(aBuf,iSize);
    APPEND_BUF_INT(aBuf,iState);
    APPEND_BUF_INT(aBuf,iHandler);
    }


// -----------------------------------------------------------------------------
// CCodSaver::ConvertDownloadNameUniqueL
// -----------------------------------------------------------------------------
//
void CCodSaver::ConvertDownloadNameUniqueL( HBufC*& filePath,
                                            HBufC*& fileName,
                                            HBufC*& fileExtn)
    {
    TBool bFound( EFalse );
    TInt index( 0 );
    HBufC* uniqueName = NULL;
    HBufC* fullNameTemp = HBufC::NewLC(KMaxFileName);
    TPtr fullNameTempPtr( fullNameTemp->Des() );

    RFs rFs;
  	User::LeaveIfError( rFs.Connect() );
  	CleanupClosePushL(rFs);

    do
        {
        bFound = EFalse;
        //Generate Unique name.
        CreateIndexedNameL( uniqueName, *fileName, index );

        //Name in \\system\\temp.
        fullNameTempPtr.Copy( *filePath );
        fullNameTempPtr.Append( *uniqueName );
        fullNameTempPtr.Append( *fileExtn );

        if( BaflUtils::FileExists( rFs , fullNameTempPtr ) )
            //Check if file name exist in Destination path.
            {
            bFound =ETrue;
            }

        }while( bFound );

    CleanupStack::PopAndDestroy(&rFs);
    CleanupStack::PopAndDestroy(fullNameTemp);
    
    // This is the unique name that we were looking for.
    CleanupStack::PopAndDestroy(fileName);
    fileName = uniqueName;
    CleanupStack::PushL(fileName);
    }

// -----------------------------------------------------------------------------
// CCodSaver::CreateIndexedNameL
// -----------------------------------------------------------------------------
//
void CCodSaver::CreateIndexedNameL( HBufC*& aUniqueName, 
                                        TDesC& aOrgName, 
                                        TInt& aIndex )
    {
    TPtrC left;
    TBuf<KMaxIndexStringLength> indexStr;

    delete aUniqueName; aUniqueName = NULL;

    if( aIndex )
        {
        indexStr.Format( KIndexString, aIndex );
        }

    TInt fullLength = aOrgName.Length() + indexStr.Length();
    if( fullLength > KMaxPath )
        // name is longer than KMaxPath.
        {
        User::Leave( KErrGeneral );
        }

    left.Set( aOrgName.Left( KDownloadNameMaxSize - indexStr.Length() ));

    aUniqueName = HBufC::NewL( fullLength );
    aUniqueName->Des().Format( _L("%S%S"), &left, 
                                           &indexStr );

    ++aIndex;
    }
    
// -----------------------------------------------------------------------------
// CCodSaver::FindUniqueDestinationFileNameL
// -----------------------------------------------------------------------------
//
void CCodSaver::FindUniqueDestinationFileNameL( TDesC& srcFile, HBufC*& destPath )
    {
        HBufC* fileExtention = HBufC::NewLC(KMaxFileName);
        HBufC* fileName = HBufC::NewLC(KMaxFileName);
        TPtr fileNamePtr( fileName->Des() );

        fileNamePtr = srcFile;
        // Retrieve the file extention.
        TInt dotInd = srcFile.LocateReverse( '.' );
        if( dotInd != KErrNotFound )
            // Filename extension found.
            {
            fileExtention->Des().Copy( srcFile.Right( srcFile.Length() - dotInd ) );
            fileNamePtr.Copy( srcFile.Left( dotInd ) );
            }

        // Retrieve the file name (excluding file extention).
        TInt lastSlashPos = fileNamePtr.LocateReverse( '\\' );
        if( lastSlashPos != KErrNotFound )
            // Filename found.
            {
            fileNamePtr.Copy( fileNamePtr.Right( fileNamePtr.Length() - lastSlashPos - 1 ) );
            }

        // Find a unique file name.
        ConvertDownloadNameUniqueL( destPath, fileName, fileExtention );

        // Found. Append file name and extention to destination path.
        destPath->Des().Append( *fileName );
        destPath->Des().Append( *fileExtention );

        CleanupStack::PopAndDestroy( fileName );
        CleanupStack::PopAndDestroy( fileExtention );
    }
