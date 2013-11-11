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
*     This class is created to provide possibility ot set up pairs of ids
*
*
*/

#ifndef ID_PAIR_H
#define ID_PAIR_H

#warning The Connection Manager API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//A class for defining a pair of ids
NONSHARABLE_CLASS(  TIdPair )
	{
public:
	IMPORT_C TIdPair( TUint32 aFirstPreference, TUint32 aSecondPreference = 0 );
	IMPORT_C TIdPair();
	TUint32 iFirstPreference;
	TUint32 iSecondPreference;
	};

#endif// End of File
