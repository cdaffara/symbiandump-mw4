/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder header file
*
*/


#ifndef NVGICON_H_
#define NVGICON_H_

#include <e32base.h>

class CNvgEngine;

/*!
 * @class MNVGIcon
 *        base class for NVG icons
 */
class MNVGIcon
    {
public:
    
    /*!
     * @fn			SetPreserveAspectRatio
     * @brief		Sets the Aspect ration for the NVG icon
     * @return	KErrNone if success, appropriate error code on failure
     */
    virtual TInt SetPreserveAspectRatio(TInt aPreserveAspectSetting, 
                                        TInt aSmilFitSetting) = 0;
                                        
    /*!
     * @fn		Rotate
     * @brief	Rotates the icon 
     * @param aAngle	angle of rotation
     * @param aX x coordinate rotation center
     * @param aY y coordinate rotation center
     * @return	KErrNone if success, appropriate error code on failure
     */
    virtual TInt Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP = 0;

    /*!
     * @fn		Draw
     * @brief	Draws the icon 
     * @param aSize	size of the icon
     * @param aNVGEngine NVG engine instance
     * @return	KErrNone if success, appropriate error code on failure
     */
    virtual TInt Draw(const TSize aSize, CNvgEngine * aNVGEngine) = 0;
    
    /*!
     * @fn		~MNVGIcon()
     * @brief	Destructs the icon 
     */
    virtual ~MNVGIcon();
    };

#endif
