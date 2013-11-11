/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Symbian screenbuffer drawing IF
*
*/




#ifndef __ALFSYMBIANSBDRAWER_H__
#define __ALFSYMBIANSBDRAWER_H__


#include "alf/alfscreenbuffer.h"

const TUid KAlfSymbianBufferDrawerUid = {0x2000e5a2}; 

class CBitmapContext;
class CFbsBitmap;

/**
 * The class for drawing screen buffer to a bitmap context.
 *  
 * Usage:
 * @code
 * 
 * //Create screen buffer instance
 * CAlfScreenBuffer* screenBuffer = CAlfScreenBuffer::NewL( *iEnv );
 * //add the candidate , who has implemented MAlfScreenBufferObserver interface, as observer
 * 
 * screenBuffer->AddObserverL( testScreenBufferUid, observer );
 * 
 *  //Get symbian bufferdrawer instance
 * CAlfSymbianBufferDrawer* drawer = ( CAlfSymbianBufferDrawer* )screenBuffer->
 * 										GetDrawingInterface( SYMBIAN_BUFFER_DRAWER_UID, 
 * 															 testScreenBufferUid );
 * 
 *  // Draw
 *  CFbsBitGc* context = NULL;
 *  User::LeaveIfError( device->CreateContext( context ) );
 *  drawer->DrawBuffer( context, TPoint(), iDisplayRect );
 * 
 * 
 * // release
 * 	drawer->ReleaseDrawer();
 * 	drawer = NULL;
 * 
 * // Cleanup  
 * 	drawer->ReleaseDrawer();
 * 	drawer = NULL;
 * 	screenBuffer->RemoveObserver( testScreenBufferUid );
 *  delete screenBuffer;
 * 
 * @endcode
 * 
 * @see ScreenBuffer::getDrawingInterface()
 * @lib osnalf.lib
 * @since S60 xxx
 */
NONSHARABLE_CLASS(CAlfSymbianBufferDrawer) : public CBase, public MAlfBufferDrawer
    {
    public:    
    	/**
     	 * Draw buffer to the bitmap context.
     	 *
     	 * @param aContext Drawing context.
     	 * @param aPos Position on destination.
     	 * @param aRect Source rectangle.
     	 */
        IMPORT_C void DrawBuffer(CBitmapContext& aContext, const TPoint& aPos, const TRect& aRect);
        
    	/**
     	 * Draw buffer to the bitmap context.
     	 *
     	 * @param aContext Drawing context.
     	 * @param aPos Position on destination.
     	 * @param aRect Source rectangle.
     	 */
        IMPORT_C void DrawBufferMasked(CBitmapContext& aContext, const CFbsBitmap* aMask,const TPoint& aPos, const TRect& aRect);
                
    	/**
     	 * Get pointers to buffer bitmaps. If the screen buffer is not yet ready, NULL pointers are returned. 
     	 * This method does not get the ownership of the bitmaps. 
     	 *
     	 * @param aBitmap Buffer bitmap.
     	 * @param aMask Buffer mask.
     	 */
   		IMPORT_C void GetBufferBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask) const;

    public: // from MAlfBufferDrawer        
        IMPORT_C void ReleaseDrawer();
        
    protected:    
    	/**
     	 * Constuctors and destructor
     	 */
        CAlfSymbianBufferDrawer();
        
        virtual ~CAlfSymbianBufferDrawer();
        
        static CAlfSymbianBufferDrawer* NewL();
        
        void ConstructL();
        
    	/**
     	 * Setup for drawing.
     	 *
     	 * @param aBitmapHandle Screen buffer bitmap handle.
     	 * @param aMaskHandle Mask for the bitmap.
     	 */
        void SetBufferL(TInt aBitmapHandle, TInt aMaskHandle);
        
        
    private:
        struct TPrivData;
        TPrivData* iPrivData;
        friend class CAlfScreenBuffer;
    };

#endif