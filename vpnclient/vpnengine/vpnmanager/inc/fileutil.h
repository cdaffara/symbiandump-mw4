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



#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "vpnapidefs.h"

/**
 * File name extensions
 */
_LIT(KPolFileExt,       ".pol");
_LIT(KPinFileExt,       ".pin");
_LIT(KCertFileExt,      ".cer");
_LIT(KPrivKeyFileExt,   ".key");

/**
 * File name patterns
 */
_LIT(KPinFilePat,           "*.pin");
_LIT(KPolFilePat,           "*.pol");
_LIT(KUserPrivKeyFilePat,   "-user*.key");
_LIT(KCaCertFilePat,        "-ca*.cer");
_LIT(KPeerCertFilePat,      "-peer*.cer");
_LIT(KAllFilesPat,          "*.*");

/**
 * File names
 */
_LIT(KPolListFile,  "vpnpolicies");

/**
 * Paths
 */

class RFs;

/**
 * A collection of methods for handling files and file names.
 */
class TFileUtil
    {
public:
    TFileUtil(RFs& aFs);
    TFileName ReplaceExtension(const TFileName& aFileName,
                               const TDesC& aExtension);
    TBool FileExists(const TFileName& aFileName);
    HBufC8* LoadFileDataL(const TFileName& aFileName);
    HBufC* LoadFileDataUL(const TFileName& aFileName);
    void SaveFileDataL(const TFileName& aFileName,
                       const TDesC8& aFileData);
    void SaveFileDataL(const TFileName& aFileName,
                       const TDesC& aFileData);
    // Ownership of the return object transferred to caller
    // in Make* and Get* functions
    HBufC* MakeFileNameLC(const TDesC& aDir, const TDesC& aName,
                           const TDesC& aExtension);
    TFileName MakeFileName(const TDesC& aDir, const TDesC& aNameAndExtension);
    HBufC* GetPolFileNameLC(const TVpnPolicyId& aPolicyId);
    HBufC* GetPinFileNameLC(const TVpnPolicyId& aPolicyId);
    HBufC* GetPolFileNameL(const TVpnPolicyId& aPolicyId);
    HBufC* GetPinFileNameL(const TVpnPolicyId& aPolicyId);
    TFileName PolListFileNameL();
    void DeleteFileL(const TFileName& aFileName);
    void DeleteFilesL(const TFileName& aFileFilter);
    void MoveFileL(const TFileName& aSrcFileName, const TFileName& aDstFileName);
    void CopyFileL(const TFileName& aSrcFileName, const TFileName& aDstFileName);
    void CreateDirIfNeededL(const TFileName& aDirName);
    HBufC8* To8BitL(const TDesC16& aDes);
    HBufC16* To16BitL(const TDesC8& aDes);
    void DeleteDirL(const TFileName& aDirName);

private:
    RFs& iFs;
    };

#endif // __FILEUTIL_H__
