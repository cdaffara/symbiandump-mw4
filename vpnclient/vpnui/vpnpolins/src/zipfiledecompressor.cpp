/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Uncompress the zipped vpn policies.
*
*/



#include "zipfiledecompressor.h"

#include <zipfile.h>
#include <bautils.h>

CZipFileDecompressor* CZipFileDecompressor::NewL()
    {
    CZipFileDecompressor* self = new (ELeave) CZipFileDecompressor;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CZipFileDecompressor::CZipFileDecompressor()
: CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }


void CZipFileDecompressor::ConstructL()
    {
    User::LeaveIfError(iFileServer.Connect());
    }


CZipFileDecompressor::~CZipFileDecompressor()
    {
    Cancel();
    
    iUncompressedFile.Close();
    delete iUncompressedData;
    
    delete iZipFileMemberIterator; 
    delete iZipFile;    

    if (iCloseZippedFile)
        {
        iZippedFile.Close();
        iCloseZippedFile = EFalse;
        }
        
    iFileServer.Close();
    }


void CZipFileDecompressor::DecompressZipFile(const TDesC& aSourceFile, 
                                             const TDesC& aTargetDir,
                                             TRequestStatus& aClientStatus)
    {
    __ASSERT_ALWAYS(!IsActive(), User::Invariant());    
    
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;

    RFile zippedFile;
    TInt err = zippedFile.Open(iFileServer, aSourceFile, EFileRead);
    if (err == KErrNone)
        {
        iCloseZippedFile = ETrue;
        DecompressZipFile(zippedFile, 
                          aTargetDir,
                          aClientStatus);
        }
    else
        {
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, err);        
        }
    }



void CZipFileDecompressor::DecompressZipFile(RFile& aSourceFile, 
                                             const TDesC& aTargetDir,
                                             TRequestStatus& aClientStatus)
    {
    __ASSERT_ALWAYS(!IsActive(), User::Invariant());
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
        
    TRAPD(err, DecompressZipFileL(aSourceFile, aTargetDir));
    if (err != KErrNone)
        {
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, err);
        }
    }


void CZipFileDecompressor::DecompressZipFileL(RFile& aSourceFile, 
                                              const TDesC& aTargetDir)
    {

    iZippedFile = aSourceFile;
    
    BaflUtils::EnsurePathExistsL(iFileServer, aTargetDir);
    User::LeaveIfError(iFileServer.SetSessionPath(aTargetDir));

    iZipFile = CZipFile::NewL(iFileServer, iZippedFile);
    
    iZipFileMemberIterator = iZipFile->GetMembersL();
    
    CZipFileMember* member = iZipFileMemberIterator->NextL();
    if (!member)
        {
        User::Leave(KErrArgument);   
        }
    CleanupStack::PushL(member);        
    DecompressNextMemberL(*member);
    CleanupStack::PopAndDestroy(member);
    }


void CZipFileDecompressor::DecompressNextMemberL(CZipFileMember& aMember)
    {
    __ASSERT_ALWAYS(!iUncompressedFile.SubSessionHandle(), 
                    User::Invariant());
    __ASSERT_ALWAYS(!iUncompressedData, User::Invariant());
    
    
    //Ignore entries that has zero uncompressed size.
    //(This includes e.g. directories) 
    if (aMember.UncompressedSize() > 0)
        {
        const TChar KDirectorySeparator('\\');
        
        TUint32 uncompressedSize = aMember.UncompressedSize();
        HBufC8* uncompressedData = HBufC8::NewLC(uncompressedSize);
        
        RZipFileMemberReaderStream* readerStream;
        User::LeaveIfError(iZipFile->GetInputStreamL(&aMember, readerStream));
        CleanupStack::PushL(readerStream);
        
        TPtr8 uncompressedDataPtr = uncompressedData->Des();
        User::LeaveIfError(readerStream->Read(uncompressedDataPtr, 
                                              uncompressedDataPtr.MaxLength()));
        
        CleanupStack::PopAndDestroy(readerStream);
        
        HBufC* fileName = aMember.Name()->AllocLC();
        TPtr fileNamePtr= fileName->Des();
        TInt lastDirectorySeparator = fileName->LocateReverse(KDirectorySeparator);
        if (lastDirectorySeparator >= 0)
            {
            fileNamePtr = fileName->Mid(lastDirectorySeparator+1);
            }
        
        TParsePtr fileNameParser(fileNamePtr);
        
        User::LeaveIfError(iUncompressedFile.Replace(iFileServer, 
                                                     fileNameParser.NameAndExt(), 
                                                     EFileWrite));            
        CleanupStack::PopAndDestroy(fileName);
        
        CleanupStack::Pop(uncompressedData);
        iUncompressedData = uncompressedData;
        
        iUncompressedFile.Write(*iUncompressedData, iStatus);
        SetActive();
        }
    else
        {
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, KErrNone);
        }
    }



void CZipFileDecompressor::RunL()
    {
    delete iUncompressedData;
    iUncompressedData = NULL;
    
    iUncompressedFile.Close();
    
    if (iStatus.Int() == KErrNone)
        {
        CZipFileMember* member = iZipFileMemberIterator->NextL();
        if (member)
            {
            CleanupStack::PushL(member);
            DecompressNextMemberL(*member);
            CleanupStack::PopAndDestroy(member);
            }
        else
            {
            delete iZipFileMemberIterator; 
            iZipFileMemberIterator = NULL;
            
            delete iZipFile;    
            iZipFile = NULL;                    
            
            if (iCloseZippedFile)
                {
                iZippedFile.Close();
                iCloseZippedFile = EFalse;
                }
            
            
            User::RequestComplete(iClientStatus, KErrNone);            
            }
        }
    else
        {
        delete iZipFileMemberIterator; 
        iZipFileMemberIterator = NULL;
        
        delete iZipFile;    
        iZipFile = NULL;               
        
        if (iCloseZippedFile)
            {
            iZippedFile.Close();
            iCloseZippedFile = EFalse;
            }
        
        
        User::RequestComplete(iClientStatus, iStatus.Int());
        }
    }


void CZipFileDecompressor::DoCancel()
    {
    //No Cancel for write...
    }


void CZipFileDecompressor::Cancel()
    {
    CActive::Cancel();
    
    delete iUncompressedData;
    iUncompressedData = NULL;
    
    iUncompressedFile.Close();

    delete iZipFileMemberIterator; 
    iZipFileMemberIterator = NULL;
    
    delete iZipFile;    
    iZipFile = NULL;
    
    if (iCloseZippedFile)
        {
        iZippedFile.Close();
        iCloseZippedFile = EFalse;
        }
    
                
    }

TInt CZipFileDecompressor::RunError(TInt aError)
    {
    delete iUncompressedData;
    iUncompressedData = NULL;
    
    iUncompressedFile.Close();

    delete iZipFileMemberIterator; 
    iZipFileMemberIterator = NULL;
    
    delete iZipFile;    
    iZipFile = NULL;    
    
    if (iCloseZippedFile)
        {
        iZippedFile.Close();
        iCloseZippedFile = EFalse;
        }
           
    
    User::RequestComplete(iClientStatus, aError);

    return KErrNone;    
    }

