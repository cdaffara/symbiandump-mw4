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
* Description:   Extracted VPN bundle file handler.
*
*/




#ifndef C_VPNBUNDLEHANDLER_H
#define C_VPNBUNDLEHANDLER_H

#include <e32base.h>
#include <f32file.h>

/**
 * Basic handler for VPN bundle (i.e. the extracted .VPN file contents)
 *
 * Can be used to verify .vpn / .p12 file existence, and to
 * extract raw .vpn / .p12 file data into buffers.
 */
class CVpnBundleHandler : public CBase 
    {
public:
    /**
     * Standard twophased constructor.
     *
     * @param aBundleDir Reference to the directory where the 
     *        extracted bundle is located.
     *
     */
    static CVpnBundleHandler* NewL(const TDesC& aBundleDir);
    /**
     * Standard twophased constructor that leaves CleanupStack
     * residuals.
     *
     * @param aBundleDir Reference to the directory where the 
     *        extracted bundle is located.
     *
     */
    static CVpnBundleHandler* NewLC(const TDesC& aBundleDir);
    ~CVpnBundleHandler();

private:
    CVpnBundleHandler(const TDesC& aBundleDir);

protected:
    void ConstructL();

public:

    /**
     * Gather information about the bundle contents:
     * Extract filenames for PKCS12 package and
     * VPN command file (if those exist) to local variables.
     *
     *
     * @leave Leaves if there are more than 1 cmd files / p12 packages
     */
    void        AnalyzeBundleContentsL();

    /**
     * Returns ETrue iff a command file was found in bundle analysis
     *
     * @return ETrue iff a command file was found in bundle analysis
     *
     */
    TBool       CommandFileExists() const;

    /**
     * Returns ETrue iff a PKCS#12 file was found in bundle analysis
     *
     * @return ETrue iff a PKSC#12 file was found in bundle analysis
     */
    TBool       PKCS12FileExists() const;

    /**
     * Extracts PKCS12 data from the PKCS#12 file into a 8bit buffer.
     *
     * @return HBufC8* buffer containing the raw PKCS#12 data.
     *         Control of the object transferred to caller.
     *
     * @leave Leaves if OOM, or if file operations fail.
     */
    HBufC8*      ExtractPKCS12DataL();

    /**
     * Extracts VPN command file XML data from the command file 
     * into a 8bit buffer.
     *
     * @return HBufC8* buffer containing the raw command file
     *         XML data. Control of the object transferred to caller.
     *
     * @leave Leaves if OOM, or if file operations fail.
     */
    HBufC8*      ExtractCommandFileDataL();

private:

    /**
     * Extracts the command file name to iCommandFileName.
     * Command file name is decided by searching the temp directory
     * into which the VPN file was extracted. If more than one command
     * file is found, or OOM, the function may leave.
     * Command file search is based on file extension pattern (".vpc")
     *
     * @leave Leaves if OOM or more than one command files found
     */
    void        LocateCommandFileL();

    /**
     * Extracts the PKCS#12 file name to iPKCS12FileName.
     * PKCS#12 file name is decided by searching the temp directory
     * into which the VPN file was extracted. If more than one PKCS#12
     * file is found, or OOM, the function may leave.
     * Command file search is based on file extension pattern 
     * (".p12")
     *
     * @leave Leaves if OOM or more than one command files found
     */
    void        LocatePKCS12FileL();

private:
    // Full filename (path included) to PKCS#12 file in .VPN bundle
    HBufC*      iPKCS12FileName;

    // Full filename (path included) to command file in .VPN bundle
    HBufC*      iCommandFileName;

    // Reference to the location of extracted .VPN bundle

    TPtrC       iBundleFileDir; // ref
    RFs         iFileServer;
    };

#endif // C_VPNPOLICYINSTALLER_H

