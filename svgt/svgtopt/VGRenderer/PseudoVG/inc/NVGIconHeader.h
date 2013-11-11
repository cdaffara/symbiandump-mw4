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
* Description:  NvgIconHeader header file
 *
*/


#ifndef _NVGICONHEADER_H_
#define _NVGICONHEADER_H_

#include <e32std.h>

/**
* TNVGHeaderData
* 
* Datastructure to hold NVG Icon Header information
*/
struct TNVGHeaderData         // be aware of padding!
{
    TUint16   sign;             
    TUint8    version;
    TUint8    headerSize;
    TUint32   reserved1;
    TUint16   reserved2;
    TUint8    reserved3;
    TUint8    reserved4:4;                // flag
    TUint8    isMarginCorrection:1;       // flag
    TUint8    isMask:1;                   // flag
    TUint8    aspectRatio:2;              // flag
    TInt32    rotation;
    TInt32    iconColor;
    TInt32    bitmapid;
};



// CONSTANTS
static const TInt KIconHeaderLength = sizeof(TNVGHeaderData);

// FIXME: find an appropriate place for me.

/**
* Class TNVGIconHeader
* 
* Class to hold NVG Icon Header information & operations
*/
class TNVGIconHeader
    {
    
public:
	  /**
    * Constructor
    * 
    * @param aData      Descriptor to store the icon header data
    */
    inline TNVGIconHeader(TDes8 &aData)
        {
        __ASSERT_ALWAYS(aData.Length() >= KIconHeaderLength, User::Panic(_L("not sufficient size"), KErrBadDescriptor));
        iHeader = (TNVGHeaderData *)(aData.LeftTPtr(KIconHeaderLength).Ptr());
        }

    /**
    * Initializes icon header before writing any data
    * 
    * @param None
    */
    inline void Initialize()
        {
        Mem::FillZ(iHeader,KIconHeaderLength);
        iHeader->headerSize = KIconHeaderLength; 
        }

    
		/**
    * Set the mask flag in icon header 
    * 
    * @param isMask          Flag to turn mask on/off
    */ 
    inline void SetIsMask(TBool isMask)
        {
        iHeader->isMask = isMask;
        }

    /**
    * Get the mask flag from icon header
    * 
    * @param None
    */
    inline TBool IsMask() const
        {
        return iHeader->isMask;
        }
 
private:  

    TNVGHeaderData *iHeader;
    };

#endif 

