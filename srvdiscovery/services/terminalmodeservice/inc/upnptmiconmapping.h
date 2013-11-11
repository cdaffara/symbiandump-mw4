/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmIconMapping class declaration.
*
*/

#ifndef __UPNPTMICONMAPPING_H__
#define __UPNPTMICONMAPPING_H__

// Include Files
#include <e32base.h>
#include <badesca.h>
#include "upnptmclienticonpref.h"

// Constants
const TInt KExpectedMaxIconParms = 20;

/**
 * Class maintains a mapping between application icon filepath and its URL. It also 
 * determines whether the conversion is needed for a particular requested 
 * application icon( http request ) and finalizes the dimensions of the icon which needs 
 * to be served
 */
class CUpnpTmIconMapping : public CBase
    {
public:
    static CUpnpTmIconMapping* NewL( );
    ~CUpnpTmIconMapping();
    
public:
    void AddIconFileInfoL( const TDesC8& aUrl, const TDesC& aPath, TInt aWidth, TInt aHeight );
    TInt GetSvgIconPath( const TDesC8& aUrl,RBuf& aSvgFilePath );
    TBool IsConversionNeeded( TInt aIndex, const CUpnpTmClientIconPref& aClientIconPref,
                                                       TInt& aIconWidth, TInt& aIconHeight );
    
private:
    CUpnpTmIconMapping();
    void ConstructL();

private:
    CDesC8Array*              iFileUrlArray;
    CDesCArray*               iFilePathArray;
    RArray<TInt>              iWidthArray;
    RArray<TInt>              iHeightArray;
    };

#endif // __UPNPTMICONMAPPING_H__
