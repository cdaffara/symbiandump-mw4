/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility class for .VPN bundle content analysis
*
*/



#include <e32std.h>

#include "vpnbundlehandler.h"
#include "policyinstaller_constants.h"
#include "logvpncommon.h"


CVpnBundleHandler* CVpnBundleHandler::NewL(const TDesC& aBundleDir) 
    {
    LOG_("-> CVpnBundleHandler::NewL()");
    
    CVpnBundleHandler* self = new (ELeave) CVpnBundleHandler(aBundleDir);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    LOG_("<- CVpnBundleHandler::NewL()");
    
    return self;
    }

CVpnBundleHandler* CVpnBundleHandler::NewLC(const TDesC& aBundleDir) 
    {
    LOG_("-> CVpnBundleHandler::NewLC()");
    
    CVpnBundleHandler* self = new (ELeave) CVpnBundleHandler(aBundleDir);
    CleanupStack::PushL(self);
    self->ConstructL();
    
    LOG_("<- CVpnBundleHandler::NewLC()");
    
    return self;
    }

CVpnBundleHandler::CVpnBundleHandler(const TDesC& aBundleDir) : iBundleFileDir(aBundleDir) 
    {
    }

CVpnBundleHandler::~CVpnBundleHandler() 
    {
    LOG_("-> CVpnBundleHandler::~CVpnBundleHandler()");

    delete iPKCS12FileName;
    delete iCommandFileName;
    iFileServer.Close();    

    LOG_("<- CVpnBundleHandler::~CVpnBundleHandler()");
    }

void CVpnBundleHandler::ConstructL() 
    {
    LOG_("-> CVpnBundleHandler::ConstructL()");

    User::LeaveIfError(iFileServer.Connect());

    LOG_("<- CVpnBundleHandler::ConstructL()");
    }

void CVpnBundleHandler::AnalyzeBundleContentsL() 
    {
    LOG_("-> CVpnBundleHandler::AnalyzeBundleContentsL()");

    // Check whether a command file exists. Leave if more than one exist.
    LocateCommandFileL();

    // Check whether a PKCS#12 file exists. Leave if more than one exist.
    LocatePKCS12FileL();

    LOG_("<- CVpnBundleHandler::AnalyzeBundleContentsL()");
    }

TBool CVpnBundleHandler::CommandFileExists() const
    {
    LOG_("-> CVpnBundleHandler::CommandFileExists()");

    TBool ret(EFalse);
    if (iCommandFileName)
        {
        ret = ETrue;
        }

    LOG_1("<- CVpnBundleHandler::CommandFileExists() ret: %d", ret);

    return ret;
    }

TBool CVpnBundleHandler::PKCS12FileExists() const
    {
    LOG_("-> CVpnBundleHandler::PKCS12FileExists() ");

    TBool ret(EFalse);
    if (iPKCS12FileName)
        {
        ret = ETrue;
        }

    LOG_1("<- CVpnBundleHandler::PKCS12FileExists() ret: %d", ret);

    return ret;
    }

HBufC8* CVpnBundleHandler::ExtractPKCS12DataL() 
    {
    LOG_("-> CVpnBundleHandler::ExtractPKCS12DataL()");

    TInt dataLen(0);
    HBufC8* ret(NULL);

    ASSERT(iPKCS12FileName);

    // Read binary PKCS#12 data from file and into an 8bit descr
    RFile file;
    CleanupClosePushL(file);

    LOG_(" Opening command file for reading");
    User::LeaveIfError(file.Open(iFileServer, 
                                 *iPKCS12FileName, 
                                 EFileStream|EFileRead));
    User::LeaveIfError(file.Size(dataLen));

    LOG_1(" File data length: %d bytes", dataLen);

    ret = HBufC8::NewLC(dataLen);
    TPtr8 dataptr = ret->Des();

    LOG_1(" Data length before read: %d B", dataptr.Length());

    LOG_(" Reading...");
    User::LeaveIfError(file.Read(dataptr));

    LOG_1(" Data length after read: %d B", dataptr.Length());

    CleanupStack::Pop(ret); // control transferred

    LOG_(" Closing file");
    CleanupStack::PopAndDestroy(); // file

    LOG_1("<- CVpnBundleHandler::ExtractPKCS12DataL() len: %d", dataLen);

    return ret;
    }

HBufC8* CVpnBundleHandler::ExtractCommandFileDataL() 
    {
    LOG_("-> CVpnBundleHandler::ExtractCommandFileDataL()");

    TInt dataLen(0);
    HBufC8* ret(NULL);


    ASSERT(iCommandFileName);

    // Read ASCII cmd file data from file and into an 8bit descr
    RFile file;
    CleanupClosePushL(file);

    LOG_(" Opening command file for reading");
    User::LeaveIfError(file.Open(iFileServer, 
                                 *iCommandFileName, 
                                 EFileStreamText|EFileRead));
    User::LeaveIfError(file.Size(dataLen));

    LOG_1(" File data length: %d bytes", dataLen);

    ret = HBufC8::NewLC(dataLen);
    TPtr8 dataptr = ret->Des();

    LOG_1(" Data length before read: %d B", dataptr.Length());

    LOG_(" Reading...");
    User::LeaveIfError(file.Read(dataptr));

    LOG_1(" Data length after read: %d B", dataptr.Length());

    CleanupStack::Pop(ret); // control transferred
    LOG_(" Closing file");
    CleanupStack::PopAndDestroy(); // file

    LOG_1("<- CVpnBundleHandler::ExtractCommandFileDataL() len: %d", dataLen);
    
    return ret;
    }

/**
 * Find PKCS#12 file from the temp dir the .vpn file was extracted to.
 */
void CVpnBundleHandler::LocateCommandFileL() 
    {
    LOG_("-> CVpnBundleHandler::LocateCommandFileL()");

    TFindFile fileFinder(iFileServer);
    CDir* fileList(NULL);



    LOG_1(" Search argument: '%S'", &(KCommandFilePattern()));
    LOG_1(" Search path: '%S'", &iBundleFileDir);

    // Find the command file using the pattern .p12
    TInt ret = fileFinder.FindWildByDir(KCommandFilePattern, 
                                        iBundleFileDir, fileList);

    if (ret == KErrNone)
        {
        CleanupStack::PushL(fileList);

        // Only allow one or less command files in the VPN bundle
        if (fileList->Count() > 1) 
            {
            LOG_("<- CVpnBundleHandler::LocateCommandFileL() LEAVE: multiple command files");
            User::Leave(KErrArgument);
            }

        // At the moment, there may be exactly 0 or 1 command files,
        // so the loop isn't strictly necessary. However,
        // in the future it may be deemed necessary to support 
        // multiple command files.
        for (TInt i = 0; i < fileList->Count(); i++)
            {
            LOG_1(" Processing cmdfile number %d", (i+1));
            TParse fileNameParser;
            fileNameParser.Set((*fileList)[i].iName, NULL, NULL);

            LOG_1(" Found cmdfile: '%S'", &(fileNameParser.FullName()));
            if (iCommandFileName) 
                {
                delete iCommandFileName;
                iCommandFileName = NULL;
                }

            // Allocate heap for the filename
            iCommandFileName = HBufC::NewL(fileNameParser.FullName().Length() + 
                                           iBundleFileDir.Length());
            //Concatenate the path + filename
            iCommandFileName->Des().Append(iBundleFileDir);
            iCommandFileName->Des().Append(fileNameParser.FullName());
            }

        CleanupStack::PopAndDestroy(fileList);
        }
    else if (ret != KErrNotFound) 
        {
        User::Leave(ret);
        }

    LOG_("<- CVpnBundleHandler::LocateCommandFileL()");
    }

/**
 * Find PKCS#12 file from the temp dir the .vpn file was extracted to.
 */
void CVpnBundleHandler::LocatePKCS12FileL() 
    {
    LOG_("-> CVpnBundleHandler::LocatePKCS12FileL()");

    TFindFile fileFinder(iFileServer);
    CDir* fileList(NULL);

    LOG_1(" Search argument: '%S'", &(KPKCS12Pattern()));
    LOG_1(" Search path: '%S'", &iBundleFileDir);

    // Find the PCKS#12 file using the pattern .p12
    TInt ret = fileFinder.FindWildByDir(KPKCS12Pattern, 
                                        iBundleFileDir, 
                                        fileList);

    if (ret == KErrNone)
        {
        CleanupStack::PushL(fileList);

        // Only allow one or less command files in the VPN bundle
        if (fileList->Count() > 1) 
            {
            LOG_("<- CVpnBundleHandler::LocatePKCS12FileL() LEAVE: multiple p12 files");
            User::Leave(KErrArgument);
            }

        // At the moment, there may be exactly 0 or 1 p12 files,
        // so the loop isn't strictly necessary. However,
        // in the future it may be deemed necessary to support 
        // multiple p12 files.
        for (TInt i = 0; i < fileList->Count(); i++)
            {
            LOG_1(" Processing p12 number %d", (i+1));
            TParse fileNameParser;
            fileNameParser.Set((*fileList)[i].iName, NULL, NULL);

            LOG_1(" Found p12: '%S'", &(fileNameParser.FullName()));
            if (iPKCS12FileName) 
                {
                delete iPKCS12FileName;
                iPKCS12FileName = NULL;
                }

            // Allocate heap for the filename
            iPKCS12FileName = HBufC::NewL(fileNameParser.FullName().Length() + 
                                           iBundleFileDir.Length());
            //Concatenate the path + filename
            iPKCS12FileName->Des().Append(iBundleFileDir);
            iPKCS12FileName->Des().Append(fileNameParser.FullName());
            }

        CleanupStack::PopAndDestroy(fileList);
        }
    else if (ret != KErrNotFound) 
        {
        User::Leave(ret);
        }

    LOG_("<- CVpnBundleHandler::LocatePKCS12FileL()");
    }


