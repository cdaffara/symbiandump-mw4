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
* Description: 
*
*/



#ifndef C_ALFEXSTICKER_H
#define C_ALFEXSTICKER_H


/* Includes */

#include <alf/alfcontrol.h>
#include "alfexstickerscontrol.h"


/* Forward declarations. */


/* Constants */
const TInt KStickerHeight = 300;
const TInt KStickerWidth = 300;


/** 
 * @class CAlfExStickerLayout
 * @discussion Wrapper class for the deck layout, text visuals and some brushes. 
 * Is used to create Stickers from the combination of those.
 * Handles placement, content and outlook of the sticker text visuals. 
 * Should probable rename the class for clarity
 */
class CAlfExStickerLayout : public CAlfDeckLayout
    {
public:

	/* Constructors and destructor. */
	
	/**
	 * @function NewL
	 * @discussion Constructs a new CAlfExStickerLayout class instance. A static factory method.
	 * @param aControl Control class for the AlfExSticker application.
	 * @param aEnv	UI Accelerator Toolkit environment object.
	 * @result Instance of AlfExSticker
	 */
	static CAlfExStickerLayout* NewL(CAlfExStickersControl& aControl, CAlfEnv& aEnv);
    
    /**
     * @function ~CAlfExStickerLayout
     * @discussion 	Destructor
  	*/
    virtual ~CAlfExStickerLayout();
    
    
    /* Methods. */
    
	/**
     * @function SetStickerPlace
     * @discussion Used to place sticker on the layout.
     * @todo Maybe make the placement more dynamic
    */ 
    void SetStickerPlaceL(CAlfExStickersControl* aControl);
    
    /**
     * @function SetStickerTextContentL
     * @discussion Used to set textual content of the sticker, 
     * or more specific content of the included text visual.
     * Also configures font, color and placement of the textual content
     * @todo Maybe make the content input more dynamic
    */ 
    void SetStickerTextContentL();
    
    /**
     * @function SetStickerDropShadowBrushL
     * @discussion Enables brushes for sticker and then add drop shadow brush for the sticker.
     * @todo Add drop shadow also in to other sides in lesser quanity, that way stickers are more
     * easile recognizable.
    */ 
    void SetStickerDropShadowBrushL();
    
    /**
     * @function SetStickerImageBrushL
     * @discussion Add sticker background image by using an image brush.
     * @note EnableBrushes() must be called before using any brushes, we did this already in
     * SetStickerDropShadowBrushL().
     * @todo Sticker needs to implement changing backgrounds in the future.
     * @param aTexture Texture that is placed for image brush.
    */ 
    void SetStickerImageBrushL(CAlfTexture& aTexture);
    
    /**
     * @function GetVisual
     * @discussion Returns the text visual used in sticker.
     * @result Returns text visual
    */ 
    CAlfVisual* GetVisual();
    
    /**
     * @function ChangeStickerSize
     * @discussion Scales text visual and resizes the deck layout
     * Is called when user taps on a sticker to zoom in or out.
    */ 
    void ChangeStickerSizeL();

    
private:

	/* Constructors and destructor. */

	/**
	 * Constructs an instance of CAlfExStickerLayout.
	 * @note Protected because this constructor only needs to be called by 
	 * derived classes. Use the provided static factory methods instead.
	 * @see NewL()
	 * @see NewLC()
	 */
    CAlfExStickerLayout();
    
    /**
     * @function ConstructL
     * @discussion Second phase constructor
 	 * @param aControl Control class for the AlfExSticker application.
	 * @param aEnv	UI Accelerator Toolkit environment object.
     */
    void ConstructL(CAlfExStickersControl& aControl, CAlfEnv& aEnv);
    
    
    /* Member variables */
    
    //UI Accelerator Toolkit environment object
    CAlfEnv* iEnv;
    
    //AlfExSticker Control
    CAlfExStickersControl* iControl;
    
    //Text visual
    CAlfTextVisual* iTextVisual;
    
    //Helps to determine current size/scale
    TBool iStickerIsLarge;

    };

#endif // C_ALFEXSTICKER_H
