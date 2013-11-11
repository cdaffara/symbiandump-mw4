/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKEv2 Proposal handling.
*
*/


#ifndef __IKEV2PROPOSAL_H_
#define __IKEV2PROPOSAL_H_
#include <e32base.h>
#include "ikepolparser.h"
#include "ikemsgheader.h"

class CIkev2Payloads;
class TPayloadIkev2;
class TProposalIkev2;
class TTransformIkev2;
class CIkeV2Identity;
class TIkev2SAData;
class TIkeV2IpsecSAData;

class Ikev2Proposal
{
public:
	static HBufC8* FromPolicyToProposaL(TIkev2SAData& aIkeSaData, 
                                        const TDesC8& aRekeySpi, 
                                        TInt aDHGroupGuess, 
                                        TBool aRekey=EFalse);	
	static HBufC8* BuildSaResponseL(TProposalIkev2* aAcceptedProp, CIkev2Payloads* aAcceptedTrans);
	static TBool GetSelectedProposalData(TIkev2SAData& aIkev2SaData,
                                         TIkeV2IpsecSAData& aChildSaData, 
                                         const CIkev2Payloads& aAcceptedProp, 
                                         const TProposalIkev2& aProp);	
	static TBool VerifySaResponseL(TIkev2SAData& aIkeSaData, 
                                   TIkeV2IpsecSAData& aIpsecSaData, 
                                   const TDesC8& aReferenceSaData, 
                                   const CIkev2Payloads& aRespProp);
	static TBool VerifySaRequestAndGetProposedSaBufferL(TIkev2SAData& aIkeSaData, 
                                                        TIkeV2IpsecSAData& aIpsecSaData,
                                                        const TDesC8& aReferenceSaData, 
                                                        const CIkev2Payloads& aProposed,
                                                        HBufC8*& aProposedSaBuffer);
	static TBool VerifyProposaL(CIkev2Payloads* aReference, TProposalIkev2* aProposal, TIkev2SAData& aIkev2SaData);
	static TBool CompareTransforms(CArrayFixFlat<TTransformIkev2*>* aRefTrans,
								   CArrayFixFlat<TTransformIkev2*>* aTrans);  
	static HBufC8* GetPSKFromPolicyL(CIkeData* aHostData);
	static TBool IkeSaRekey(CIkev2Payloads* aIkeMsg);
	static TBool GetRekeySpi(CIkev2Payloads* aIkeMsg, TIkeSPI& aSPI);
	static void ChangeSpiInProposal(HBufC8* aSaBfr, TIkeSPI& aSPI);	
	static TUint16 GetDHGroup(TInt aDHGroup);
	static CIkeV2Identity* GetRemoteIdentityL(CIkeData* aHostData);	

	static inline TBool PkiServiceNeeded(CIkeData* aHostData)
	{
	    ASSERT(aHostData); 
	    if ( aHostData->iCAList && aHostData->iCAList->Count() )
		    return ETrue;  
        else return EFalse;
	}
};
	
#endif
