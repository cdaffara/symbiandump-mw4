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
* Description:   Implementation of alfred server side text style message handlers.
*
*/



#ifndef ALFTEXTSTYLEHANDLERS_H
#define ALFTEXTSTYLEHANDLERS_H

#include <alf/alfextensionfactory.h>

class THuiTextStyle;

/**
 * Server - side handler object for CAlfTextStyle. Processes CAlfTextStyle commands
 * and passes them onwards to Hitchcockcore THuiTextStyle.
 */
NONSHARABLE_CLASS(TAlfTextStyleHandler) : public MAlfExtension
{
public:

	/**
	 * Creates a new platform text style handler
	 *
	 * @param aResolver 	The alf interface provider.
	 *
	 * The meaning of parameters aId1 and aId2 depends on the requested construction type.
	 *
	 * When aConstructionType is EAlfPlatformTextStyleCreate:
	 *
	 * Creates a new platform text style handler based on passed in font style id and
	 * parent id.
	 * 
	 * @param aId1 	The platform specific font style id to be used to create this platform
	 * 				text style.
	 * @param aId2	Id of the parent text style.
	 *
	 *
	 * When aConstructionType is EAlfPreconfiguredTextStyleCreate:
	 *
	 * Creates a representation of a preconfigured text style.
	 * 
	 * @param aId1	Id of the preconfigured text style to represent with this handler.
	 *
	 *
	 * When aConstructionType is EAlfPlatformTextStyleCopy:
	 *
	 * Creates a new platform text style handler for new text style created from an existing
	 * text style.
	 * 
	 * @param aId1	Id of the existing text style to be used to create this text style.
	 */
	TAlfTextStyleHandler(MAlfInterfaceProvider& aResolver, TInt aId1, TInt aId2, TInt aConstructionType);


	/**
	 * Releases this text style handler.
	 */
    void Release();
    
    /**
     * Returns the associated Hithcockcore interface object.
     */
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    
    /**
     * Handles the incoming commands from the client side associated with this text style.
     */
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);
    
    /**
     * Returns the text style id of associated THuiTextStyle object.
     */
    TInt TextStyleId() const;
    
private:
	
    MAlfInterfaceProvider& iResolver; // Not owned
    THuiTextStyle* iTextStyle; // Not owned
};

#endif // ALFTEXTSTYLEHANDLERS_H
