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
* Description:   contains a class for use by the GfxTransEffect adapter to communicate
*                with the transition server
*
*/


#ifndef R_ALFTFXSERVEROBSERVER
#define R_ALFTFXSERVEROBSERVER


#include <e32base.h>
class TControlPolicy;

/**
 *  interface to implement, to recieve callbacks from server.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( MTfxServerObserver )
	{
	public:
		/**
		 * Called when a new policy arrives
	     * @param aPolicy the new policy
	    */  
		virtual void PolicyUpdate( TControlPolicy* aPolicy ) = 0;
		
		/**
		 * Called when all policies clientside needs to be cleared.
		 */
		virtual void ClearAllPolicies() = 0;
		
		/**
		 * Called when a transition finishes for this handle
	     * @param aHandle handle of finished transition
	    */  
//		virtual void TransitionFinished(TInt aHandle) = 0;

	};

#endif // R_ALFTFXSERVEROBSERVER

