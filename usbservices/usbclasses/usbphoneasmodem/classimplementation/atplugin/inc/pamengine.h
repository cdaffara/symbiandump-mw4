// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// class definition for phone as modem at extension plugin
// 
//

#ifndef PAMENGINE_H
#define PAMENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <etel3rdparty.h>
#include <rmmcustomapi.h>
#include <etelmm.h>
#include <mmtsy_names.h>
#include <cmmanager.h>
#include <cmdestination.h>
#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginpacketdatadef.h>


// FORWARD DECLARATIONS

// CLASS DECLARATION
class CPamPlugin;
/**
*  CPamEngine
*/
class CPamEngine : public CActive
	{
public: // Constructor, destructor
	/**
	* Two-phased contructor
	* @param aOwner
	* @return Instance of CPamEngine
	*/
	static CPamEngine* CPamEngine::NewL( CPamPlugin* aOwner );
						   
	/**
	* Two-phased contructor
	* @param aOwner
	* @return Instance of CPamEngine
	*/
	static CPamEngine* NewLC( CPamPlugin* aOwner );


	/**
	* C++ Destructor
	*/
	~CPamEngine();


	/**
	* CPamEngine::ReadAccessPointTableL()
	* @param aAccessPoint
	* @param apArray 
	*/
	void ReadAccessPointTableL( TInt aAccessPoint, CDesC8ArrayFlat*& apArray );


	/**
	*  CPamEngine::ReadPacketServicetableL()
	*/
	void ReadPacketServicetableL( CMManager::TPacketDataApType  aAccessPointType, CDesC8ArrayFlat*& apArray );


	/**
	* Adds quotationmarks before and after string if not empty
	*/
	void AddQuotationmarks( const TDesC& aString, HBufC16*& aBuf );


	/**
	* Reads supported access points
	*/
	void SupportedAccessPoints( HBufC16*& aBuf );


	/**
	* Gets Service provider name
	*/
	void GetServiceProviderName();


private: // from CActive
	/**
	* AO callback
	*/    
   void RunL();

	/**
	* AO callback
	*/    
   void DoCancel();


private:
	/**
	* 2nd phase Constructor
	*/
	void ConstructL();

	/**
	* C++ Constructor
	* @param aObserver
	* @param aObserver
	*/

	CPamEngine( CPamPlugin* aOwner );

private:
    enum TAccessPointType
    {
    EINTERNETACCESSPOINT = 0,
    EWAPACCESSPOINT,
    EBOTHACCESSPOINT
    };

private: // Data
	CTelephony::TNetworkNameV1 iNWName;
	CTelephony* iTelephony;
	CPamPlugin* iOwner;
	};  
	
   
#endif // PAMENGINE_H

// End of File

