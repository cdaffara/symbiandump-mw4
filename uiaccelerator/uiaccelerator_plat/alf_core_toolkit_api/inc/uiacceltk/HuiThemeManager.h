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
* Description:   Declares CHuiThemeManager class which is the interface for 
*                ThemeManager component.
*
*/



#ifndef _HUITHEME_MANAGER
#define _HUITHEME_MANAGER

#include <e32base.h>
#include <AknsItemID.h>
#include <alf/alfconstants.h>

namespace Alf
    {
    class ThemeManager; // forward declaration.
    }
using namespace Alf;

struct THuiThemeImageparams
{
    TAknsItemID itemId;
    TAknsItemID aSkinId;
    TFileName iSkinLogicalName; 
    //THuiTextureUploadFlags aFlag;   
    TInt iDefaultBitmapId;
    TInt iDefaultMaskId;	
};

/**
 * @deprecated <b>Deprecated class</b>
 * 
 * CHuiThemeManager is the interface for Hui to access the ThemeManager 
 * functionality
 *
 * @see 
 * @see 
 *
 */
NONSHARABLE_CLASS(CHuiThemeManager) : public CBase
{

public:


	virtual ~CHuiThemeManager();
	
	    
    /** @beginAPI */

	/**
     * Exported method. Two phase constructor
     * renderer.
     */
	IMPORT_C void ConstructL();
	
    /**
     * @deprecated Use GetItemIdL instead.
     */
	IMPORT_C void GetItemID(const TDesC& aSkinId, TAknsItemID& aItemID);
	
    /**
     * @deprecated User GetSkinImageParamsL instead.
     */
	IMPORT_C THuiThemeImageparams GetSkinImageParams(const TDesC& aSkinLogicalName);

    /**
     * @deprecated <b>Doesn't retrieve TAknsItemID anymore!</b>
     * 
     * Exported method. Retrieves a TAknsItemID based on input text skin 
     * image id.
     *
     * @param aSkinId Skin identifier.
     * @param aItemID On returns contains the skin item ID.
     */
	IMPORT_C void GetItemIdL(const TDesC& aSkinId, TAknsItemID& aItemID);
	
    /**
     * @deprecated <b>Doesn't return image parameters anymore!</b>
     * 
     * Exported method. Gets Skin Image parameters based on input 
     * skin image id.
     *
     * @param aSkinLogicalName Logical name of the skin
     * 
     * @return Image parameters.
     */
	IMPORT_C THuiThemeImageparams GetSkinImageParamsL(const TDesC& aSkinLogicalName);

    /** @endAPI */
	
public:
    /**
     * Constructs a new CHuiThemeManager
     *
     * @param 
     */	
     static CHuiThemeManager* NewL();

	
    /**
     * Constructs a new CHuiThemeManager
     *
     * @param 
     */	
	static CHuiThemeManager* NewLC();
private:
	/**
	 * Private constructor
	 */
	CHuiThemeManager();
	
private:

	ThemeManager* iThemeManager;			// Thememanager 

};

#endif
