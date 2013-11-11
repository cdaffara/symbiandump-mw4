/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Text style manager allows creation and management of
*                platform text styles in Hitchcock UI Toolkit usage.
*
*/




#ifndef __HUITEXTSTYLEMANAGER_H__
#define __HUITEXTSTYLEMANAGER_H__

#include <e32base.h>
#include <uiacceltk/huitextstyle.h>

/** 
 * Preconfigured text styles.
 * Matching text styles always exist in the text style manager.
 * It is possible to retrieve the associated text style object through
 * CHuiTextStyleManager::TextStyle() - interface.
 */
enum THuiPreconfiguredTextStyle
    {
    EHuiTextStyleNormal = 0,
    EHuiTextStyleNormalItalic,
    EHuiTextStyleMenuItem,
    EHuiTextStyleSmall,
    EHuiTextStyleMenuSmall,
    EHuiTextStyleMenuTitle,
    EHuiTextStyleLarge,
    EHuiTextStyleSoftkey,
    EHuiTextStyleSupplement,
    EHuiTextStyleTitle
    };

/**
 * CHuiTextStyleManager allows user to create text styles supported by the platform
 * that can be used in Hitchcock UI Toolkit user interfaces. Text styles are referred  
 * to by Hithcock UI Toolkit session specific unique IDs. Collection of preconfigured
 * text styles are constructed by default. @see THuiPreconfiguredTextStyle. 
 */
NONSHARABLE_CLASS(CHuiTextStyleManager): public CBase
{
public:

    /* Constructors and destructor. */

    /**
     * Constructor.
     */
    static CHuiTextStyleManager* NewL();

    /**
     * Constructor.
     */
    static CHuiTextStyleManager* NewLC();

    /**
     * Destructor.
     */    
     ~CHuiTextStyleManager();
     
    /**
     * Creates a new text style object based on the S60 font style id passed into this method. 
     * Text style font and size are set according to the given S60 id. Ids are defined in
     * avkon.hrh in TAknLogicalFontId enumeration.
     * A unique toolkit specific ID is constructed and returned. This text style can be
     * accessed anytime through the TextStyle - method based on the unique ID. The default 
     * text style has always ID 0.
     * Text styles cascade properties from their parent styles. By default all text styles
     * are descended from EHuiTextStyleNormal text style.
     * If an invalid parent id is given the created text style will use EHuiTextStyleNormal
     * as its parent.
     * @see TextStyle()
     *
     * Created text styles must be deleted with DeleteTextStyle.
     * @see DeleteTextStyle()
     * 
     * @param aFontStyleId The S60 font style id based on which the text style object will be created.
     * @param aParentId Toolkit specific ID of the parent text style. The text styles are cascaded.
     *                          The parent of the created text style is EHuiTextStyleNormal by default.
     * @return Unique toolkit specific ID for the created text style.
     */
     IMPORT_C TInt CreatePlatformTextStyleL(TInt aFontStyleId, TInt aParentId = EHuiTextStyleNormal);
     
    /**
     * Returns a text style object based on the text style id passed into this method.
     * If no matching text style is found from the system the method returns the EHuiTextStyleNormal
     * text style. There is a collection of preconfigured text styles available. They can
     * be queried using the id enumerations from THuiPreconfiguredTextStyle.
     * @see CreatePlatformTextStyleL()
     * 
     * @param aId A text style id identifying the requested text style in the text style manager.
     * @return A pointer to a requested text style. Various attributes of the text style 
     * can be changed using the returned pointer.
     */
     IMPORT_C THuiTextStyle* TextStyle(TInt aId);
     
    /**
     * Creates a new text style object that is identical with the given source text style.
     * All the properties of the source text style are copied. The only difference is the 
     * text style ID. Each text style has a unique toolkit specific ID.
     *
     * Created text styles must be deleted with DeleteTextStyle.
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

    /**
     * Constructs the collection of built-in preconfigured text styles.
     * @see THuiPreconfiguredTextStyle
     */
     void ConstructBuiltInStylesL();

     /**
      * 
      */
     void SetSoftkeyTextStyle(TInt aTextStyleId);
     
     void NotifyDisplaySizeChangedL();
     
protected:

    /* Constructors. */

    /**
     * Default constructor.
     */
    CHuiTextStyleManager();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
    void InsertIntoArrayAndSetIdL(THuiTextStyle& style);

private:
    
    /** Registry of all text styles within this manager. These text styles are  
     *  accessed by text style id. 
     */
    RPointerArray<THuiTextStyle> iTextStyles;
    
    /**
     * Last id of built in styles.
     */
    TInt iLastBuiltInStyleId;
};

#endif  //__HUITEXTSTYLEMANAGER_H__
