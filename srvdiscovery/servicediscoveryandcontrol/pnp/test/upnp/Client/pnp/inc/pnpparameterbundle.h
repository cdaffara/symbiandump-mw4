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

#ifndef __PNPPARAMETERBUNDLE_H__
#define __PNPPARAMETERBUNDLE_H__

#include <es_sock.h>
#include <upnpparamset.h>
#include <comms-infras/es_parameterfamily.h>
class MPnPObserver;


/**
Parameter bundle class contains a bundle of RParameterFamiles and a pointer to callback.
Callback pointer should be set if calling client is expecting to recieve any response.
@publishedPartner
@prototype
*/ 
 class RPnPParameterBundleBase : public RParameterFamilyBundle
	 {
 public:
	 
	 IMPORT_C TInt Open();	 
	 IMPORT_C void Close();
	 };
 /*
 @publishedPartner
@prototype
 */ 

class RPnPParameterBundle : public RPnPParameterBundleBase
	{
public:
 	
 	/* Set function for MPnPObserver callback */	
 	IMPORT_C void SetPnPObserver( MPnPObserver* aPnPObserver );
 	/* Get function for MPnPObserver callback */
 	IMPORT_C MPnPObserver * PnPObserver( ) const; 	
 	
private :
 	
 	/* Pointer to callback */
 	MPnPObserver * iPnPObserver;
 
 	};
#endif //__PNPPARAMETERBUNDLE_H__
