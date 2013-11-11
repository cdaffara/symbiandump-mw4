/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VPN Connection Agent definitions..
*
*/



#ifndef _VPNCONNAGTDEFS_H_
#define _VPNCONNAGTDEFS_H_

// INCLUDES
#include "vpnmandefs.h"
#include "vpnapidefs.h"
#include "eventdefssit.h"

#define NORMAL_DISCONNECT_MODE 0
#define SILENT_DISCONNECT_MODE 1

/**
 * Connection configuration parameter class declaration and implementation.
 */
class TVPNParameters 
	{
	public:

		/**
		 * Returns the id of the VPN IAP.
		 * @return Returns the id of the VPN IAP.
		 */
		inline TUint32 GetVPNIapId()
				{return iVPNIapId;};

		/**
		 * Sets the id of the VPN IAP.
		 * @param aVPNIapId Saves the id of the VPN IAP.
		 */
		inline void SetVPNIapId(TUint32 aVPNIapId)
				{iVPNIapId=aVPNIapId;};

		/**
		 * Returns the id of the VPN Network.
		 * @return Returns the id of the VPN Network.
		 */
		inline TUint32 GetVPNNetworkId()
				{return iVPNNetworkId;};

		/**
		 * Sets the id of the VPN Network.
		 * @param aVPNNetworkId Saves the id of the VPN Network.
		 */
		inline void SetVPNNetworkId(TUint32 aVPNNetworkId)
				{iVPNNetworkId=aVPNNetworkId;};

		/**
		 * Returns the id of the real IAP.
		 * @return Returns the id of the real IAP.
		 */
		inline TUint32 GetRealIapId()
				{return iRealIapId;};

		/**
		 * Sets the id of the real IAP.
		 * @param aRealIapId Saves the id of the real IAP.
		 */
		inline void SetRealIapId(TUint32 aRealIapId)
				{iRealIapId=aRealIapId;};

		/**
		 * Returns the id of the real Network.
		 * @return Returns the id of the real Network.
		 */
		inline TUint32 GetRealNetworkId()
				{return iRealNetworkId;};
		
		/**
		 * Sets the id of the real Network.
		 * @param aRealNetworkId Saves the id of the real Network.
		 */
		inline void SetRealNetworkId(TUint32 aRealNetworkId)
				{iRealNetworkId=aRealNetworkId;};

		/**
		 * Returns the pointer to the name of the VPN Nif.
		 * @return A name of the VPN Nif.
		 */
		inline TName* GetVPNNifName()
				{return &iVPNNifName;};

		/**
		 * Saves the name of the VPN Nif.
		 * @param aVPNNifName Reference to a name of the VPN Nif.
		 */
		inline void SetVPNNifName(TName& aVPNNifName)
				{iVPNNifName=aVPNNifName;};

		/**
		 * Saves the TVPNAddress object.
		 * @param aVPNAddresses The address object to be saved.
		 */
		inline void SetAddresses(TVPNAddress& aVPNAddresses)
				{iVPNAddresses=aVPNAddresses;};

		/**
		 * Returns the reference to the TVPNAddress object.
		 * @return Returns the reference to the TVPNAddress object.
		 */
		inline TVPNAddress& GetAddresses()
				{return iVPNAddresses;};

		/**
		 * Returns the IP address given to the VPN Nif.
		 * @return Returns the reference to the TInetAddr type address object.
		 */
		inline TInetAddr& GetVPNNifAddress()
				{return iVPNAddresses.iVPNIfAddr;};

		/**
		 * Saves the IP address to be given to the VPN Nif.
		 * @param aVPNNifAddress A reference to the TInetAddr type address object.
		 */
		inline void SetVPNNifAddress(TInetAddr& aVPNNifAddress)
				{iVPNAddresses.iVPNIfAddr=aVPNNifAddress;};

		/**
		 * Returns the first Domain Name Server address.
		 * @return Returns the reference to the TInetAddr type object.
		 */
		inline TInetAddr& GetVPNNifDNS1()
				{return iVPNAddresses.iVPNIfDNS1;};

		/**
		 * Saves the address of the first Domain Name Server adress.
		 * @param aVPNNifDNS1 A reference to a TInetAddr type object to
		 * be saved.
		 */
		inline void SetVPNNifDNS1(TInetAddr& aVPNNifDNS1)
				{iVPNAddresses.iVPNIfDNS1=aVPNNifDNS1;};

		/**
		 * Returns the second Domain Name Server address.
		 * @return Returns the reference to the TInetAddr type object.
		 */
		inline TInetAddr& GetVPNNifDNS2()
				{return iVPNAddresses.iVPNIfDNS2;};
		/**
		 * Saves the address of the second Domain Name Server adress.
		 * @param aVPNNifDNS2 A reference to a TInetAddr type object to
		 * be saved.
		 */
		inline void SetVPNNifDNS2(TInetAddr& aVPNNifDNS2)
				{iVPNAddresses.iVPNIfDNS2=aVPNNifDNS2;};

		/**
		 * Returns the pointer to the name of the VPN policy.
		 * @return Returns the pointer to the TName object containinge VPN policy name.
		 */
		inline TVpnPolicyId* GetVPNPolicy()
				{return &iVPNPolicyId;};
		/**
		 * Saves the name of the VPN policy.
		 * @param aVPNPolicyId A reference to a descriptor containing the name of the policy.
		 */
		inline void SetVPNPolicy(TVpnPolicyId& aVPNPolicyId)
				{iVPNPolicyId=aVPNPolicyId;};

        inline void SetProtocolVersion(TProtocolVersion& aProtocolVersion)
                {iProtocolVersion = aProtocolVersion;};
        inline TProtocolVersion& GetProtocolVersion()
                {return iProtocolVersion;};

        inline void SetIkePolicyHandle(TPolicyHandle& aIkePolicyHandle)
                {iIkePolicyHandle.iHandle = aIkePolicyHandle.iHandle;};
        inline TPolicyHandle& GetIkePolicyHandle()
                {return iIkePolicyHandle;};

        inline void SetIpSecPolicyHandle(TPolicyHandle& aIpsecPolicyHandle)
                {iIpsecPolicyHandle.iHandle = aIpsecPolicyHandle.iHandle;};
        inline TPolicyHandle& GetIpsecPolicyHandle()
                {return iIpsecPolicyHandle;};

	private:

		/** CommDb Id of the VPN IAP. */
		TUint32		iVPNIapId;

		/** CommDb Id of the VPN Network. */
		TUint32		iVPNNetworkId;

		/** CommDb Id of the real IAP. */
		TUint32		iRealIapId;

		/** CommDb Id of the real Network */
		TUint32		iRealNetworkId;
	
		/** Name of the VPN Nif. */
		TName		iVPNNifName;

		/** IP address of the VPN Nif and DNS addresses. */
		TVPNAddress	iVPNAddresses;

		/** Id of the VPN policy. */
		TVpnPolicyId	iVPNPolicyId;

        /** Version of the used protocol (ip or ip6) */
        TProtocolVersion iProtocolVersion;

        /** Ike policy handle */
        TPolicyHandle iIkePolicyHandle;
        
        /** Ipsec policy Handle */
        TPolicyHandle iIpsecPolicyHandle;
	};


#endif	// VPNCONNAGTDEFS_H

// End of file
