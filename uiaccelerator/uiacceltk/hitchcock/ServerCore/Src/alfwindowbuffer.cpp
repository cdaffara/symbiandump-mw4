/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include <fbs.h>
#include <bitdev.h>
#include "alfwindowbuffer.h"

// ---------------------------------------------------------------------------
// TAlfWindowBufferData class declaration
// ---------------------------------------------------------------------------
//
class CAlfWindowBuffer::TAlfWindowBufferData
    {
    public:   
    TSize iSize;
    TInt iFormat;
    TUint iStride;
    TAny* iBufferPtr;
    CFbsBitmap* iTemp; // RnD enablers
    };

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfWindowBuffer* CAlfWindowBuffer::NewL( const TSize& aSize, 
                                            TInt aBufferFormat, 
                                            TUint aStrideInBytes, 
                                            TAny* aPtr)
    {
    CAlfWindowBuffer* self = new (ELeave) CAlfWindowBuffer;
    CleanupStack::PushL(self);
    self->ConstructL(aSize, aBufferFormat, aStrideInBytes, aPtr);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfWindowBuffer::~CAlfWindowBuffer()
    {
    if (iData)
        {
        delete iData->iTemp;
        delete iData;
        }
    }
        
// ---------------------------------------------------------------------------
// Size
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CAlfWindowBuffer::Size() const
    {
    if (iData)
        {
        return iData->iSize;
        }
    return TSize(0,0);
    }
    
// ---------------------------------------------------------------------------
// Format
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfWindowBuffer::Format() const
    {
    if (iData)
        {
        return EColor16MA;
        //return iData->iFormat;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// Stride
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CAlfWindowBuffer::Stride() const
    {
    if (iData)
        {
        return iData->iTemp->DataStride();
        //return iData->iStride;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// BufferPtr
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CAlfWindowBuffer::BufferPtr() const
    {
    if (iData)
        {
        return iData->iTemp->DataAddress(); // currently safe...
        //return iData->iBufferPtr;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// SetSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindowBuffer::SetSize(const TSize& aSize)
    {
    iData->iSize = aSize;
    //RnD
    iData->iTemp->Resize(aSize);
    }

// ---------------------------------------------------------------------------
// SetFormat
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindowBuffer::SetFormat(TInt aBufferFormat)   
    {
    iData->iFormat = aBufferFormat;
    }

// ---------------------------------------------------------------------------
// SetStride
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindowBuffer::SetStride(TUint aStrideInBytes)
    {
    iData->iStride = aStrideInBytes;
    }
    
// ---------------------------------------------------------------------------
// SetBufferPtr
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindowBuffer::SetBufferPtr(TAny* aPtr) const
    {
    iData->iBufferPtr = aPtr;
    }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfWindowBuffer::CAlfWindowBuffer()
    {
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfWindowBuffer::ConstructL(const TSize& aSize, TInt aBufferFormat, TUint aStride, TAny* aPtr)
    {
    iData = new (ELeave)TAlfWindowBufferData;
    memset(iData,0,sizeof(TAlfWindowBufferData));
    iData->iBufferPtr = aPtr;
    iData->iFormat = aBufferFormat;
    iData->iSize = aSize;
    iData->iStride = aStride;    
    // RnD
    iData->iTemp = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iData->iTemp->Create(aSize, EColor16MA));
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
CAlfWindowBuffer* CAlfWindowBuffer::CreateCopyL(TBool aCopyContents )
    {
    CAlfWindowBuffer* copy = CAlfWindowBuffer::NewL( iData->iSize, 
                                            iData->iFormat, 
                                            iData->iStride, 
                                            iData->iBufferPtr);
    
    
    if (aCopyContents)
        {
        // Todo: memcopy would be likely faster and this works for bmp only..
        CleanupStack::PushL(copy);
        CFbsBitmapDevice* bd = CFbsBitmapDevice::NewL( copy->Bmp() );
        CleanupStack::PushL(bd);
        CFbsBitGc * gc = 0; 
        User::LeaveIfError(bd->CreateContext( gc ));
        gc->BitBlt(TPoint(0,0), Bmp());
        delete gc;
        CleanupStack::PopAndDestroy(2);
        }
       

    
    return copy;
    }



// ---------------------------------------------------------------------------
// Handle
// ---------------------------------------------------------------------------
//
TInt CAlfWindowBuffer::Handle()
    {
    if (iData && iData->iTemp)
        {
        return iData->iTemp->Handle();
        }
    
    return 0;
    }
    
// ---------------------------------------------------------------------------
// Bmp
// ---------------------------------------------------------------------------
//
CFbsBitmap* CAlfWindowBuffer::Bmp()
    {
    if (iData && iData->iTemp)
        {
        return iData->iTemp;
        }
    
    return 0;
    }    
