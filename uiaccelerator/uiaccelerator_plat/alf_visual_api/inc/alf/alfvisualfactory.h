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
* Description:   Factory for visuals and layouts.
*
*/



#ifndef ALFVISUALFACTORY_H
#define ALFVISUALFACTORY_H

#include <alf/alfvisual.h>
#include <alf/alflayout.h>

class CAlfEnv;

/**
 *  Factory for visuals and layouts.
 *
 *  This factory object is meant to be used by toolkit extensions only. It is recommended
 *  to use the AddNewL() - method of the visual or layout itself. You can also use
 *  CAlfControl::AppendVisualL() or CAlfControl::AppendLayoutL() - methods.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( AlfVisualFactory )
	{
public:

	/**
	 * Creates a new layout visual.
	 *
	 * CAlfControl::Append() - method has to be explicitly called after this method
	 * has been called to complete the control's ownership of the created layout. After this
	 * method returns the CAlfVisual::Owner() of the created layout will point
	 * to the owner control passed as the parameter, but the layout has not yet been
	 * added to the visual array of the control.
	 *
	 * @param aLayoutType  Type of the layout.
	 * @param aOwner Owner control
	 * @param aEnv The environment.
	 * @leave KErrNotFound  The layout type was not registered.
	 */
	IMPORT_C static CAlfLayout* NewLayoutL(
	    TAlfLayoutType aLayoutType,
	    CAlfLayout* aParentLayout,
	    CAlfControl& aOwner,
	    CAlfEnv& aEnv,
	    TInt aImplementationUid = 0);
	
	/**
	 * Creates a new layout visual, leaving it on the cleanup stack.
	 *
	 * CAlfControl::Append() - method has to be explicitly called after this method
	 * has been called to complete the control's ownership of the created layout. After this
	 * method returns the CAlfVisual::Owner() of the created layout will point
	 * to the owner control passed as the parameter, but the layout has not yet been
	 * added to the visual array of the control.
	 *
	 * @param aLayoutType  Type of the layout.
	 * @param aOwner Owner control
	 * @param aEnv The environment.
	 * @leave KErrNotFound  The layout type was not registered.
	 */
	IMPORT_C static CAlfLayout* NewLayoutLC(TAlfLayoutType aLayoutType,
                                	    CAlfLayout* aParentLayout,
	                                    CAlfControl& aOwner,
	                                    CAlfEnv& aEnv,
	                                    TInt aImplementationUid = 0);
	                                 
	/**
	 * Creates a new visual.
	 *
	 * CAlfControl::Append() - method has to be explicitly called after this method
	 * has been called to complete the control's ownership of the created visual. After this
	 * method returns the CAlfVisual::Owner() of the created visual will point
	 * to the owner control passed as the parameter, but the visual has not yet been
	 * added to the visual array of the control.
	 *
	 * @param aVisualType  Type of the visual.
	 * @param aOwner Owner control
	 * @param aEnv The environment.
	 * @leave KErrNotFound  The visual type was not registered.
	 */
    IMPORT_C static CAlfVisual* NewVisualL( TAlfVisualType aVisualType,
                                    	    CAlfLayout* aParentLayout,
	                                        CAlfControl& aOwner,
	                                        CAlfEnv& aEnv,
	                                        TInt aImplementationUid = 0);
                                    
	/**
	 * Creates a new visual, leaving it on the cleanup stack.
	 *
	 * CAlfControl::Append() - method has to be explicitly called after this method
	 * has been called to complete the control's ownership of the created visual. After this
	 * method returns the CAlfVisual::Owner() of the created visual will point
	 * to the owner control passed as the parameter, but the visual has not yet been
	 * added to the visual array of the control.
	 *
	 * @param aVisualType  Type of the visual.
	 * @param aOwner Owner control
	 * @param aEnv The environment.
	 * @leave KErrNotFound  The visual type was not registered.
	 */
    IMPORT_C static CAlfVisual* NewVisualLC(TAlfVisualType aVisualType,
                                    	    CAlfLayout* aParentLayout,
	                                        CAlfControl& aOwner,
	                                        CAlfEnv& aEnv,
	                                        TInt aImplementationUid = 0);

	};

#endif  // ALFVISUALFACTORY_H
