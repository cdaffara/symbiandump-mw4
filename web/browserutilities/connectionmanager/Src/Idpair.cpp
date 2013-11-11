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
*     This class provides connection/session related information.
*	  Both the requested and the stored AP's are here.
*
*
*/


//User Includes
#include <e32std.h>
#include "IdPair.h"
//TIdPair
EXPORT_C TIdPair::TIdPair( TUint32 aFirstPreference, TUint32 aSecondPreference ): 
	iFirstPreference( aFirstPreference), iSecondPreference( aSecondPreference )
	{
	}

EXPORT_C TIdPair::TIdPair( ) 
	{
	}

//End of File
