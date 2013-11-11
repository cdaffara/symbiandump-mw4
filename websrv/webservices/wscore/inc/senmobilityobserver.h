/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    
*
*/




#ifndef M_SEN_MOBILITY_OBSERVER_H
#define M_SEN_MOBILITY_OBSERVER_H

//  INCLUDES
#include <e32std.h>
#include <in_sock.h>
#include <es_enum.h>
#include <comms-infras/cs_mobility_apiext.h>
#include "SenXmlReader.h"
#include "MSenTransport.h"
#include "SenWSDescription.h"

class CALRObserver  : public CActive, public MMobilityProtocolResp
	{
	public:  // Constructors and destructor

	/**
	* NewL is first phase of two-phased constructor.
	*/
	static CALRObserver * NewL(MMobilityProtocolResp &aMobilityObserver, CSenXmlReader &aReader) ;
	/**
	* Destructor.
	*/
	~CALRObserver ();

	public: // From MMobilityProtocolResp

	void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
	                                TAccessPointInfo aNewAPInfo,
	                                TBool aIsUpgrade,
	                                TBool aIsSeamless );

	void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless );

	void Error( TInt aError );

	//Wrapper for MMobilityProtocolResp methods

	void MigrateToPreferredCarrier(); 

	void IgnorePreferredCarrier(); 

	void NewCarrierAccepted(); 

	void NewCarrierRejected();
	
	// From CActive
	void DoCancel() ;
	void RunL() ;
	TInt RunError( TInt aError );

	//functions for Client session usage
	TUint32 GetActiveIap() ;
	HBufC8* GetNewIapAsTransportPropertyL() ;
	TUint32 GetActiveSnap() ;
	TInt OpenConnectionL(TDesC8& aAppTransportProperties,
							 MSenTransport &aTransport,
							 CSenWSDescription& aInitializer,
							 HBufC8*& aNewTransportProperties) ;


	private:

	/** 
	* C++ default constructor.
	*/
	CALRObserver(MMobilityProtocolResp &aMobilityObserver, CSenXmlReader &aReader)  ;
	TInt StartConnection(TUint32& aId, TBool aIsSnapId) ;
	TInt OpenSocketSever() ;
	void RefreshAvailabilityL() ;
	TInt SetID(TUint32 aId, TBool aDialogPref, RConnection& aConnection, RSocketServ& aSocketServer, TBool aSNAP);
	TBool IsOCCImplementedSDK();
	TInt GetS60PlatformVersion(TUint& aMajor, TUint& aMinor);

	/**
	* By default Symbian OS constructor is private.
	*/
	void ConstructL();
	RConnection iConnection ;
	RSocketServ iSocketServer;
	MMobilityProtocolResp &iMobilityObserver ;
	CActiveCommsMobilityApiExt*   iMobility;
	TUint32 iIapId;
	TUint32 iNewIapId;
	TUint32 iSnapId;
	CSenXmlReader &iReader ;
	TBool iOCCenabled;
	private:    // Data	      
	};
    
#endif /*M_SEN_MOBILITY_OBSERVER_H    */
