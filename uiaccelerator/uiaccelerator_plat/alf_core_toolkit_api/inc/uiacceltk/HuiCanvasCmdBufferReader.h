/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#ifndef __HUICANVASCMDBUFFERREADER_H__
#define __HUICANVASCMDBUFFERREADER_H__


#include <e32base.h>
#include <s32mem.h>
#include <bitstd.h>
#include <fbs.h>
#include <gdi.h>

	class THuiCanvasTextParameters
		{
	public:
		THuiCanvasTextParameters():
			iStart(0),
			iEnd(KMaxTInt),
			iFlags(0)
				{
				}
		TInt iStart;
		TInt iEnd;
		TUint16 iFlags;
		/* Reserved for future use */
		TAny* iReserved1;
		TAny* iReserved2;
		TAny* iReserved3;
		TAny* iReserved4;
		};

class CHuiCanvasCmdBufferReader : public CBase
    {
public:

	/**
	 * Constructor.
	 */
	CHuiCanvasCmdBufferReader();

	/**
	 * Constructor.
	 */
	CHuiCanvasCmdBufferReader(RMemReadStream aStream);

	/**
	 * Destructor.
	 */ 
	~CHuiCanvasCmdBufferReader();

    /**
     * Initialization, opens a stream for reading commandbuffer.
     *
     * @since S60 ?S60_version
     * @param 
     * @param 
     */
    void Init( TPtr8& aDes,TInt aLength);

    /**
     * ReadInt32L
     * Convinience function for reading a TInt32 from stream. 
     *
     * @since S60 ?S60_version
     * @param aPoint  
     */
    void ReadInt32L( TInt& aValue );
        
    /**
     * ReadPointL
     * Convinience function for reading a TPoint from stream
     *
     * @since S60 ?S60_version
     * @param aPoint  
     */
    void ReadPointL( TPoint& aPoint );
    
    /**
     * ReadRectL
     * Convinience function for reading a TRect from stream
     *
     * @since S60 ?S60_version
     * @param TRect  
     */
    void ReadRectL(TRect& aRect );
    
    /**
     * ReadBitmapL
     * Convinience function for reading a bitmap from stream. It duplicates the bitmap from a bitmap handle.
     *
     * @since S60 ?S60_version
     * @param Bitmap
     */
    void ReadBitmapLC( CFbsBitmap*& aBitmap  );

    /**
     * ReadBitmapHandleL
     * Convinience function for reading a bitmap from stream. It duplicates the bitmap from a bitmap handle.
     *
     * @since S60 ?S60_version
     * @param Bitmap
     */
    void ReadBitmapHandleL( TInt& aBitmapHandle  );
    
    /**
     * ReadTextL
     * Returns TPtr to a text in the command buffer stream. Note, that this 
     * function does not make a copy of the text.
     *
     * @since S60 ?S60_version
     * @param aPoint  
	 * @return EFalse, if TTextParameters are not legal. Otherwise ETrue.
     */
    TBool ReadTextL( TPtr& aText, THuiCanvasTextParameters& aTextParameters );
    
    /**
     * ReadPointArrayL
     * Convinience function for reading a pointer array from stream
     *
     * @since S60 ?S60_version
     * @param Pointer array
     */
    void ReadPointArrayL(CArrayFix<TPoint>*& aPointArray );
    
    /**
     * ReadPointArrayL
     * Convinience function for reading a pointer array from stream
     *
     * @since S60 ?S60_version
     * @param Pointer array
     */
    void ReadPointArrayL( TPoint*& aPointArray, TInt& aNumPoints );
    
    /**
     * ReadRegionL
     * Convinience function for reading a RRegion from stream
     *
     * @since S60 ?S60_version
     * @param Region  
     */
    void ReadRegionL( RRegion& aRegion );
    
    /**
     * DeletePointArrayL
     * Convinience function deleting the point array
     *
     * @since S60 ?S60_version
     * @param aPointArray
     * @param aNumPoints    Number of points
     */
    void DeletePointArrayL( TPoint*& aPointArray, TInt& aNumPoints );
        
    /**
     * ReadColorL
     * Convinience function for reading a TRgb from stream
     *
     * @since S60 ?S60_version
     * @param TRgb  
     */
    void ReadColorL( TRgb& aColor );
    
    /**
     * ReadIntsL
     * Convinience function for reading maximum of 5 TInts from the stream
     *
     * @since S60 ?S60_version
     * @param TInt array  
     */
    void ReadIntsL( TInt aCount, TInt aValues[5] );

    
    /**
     * ReadInt8L
     * Convinience function for reading Int8 from stream
     *
     * @since S60 ?S60_version
     */    
    inline TUint8 ReadInt8L( ){ return iStream.ReadInt8L();}

    inline void ReadL( TUint8* aTargetPtr, TInt aLength )
        {
        iStream.ReadL( aTargetPtr, aLength );
        }
    /**
     * Convinience function for getting a reference to stream.
     *
     * @since S60 ?S60_version
     */
    RMemReadStream& Stream();

private:

    /*
     * Readstream for the command buffer.
     */
    RMemReadStream iStream;

    TInt iBufferIndex;    

    TInt iBufferLength; 
    
    TPtr8* iBasePtr;

    TPtr16* iCachedText;
    
    };

#endif        
    
