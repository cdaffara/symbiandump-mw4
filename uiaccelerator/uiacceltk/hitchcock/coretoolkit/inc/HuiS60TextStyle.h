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
* Description:   S60 text style encapsulates a S60 - specific unique text style 
*                in Hitchcock UI Toolkit usage.
*
*/



#ifndef __THUIS60TEXTSTYLE_H__
#define __THUIS60TEXTSTYLE_H__

#include <e32base.h>
#include <uiacceltk/HuiFont.h>
#include <uiacceltk/huitextstyle.h>

/**
 * THuis60TextStyle presents a S60 specific text style. It implements handling and
 * rendering of Series 60 specific text styles.
 */
class THuiS60TextStyle: public THuiTextStyle 
{
public:
    /* Constructors and destructor. */

    /** @beginAPI */

    /**
     * Default constructor. Create a new S60 specific text style.
     *
     * @param aParent   A handle id set into the THuiFont by the Texture manager or skin
     * @param aFontSpec Symbian OS Font Specification to store
     */
    IMPORT_C THuiS60TextStyle(TInt aFontStyleId, TInt aParentId);

    /** @endAPI */
    
    void Init();
    
private:

};

#endif  //__THUIS60TEXTSTYLE_H__
