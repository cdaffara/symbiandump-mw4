/*
* Copyright (c) 2007 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class, which handles storing and accessing certificate requests.
*
*/



#include <bautils.h>

#include "certificaterequeststore.h"


_LIT(KCertReqPat, "*.req");
_LIT(KCertReqFileNameFormat, "certreq(%d).req");


CCertificateRequestStore* CCertificateRequestStore::NewL()
    {
    CCertificateRequestStore* self = new (ELeave) CCertificateRequestStore;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


void CCertificateRequestStore::ConstructL()
    {
    User::LeaveIfError(iFileSession.Connect());
    User::LeaveIfError(iFileSession.PrivatePath(iPrivatePath));
    User::LeaveIfError(iFileSession.SetSessionPath(iPrivatePath));
    }


CCertificateRequestStore::~CCertificateRequestStore()
    {
    iFileSession.Close();
    }


TInt CCertificateRequestStore::CertReqCountL()
    {
    TInt count(0);

    TFindFile* fileFinder = new (ELeave) TFindFile(iFileSession);
    CleanupDeletePushL(fileFinder);
    
    CDir* fileList = NULL;
    TInt err = fileFinder->FindWildByDir(KCertReqPat, iPrivatePath, fileList);
    
    switch(err)
        {
        case KErrNone:
            count = fileList->Count();
            delete fileList;
            fileList = NULL;
            break;
        case KErrNotFound:
            //No saved requests.
            //Returns zero.
            break;
        default:
            User::Leave(err);
            break;                
        }
      
    CleanupStack::PopAndDestroy(fileFinder);
    return count;
    
    }

   

CArrayFixFlat<TCertificateRequestListEntry>* CCertificateRequestStore::GetCertReqListLC()
    {
    CArrayFixFlat<TCertificateRequestListEntry>* list = 
                new (ELeave) CArrayFixFlat<TCertificateRequestListEntry>(3);
    CleanupStack::PushL(list);
        
    TCertificateRequestListEntry certReqInfo;
    
    TFindFile* fileFinder = new (ELeave)TFindFile(iFileSession);
    CleanupDeletePushL(fileFinder);
    
    CDir* fileList;
    TInt err = fileFinder->FindWildByDir(KCertReqPat, iPrivatePath, fileList);

    switch(err)
        {        
        case KErrNone:        
            CleanupStack::PushL(fileList);            
            for (TInt i = 0; i < fileList->Count(); i++)
                {
                certReqInfo.iObjectName = (*fileList)[i].iName;                
                list->AppendL(certReqInfo);                
                }            
            CleanupStack::PopAndDestroy(); // fileList
            break;
        case KErrNotFound:
            //No saved requests.
            //Returns zero length array.
            break;
        default:
            User::Leave(err);
            break;
        }
    
    CleanupStack::PopAndDestroy(fileFinder);
    return list;
    }


HBufC* CCertificateRequestStore::SaveCertRequestLC(const TDesC8& aCertReqData)
    {
    
    TFileName* certReqFileName = GetNewCertReqFileNameLC();
    
    RFile certReqFile;    
    User::LeaveIfError(certReqFile.Create(iFileSession, *certReqFileName, EFileWrite));
    CleanupClosePushL(certReqFile);    
    
    TInt err = certReqFile.Write(aCertReqData);    
    
    CleanupStack::PopAndDestroy(); //certReqFile
    
    if (err != KErrNone)
        {
        //Just in case
        iFileSession.Delete(*certReqFileName);
        User::Leave(err);
        }
    

    HBufC* certReqObjectName = certReqFileName->AllocL();    
    CleanupStack::PopAndDestroy(certReqFileName);
    CleanupStack::PushL(certReqObjectName);    
    
    return certReqObjectName;
    }


HBufC8* CCertificateRequestStore::ReadCertRequestLC(const TDesC& aCertReqObjectName)
    {
    RFile file;
    User::LeaveIfError(file.Open(iFileSession, aCertReqObjectName, EFileRead));
    CleanupClosePushL(file);

    TInt requestSize(0);
    User::LeaveIfError(file.Size(requestSize));
    
    HBufC8* requestData = HBufC8::NewLC(requestSize);
    TPtr8 requestDataPtr(requestData->Des());
    
    User::LeaveIfError(file.Read(requestDataPtr));

    CleanupStack::Pop(requestData);
    CleanupStack::PopAndDestroy(); //file
    
    CleanupStack::PushL(requestData);
    
    return requestData;
    }


void CCertificateRequestStore::DeleteCertRequestL(const TDesC& aCertReqObjectName)
    {
    User::LeaveIfError(iFileSession.Delete(aCertReqObjectName));
    }


TFileName* CCertificateRequestStore::GetNewCertReqFileNameLC()
    {
    TFileName* fileName = new (ELeave) TFileName;
    CleanupDeletePushL(fileName);
             
    TInt i = 0;    
    do
        {
        fileName->Format(KCertReqFileNameFormat, i);
        if (!BaflUtils::FileExists(iFileSession, *fileName))
            {
            break;
            }
        ++i;
        }while(i > 0); //loop infinite.
            
    return fileName;
    }
