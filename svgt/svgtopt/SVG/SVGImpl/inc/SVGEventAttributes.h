/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_SVGEVENTATTRIBUTE_
#define _INC_SVGEVENTATTRIBUTE_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

/**
 *  This is group all 'on' attributes into one class.
 *  CSvgElementImpl only has to hold one pointer to this class,
 *  thus saving memory for most instances of svg elements, which
 *  usually do not have these 'on' attributes.
 */
class CSvgEventAttributes : public CBase
{
public:
    /**
    * Constructor
    *
    * @since 1.0
    * @param None
    * @return None
    */
    CSvgEventAttributes();

    /**
    * Destructor
    *
    * @since 1.0
    * @param None
    * @return None
    */
    virtual ~CSvgEventAttributes();

    /**
    * Get the 'onclick' attribute value
    *
    * @since 1.0
    * @param None
    * @return 'onclick' value
    */
    const HBufC* OnClick();

    /**
    * Set the 'onclick' attribute value
    *
    * @since 1.0
    * @param aString -- string to set value
    * @return none
    */
    void SetOnClickL( const TDesC& aString );

    /**
    * Get the 'onload' attribute value
    *
    * @since 1.0
    * @param None
    * @return 'onclick' value
    */
    const HBufC* OnLoad();

    /**
    * Set the 'onload' attribute value
    *
    * @since 1.0
    * @param aString -- string to set value
    * @return none
    */
    void SetOnLoadL( const TDesC& aString );

    /**
    * Get the 'onmousemove' attribute value
    *
    * @since 1.0
    * @param None
    * @return 'onclick' value
    */
    const HBufC* OnMouseMove();

    /**
    * Set the 'onmousemove' attribute value
    *
    * @since 1.0
    * @param aString -- string to set value
    * @return none
    */
    void SetOnMouseMoveL( const TDesC& aString );

private:
    HBufC* iOnClick;
    HBufC* iOnLoad;
    HBufC* iOnMouseMove;
};



#endif
