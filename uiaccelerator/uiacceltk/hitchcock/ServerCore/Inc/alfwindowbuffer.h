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



#ifndef __ALFWINDOWBUFFER_H__
#define __ALFWINDOWBUFFER_H__

#include <e32base.h>

class CFbsBitmap;

/**
 *  CAlfWindowBuffer
 *
 *  @lib alfappservercore.dll
 *  @since S60 5.0
 */
NONSHARABLE_CLASS(CAlfWindowBuffer): public CBase
    {
public:
    /**
     * ConstructL
     *
     * @since S60 5.0
     * @param aSize Size of the window
     * @param aBufferFormat See TDisplayMode for further details
     * @param aStride   Stride of the window
     * @param aPtr  Pointer to a buffer
     */
    IMPORT_C static CAlfWindowBuffer* NewL( const TSize& aSize, 
                                        TInt aBufferFormat, 
                                        TUint aStrideInBytes, 
                                        TAny* aPtr);
    IMPORT_C ~CAlfWindowBuffer();
  
    /**
     * Size
     *
     * @since S60 5.0
     * @return  The size of the window
     */
    IMPORT_C TSize Size() const;
    
    /**
     * Format
     *
     * @since S60 5.0
     * @return  The format of the window
     */
    
    IMPORT_C TInt Format() const;

    /**
     * Stride
     *
     * @since S60 5.0
     * @return  The stride of the window
     */
    IMPORT_C TUint Stride() const;
   
    /**
     * BufferPtr
     *
     * @since S60 5.0
     * @return pointer to actual presentation
     **/
    IMPORT_C TAny* BufferPtr() const;
    
    /**
     * SetSize
     *
     * Sets the size of the window
     * @since S60 5.0
     * @param  aSize
     */
    IMPORT_C void SetSize(const TSize& aSize);
   
    /**
     * SetFormat
     *
     * @since S60 5.0
     * @param  aBufferFormat
     */
    IMPORT_C void SetFormat(TInt aBufferFormat);
   
    /**
     * SetStride
     *
     * @since S60 5.0
     * @param  aStrideInBytes   The stride of the window
     */
    IMPORT_C void SetStride(TUint aStrideInBytes);
    
    /**
     * SetBufferPtr
     *
     * @since S60 5.0
     * @param   aPtr
     */
    IMPORT_C void SetBufferPtr(TAny* aPtr) const;
    
    // for RnD only
    TInt Handle();
    
    CFbsBitmap* Bmp();
    
    CAlfWindowBuffer* CreateCopyL(TBool aCopyContents = EFalse);
    
private:
    
    /**
     * ConstructL
     *
     * @since S60 5.0
     * @param aSize Size of the window
     * @param aBufferFormat See TDisplayMode for further details
     * @param aStride   Stride of the window
     * @param aPtr  Pointer to a buffer
     */
    void ConstructL(const TSize& aSize, TInt aBufferFormat, TUint aStride, TAny* aPtr);
    
    CAlfWindowBuffer();
    
    class TAlfWindowBufferData;
    
    /*
     *  Window data
     *  Own.
     */
    TAlfWindowBufferData* iData;
    }; 

#endif