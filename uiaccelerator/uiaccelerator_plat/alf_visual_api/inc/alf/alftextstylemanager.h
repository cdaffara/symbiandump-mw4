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
* Description:   Text style manager class.
*
*/



#ifndef C_ALFTEXTSTYLEMANAGER_H
#define C_ALFTEXTSTYLEMANAGER_H

#include <e32base.h>

/** 
 * Preconfigured text styles.
 * Matching text styles always exist in the text style manager.
 * It is possible to retrieve the associated text style object through
 * CAlfTextStyleManager::TextStyle() - interface.
 */
enum TAlfPreconfiguredTextStyle
    {
    EAlfTextStyleNormal = 0,
    EAlfTextStyleNormalItalic,
    EAlfTextStyleMenuItem,
    EAlfTextStyleSmall,
    EAlfTextStyleMenuSmall,
    EAlfTextStyleMenuTitle,
    EAlfTextStyleLarge,
    EAlfTextStyleSoftkey,
    EAlfTextStyleSupplement,
    EAlfTextStyleTitle,
    
    /** Not a default alf style, will be treated as normal. */
    EAlfTextStyleNotAHuiStyle = EAlfTextStyleNormal
    };

// Forward declarations.
class CAlfTextStyle;
class CAlfTextVisual;
class CAlfEnv;

/**
 * Text style manager class. This class provides means to create and
 * query text styles that will be used to render text.
 * Usage:
 * 
 * @code
 *  //Get Textstyle Manager
 * CAlfTextStyleManager* styleMgr = &iEnv->TextStyleManager();
 * 
 *  // Create a textstyle, by passing fontid and preconfigured textstyle as parent id
 * TInt newStyleId= styleMgr->CreatePlatformTextStyleL( fontStyleId,
 * 							 EAlfTextStyleNormal );
 * CAlfTextStyle* newStyle =  styleMgr->TextStyle( newStyleId );
 * 
 * //Copy textstyle
 * TInt copiedId =  styleMgr->CopyTextStyle( newStyleId );
 * CAlfTextstyle* copiedStyle =  styleMgr->TextStyle( copiedId );
 * 
 * //Delete textstyle
 * TBool issuccess = styleMgr->DeleteTextStyle( newStyleId );
 * issuccess = styleMgr->DeleteTextStyle( copiedId );
 * @endcode
 * 
 * @see CAlfTextstyle
 * @see CAlfEnv::TextstyleManager()
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfTextStyleManager ): public CBase
    {
public:

    /* Constructors and destructor. */
    
    /**
     * Constructor.
     */
    static CAlfTextStyleManager* NewL(CAlfEnv& aEnv);
    
    /**
     * Constructor.
     */
    static CAlfTextStyleManager* NewLC(CAlfEnv& aEnv);
    
    /**
     * Destructor.
     */    
    ~CAlfTextStyleManager();

public:
    
    /**
     * Creates a new text style object based on the S60 font style id passed into this method. 
     * Text style font and size are set according to the given S60 id. Ids are defined in
     * avkon.hrh in TAknLogicalFontId enumeration.
     * A unique toolkit specific ID is constructed and returned. This text style can be
     * accessed anytime through the TextStyle - method based on the unique ID. The default 
     * text style has always ID 0.
     * Text styles cascade properties from their parent styles. By default all text styles
     * are descended from EAlfTextStyleNormal text style.
     * If an invalid parent id is given the created text style will use EAlfTextStyleNormal
     * as its parent.
     * @see TextStyle()
     *
     * Created text styles must be deleted with DeleteTextStyle.
     * @see DeleteTextStyle()
     * 
     * @param aFontStyleId The S60 font style id based on which the text style object will be created.
     * @param aParentId Toolkit specific ID of the parent text style. The text styles are cascaded.
     *      
     * @return Unique toolkit specific ID for the created text style.
     */
    IMPORT_C TInt CreatePlatformTextStyleL(TInt aFontStyleId, TInt aParentId);
    
    /**
     * Creates a new text style object based on the parent text style id passed into this method. 
     * Text style font and size will be the same as the given parent style has.
     * A unique toolkit specific ID is constructed and returned. This text style can be
     * accessed anytime through the TextStyle - method based on the unique ID.
     * Text styles cascade properties from their parent styles. By default all text styles
     * are descended from EAlfTextStyleNormal text style.
     * If an invalid parent id is given the created text style will use EAlfTextStyleNormal
     * as its parent.
     * @see TextStyle()
     *
     * Created text styles must be deleted with DeleteTextStyle.
     * @see DeleteTextStyle()
     * 
     * @param aParentId Toolkit specific ID of the parent text style. The text styles are cascaded.
     *                  The parent of the created text style is EAlfTextStyleNormal by default.
     * @return Unique toolkit specific ID for the created text style.
     */
    IMPORT_C TInt CreatePlatformTextStyleL(TInt aParentId = EAlfTextStyleNormal);
    
    /**
     * Returns a text style object based on the text style id passed into this method.
     * If no matching text style is found from the system the method returns the EAlfTextStyleNormal
     * text style. There is a collection of preconfigured text styles available. They can
     * be queried using the id enumerations from TAlfPreconfiguredTextStyle.
     * @see CreatePlatformTextStyleL()
     * 
     * @param aId A text style id identifying the requested text style in the text style manager.
     * @return A pointer to a requested text style. Various attributes of the text style 
     * can be changed using the returned pointer.
     */
    IMPORT_C CAlfTextStyle* TextStyle(TInt aId);    
    
    /**
     * Creates a new text style object that is identical with the given source text style.
     *
     * Created/copied text styles must be deleted with DeleteTextStyle.
     * @see DeleteTextStyle()
     *
     * @param aSourceId A toolkit specific ID of the text style to be cloned.
     * @return Unique toolkit specific ID for the created text style.
     */
    IMPORT_C TInt CopyTextStyleL(TInt aSourceId);
  
    /**
     * Deletes the given text style.
     *
     * @param aId Text style ID to delete.
     *
     * @return Error code. KErrNone if text style was deleted
     *                     KErrArgument if there is no style defined with given ID
     *                     KErrAccessDenied if the ID refers to pre-defined styles.
     */ 
    IMPORT_C TInt DeleteTextStyle(TInt aId);

public: // internal utils
    CAlfTextStyle* SwitchTextStyle(TInt aStyle, CAlfTextVisual* aUser);
    void Unregister(CAlfTextVisual* aVisual);
    void RefreshVisuals(TInt aStyle);
    void ReleaseAllVisuals();
    void RefreshAllVisuals();
    
protected:

    /* Constructors. */
    
    /**
     * Default constructor.
     */
    CAlfTextStyleManager();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL(CAlfEnv& aEnv);
    
private:
	
    /**
     * Constructs a collection of CAlfTextStyle objects to match the ones
     * in TAlfPreconfiguredTextStyle.
     * @see TAlfPreconfiguredTextStyle
     */
    void ConstructPreconfiguredStylesL();	

    /**
     * Find free slot index
     *
     * @return free slot index
     */    
    TInt FindFreeSlotIndex() const;
    
    /**
     * Create a platform style.
     *
     * @see CreatePlatformTextStyleL
     * @see CopyTextStyleL
     *
     * @param aFontStyleId The S60 font style id based on which the text style object will be created.
     * @param aParentId Toolkit specific ID of the parent text style. The text styles are cascaded.
     * @param aImplementationId 	Describes the type of text style that is created (platform text style, etc.)
     * @return Unique toolkit specific ID for the created text style.
     */
    TInt DoCreatePlatformTextStyleL(TInt aFontStyleId, TInt aParentId, TInt aImplementationId);

private:
	
	/* Private data. Owned */
    struct TPrivateData;    
    TPrivateData* iData;
    
    };

#endif // C_ALFTEXTSTYLEMANAGER_H
