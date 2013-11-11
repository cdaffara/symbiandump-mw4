/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This interface class defines the protocolconnection/session
*	  handling.
*	
*
*/


#ifndef AP_CHANGE_OBSERVER_H
#define AP_CHANGE_OBSERVER_H

#warning The Connection Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//This class specifies a protocol for observing the AP changes
class MApChangeObserver
	{
	public:
	virtual void ApChangedL( TUint32 aNewAPId ) = 0;
	};

#endif;//AP_CHANGE_OBSERVER_H// End of File
