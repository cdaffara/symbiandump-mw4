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

#ifndef __APP_PROTINF_FACT_H__
#define __APP_PROTINF_FACT_H__

#include <elements/factory.h>
#include "app_protintf_base.h"

class CApplicationProtocolIntfFactory : public CFactory
	{
	public:
		// ECOM interface implementation ID
		static const TInt iUid = 0x2000D05B;
		static CApplicationProtocolIntfFactory* NewL ( );
		~CApplicationProtocolIntfFactory ();		

	protected:
		CApplicationProtocolIntfFactory ( );
		
		// From CFactory
		CFactoryObject* DoCreateFactoryObjectL ( Factories::MFactoryQuery& aFactoryQuery );
		
	
	private:
		void ConstructL ();
	};
#endif // __APP_PROTINF_FACT_H__
