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
* Description:  IKEv2 SA data definition
*                Class TIkev2SAData is the IKEv2 SA parameter definition which
*                is used to pass SA information between IKE server and IKEv2
*                plug-in.
*
*/

#include "ikev2SAdata.h"
#include "ikecrypto.h"
#include "ikev2const.h"

TIkev2SAData::TIkev2SAData()
://iSAId(0),
 iSPI_I(),
 iSPI_R(),
 iSAState(0),
 iInitiator(EFalse),
 iIkeData(NULL),
 iVpnIapId(0),
 iLocalAddr(),
 iVirtualAddr(),
 iRemoteAddr(),
 iDestinAddr(),
 iNATFlags(0),
 iFloatedPort(EFalse),
 iWindowSize(0),
 iRespRetryCount(0),
 iEncrAlg(0),
 iPRFAlg(0),
 iIntegAlg(0),
 iDHGroup(0),
 iEAPType(0),
 iAuthMethod(0),
 iLifetime(0),
 iCipherKeyLth(0),
 iCipherBlkLth(0),
 iIntChkSumLth(0),
 iMobikeUsed(EFalse)
    {
    iSPI_I.SetLength(iSPI_I.MaxLength());
    iSPI_I.FillZ();
    
    iSPI_R.SetLength(iSPI_R.MaxLength());
    iSPI_R.FillZ();
    }


void TIkev2SAData::CleanUp() 
    { 
    FreeRespMsg(); 
    FreeRequestMsg(); 
    }


void TIkev2SAData::FreeRespMsg()
    {
    delete iLastResponse;
    iLastResponse = NULL;
    }


void TIkev2SAData::FreeRequestMsg()
    {
    delete iLastRequest;
    iLastRequest = NULL;
    }


void TIkev2SAData::StoreVirtualIp(const TInetAddr& aVirtualAddr)
    {       
    iVirtualAddr = aVirtualAddr;
    }


void TIkev2SAData::SaveRespMsg(CIkeV2Message* aRespMsg)
    {
    FreeRespMsg();
    iLastResponse = aRespMsg;
    }


void TIkev2SAData::SaveRequestMsg(CIkeV2Message* aRequestMsg)
    {
    FreeRequestMsg();
    iLastRequest = aRequestMsg;
    }


void TIkev2SAData::Copy(TIkev2SAData& aSrc)
    {
    TInetAddr savedVirtualAddr = iVirtualAddr;  
    CIkeV2Message* savedLastResponse = iLastResponse;
    CIkeV2Message* savedLastRequest = iLastRequest;
    
    Mem::Copy((TUint8*)&iSAId, (TUint8*)&aSrc.iSAId, sizeof(TIkev2SAData));
    
    if (iLastResponse != NULL)
        {
        delete savedLastResponse;
        aSrc.iLastResponse = NULL;
        }
    else
        {           
        iLastResponse = savedLastResponse;
        }
    
    if (iLastRequest != NULL)
        {
        delete savedLastRequest;
        aSrc.iLastRequest = NULL;
        }
    else
        {
        iLastRequest = savedLastRequest;
        }
    
    if ( iVirtualAddr.IsUnspecified() )
        iVirtualAddr = savedVirtualAddr; 
    }


TUint32 TIkev2SAData::SaId() const 
    {
    return iSAId;
    }


void TIkev2SAData::SetSaId(TUint32 aSaId)
    {
    iSAId = aSaId;
    }


TIkeSPI& TIkev2SAData::SpiI()
    {
    return iSPI_I;
    }


void TIkev2SAData::SetSpiI(const TIkeSPI& aSpiI)
    {
    iSPI_I = aSpiI;
    }


TIkeSPI& TIkev2SAData::SpiR()
    {
    return iSPI_R; 
    }


void TIkev2SAData::SetSpiR(const TIkeSPI& aSpiR)
{
    iSPI_R = aSpiR;
}

void TIkev2SAData::GenerateIkeKeyDerivatesL(const TDesC8& aSKEYSEED,TUint16 aPrfAlg,
                                            const TDesC8& aNonceI, const TDesC8& aNonceR)
{
    //
    //  Generate IKE keying information from SKEYDSEED (its
    //  derivates).
    //  SK_d | SK_ai | SK_ar | SK_ei | SK_er | SK_pi | SK_pr  
    //  = prf+ (SKEYSEED, Ni | Nr | SPIi | SPIr )
    //  Since the amount of keying material needed may be greater than
    //  the size of the output of the prf algorithm prf+ is used as
    //  follows    prf+ (SKEYSEED,S) = T1 | T2 | T3 | T4 | ...
    //    where:   T1 = prf (SKEYSEED, S | 0x01)
    //             T2 = prf (SKEYSEED, T1 | S | 0x02) ..
    //             TN = prf (SKEYSEED, TN-1 | S | 0xN ) ;[ N < 256 ]
    //  Calculate first required key material length:
    //  Length of SK_d = Length of PRF algorithm output
    //  Length of SK_ai and SK_ar = Length of integrity algorithm key
    //  Length of SK_ei and SK_er = Length of cipher algorithm key
    //  Length of SK_pi and SK_pr = Length of PRF output
    //
    TInt EncKeyLth = IkeCrypto::AlgorithmInfo(IKEV2_ENCR, iEncrAlg, &iCipherBlkLth);
    if ( iCipherKeyLth == 0 )
       iCipherKeyLth = EncKeyLth;
    TInt IntKeyLth = IkeCrypto::AlgorithmInfo(IKEV2_INTEG, iIntegAlg, &iIntChkSumLth);
    TInt PrfKeyLth = IkeCrypto::AlgorithmInfo(IKEV2_PRF, iPRFAlg, NULL);
    TInt KeyMatLth = 2*iCipherKeyLth + 2*IntKeyLth + 3*PrfKeyLth;
    HBufC8* S      = HBufC8::NewL(aNonceI.Length() + aNonceR.Length() + 2*IKEV2_SPI_SIZE);  
    CleanupStack::PushL(S);
    //
    // Copy value S = (Ni | Nr | SPIi | SPIr) into work buffer S
    //
    S->Des().Copy(aNonceI);
    S->Des().Append(aNonceR);
    S->Des().Append(SpiI());
    S->Des().Append(SpiR());

    HBufC8* KeyMat = IkeCrypto::GenerateKeyingMaterialL(aSKEYSEED, S->Des(), KeyMatLth, aPrfAlg);
    //
    // Store derived key material into negotiation object in the
    // following order: Key material = 
    // SK_d | SK_ai | SK_ar | SK_ei | SK_er | SK_pi | SK_pr  
    //
    TUint8* KeyMatBfr = (TUint8*)KeyMat->Ptr();  // Keymaterial buffer start
    iSK_d.Copy(KeyMatBfr, PrfKeyLth);
    KeyMatBfr += PrfKeyLth;
    iSK_ai.Copy(KeyMatBfr,IntKeyLth );
    KeyMatBfr += IntKeyLth; 
    iSK_ar.Copy(KeyMatBfr, IntKeyLth);
    KeyMatBfr += IntKeyLth; 
    iSK_ei.Copy(KeyMatBfr, iCipherKeyLth);
    KeyMatBfr += iCipherKeyLth;    
    iSK_er.Copy(KeyMatBfr, iCipherKeyLth);
    KeyMatBfr += iCipherKeyLth;    
    iSK_pi.Copy(KeyMatBfr,PrfKeyLth );
    KeyMatBfr += PrfKeyLth; 
    iSK_pr.Copy(KeyMatBfr, PrfKeyLth);

    KeyMat->Des().FillZ(); // Wipe out key material (T1 | T2 | ...) data from buffer
    delete KeyMat;
    
    CleanupStack::PopAndDestroy();  //S
}


TUint32 TIkev2SAData::NextRequestId() const
{
    TUint32 msgId = 0;
    if(iLastRequest != NULL)
        {
        msgId = iLastRequest->MessageId() + 1;
        }
    return msgId;
}


TUint32 TIkev2SAData::ExpectedResponseId() const
{
    __ASSERT_DEBUG(iLastRequest != NULL, User::Invariant());
    return iLastRequest->MessageId();
}

TUint32 TIkev2SAData::ExpectedRequestId() const
{
    TUint32 msgId = 0;
    if(iLastResponse != NULL)
        {
        msgId = iLastResponse->MessageId() + 1;
        }
    return msgId;
}
