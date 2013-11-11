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
* Description:   ?Description
*
*/



#ifndef __HUIVISUALFACTORY_H__
#define __HUIVISUALFACTORY_H__


#include <e32base.h>
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiLayout.h"


/**
 * Factory class for creating visuals.
 * This class can create new instances of visuals. The desired
 * type is passed in, and a new visual is returned.
 */
NONSHARABLE_CLASS(CHuiVisualFactory) : public CBase
	{
public:

	/* Constructors and destructor. */

	/**
	 * Constructor.
	 */
	CHuiVisualFactory(CHuiEnv& aEnv);

	/**
	 * Destructor.
	 */
	virtual ~CHuiVisualFactory();


	/* Methods. */

	/**
	 * Create a new layout visual.
	 *
	 * @param aLayoutType  Type of the layout.
	 * 
	 * @leave KErrNotFound  The layout type was not registered.
	 */
	CHuiLayout* NewLayoutL(THuiLayoutType aLayoutType,
	                       CHuiControl& aOwner) const;
	
	/**
	 * Create a new layout visual, leaving it on the cleanup stack.
	 *
	 * @param aLayoutType  Type of the layout.
	 *
	 * @leave KErrNotFound  The layout type was not registered.
	 */
	CHuiLayout* NewLayoutLC(THuiLayoutType aLayoutType,
	                        CHuiControl& aOwner) const;
	                                 
	/**
	 * Create a new visual.
	 *
	 * @param aVisualType  Type of the visual.
	 * 
	 * @leave KErrNotFound  The visual type was not registered.
	 */
    CHuiVisual* NewVisualL(THuiVisualType aVisualType,
                           CHuiControl& aOwner) const;
                                    
	/**
	 * Create a new visual, leaving it on the cleanup stack.
	 *
	 * @param aVisualType  Type of the visual.
	 *
	 * @leave KErrNotFound  The visual type was not registered.
	 */
    CHuiVisual* NewVisualLC(THuiVisualType aVisualType,
                            CHuiControl& aOwner) const;	                                 


private:
    
    /* Private methods */


private:

    /** Environment where the factory belongs to. */
    CHuiEnv& iEnv;
    
	};

#endif  // __HUIVISUALFACTORY_H__
