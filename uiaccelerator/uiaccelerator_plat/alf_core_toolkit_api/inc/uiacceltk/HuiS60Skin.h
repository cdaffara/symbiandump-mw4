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
* Description:   Definition of CHuiS60Skin. CHuiS60Skin is a HUITK skin that uses 
*                resources from the current S60 skin.
*
*/



#ifndef __HUIS60SKIN_H__
#define __HUIS60SKIN_H__


#include <e32base.h>
#include <avkon.hrh>
#include <AknsUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/huidisplaybackgrounditem.h>
#include <AknsSrvClient.h>

/**
 * An S60 skin implementation. Uses bitmaps, fonts, and layout from the 
 * currently active S60 skin.
 */
class CHuiS60Skin : public CHuiSkin, public MAknsSkinChangeObserver
	{
public:

	/* Constructors and destructor. */

	/**
	 * Constructor.
	 */
	IMPORT_C CHuiS60Skin(CHuiEnv& aEnv);

	/**
	 * From CHuiSkin. Second-phase constructor.
	 */
	IMPORT_C void ConstructL();

	/**
	 * Destructor.
	 */
	IMPORT_C ~CHuiS60Skin();


	/* Methods from MAknsSkinChangeObserver */
	IMPORT_C void SkinContentChanged();

	IMPORT_C void SkinConfigurationChanged(
	          const TAknsSkinStatusConfigurationChangeReason aReason );
	  
	IMPORT_C void SkinPackageChanged(
	         const TAknsSkinStatusPackageChangeReason aReason );
public:

    /**
	 * From CHuiSkin. Determines the preferred location for an element of the skin.
     * The location of skin elements is determined with Avkon layout utilities.
	 *
	 * @param aElement  Element whose location to determine.
	 *
	 * @return Location specifier.
     */
    IMPORT_C THuiSkinLocation Location(THuiSkinElement aElement);

	/**
	 * From CHuiSkin. Returns a Series60 theme specific background image when
	 * parameter EHuiSkinBackgroundTexture is specified.
	 */
	IMPORT_C const CHuiTexture& TextureL(TInt aSkinTextureId);
	
    /**
     * From CHuiSkin. Release a skin texture.
     * Will free up the given texture used by the skin.
     * @param aSkinTextureResource The THuiSkinTextureResource skin texture ID to free.
     */
	IMPORT_C void ReleaseTexture(TInt aSkinTextureResource);	
	
    /**
     * From CHuiSkin. Notification received when display size has changed.
     */
    IMPORT_C void NotifyDisplaySizeChangedL();

    /**
     * From CHuiSkin. Get text color based on style and background.
     * Determines the colour to use for text, based on the current text style
     * and the background type.
     * @param aStyle The text style that is being used.
     * @param aBackgroundType The background type that is currently under the text.
     * @return The TRgb value of the text color to use.
     */
    IMPORT_C TRgb StyleTextColor(THuiPreconfiguredTextStyle aStyle, 
                                         THuiBackgroundType aBackgroundType) const;
       
    /**
     * From CHuiSkin. Restore content of texture.
     * Restore content of the given texture.
     * @param aTexture Texture to restore the content of.
     */
    IMPORT_C void RestoreTextureContentL(CHuiTexture& aTexture);

    /** ! From CHuiSkin.*/
    IMPORT_C THuiSkinOrientation Orientation() const;     
    IMPORT_C void SetContext(CHuiGc& aGc);
	IMPORT_C CHuiGc& Context() const;
    IMPORT_C TInt GetTexture(TInt aSkinTextureResource, const CHuiTexture*& aOutTexture);	
	
    /**
     * Return the skin control context.
     */
	inline CAknsBasicBackgroundControlContext* SkinControlContext() const
	{
		return iSkinControlContext;
	}
    void UpdateBackgroundsL(const RArray<THuiDisplayBackgroundItem>& aItems);
    IMPORT_C CHuiTexture* BackgroundTexture(const TAknsItemID& aID);
    
    TRect SkinRect(const TAknsItemID& aID);
    
protected: // from CHuiSkin
    IMPORT_C void SkinExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);
    void FreeBackgrounds();
    CHuiTexture* CreateSkinBackgroundL(const TAknsItemID& aID);
    void ReloadBgTexturesL();
protected:
    
    /* Methods. */

    MAknsSkinInstance* SkinInstance() const;

    /**
     * Copies the S60 skin background bitmap onto a texture.
     */
    void UpdateBackgroundL();

private:
    
    void FreeAllBackgroundTextureResources();

private:

    /** Control context for the skin. */
    CAknsBasicBackgroundControlContext* iSkinControlContext;

    /** Background texture from S60. */
    CHuiTexture* iBackgroundTexture;
    CFbsBitmap* iBackgroundBitmap;
    TRect iBackgroundRect;

    /** To get CallBack from SkinServer when skin is changed */
    RAknsSrvSession iSkinSrvSession;
    TBool iSkinSrvConnected;
     
    /** Background should be reloaded the next time it is needed. */
    TBool iReloadBackground;

    class CSkinItem : public CBase
        {
        public :
        
        ~CSkinItem()
            {
            delete iBitmap;
            };
        TAknsItemID iId;
        TRect iSkinRect;
        CFbsBitmap* iBitmap;
        };
    RPointerArray<CSkinItem> iCachedSkinItems;
    
    TAny* iSpare;    

private: // helpers
    
    TInt SearchCachedSkinItemIndex(const TAknsItemID& aId);
    TRect SearchCachedSkinItemRect(const TAknsItemID& aId);
    CFbsBitmap* SearchCachedSkinItemBitmap(const TAknsItemID& aId);
	};


#endif  // __HUIS60SKIN_H__
