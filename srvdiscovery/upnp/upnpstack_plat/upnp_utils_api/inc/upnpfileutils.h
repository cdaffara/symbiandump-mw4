/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares File functions
 *
*/


#ifndef C_UPNPFILEUTILS_H
#define C_UPNPFILEUTILS_H

#include <e32base.h>
#include <f32file.h>

//CONSTANTS

//media types
_LIT8( KHtml, "text/html");
_LIT8( KXml, "text/xml");
_LIT8( KTxt, "text/plain");
_LIT8( KJpg, "image/jpeg");
_LIT8( KGif, "image/gif");
_LIT8( KPng, "image/png");
_LIT8( KMpeg, "video/mpeg");
_LIT8( KAvi, "video/x-msvideo");
_LIT8( KMp3, "audio/mpeg");

// media types - file extensions
_LIT16( KHtml16, ".HTML");
_LIT16( KXml16, ".XML");
_LIT16( KTxt16, ".TXT");
_LIT16( KJpeg16, ".JPEG");
_LIT16( KJpg16, ".JPG");
_LIT16( KGif16, ".GIF");
_LIT16( KPng16, ".PNG");
_LIT16( KMpeg16, ".MPEG");
_LIT16( KMpg16, ".MPG");
_LIT16( KAvi16, ".AVI");
_LIT16( KMp316, ".MP3");

//for extracting the IP address
const TInt KIPNumMax = 255;
const TInt KIPNumMin = 1;

// NAMESPACE DECLARATION
namespace UpnpFileUtil
    {
    /**
     ReadFileL
     
     Reads the specified file and returns a pointer to HBufC8 holding the 
     just read information
     @param	aFilename	= name of the file to be read
     @return	HBufC8 holding the contents of the read file
     **/
    IMPORT_C HBufC8* ReadFileL(const TDesC16& aFilename);

    /**
     GetMimeTypeForFileL
     
     - In emulator, checks the filename, and returns mimetype-string depending on extension.
     - In Target device, uses RApaLsSession (Application server) to find out file's mime type.
     @param	aFilename	name of file	
     @return	mimetype
     **/
    IMPORT_C HBufC8* GetMimeTypeForFileL(const TDesC16& aFilename);

    /**
     GetMimeTypeByExtension
     @param	aFilename	name of file	
     @return	mimetype
     **/
    HBufC8* GetMimeTypeByExtensionL( const TDesC16& aFilename );
    /**
     Util method.Extract path from URL defined as 
     scheme://username:password@host:port/path?parameter=value#anchor
     Additionaly path is without last file name
     Example
     in:		http://10.132.11.36:50001/Media/1
     out(return, filename, scheme):	Media	1	http
     
     in:		http://10.132.11.36:50001/Super/Media/1
     out(return, filename, scheme):	Super/Media	1	http
     
     in:		file:///C:/Super/Media/1
     out(return, filename, scheme):	C:/Super/Media	1	file
     
     in:		/Media/1
     out(return, filename, scheme):	Media	1	file
     
     
     @param aURL full/partial url
     @param aFilename out parameter, filename without any shlashes
     @param aScheme out parameter, scheme part of URL
     @return path with slash at the end and without at the begin
     **/
    IMPORT_C TPtrC8 ExtractUrlPath(
        const TDesC8& aUrl, TPtrC8& aFilename, TPtrC8& aScheme, TInt& aParseError );

    /**
     @param aUrl
     @return True/False
     **/
    TBool ExtractUrlPathHost(const TDesC8& aUrl, TInt& aParseError );

    /**
     @param ippart
     @return TInt - KErrNotFound or a number in from a IP
     **/
    TInt ExtractNumFromIP(TPtrC8 ippart);
    /**
     @gets drive number from given aPtah
     @param aPath, aFs
     @return TInt - drive number
     **/
    TInt DriveNumberFromPath( const TDesC16& aPath, RFs& aFs );

    /**
     Util method.Extract path from URL defined as 
     scheme://username:password@host:port/path?parameter=value#anchor
     @param aURL full/partial url
     @param aFilename out parameter, filename without any shlashes
     @return path with slash at the end and without at the begin
     **/
    IMPORT_C TPtrC8 ExtractUrlPath(const TDesC8& aUrl, TPtrC8& aFilename, TInt& aParseError);

    /**
     Util method.Extract path from URL defined as 
     scheme://username:password@host:port/path?parameter=value#anchor
     @param aURL full/partial url
     @return path with slash at the end and without at the begin
     **/
    IMPORT_C TPtrC8 ExtractUrlPath(const TDesC8& aUrl, TInt& aParseError);

    /**
     Util method.Convert / to \\ from url convention to folder
     Example
     in:		Media/foo
     out:	Media\\foo
     
     
     @param aURL path from URL
     @return path in system format
     **/
    IMPORT_C HBufC8* SwitchToBackSlashL(const TDesC8& aUrl);
    IMPORT_C HBufC* SwitchToBackSlashL(const TDesC& aUrl);

    /**
     Util method. Checks if the drive from a path is short of the space requested
     @param aPath, aSize, aFs
     @return True/False
     **/
    IMPORT_C TBool CheckDiskSpaceShortL( const TDesC16& aPath, TInt aSize, RFs& aFs );

    /**
     Util method. Checks if the drive from a path is short of the space requested
     @param aDriveNumber, aSize
     @return True/False
     **/
    IMPORT_C TBool CheckDiskSpaceShortL( TDriveNumber aDriveNumber, TInt aSize );
    
    /**
    * Parses uri
    * @since Series60 5.0 HN
    * @param aUrl url to parse
    * return error code
    **/
    IMPORT_C TInt ParseUri( const TDesC8& aUrl );    

    }

#endif // C_UPNPFILEUTILS_H