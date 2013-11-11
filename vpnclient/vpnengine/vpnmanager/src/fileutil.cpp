/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Provides file handling functions.
*
*/



#include <f32file.h>
#include "fileutil.h"

TFileUtil::TFileUtil(RFs& aFs) : iFs(aFs)
    {
    }
    
TFileName TFileUtil::ReplaceExtension(const TFileName& aFileName,
                                      const TDesC& aExtension)
    {
    TFileName newFileName;
    
    TInt extensionOffset = aFileName.LocateReverse('.');
    
    if (extensionOffset == KErrNotFound)
        {
        newFileName = aFileName;
        newFileName.Append('.');
        newFileName.Append(aExtension);
        }
    else
        {
        newFileName = aFileName.Left(extensionOffset + 1);
        newFileName.Append(aExtension);
        }

    return newFileName;
    }

TBool TFileUtil::FileExists(const TFileName& aFileName)
    {
    TUint fileAttr;

    if (iFs.Att(aFileName, fileAttr) == KErrNone)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

HBufC8* TFileUtil::LoadFileDataL(const TFileName& aFileName)
    {
    RFile file;
    CleanupClosePushL(file);

    User::LeaveIfError(file.Open(iFs, aFileName, EFileRead));

    TInt fileSize;
    User::LeaveIfError(file.Size(fileSize));

    HBufC8* fileData = HBufC8::NewLC(fileSize);
    TPtr8 ptrFileData = fileData->Des();

    User::LeaveIfError(file.Read(ptrFileData));

    CleanupStack::Pop(); // fileData
    CleanupStack::PopAndDestroy(); // file

    return fileData;
    }

HBufC* TFileUtil::LoadFileDataUL(const TFileName& aFileName)
    {
    HBufC8* fileData8 = LoadFileDataL(aFileName);
    CleanupStack::PushL(fileData8);

    HBufC* fileData = To16BitL(*fileData8);
    
    CleanupStack::PopAndDestroy(); // fileData8

    return fileData;
    }

void TFileUtil::SaveFileDataL(const TFileName& aFileName,
                              const TDesC& aFileData)
    {
    HBufC8* fileData8 = To8BitL(aFileData);
    CleanupStack::PushL(fileData8);

    SaveFileDataL(aFileName, *fileData8);

    CleanupStack::PopAndDestroy(); // fileData8
    }
    
void TFileUtil::SaveFileDataL(const TFileName& aFileName,
                              const TDesC8& aFileData)
    {
    RFile file;
    CleanupClosePushL(file);

    User::LeaveIfError(file.Replace(iFs, aFileName, EFileWrite));

    User::LeaveIfError(file.Write(aFileData));

    CleanupStack::PopAndDestroy(); // file
    }

HBufC* TFileUtil::MakeFileNameLC(const TDesC& aDir, const TDesC& aName,
                                  const TDesC& aExtension)
    {
    HBufC* fileName = HBufC::NewL(aDir.Length() + aName.Length() + aExtension.Length());
    CleanupStack::PushL(fileName);
    TPtr fileNamePtr = fileName->Des();
    fileNamePtr.Append(aDir);
    fileNamePtr.Append(aName);
    fileNamePtr.Append(aExtension);

    return fileName;
    }

TFileName TFileUtil::MakeFileName(const TDesC& aDir, const TDesC& aNameAndExtension)
    {
    TFileName fileName;
    
    fileName.Copy(aDir);
    fileName.Append(aNameAndExtension);

    return fileName;
    }
    
HBufC* TFileUtil::GetPolFileNameLC(const TVpnPolicyId& aPolicyId)
    {
   	TPath privateDir;
	User::LeaveIfError(iFs.PrivatePath(privateDir));
	
	return MakeFileNameLC(privateDir, aPolicyId, KPolFileExt);
    }

HBufC* TFileUtil::GetPolFileNameL(const TVpnPolicyId& aPolicyId)
    {
    HBufC* ret(NULL);
   	TPath privateDir;
	User::LeaveIfError(iFs.PrivatePath(privateDir));
	ret = MakeFileNameLC(privateDir, aPolicyId, KPolFileExt);
	CleanupStack::Pop();
	return ret;
    }

HBufC* TFileUtil::GetPinFileNameLC(const TVpnPolicyId& aPolicyId)
    {
    TPath privateDir;
    User::LeaveIfError(iFs.PrivatePath(privateDir));
    return MakeFileNameLC(privateDir, aPolicyId, KPinFileExt);
    }

HBufC* TFileUtil::GetPinFileNameL(const TVpnPolicyId& aPolicyId)
    {
    HBufC* ret(NULL);
    TPath privateDir;
    User::LeaveIfError(iFs.PrivatePath(privateDir));
    ret = MakeFileNameLC(privateDir, aPolicyId, KPinFileExt);
    CleanupStack::Pop();
    return ret;
    }

TFileName TFileUtil::PolListFileNameL()
    {
    TPath privateDir;
    TInt err = iFs.PrivatePath(privateDir);
    return MakeFileName(privateDir, KPolListFile);
    }

void TFileUtil::DeleteFileL(const TFileName& aFileName)
    {
    User::LeaveIfError(iFs.Delete(aFileName));
    }

void TFileUtil::DeleteFilesL(const TFileName& aFileFilter)
    {
    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);

    User::LeaveIfError(fileMan->Delete(aFileFilter));

    CleanupStack::PopAndDestroy(); // fileMan
    }

void TFileUtil::MoveFileL(const TFileName& aSrcFileName, const TFileName& aDstFileName)
    {
    User::LeaveIfError(iFs.Rename(aSrcFileName, aDstFileName));
    }

void TFileUtil::CopyFileL(const TFileName& aSrcFileName, const TFileName& aDstFileName)
    {
    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);

    User::LeaveIfError(fileMan->Copy(aSrcFileName, aDstFileName));

    CleanupStack::PopAndDestroy(); // fileMan
    }
    
void TFileUtil::CreateDirIfNeededL(const TFileName& aDirName)
    {
    TFileName dirName;
    
    dirName.Copy(aDirName);
    
    if (dirName[dirName.Length() - 1] != '\\')
        {
        dirName.Append('\\');
        }
    
    if (!FileExists(dirName))
        {
        User::LeaveIfError(iFs.MkDirAll(dirName));
        }
    }
    
HBufC8* TFileUtil::To8BitL(const TDesC16& aDes)
    {
    HBufC8* desCopy;
    
    TInt desLength = aDes.Length();
    
    if (desLength > 0)
        {
        desCopy = HBufC8::NewL(desLength);
        desCopy->Des().Copy(aDes);
        }
    else
        {
        desCopy = HBufC8::NewL(1);
        }

    return desCopy;
    }

HBufC16* TFileUtil::To16BitL(const TDesC8& aDes)
    {
    HBufC16* desCopy;
    
    TInt desLength = aDes.Length();
    
    if (desLength > 0)
        {
        desCopy = HBufC16::NewL(desLength);
        desCopy->Des().Copy(aDes);
        }
    else
        {
        desCopy = HBufC16::NewL(1);
        }

    return desCopy;
    }

void TFileUtil::DeleteDirL(const TFileName& aDirName)
    {
    if (FileExists(aDirName))
        {
        CFileMan* fileMan = CFileMan::NewL(iFs);
        CleanupStack::PushL(fileMan);

        User::LeaveIfError(fileMan->RmDir(aDirName));
        
        CleanupStack::PopAndDestroy(); // fileMan
        }
    }
