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



#ifndef C_ZIPFILEDECOMPRESSOR_H
#define C_ZIPFILEDECOMPRESSOR_H

#include <e32base.h>
#include <f32file.h>


class CZipFile;
class CZipFileMemberIterator;
class CZipFileMember;

class CZipFileDecompressor : public CActive
    {
public:
    static CZipFileDecompressor* NewL();
    ~CZipFileDecompressor();

    void DecompressZipFile(const TDesC& aSourceFile, 
                           const TDesC& aTargetDir,
                           TRequestStatus& aClientStatus);


    void DecompressZipFile(RFile& aSourceFile, 
                           const TDesC& aTargetDir,
                           TRequestStatus& aClientStatus);


    void Cancel();
protected:

    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
private:
    CZipFileDecompressor();
    void ConstructL();



    void DecompressZipFileL(RFile& aSourceFile, 
                            const TDesC& aTargetDir);

    
    void DecompressNextMemberL(CZipFileMember& aMember);

    
    RFs     iFileServer;
    RFile   iZippedFile;
    TBool   iCloseZippedFile;
    TRequestStatus* iClientStatus;
    
    CZipFile* iZipFile;
    CZipFileMemberIterator *iZipFileMemberIterator;
    
    HBufC8* iUncompressedData;
    RFile  iUncompressedFile;
    };

#endif //C_ZIPFILEDECOMPRESSOR_H