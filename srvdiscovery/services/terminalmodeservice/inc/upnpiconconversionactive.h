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
* Description: CUpnpIconConversionActive class declaration.
*
*/

#ifndef __UPNPICONCONVERSIONACTIVE_H__
#define __UPNPICONCONVERSIONACTIVE_H__

// Include Files
#include <imageconversion.h>

/** 
 * Class encapsulates an active object (AO) which handles the actual image conversion 
 * in the new thread created.
 */ 
class CUpnpIconConversionActive: public CActive 
    {
public:
    static CUpnpIconConversionActive* NewL( RFile& aBitmapFile );
    ~CUpnpIconConversionActive();
    
    void Convert( CFbsBitmap& aBitmap );
    TInt FetchError();
    void RunL();
    void DoCancel();
    
private :
    CUpnpIconConversionActive();
    void ConstructL( RFile& aBitmapFile );
        
private:
    /**
     * CImageEncoder class provides functions that convert image data held 
     * in CFbsBitmap objects into well know formats( "bitmap" in this case ) 
     */
    CImageEncoder*                iEncoder; 
    };

#endif  //__UPNPICONCONVERSIONACTIVE_H__
