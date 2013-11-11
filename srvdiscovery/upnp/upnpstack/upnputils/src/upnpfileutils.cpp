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
* Description:  File utils
 *
*/


// INCLUDES
#include <e32base.h>
#include <charconv.h>
#include <f32file.h>
#include <apgcli.h>
#include <apmrec.h>
#include <sysutil.h>
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnpcons.h"
#include "upnpfileutils.h"
#include "upnpstring.h"
#include "uri8.h"
#include "in_sock.h"

// ================= MEMBER FUNCTIONS =======================
namespace UpnpFileUtil
    {
    // -----------------------------------------------------------------------------
    // UpnpFileUtil::ReadFileL
    // Reads the specified file and returns a pointer to HBufC8 holding the 
    // information.
    // -----------------------------------------------------------------------------
    //
    EXPORT_C HBufC8* ReadFileL(const TDesC16& aFilename)
        {
        LOGS1("UpnpFileUtil::ReadFileL - File: %S", &aFilename);

        RFs fs;
        HBufC8* buffer = NULL;
        TInt error = fs.Connect();
        if ( error == KErrNone )
            {
            CleanupClosePushL( fs );

            RFile file;
            error = file.Open(fs, aFilename, EFileRead|EFileShareAny );
            if ( error == KErrNone )
                {
                CleanupClosePushL( file );

                TInt fileSize;
                error = file.Size(fileSize);
                if ( error == KErrNone )
                    {
                    buffer = HBufC8::NewLC(fileSize);
                    buffer->Des().SetLength(0);

                    TPtr8 ptr = buffer->Des();
                    error = file.Read( ptr );
                    if ( error == KErrNone )
                        {
                        CleanupStack::Pop( buffer );
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy( buffer );
                        buffer = NULL;
                        }
                    }

                // Close File
                CleanupStack::PopAndDestroy(); 
                }
            else
                {
                LOGS1("UpnpFileUtil::ReadFileL - RFile::Open() failed: %d", error);
                }

            // Close FS
            CleanupStack::PopAndDestroy(); 
            }
        else
            {
            LOGS1("UpnpFileUtil::ReadFileL - RFs::Connect() failed: %d", error );
            }

        // In case of an error, we leave as error would mean that the 
        // file system is down. Shouldn't fail in any other case.
        User::LeaveIfError( error );

        return buffer;
        }

    // -----------------------------------------------------------------------------
    // UpnpFileUtil::GetMimeTypeForFileL
    // - In emulator, checks the filename, and returns mimetype-string depending 
    //   on extension.
    // - In Target device, uses RApaLsSession (Application server) to find out 
    //   file's mime type.
    // -----------------------------------------------------------------------------
    //
    EXPORT_C HBufC8* GetMimeTypeForFileL(const TDesC16& aFilename)
        {
        LOGS1("UpnpFileUtil::GetMimeTypeForFileL - Getting mime type for file: %S", &aFilename);

        HBufC8* mimetype = NULL;

        // Added extra functionality, due to DLNA requirements for XML files mime type.
        // In mime type for xml files there should be explicitly defined used charset
        // and RApaLsSession service doesn't provide such a functionality.

        // Check extension, if its .xml - set mime type to text/xml and check used encoding
        _LIT8(KXmlUtf8, "text/xml; charset=\"utf-8\"");
        TParse parse;
        parse.Set( aFilename, NULL, NULL );
        TBufC16<KMaxName> ext( parse.Ext() );

        if ( ext.FindC( KXml16 ) == 0 && ext.Length() == KXml16().Length() )
            {
            // Extension says that's XML but we check content to be sure and get encoding
            

            mimetype = HBufC8::NewL( KXmlUtf8().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KXmlUtf8() );
            return mimetype;
            }

        // If it's not a XML file then use built-in service.

        RApaLsSession sess;
        TInt error = sess.Connect();
        // We have FS, File and RApaLs open
        if ( error == KErrNone )
            {
            CleanupClosePushL( sess );

            TUid id = TUid::Uid( 0 );

            TDataType type;

            error = sess.AppForDocument( aFilename, id, type );
            if( error == KErrNone && type != TDataType())
                {
                mimetype = HBufC8::NewL( type.Des8().Length() + UpnpString::KLineFeed().Length() );
                mimetype->Des().Append( type.Des8() );
                }
            else
                {
                error = KErrNotFound;
                LOGS1("UpnpFileUtil::GetMimeTypeForFileL - AppForDocument failed: %d", error );
                }

            CleanupStack::PopAndDestroy( &sess );
            }
        else
            {
            LOGS1("UpnpFileUtil::GetMimeTypeForFileL - RFile::Open failed: %d", error );
            }

        if( error != KErrNone)
            {
            mimetype = GetMimeTypeByExtensionL( aFilename );
            }

        LOGS("UpnpFileUtil::GetMimeTypeForFileL - MIME type resolved: " );
        LOGT( mimetype->Des() );

        return mimetype;
        }

    // -----------------------------------------------------------------------------
    // UpnpFileUtil::GetMimeTypeByExtensionL
    // - In emulator, checks the filename, and returns mimetype-string depending 
    //   on extension.
    // - In Target device, uses RApaLsSession (Application server) to find out 
    //   file's mime type.
    // -----------------------------------------------------------------------------
    //
    HBufC8* GetMimeTypeByExtensionL( const TDesC16& aFilename )
        {
        LOGS("UpnpFileUtil::GetMimeTypeForFileL - Starting search by file extension");

        HBufC8* mimetype = NULL;

        TParse parse;

        parse.Set( aFilename, NULL, NULL );

        TBufC16<KMaxName> ext( parse.Ext() );

        // FindC and length calculation is used,
        // beacause compareC does not work for undefined reason  

        if ( ext.FindC( KHtml16 ) == 0 && ext.Length() == KHtml16().Length() )
            {
            mimetype = HBufC8::NewLC( KHtml().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KHtml() );
            }
        else if ( ext.FindC( KXml16 ) == 0 && ext.Length() == KXml16().Length() )
            {
            mimetype = HBufC8::NewLC( KXml().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KXml() );
            }
        else if ( ext.FindC( KTxt16 ) == 0 && ext.Length() == KTxt16().Length() )
            {
            mimetype = HBufC8::NewLC( KTxt().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KTxt() );
            }
        else if ( ( ext.FindC( KJpg16 ) == 0 && ext.Length() == KJpg16().Length() ) ||
                ( ext.FindC( KJpeg16 ) == 0 && ext.Length() == KJpeg16().Length() ) )
            {
            mimetype = HBufC8::NewLC( KJpg().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KJpg() );
            }
        else if ( ext.FindC( KGif16 ) == 0 && ext.Length() == KGif16().Length() )
            {
            mimetype = HBufC8::NewLC( KGif().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KGif() );
            }
        else if ( ext.FindC( KPng16 ) == 0 && ext.Length() == KPng16().Length() )
            {
            mimetype = HBufC8::NewLC( KPng().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KPng() );
            }
        else if ( ( ext.FindC( KMpg16 ) == 0 && ext.Length() == KMpg16().Length() ) ||
                ( ext.FindC( KMpeg16 ) == 0 && ext.Length() == KMpeg16().Length() ) )
            {
            mimetype = HBufC8::NewLC( KMpeg().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KMpeg() );
            }
        else if ( ext.FindC( KAvi16 ) == 0 && ext.Length() == KAvi16().Length() )
            {
            mimetype =HBufC8::NewLC( KAvi().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KAvi() );
            }
        else if ( ext.FindC( KMp316 ) == 0 && ext.Length() == KMp316().Length() )
            {
            mimetype = HBufC8::NewLC( KMp3().Length() );
            mimetype->Des().Zero();
            mimetype->Des().Append( KMp3() );
            }

        // If type was not found, then set empty it here
        if ( !mimetype )
            {
            // We can't return mimetype=NULL;
            
            // Allocate one lenght mime.
            mimetype = HBufC8::NewLC( 1 );
            }
        CleanupStack::Pop( mimetype );

        return mimetype;
        }

    // -----------------------------------------------------------------------------
    // ExtractNumFromIP
    // -----------------------------------------------------------------------------
    //    
    TInt ExtractNumFromIP(TPtrC8 ippart)
        {

        TInt ipdef=KIPNumMin;
        TLex8 lexip = TLex8(ippart);
        TInt errorip = lexip.Val(ipdef);

        if (errorip == KErrNone)
        return ipdef;
        else
        return KErrNotFound;

        }
    // -----------------------------------------------------------------------------
    // Returns drive number
    // -----------------------------------------------------------------------------
    //
    TInt DriveNumberFromPath( const TDesC16& aPath, RFs& aFs )
        {
        TInt drive_num;
        TParse p;
        p.Set( aPath, NULL, NULL );
        TPtrC pointer = p.Drive();
        TLex lineParser( pointer.Ptr() );
        TChar ch = lineParser.Get();

        //sets the default drive or the found value
        if( aFs.CharToDrive( ch, drive_num )<KErrNone )
            {
            drive_num=EDriveC;
            }
        return drive_num;
        }

    // -----------------------------------------------------------------------------
    // ExtractUrlPathHost
    // -----------------------------------------------------------------------------
    //
    TBool ExtractUrlPathHost(const TDesC8& aUrl, TInt& aParseError )
        {
        TUriParser8 up;
        aParseError = up.Parse(aUrl);
        if ( aParseError != KErrNone )
            {
            return EFalse;
            }

        TPtrC8 hostt(up.Extract(EUriHost));
        TInt posOfFirstDot( 0 );
        TInt posOfSecondDot( 0 );
        TInt posOfThirdDot( 0 );

        posOfFirstDot = hostt.FindC( UpnpString::KDot8() );

        //position of the first dot in IP address must be from 1 to 3
        if( (posOfFirstDot < KErrNone ) || ((posOfFirstDot> 3 ) || (posOfFirstDot < 1)))
            {
            return EFalse;
            }
        else
            {
            TPtrC8 ip1;
            ip1.Set( hostt.Left( posOfFirstDot));

            TInt ip1def = ExtractNumFromIP(ip1);

            if ((ip1def> KIPNumMax) || (ip1def < KIPNumMin ))
                {
                return EFalse;
                }

            hostt.Set(hostt.Mid(posOfFirstDot + UpnpString::KDot8().Length()));
            posOfSecondDot = hostt.FindC( UpnpString::KDot8() );

            //position of the dot in IP address must from 1 to 3 starting from the position of former dot
            if( (posOfSecondDot < KErrNone) || ((posOfSecondDot> 3 ) || (posOfSecondDot < 1)))
                {
                return EFalse;
                }
            else
                {
                TPtrC8 ip2;
                ip2.Set( hostt.Left( posOfSecondDot));

                ip1def = ExtractNumFromIP(ip2);

                if ((ip1def == KErrNotFound) || (ip1def> KIPNumMax))
                    {
                    return EFalse;
                    }

                hostt.Set(hostt.Mid(posOfSecondDot + UpnpString::KDot8().Length()));
                posOfThirdDot = hostt.FindC( UpnpString::KDot8() );

                //position of the dot in IP address must from 1 to 3 starting from the position of former dot					
                if( (posOfThirdDot < KErrNone ) || ((posOfThirdDot> 3 ) || (posOfThirdDot < 1)))
                    {
                    return EFalse;
                    }
                else
                    {
                    TPtrC8 ip3;
                    ip3.Set( hostt.Left( posOfThirdDot));

                    ip1def = ExtractNumFromIP(ip3);

                    if ((ip1def == KErrNotFound) || (ip1def> KIPNumMax))
                        {
                        return EFalse;
                        }

                    TPtrC8 ip4;
                    ip4.Set(hostt.Mid(posOfThirdDot + UpnpString::KDot8().Length()));

                    //position of the dot in IP address must from 1 to 3 starting from the position of former dot							
                    if((ip4.Length() < 1) || (ip4.Length()> 3))
                        {
                        return EFalse;
                        }

                    ip1def = ExtractNumFromIP (ip4);

                    if ((ip1def == KErrNotFound) || (ip1def> KIPNumMax))
                        {
                        return EFalse;
                        }
                    else
                        {
                        return ETrue;
                        }

                    }
                }
            }
        }

    // -----------------------------------------------------------------------------
    // ExtractUrlPath
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TPtrC8 ExtractUrlPath(
        const TDesC8& aUrl, TPtrC8& aFilename, TPtrC8& aScheme, TInt& aParseError )
        {

        LOGS("CHttpFolderUrlMatch::ExtractUrlPath()");
        TUriParser8 up;
        aParseError = up.Parse(aUrl);
        if( aParseError != KErrNone)
            {
            return KNullDesC8();
            }
        TPtrC8 path(up.Extract(EUriPath));
        TPtrC8 scheme(up.Extract(EUriScheme));
        TPtrC8 port(up.Extract(EUriPort));
        TPtrC8 host(up.Extract(EUriHost));
        TInt position (0);
        TPtrC8 urlPath;

        if(up.IsPresent(EUriScheme))
            {

            if(scheme.Compare(UpnpHTTP::KSchemeHTTP8()) == 0)
                {
                aScheme.Set(scheme);

                //separator test
                position = aUrl.FindC(UpnpHTTP::KSchemeSeparator());
                if (position != KErrNotFound)
                    {
                    if ((up.IsPresent(EUriHost)) && (host != KNullDesC8()))
                        {
                        if( ExtractUrlPathHost(aUrl, aParseError ) )
                            {
                            TUint portdef(0);
                            TLex8 lex = TLex8(port);
                            TInt error = lex.Val(portdef, EDecimal);
                            if((up.IsPresent(EUriPort)) && (port != KNullDesC8()) && (error == KErrNone) )
                                {
                                urlPath.Set(path);
                                }
                            else
                                {
                                return KNullDesC8();
                                }
                            }
                        else
                            {
                            return KNullDesC8();
                            }
                        }
                    else
                        {
                        return KNullDesC8();
                        }
                    }
                else
                    {
                    return KNullDesC8();
                    }

                }
            else
                {
                return KNullDesC8();
                }
            }
        else
            {
            TInt posOfFirstSlashnohttp( 0 );
            posOfFirstSlashnohttp = path.FindC( UpnpString::KSlash() );

            if( (posOfFirstSlashnohttp < KErrNone ) || (posOfFirstSlashnohttp != 0))
                {
                return KNullDesC8();
                }
            else
                {
                urlPath.Set(aUrl);
                }
            }

        TInt posOfFirstSlash( 0 );
        TInt posOfLastSlash( 0 );
        posOfFirstSlash = urlPath.FindC( UpnpString::KSlash() );

        if( posOfFirstSlash < KErrNone )
            {
            return KNullDesC8();
            }
        else
            {
            TInt posOfTwoSlashes( 0 );
            posOfTwoSlashes = urlPath.FindC(UpnpString::KDoubleSlash());
            if((posOfTwoSlashes == KErrNone ) || (posOfTwoSlashes> KErrNone) )
                {
                return KNullDesC8();
                }
            posOfLastSlash = urlPath.LocateReverse( UpnpString::KSlash()[0] );
            if( posOfLastSlash == posOfFirstSlash )
                {
                aFilename.Set( urlPath.Mid( posOfLastSlash + UpnpString::KSlash().Length()) );
                return KNullDesC8();
                }
            else
                {
                aFilename.Set( urlPath.Mid( posOfLastSlash + UpnpString::KSlash().Length()) );
                urlPath.Set( urlPath.Mid( posOfFirstSlash + UpnpString::KSlash().Length() ) );
                urlPath.Set( urlPath.Left( posOfLastSlash - UpnpString::KSlash().Length() ) );
                return urlPath;

                }
            }
        }

    // -----------------------------------------------------------------------------
    // ExtractUrlPath
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TPtrC8 ExtractUrlPath(const TDesC8& aUrl, TInt& aParseError )
        {
        TPtrC8 fileName;
        TPtrC8 scheme;
        return ExtractUrlPath(aUrl, fileName, scheme, aParseError );
        }
    // -----------------------------------------------------------------------------
    // ExtractUrlPath
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TPtrC8 ExtractUrlPath(const TDesC8& aUrl, TPtrC8& aFilename, TInt& aParseError)
        {
        TPtrC8 scheme;
        return ExtractUrlPath(aUrl, aFilename, scheme, aParseError );
        }

    // -----------------------------------------------------------------------------
    // ConvertUrlToFolder
    // -----------------------------------------------------------------------------
    //
    EXPORT_C HBufC8* SwitchToBackSlashL(const TDesC8& aUrl)
        {
        TInt position( 0 );

        HBufC8* result = HBufC8::NewL(aUrl.Length() + aUrl.Length()/2);

        TPtr8 ptrResult(result->Des());
        ptrResult.Copy(aUrl);

        // replace / with '\\'
        while ( ( position = ptrResult.Locate( UpnpString::KSlash()[0] ) ) != KErrNotFound )
            {
            ptrResult.Replace( position, 1, UpnpString::KDoubleBackSlash() );
            }
        return result;

        }
    // -----------------------------------------------------------------------------
    // ConvertUrlToFolder
    // -----------------------------------------------------------------------------
    //
    EXPORT_C HBufC* SwitchToBackSlashL(const TDesC& aUrl)
        {
        TInt position( 0 );

        HBufC* result = HBufC::NewL(aUrl.Length() + aUrl.Length()/2);

        TPtr ptrResult(result->Des());
        ptrResult.Copy(aUrl);

        // replace / with '\\'
        while ( ( position = ptrResult.Locate( UpnpString::KSlash()[0] ) ) != KErrNotFound )
            {
            ptrResult.Replace( position, 1, UpnpString::KDoubleBackSlash16() );
            }
        return result;

        }
    // -----------------------------------------------------------------------------
    // Checks if the disk is short of the space requested
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TBool CheckDiskSpaceShortL( const TDesC16& aPath, TInt aSize, RFs& aFs )
        {
        TInt drive_num = UpnpFileUtil::DriveNumberFromPath( aPath, aFs );
        return SysUtil::DiskSpaceBelowCriticalLevelL ( &aFs, aSize, drive_num );
        }

    // -----------------------------------------------------------------------------
    // Checks if the disk is short of the space requested
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TBool CheckDiskSpaceShortL( TDriveNumber aDriveNumber, TInt aSize )
        {
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        TBool result = SysUtil::DiskSpaceBelowCriticalLevelL ( &fs, aSize, aDriveNumber );
        CleanupStack::PopAndDestroy( &fs );
        return result;
        }
     
    // -----------------------------------------------------------------------------
    // Parses uri
    // -----------------------------------------------------------------------------
    //
    EXPORT_C TInt ParseUri( const TDesC8& aUrl )
        {
        TUriParser8 up;
        return up.Parse( aUrl );
        }
    

    }

//  End of File
