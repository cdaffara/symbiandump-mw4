// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

void RUPnPParameterFamily::DeleteParamSetL ( TInt aIndex )
	{
	// UPnP is not having any usage of EAcceptable,EGranted
	// so call three deletes that make sure, the null elements 
	// as well are deleted.
	
	DeleteParameterSetL ( aIndex * ENumValues );
	if ( CountParameterSets() )
		{
		if ( aIndex == 0 )
			{
			DeleteParameterSetL ( aIndex );
			DeleteParameterSetL ( aIndex );

			}
		else
			{
			DeleteParameterSetL ( aIndex * ENumValues - RParameterFamily::EAcceptable );
			DeleteParameterSetL ( aIndex * ENumValues - RParameterFamily::EGranted );					
			}
		}
	}
