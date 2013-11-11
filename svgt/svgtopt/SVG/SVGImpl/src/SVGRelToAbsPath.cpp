/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#include <f32file.h>

#include "SVGRelToAbsPath.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt TSvgRelToAbsPath::GetFullPath( TParse& aParse,
                                    const TText8* upath,
                                    RFs& aSession,
                                    TDes* aFileName )
    {
    TInt r;
    TBuf<4> drive;
    TFileName nextBit;
    TText8 c =* upath;

    if ( c && upath[1] == KDriveDelimiter )
        {
        // drive name specified
        drive.Copy( TPtrC8( upath, 2 ) );
        drive.UpperCase();
        upath += 2;
        drive.Append( TChar( KPathDelimiter ) );
        }
    else
        {
        // no leading drive specifier
        drive.Zero();
        if ( c == KPathDelimiter || c == '/' )
            {
            upath += 1;
            drive.Append( TChar( KPathDelimiter ) );
            }
        }

    r = aSession.Parse( drive, aParse );

    // upath now looks like a relative pathname, to be added onto
    // aParse a directory at a time.

    c = *upath;
    while ( c && ( r == KErrNone ) )
        {
        const TText8* ustart = upath;
        do
            c = *upath++;
        while ( c && c != KPathDelimiter && c != '/' );

        TInt len = ( upath - ustart ) - 1; // excludes delimiter
        if ( len == 0 )
            continue;
        if ( ustart[0] == '.' )
            {
            if ( len == 1 )
                continue; // directory . ignored
            if ( len == 2 && ustart[1] == '.' )
                {
                // directory ..
                aParse.PopDir();
                continue;
                }
            }
        if ( len >= KMaxFileName )
            return KErrBadName;
        if ( c == '\0' && aFileName != NULL )
            {
            // it's the trailing filename
            aFileName->Copy( TPtrC8( ustart, len ) );
            break;
            }
        else
            {
            // it's a component of the accumulating path
            nextBit.Copy( TPtrC8( ustart, len ) );
            r = aParse.AddDir( nextBit );
            }
        }
    return( r );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt TSvgRelToAbsPath::GetFullFile( TFileName& aName,
                                    const TText8* upath,
                                    RFs& aSession )
    {
    TParse path;
    TInt err = GetFullPath( path, upath, aSession, & aName );
    if ( !err )
        {
        err = path.SetNoWild( path.DriveAndPath(), NULL, & aName );
        if ( !err )
            aName = path.FullName();
        }
    return err;
    }


