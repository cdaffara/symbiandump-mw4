/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKEv1 SA
*
*/


#ifndef C_IKEV1SA_H
#define C_IKEV1SA_H

#include "ikev1SAdata.h"
#include "ikev1keepalive.h"

#define SECOND 1000000  // One second is 1000000 us. (1 us. per tick)
#define ISAKMP_DELETE_TIME 2*SECOND    //Expiration time for a ISAKMP SA after it's erased

class CIkev1PluginSession;
class CIkev1NokiaNattKeepAlive;
class MIkeDebug;

//List of the IPSEC SAs negotiated by the ISAKMP SA
NONSHARABLE_CLASS(CIpsecSPIList) : public CArrayPtrFlat<TIpsecSPI>
{
	public:
		CIpsecSPIList(TInt aGranularity);
		~CIpsecSPIList();
};

NONSHARABLE_CLASS(CIkev1SA) : public CTimer, public MDpdHeartBeatEventHandler
{
	public:
		static CIkev1SA* NewL( CIkev1PluginSession& aPluginSession,
		                       TIkev1SAData& aIkev1SAdata,
		                       CSARekeyInfo* aSaRekey,
		                       MIkeDebug& aDebug );	
		~CIkev1SA();	

		void UpdateSAL( TBool aExpired,
		                TIkev1SAData* aIkev1SAdata );
		void ExpireSA();
		void AddIpsecSPIL( TIpsecSPI& aIpsecSpi );
        TBool FindIpsecSPI( TUint32 aSPI,
                            TBool aInbound );
		TBool DeleteIpsecSPI( TUint32 aSPI,
		                      TBool aInbound );
		/**
		 * Deletes IPsec SAs and sends delete payload to the GW.
		 */
		void DeleteIpsecSAs();
		/**
		 * Deletes IPsec SAs without sending delete payload to the GW.
		 * Used when connection is lost.
		 */
		void DeleteIpsecSAsForced();	
		void SetExpired();
		inline TBool IsExpired() {return iExpired;}
		void EventHandlerL();
		
		inline void SetDeactivating( TBool aDeactivating ) { iDeactivating = aDeactivating; }
		inline TBool IsDeactivating() { return iDeactivating; }
		
		void CancelRekey();

	protected:
	//
	// CActive methods
	//
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);

	private:
		CIkev1SA( CIkev1PluginSession& aPluginSession,
		          MIkeDebug& aDebug );
		void ConstructL( TIkev1SAData& aIkev1SAdata,
		                 CSARekeyInfo* aSaRekey );

		void StartTimer();

	public:	
		TIkev1SAData iHdr;      // Common negotiation info
	    CIpsecSPIList* iSPIList;    //Contains a SPIs List to know the direction when a delete received

	private:	
	    CIkev1PluginSession&  iPluginSession;
	
		TUint32 iRemainingTime;     //Timer remaining
		TUint32 iRemainingKB;       //KB life remaining
		TUint32 iCurrentBytes;      //Bytes life currently added

		TUint32 iLeftOverTime;      //Time remaining after rekey started
		TBool   iRekeyed;           //Rekeyed SA

		CIkeV1KeepAlive* iIkeKeepAlive; // Pointer to common IKE keepalive object

		TBool iExpired;     //The ISAKMP SA may be expired but waiting possible delete IPSEC SA msgs that arrive after
		
		CIkev1NokiaNattKeepAlive *iNokiaNatt; // Pointer to Nokia NAT-T keepalive object
		
		TBool iDeactivating;
		
		MIkeDebug& iDebug;		
};

#endif // C_IKEV1SA_H
