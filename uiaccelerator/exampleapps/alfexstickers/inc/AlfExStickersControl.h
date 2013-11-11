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



#ifndef C_ALFEXSTICKERSCONTROL_H
#define C_ALFEXSTICKERSCONTROL_H


/* Includes */

#include <alf/alfcontrol.h>

//TEMP
#include <alf/alfdecklayout.h>


/* Forward declarations. */

class CAlfViewportLayout;
class CAlfExStickerLayout;
class CAlfImageLoaderUtil;


/* Constants */

//Number of example stickers
const TInt KStickerCount = 2;

/** 
 * @class CAlfExStickersControl
 * @discussion Control class for the AlfExSticker application. Handles creation of application elements
 * like viewport and sticker text visuals. Also handles user interaction with application elements.
 */
class CAlfExStickersControl : public CAlfControl
    {
public:

	/* Constructors and destructor. */
	
	/**
	 * @function NewL
	 * @discussion Constructs a new CAlfExStickersControl class instance. A static factory method.
	 * @param aEnv	UI Accelerator Toolkit environment object.
	 * @param aFs	A handle to a file server session.
	 * @result Control for the AlfExStickers application
	 */
	static CAlfExStickersControl* NewL(CAlfEnv& aEnv, RFs* aFs);
	
	/**
	 * @function NewLC
	 * @discussion Constructs a new CAlfExStickersControl class instance. A static factory method.
	 * @param aEnv	UI Accelerator Toolkit environment object.
	 * @param aFs	A handle to a file server session.
	 * @result Control for the AlfExStickers application
	 */
    static CAlfExStickersControl* NewLC(CAlfEnv& aEnv, RFs* aFs);
    
    /**
     * @function ~CAlfExStickersControl
     * @discussion 	Destructor
     */
    virtual ~CAlfExStickersControl();
    

    /* Methods. */
    
    /**
     * @function OfferEventL
     * @discussion Handles events from the framework
     * @param aEvent Catched event
     * @result Success or failure
     */    
    virtual TBool OfferEventL(const TAlfEvent& aEvent);
    
    /**
     * @function GetControl
     * @discussion Provides Control to stickers
     * @result CAlfControl to child visuals
     */    
    CAlfControl* GetControl();
    
    //Control for the visuals
    CAlfControl* iControl;
    
    
private:
	
	/* Constructors*/

	/**
	 * Constructs an instance of CAlfExStickersControl.
	 * @note Protected because this constructor only needs to be called by 
	 * derived classes. Use the provided static factory methods instead.
	 * @see NewL()
	 * @see NewLC()
	 */
    CAlfExStickersControl();
    
    /**
     * @function ConstructL
     * @discussion Second phase constructor
	 * @param aEnv	UI Accelerator Toolkit environment object.
	 * @param aFs	A handle to a file server session.
     */
    void ConstructL(CAlfEnv& aEnv, RFs* aFs);
    
    
    /* Methods. */
    
    /**
     * @function CreateViewportL
     * @discussion Creates Viewport layout to the application to act as the root layout.
     * Also function is used to set the size for the viewport and to enable scrolling, ie.
     * dragging interaction with the background
     */
    void CreateViewportL();
    
    /**
     * @function SetTextureManagerL
     * @discussion The function is used to find out path to applications graphical
     * resources and giving the path to Texture Manager. 
     */
    void SetTextureManagerL();
    
    /**
     * @function LoadBackgroundImageL
     * @discussion Function uses Texture Manager to load board (application background)
     * texture from a png-file. Then creates new image visual to cover application background and
     * adds the loaded board texture in the visual.
     */
    void LoadBackgroundImageL();
    
    /**
     * @function CreateSVGTextureL
     * @discussion Function used to create SVG texture from SVG file
     * @param aFileName SVG file filename.
     * @param aSize Texture size
     * @param aImageLoaderUtil Image loader utility.
     * @result Reference to created texture. This texture is owned by texture manager.
     */
    CAlfTexture& CreateSVGTextureL(const TFileName& aFileName, TSize& aSize, CAlfImageLoaderUtil* aImageLoaderUtil);
    
    /**
     * @function CreateStickersL
     * @discussion Function used to create several stickers and applying the needed
     * changes into the visuals.
     * 
     * Easiest way to create text visual would be to use:
     * CAlfTextVisual* visual = CAlfTextVisual::AddNewL(*this, iPlainLayout);
     * We have however created new class for the text visual (alfexsticker).
     */
    void CreateStickersL();

    /**
     * @function LoadStickerImageL
     * @discussion Function used to load sticker background image from SVG file.
     * @param aIndex An index number of sticker where background is loaded.
     */
    void LoadStickerImageL(TInt aIndex);
    
    /* Member variables */
    
    //UI Accelerator Toolkit environment object
    CAlfEnv* iEnv;
    
    //Handle to file server session
    RFs* iFs;

    //Texture path
    TFileName iPrivateFolderPath;
    
    //Layouts && Co
    CAlfViewportLayout* iViewPort;
    CAlfLayout* iPlainLayout;
    
    //Application view and viewport sizes
    TSize iVirtualSize;
    TSize iLayoutsize;
    
    //Sticker array
    CAlfExStickerLayout* iStickerArray[KStickerCount];
    
    //Default timed values
    TAlfTimedValue x;
    TAlfTimedValue y;
    
    //Member data needed to handle interaction, like drag and drop
    CAlfVisual* iOnVisual;
    CAlfExStickerLayout* iSelectedDeck;
    TBool iDraggingVisual;
    TPoint iStartVisualPos;    // Original position of dragged visual.
    TPoint iStartDisplayPoint; // Point of display for Pointer Down.
    TAlfRealPoint iDraggedPosition;
    TAlfRealPoint iViewPos;
    TBool iHasBeenDragged;
    
    //Image loader array.
    RArray<CAlfImageLoaderUtil*> iImageLoaderUtilArray;
    };

#endif // C_ALFEXSTICKERSCONTROL_H
