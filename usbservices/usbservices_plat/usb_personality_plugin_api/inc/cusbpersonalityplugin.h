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
* Description:  Header file for CUsbPersonalityPlugin class
*
*/


#ifndef CUSBPERSONALITYPLUGIN_H__
#define CUSBPERSONALITYPLUGIN_H__

#include <cusbpersonality.h>

class RUsb;

/**
 *  All personalities must be inherited from this class.
 *  ECOM framework is used here.
 *
 *  @lib euser.lib, ecom.lib
 *  @since S60 3.2
 */
class CUsbPersonalityPlugin : public CUsbPersonality
	{
public: 
	/** 
	 * Factory function
	 *
     * @since S60 3.2
	 * @param aPersonalityParams Reference to container class
	 * @param aImplementationId The UID of this implementation.
	 * @return Ownership of a new CUsbPersonalityPlugin.
	 */
	IMPORT_C static CUsbPersonalityPlugin* NewL(TUsbPersonalityParams& aPersonalityParams, TUid aImplementationId);

	/** 
	 * Destructor
	 */
	IMPORT_C virtual ~CUsbPersonalityPlugin();

protected:

	/** 
	 * Constructor
	 */
	IMPORT_C CUsbPersonalityPlugin(TUsbPersonalityParams& aPersonalityParams);

protected:
	
	/**
	 * UID given to us by ECOM when the instance is created. Used when the 
     * instance is destroyed.
     */
	TUid iPrivateEComUID;
	};

#endif
