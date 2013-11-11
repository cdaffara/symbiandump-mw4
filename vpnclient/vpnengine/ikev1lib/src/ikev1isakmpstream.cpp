/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stream class implementation for ISAKMP
*
*/


#include "ikev1isakmpstream.h"
#include "ikemsgheader.h"
#include "ikev1negotiation.h"
#include "ikedebug.h"
#include "ikev1trans.h"
#include "ikecalist.h"
#include "ikecaelem.h"
#include "ikepolparser.h"
#include "ikev1crypto.h"
#include "ikev1pkiservice.h"
#include "ikev1private.h"
#include "ikepkiutils.h"
#include "ikev1natdiscovery.h"

//
//  TIkev1IsakmpStream
//

TIkev1IsakmpStream::TIkev1IsakmpStream( MIkeDebug& aDebug )
 : iDebug( aDebug )
    {    
    }

void TIkev1IsakmpStream::IsakmpInit(CIkev1Negotiation *aSession)
{
    ThdrISAKMP hdr;

    iError = EFalse;
    iNegotiation = aSession;    //stores it to avoid passing it for all the functions
    DEBUG_LOG(_L("[HDR]"));
    hdr.SetCookieI(iNegotiation->iCookie_I);
    hdr.SetCookieR(iNegotiation->iCookie_R);
    hdr.SetPayload(0);  //Is set later through the pointer iNextPayload
    hdr.SetVersion(0x10);   //MAJOR=1 MINOR=0
    hdr.SetExchange(iNegotiation->iExchange);
    hdr.SetFlags(iNegotiation->iFlags);
    if ((iNegotiation->iExchange == ISAKMP_EXCHANGE_ID) || (iNegotiation->iExchange == ISAKMP_EXCHANGE_AGGR))
        hdr.SetMessageId(0);
    else //QUICK mode ,INFORMATIONAL Mode or Transaction exchange
        hdr.SetMessageId(iNegotiation->iMessageId);
    
    iBuf.SetLength(0);  //Set correctly when sending SendL()
    iBuf.Copy((TUint8 *)&hdr, sizeof(hdr)); //Always called the first so not Append
    iNextPayload = (TUint8 *)(iBuf.Ptr() + 16); //saves the adress. 
                                                //Will be filled by the next called function
}


void TIkev1IsakmpStream::IsakmpSa()
{
    TSAISAKMP sa;
    DEBUG_LOG(_L("[SA]"));
    sa.SetPayload(0);   //Not proposal or transform RFC. After reserved is already 0
    sa.SetDOI(iNegotiation->iDOI);  //Always the same. Otherwise should be the one contained in CIkev1Negotiation
    sa.SetSIT(IPSEC_SIT_IDENTITY_ONLY); //That means no Labeled Domain Identifier
    
    *iNextPayload = ISAKMP_PAYLOAD_SA;      //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address. 
                                            //Will be filled by the next called function

    TUint16 pos=(TUint16)iBuf.Length(); //save the position to know the total size later and insert it
    iBuf.SetLength(pos + sizeof(sa)); //leave room to insert later the proposal
                                                                    //including the SPI
    IsakmpProposal();
    sa.SetLength((TUint16)(iBuf.Length() - pos));
    TBuf8<sizeof(sa)> aux;
    aux.Append((TUint8 *)&sa, sizeof(sa));
    iBuf.Replace(pos,aux.Length(),aux);

}

//SPI not included yet.
//Sends msg stage 2. Should be used for phase 2 as well
void TIkev1IsakmpStream::IsakmpProposal()
{
    TProposalISAKMP proposal;
    TUint16 pos = 0;
    TUint8  next_payload = ISAKMP_PAYLOAD_T; // default: more than one transform

    if (iNegotiation->iPhase == PHASE_I)
    {
        TProposal *prop;
        
        if (iNegotiation->iRole==INITIATOR) 
        {
            prop=&iNegotiation->iProposal_I;
        }
        else
        {
            prop=&iNegotiation->iChosenProposal_I;
        }
        proposal.SetPayload(ISAKMP_PAYLOAD_NONE);   //Only one proposal
        proposal.SetNum(prop->iProposalNum);
        proposal.SetProtocol(prop->iProtocol);
        proposal.SetSPISize(0);
        proposal.SetNumTrans(prop->iNumTransforms);
        pos=(TUint16)iBuf.Length(); //save the position to insert later
                                                                    //including the SPI
        iBuf.SetLength(pos + sizeof(proposal)); //leave room to insert later the proposal
        TAttrib *attr=prop->iAttrList;
        for (TInt i=0; (i < proposal.GetNumTrans()) && (attr != NULL); i++) //Adds all the transforms
        {
            if ( !attr->iNext )
               next_payload = ISAKMP_PAYLOAD_NONE; // Last trasnform    
            IsakmpTransform((TUint8 *)attr, next_payload);
            attr=attr->iNext;
        }
        proposal.SetLength((TUint16)(iBuf.Length() - pos));
        TBuf8<sizeof(proposal) + MAX_SPI_SIZE> aux;
        aux.Append((TUint8 *)&proposal, sizeof(proposal));
        //aux.Append(prop->iSPI);   //insert the SPI
        iBuf.Replace(pos,aux.Length(),aux);
        return;
    }

    //PHASE_II
    TAttrib_II *attr_II=NULL;
    CProposal_IIList *propII_List = NULL;
    CProposal_II *propII = NULL;
    TBuf8<MAX_SPI_SIZE> SPI;
    TInt i, index = 0;
    TUint32 in_spi;
    TUint8  num_transforms;
    CArrayFixFlat<TSPINode> *list = iNegotiation->iInboundSPIList;
    
    if (iNegotiation->iRole==INITIATOR) 
        propII_List = iNegotiation->iProposal_IIList;
    else    //RESPONDER
    {   
        for (index = 0; index < list->Count(); index++)
        {
            if (list->At(index).iPropNum == iNegotiation->iProposalNum)
                break;
        }
        propII_List = iNegotiation->iChosenProp_IIList;
    }

    TInt count = propII_List->Count();

    for (i = 0; i < count; i++)
    {
        propII = propII_List->At(i);
        //Choose the correct SPI
        if (iNegotiation->iRole==INITIATOR) 
            SPI = propII->iSPI;
        else    //RESPONDER
        {
            in_spi = list->At(index).iSPI;
            SPI.Copy((TUint8 *)&in_spi, sizeof(TUint32));
            index++;    
        }

        if (i == count - 1) //Last proposal
            proposal.SetPayload(ISAKMP_PAYLOAD_NONE);
        else    //There are more
            proposal.SetPayload(ISAKMP_PAYLOAD_P);

        num_transforms = propII->iNumTransforms;
        proposal.SetNum(propII->iProposalNum);
        proposal.SetProtocol(propII->iProtocol);
        proposal.SetNumTrans(num_transforms);
        proposal.SetSPISize((TUint8)SPI.Length());  //The chosen contains the inbound SPI
        pos=(TUint16)iBuf.Length(); //save the position to insert later
        iBuf.SetLength(pos + sizeof(proposal) + proposal.GetSPISize()); //leave room to insert later the proposal
                                                                //including the SPI
        for (TInt j = 0; j < num_transforms; j++)   //Adds all the transforms
        {
            attr_II = propII->iAttrList->At(j);
            if ( (num_transforms - j) == 1 )
               next_payload = ISAKMP_PAYLOAD_NONE; // Last trasnform    
            IsakmpTransform((TUint8 *)attr_II, next_payload);
        }
        proposal.SetLength((TUint16)(iBuf.Length() - pos));
        TBuf8<sizeof(proposal) + MAX_SPI_SIZE> aux;
        aux.Append((TUint8 *)&proposal, sizeof(proposal));
        aux.Append(SPI);    //insert the SPI
        iBuf.Replace(pos, aux.Length(), aux);
        
    }

}

//Transform data received as TUint8 to allow both TAttrib and TAttrib_II
void TIkev1IsakmpStream::IsakmpTransform(TUint8 *aTransform, TUint8 aNextPayload)
{
    TTransformISAKMP transf;

    //transf.SetNum(1);//SHOULD BE the selected transform but Linux doesn't let it use it!!!
    if (iNegotiation->iPhase == PHASE_I)
    {
        TAttrib *attr=(TAttrib *)aTransform;
        transf.SetNum(attr->iTransformNum);
        transf.SetID(attr->iTransformID);
        transf.SetPayload(aNextPayload);
    }
    else    //PHASE_II
    {
        TAttrib_II *attr_II=(TAttrib_II *)aTransform;
        transf.SetNum(attr_II->iTransformNum);
        transf.SetID(attr_II->iTransformID);
        transf.SetPayload(aNextPayload);        
    }
    
    TInt pos=iBuf.Length(); //save the position to insert later
    iBuf.SetLength(pos + sizeof(transf)); //leave room to insert later
    IsakmpAttrib(aTransform);
    transf.SetLength((TUint16)(iBuf.Length() - pos));
    TBuf8<sizeof(transf)> aux;
    aux.Append((TUint8 *)&transf, sizeof(transf));
    iBuf.Replace(pos,aux.Length(),aux);
}


//Creates a data payload with the desired SA attributes. either Phase I or II
void TIkev1IsakmpStream::IsakmpAttrib(TUint8 *aTransform)
{
    if (iNegotiation->iPhase == PHASE_I)
        IsakmpAttrib1((TAttrib *)aTransform);
    else    //PHASE_II
        IsakmpAttrib2((TAttrib_II *)aTransform);
}

//Phase_I attributes
void TIkev1IsakmpStream::IsakmpAttrib1(TAttrib *aTransform)
{
    TDataISAKMP attr;
    TUint length;
    TUint16 val;
    TAttrib trans;

    trans=*aTransform;

    val=trans.iEncrAlg;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_ENCR_ALG);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=trans.iHashAlg;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_HASH_ALG);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=CTransNegotiation::GetAuthMethod(trans.iAuthMethod, trans.iXauthUsed, trans.iRole);           
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_AUTH_METH);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=trans.iGroupDesc;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_DESC);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=trans.iGroupType;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_TYPE);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    length=trans.iGroupPrime.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_PRIME);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupPrime);
    }

    length=trans.iGroupGen1.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_GEN1);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupGen1);
    }

    length=trans.iGroupGen2.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_GEN2);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupGen2);
    }
    
    length=trans.iGroupCurveA.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_CRVA);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupCurveA);
    }

    length=trans.iGroupCurveB.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_CRVB);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupCurveB);
    }

    length=trans.iLifeDurationSecs.Length();
    if (length!=0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_TYPE);
        attr.SetValue(SECONDS);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));

        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_DUR);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iLifeDurationSecs);
        
    }

    length=trans.iLifeDurationKBytes.Length();
    if (length!=0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_TYPE);
        attr.SetValue(KBYTES);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));

        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_DUR);
        attr.SetLength((TUint16)(length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iLifeDurationKBytes);
        
    }

    val=trans.iPRF;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_PRF);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=trans.iKeyLength;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_KEY_LEN);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    val=trans.iFieldSize;
    if (val != 0)
    {
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_FIELD_SIZE);
        attr.SetValue(val);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
    }

    length=trans.iGroupOrder.Length();
    if (length!=0)
    {
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_GROUP_ORDER);
        attr.SetLength((TUint16)(sizeof(attr)+length));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(trans.iGroupOrder);
    }
}


//Phase_II attributes
void TIkev1IsakmpStream::IsakmpAttrib2(TAttrib_II *aTransform)
{
        TDataISAKMP attr;
        TUint length;
        TUint16 val;

        TAttrib_II trans=*aTransform;
        length=trans.iLifeDurationSecs.Length();
        if (length!=0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_LIFE_TYPE);
            attr.SetValue(SECONDS);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));

            attr.SetBasic(EFalse);
            attr.SetType(DOI_ATTR_TYPE_LIFE_DUR);
            attr.SetLength((TUint16)length);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
            iBuf.Append(trans.iLifeDurationSecs.Ptr(),trans.iLifeDurationSecs.Length());


        }   

        length=trans.iLifeDurationKBytes.Length();
        if (length!=0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_LIFE_TYPE);
            attr.SetValue(KBYTES);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));

            attr.SetBasic(EFalse);
            attr.SetType(DOI_ATTR_TYPE_LIFE_DUR);
            attr.SetLength((TUint16)length);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
            iBuf.Append(trans.iLifeDurationKBytes.Ptr(),trans.iLifeDurationKBytes.Length());

        }

        if (iNegotiation->iPFS)
        {   //Only sent if PFS in use. The same used in Phase I
            val = trans.iGroupDesc;
            if (val != 0)
            {
                attr.SetBasic(ETrue);
                attr.SetType(DOI_ATTR_TYPE_GROUP_DESC);
                attr.SetValue(val);
                iBuf.Append((TUint8 *)&attr, sizeof(attr));
            }
        }

        val=trans.iEncMode;
        if (val != 0)
        {
            //
            // If ESP UDP encapsulation required (because of NAT device in between)
            // modify encapsulation type code values
            //
            if ( iNegotiation->iNAT_D_Flags ) {
               if ( val == DOI_TUNNEL )
                    val = UDP_ENC_TUNNEL;
               else val = UDP_ENC_TRANSPORT;
            }   
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_ENC_MODE);
            attr.SetValue(val);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
        }

        val=trans.iAuthAlg;
        if (val != 0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_AUTH_ALG);
            attr.SetValue(val);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
        }   

        val=trans.iKeyLength;
        if (val != 0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_KEY_LEN);
            attr.SetValue(val);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
        }   

        val=trans.iKeyRounds;
        if (val != 0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_KEY_ROUNDS);
            attr.SetValue(val);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
        }   
            
        val=trans.iComprDicSize;
        if (val != 0)
        {
            attr.SetBasic(ETrue);
            attr.SetType(DOI_ATTR_TYPE_COMP_DIC_SIZE);
            attr.SetValue(val);
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
        }   

        length=trans.iComprPrivAlg.Length();
        if (length!=0)
        {
            attr.SetBasic(EFalse);
            attr.SetType(DOI_ATTR_TYPE_COMP_PRIV_ALG);
            attr.SetLength((TUint16)(length));
            iBuf.Append((TUint8 *)&attr, sizeof(attr));
            iBuf.Append(trans.iComprPrivAlg);
        }

}


//Adds the key payload to the buffer
void TIkev1IsakmpStream::IsakmpKeyL()
{
    TKeyISAKMP key_payload;

    if (!iNegotiation->ComputeDHPublicValueL()) //Computes the required key values. Needed to ComputeKeys
        return; //No key payload generated
    TPtrC8 key_ptr(iNegotiation->iOwnPublicKey_ptr);
	TInt PadLth = 0;
	TInt ModLth = iNegotiation->iOwnKeys->ModulusLength();
	if ( ModLth > key_ptr.Length() )
	{	
		PadLth = ModLth - key_ptr.Length();
		DEBUG_LOG(_L("[KE(filled)]"));			
	}
	DEBUG_LOG(_L("[KE]"));	
    key_payload.SetLength((TUint16)(sizeof(key_payload) + ModLth));
    *iNextPayload = ISAKMP_PAYLOAD_KE;      //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
                                                    //Will be filled by the next called function    
    iBuf.Append((TUint8 *)&key_payload, sizeof(key_payload));
    if ( PadLth )
	{
	   TChar zero(0);	
	   iBuf.AppendFill(zero, PadLth);    //Fill prepending zero bits to DH public value
	}   
    iBuf.Append(key_ptr);    //variable size DH public value
}

void TIkev1IsakmpStream::IsakmpOwnIdentL()
{
    IsakmpIdentL(ETrue);
}

void TIkev1IsakmpStream::IsakmpPeerIdentL()
{
    IsakmpIdentL(EFalse);
}

void TIkev1IsakmpStream::IsakmpIdentL(TBool aIsOwn)
{
    TIdentISAKMP id_payload;
	
    TBuf8<256> buf;     //Max size for FQDN or DER ASN1 DN
                        // (buf should be allocated from heap !!)   
    TUint32 num;
    const TUint8 *pnum;

    //DOI IPSEC
    if (iNegotiation->iPhase == PHASE_I)
        id_payload.SetProtocol(KProtocolInetUdp);
    else
        id_payload.SetProtocol(iNegotiation->iIDProtocol);  //defined when receiving the acquire (INIT) or when receiveind the ID payload (RESP)

    if (aIsOwn) //Own ID!
    {
        if (iNegotiation->iPhase == PHASE_I)
        {
            DEBUG_LOG(_L("[IDi]"));
			Isakmp_Phase1_IdL(buf, id_payload);
        }
        else //PHASE_II
        {
            DEBUG_LOG(_L("[IDci]"));
            id_payload.SetIDType(iNegotiation->iLocalIDType_II);

            id_payload.SetPort(iNegotiation->iIDLocalPort); //defined when receiving the acquire (INIT) or when receiveind the ID payload (RESP)
            
            switch (iNegotiation->iLocalIDType_II)
            {
            case ID_IPV4_ADDR:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + sizeof(TUint32)));  //IPV4 addr TInt32
                num = ByteOrder::Swap32(iNegotiation->iLocalAddr1_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                break;
            case ID_IPV6_ADDR:
			   if (iNegotiation->iLocalAddr.IsV4Mapped())
				{
				id_payload.SetLength((TUint16)(sizeof(id_payload) + sizeof(TUint32)));  //IPV4 addr TInt32
				id_payload.SetIDType(ID_IPV4_ADDR);
				num = ByteOrder::Swap32(iNegotiation->iLocalAddr.Address());//Put in network order
				pnum= (TUint8*)&num;
				buf.Append(pnum,sizeof(TUint32));
				}
			   else 
				{
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 16));   //IPV6 size is 16
                pnum = &iNegotiation->iLocalAddr1_ID_II.Ip6Address().u.iAddr8[0];   //Address in a bytestream
                buf.Append(pnum, 16);
				}
                break;
            case ID_IPV4_ADDR_SUBNET:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 2*sizeof(TUint32)));    //IPV4 addr TInt32
                num = ByteOrder::Swap32(iNegotiation->iLocalAddr1_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                num = ByteOrder::Swap32(iNegotiation->iLocalAddr2_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                break;
            case ID_IPV6_ADDR_SUBNET:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 32));   //one IPV6 addr size is 16
                pnum = &iNegotiation->iLocalAddr1_ID_II.Ip6Address().u.iAddr8[0];   //Address in a bytestream
                buf.Append(pnum, 16);
                pnum = &iNegotiation->iLocalAddr2_ID_II.Ip6Address().u.iAddr8[0];   //Address in a bytestream
                buf.Append(pnum, 16);
                break;
            //No need for a default. Must be controlled way before when acquire or IDs received
            }   
        }
    }
    else //Peer Id.
    {
        if (iNegotiation->iPhase == PHASE_I)
        {
            DEBUG_LOG(_L("[IDr]"));
			Isakmp_Phase1_IdL(buf, id_payload);					
        }
        else //PHASE_II
        {
            DEBUG_LOG(_L("[IDcr]"));
            id_payload.SetIDType(iNegotiation->iRemoteIDType_II);

            id_payload.SetPort(iNegotiation->iIDRemotePort);    //defined when receiving the acquire (INIT) or when receiving the ID payload (RESP)
            
            switch (iNegotiation->iRemoteIDType_II)
            {
            case ID_IPV4_ADDR:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + sizeof(TUint32)));  //IPV4 addr TInt32
                num = ByteOrder::Swap32(iNegotiation->iRemoteAddr1_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                break;
            case ID_IPV6_ADDR:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 16));   //IPV6 size is 16
                pnum = &iNegotiation->iRemoteAddr1_ID_II.Ip6Address().u.iAddr8[0];  //Address in a bytestream
                buf.Append(pnum, 16);
                break;
            case ID_IPV4_ADDR_SUBNET:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 2*sizeof(TUint32)));    //IPV4 addr TInt32
                num = ByteOrder::Swap32(iNegotiation->iRemoteAddr1_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                num = ByteOrder::Swap32(iNegotiation->iRemoteAddr2_ID_II.Address());//Put in network order
                pnum= (TUint8*)&num;
                buf.Append(pnum,sizeof(TUint32));
                break;
            case ID_IPV6_ADDR_SUBNET:
                id_payload.SetLength((TUint16)(sizeof(id_payload) + 32));   //one IPV6 addr size is 16
                pnum = &iNegotiation->iRemoteAddr1_ID_II.Ip6Address().u.iAddr8[0];  //Address in a bytestream
                buf.Append(pnum, 16);
                pnum = &iNegotiation->iRemoteAddr2_ID_II.Ip6Address().u.iAddr8[0];  //Address in a bytestream
                buf.Append(pnum, 16);
                break;
            default:
                DEBUG_LOG(_L("Bad Remote Phase_II ID type"));
                iNegotiation->SetFinished();
                return;
            //No need for a default. Must be controlled way before when acquire or IDs received
            }   
        }
    }
    *iNextPayload = ISAKMP_PAYLOAD_ID;      //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the adress.
                                                    //Will be filled by the next called function
    iBuf.Append((TUint8 *)&id_payload, sizeof(id_payload));
    iBuf.Append(buf);   //variable size

    //stores the payload to be used in HASH_R/I computation (Only for phase I)
    if ((iNegotiation->iPhase == PHASE_I) && (aIsOwn))
    {
        TUint8 *p_ident=iNextPayload+sizeof(TPayloadISAKMP);    //Data field

        iNegotiation->iOwnIdentPayloadSize=id_payload.GetLength()-sizeof(TPayloadISAKMP);   //without the generic part!

        if (iNegotiation->iOwnIdentPayload) //in case it contains info
            {                
            delete iNegotiation->iOwnIdentPayload;
            iNegotiation->iOwnIdentPayload = NULL;
            }
        iNegotiation->iOwnIdentPayload = new (ELeave) TUint8[iNegotiation->iOwnIdentPayloadSize];   //Generic payload NOT included
        Mem::Copy(iNegotiation->iOwnIdentPayload,p_ident,iNegotiation->iOwnIdentPayloadSize);
    }

}

void TIkev1IsakmpStream::Isakmp_Phase1_IdL(TDes8& aIdData, TIdentISAKMP& aIdPayload)
{
	if ( iNegotiation->iNAT_D_Flags )
		 aIdPayload.SetPort(0);				
	else aIdPayload.SetPort(IKE_PORT);

	TBool IdOk = EFalse;
	TUint8 IdType = iNegotiation->iHostData->iIdType;
	if (IdType == ID_USER_FQDN || IdType == ID_DER_ASN1_DN)
	{
	    if (!iNegotiation->iOwnCert)    //If not yet read
	    {
	        iNegotiation->ReadOwnCertL();
	    }
	}
	if ( iNegotiation->iOwnCert && iNegotiation->iPkiService )
	{
		//
		// Priority 1 : Use IKE identity based on local certificate
		//
		TPtrC8 Cert(iNegotiation->iOwnCert->Des());
		HBufC8* CertIdBfr = IkePkiUtils::GetIdentityFromCertL(IdType, Cert);
		if ( CertIdBfr )
		{
			if ( CertIdBfr->Des().Length() <= aIdData.MaxLength() )
			{
			   aIdData.Copy(CertIdBfr->Des());	
			   if ( IdType == 0 )
				  IdType = ID_DER_ASN1_DN;
			   IdOk = ETrue;
			} 
			delete CertIdBfr; 
		}	  
	}
	
	if ( !IdOk )
	{
		//
		// Priority 2 : Use identity defined in policy 
		//
		TInt DataLength = iNegotiation->iHostData->iFQDN.Length();
		IdType = iNegotiation->iHostData->iIdType;                               
		if ( ( DataLength > 0 ) && ( DataLength <= aIdData.MaxLength()) )
		{
			if ( (IdType != ID_KEY_ID) && (IdType != ID_USER_FQDN) && (IdType != ID_FQDN) )
				IdType = ID_KEY_ID;
			aIdData.Copy(iNegotiation->iHostData->iFQDN);
			IdOk = ETrue;
		}
	}

	if ( !IdOk )
	{
		//
		// Priority 3 : Build local id from own IP address
		//
		TUint32 num;
		const TUint8 *pnum;
		if ((iNegotiation->iLocalAddr.Family() == KAfInet) || iNegotiation->iLocalAddr.IsV4Mapped() )
		{
			IdType = ID_IPV4_ADDR;
			num  = ByteOrder::Swap32(iNegotiation->iLocalAddr.Address());//Put in network order
			pnum = (TUint8*)&num;
			aIdData.Copy(pnum, sizeof(TUint32));
		}
		else 
		{
			IdType = ID_IPV6_ADDR;			
			pnum = &iNegotiation->iLocalAddr.Ip6Address().u.iAddr8[0];  //Address in a bytestream
			aIdData.Copy(pnum, 16);
		}       
	}

	aIdPayload.SetLength((TUint16)(sizeof(aIdPayload) + aIdData.Length())); 
	aIdPayload.SetIDType(IdType);
	
}



void TIkev1IsakmpStream::IsakmpCertificateL()
{
    TCertificateISAKMP *cert;

    if (!iNegotiation->iSendCert)   //Set by a received cert request or ourselves if initiator
        return;

    if (!iNegotiation->iOwnCert)    //If not yet read
    {
        if (!iNegotiation->ReadOwnCertL())
        {
            iNegotiation->SetFinished();
            return;
        }
    }   
    DEBUG_LOG(_L("[CERT]"));
    *iNextPayload = ISAKMP_PAYLOAD_CERT;        //Fills the previous payload next field
    cert = (TCertificateISAKMP*)(iBuf.Ptr() + iBuf.Length());
    iNextPayload = (TUint8 *)cert;//saves the adress.Will be filled by the next called function
    
    TPtr8 cert_ptr((TUint8 *)iBuf.Ptr() + iBuf.Length() + TCertificateISAKMP::Size(), 0, iBuf.MaxLength() - iBuf.Length()); //Pointer to the Cert. Data

    cert_ptr.Copy(iNegotiation->iOwnCert->Des());
    cert->SetReserved(0);
    cert->SetLength((TUint16)(TCertificateISAKMP::Size() + cert_ptr.Size()));
    cert->SetEncoding(iNegotiation->iEncoding); //If responder the same as initiator, otherwise any? (now only X509)    
    iBuf.SetLength(iBuf.Length() + TCertificateISAKMP::Size() + cert_ptr.Size());   //The new info just added   
    if ( iNegotiation->iICA2 )
        {
        DEBUG_LOG(_L("[Level 2 INTERMEDIATE CERT]"));
        *iNextPayload = ISAKMP_PAYLOAD_CERT;        //Fills the previous payload next field
        cert = (TCertificateISAKMP*)(iBuf.Ptr() + iBuf.Length());
        iNextPayload = (TUint8 *)cert;//saves the adress.Will be filled by the next called function
         
        TPtr8 cert_ptr((TUint8 *)iBuf.Ptr() + iBuf.Length() + TCertificateISAKMP::Size(), 0, iBuf.MaxLength() - iBuf.Length()); //Pointer to the Cert. Data

        cert_ptr.Copy(iNegotiation->iICA2->Des());
        cert->SetReserved(0);
        cert->SetLength((TUint16)(TCertificateISAKMP::Size() + cert_ptr.Size()));
        cert->SetEncoding(iNegotiation->iEncoding); //If responder the same as initiator, otherwise any? (now only X509)    
        iBuf.SetLength(iBuf.Length() + TCertificateISAKMP::Size() + cert_ptr.Size());   //The new info just added
        }
    if ( iNegotiation->iICA1 )
        {
        DEBUG_LOG(_L("[Level 1 INTERMEDIATE CERT]"));
        *iNextPayload = ISAKMP_PAYLOAD_CERT;        //Fills the previous payload next field
        cert = (TCertificateISAKMP*)(iBuf.Ptr() + iBuf.Length());
        iNextPayload = (TUint8 *)cert;//saves the adress.Will be filled by the next called function
             
        TPtr8 cert_ptr((TUint8 *)iBuf.Ptr() + iBuf.Length() + TCertificateISAKMP::Size(), 0, iBuf.MaxLength() - iBuf.Length()); //Pointer to the Cert. Data

        cert_ptr.Copy(iNegotiation->iICA1->Des());
        cert->SetReserved(0);
        cert->SetLength((TUint16)(TCertificateISAKMP::Size() + cert_ptr.Size()));
        cert->SetEncoding(iNegotiation->iEncoding); //If responder the same as initiator, otherwise any? (now only X509)    
        iBuf.SetLength(iBuf.Length() + TCertificateISAKMP::Size() + cert_ptr.Size());   //The new info just added
        }
}


void TIkev1IsakmpStream::IsakmpCertificateReqL()
{
    if ( !iNegotiation->iPkiService || !iNegotiation->iPkiService->CaList())
       return;
	CIkeCaList* CaList = iNegotiation->iPkiService->CaList();
    TCertificateReqISAKMP cert_req;
    CX509Certificate      *ca_cert;
    TInt                  count = CaList->Count();
    TInt                  i     = 0;

    while ( i < count ) {
        
        DEBUG_LOG(_L("[CR]"));
        ca_cert = CaList->At(i)->Certificate();
        *iNextPayload = ISAKMP_PAYLOAD_CR;  //Fills the previous payload next field
        iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address. Will be filled by the next called function
        TPtr8 certreq_data_ptr((TUint8 *)iBuf.Ptr() + iBuf.Length() + TCertificateReqISAKMP::Size(), 0, iBuf.MaxLength() - iBuf.Length());  //Pointer to the CertReq. Data
		IkePkiUtils::GetCertSubjectNameDERL(ca_cert, certreq_data_ptr); 
        cert_req.SetLength((TUint16)(TCertificateReqISAKMP::Size() + certreq_data_ptr.Size()));
        cert_req.SetEncoding(iNegotiation->iEncoding);  //If responder the same as initiator, otherwise any? (now only X509)
        iBuf.Append((TUint8 *)&cert_req, TCertificateReqISAKMP::Size());
        iBuf.SetLength(iBuf.Length()+ certreq_data_ptr.Size());
        
        i ++;
    }
    
    iNegotiation->iCertRequested = ETrue;
}


void TIkev1IsakmpStream::IsakmpHashL()
{
    DEBUG_LOG(_L("[HASH]"));
    THashISAKMP hash_payload;
    TBuf8<ISAKMP_HASH_SIZE> hash;
    TUint8 exchange = ThdrISAKMP::Ptr(iBuf)->GetExchange();
    switch (exchange) 
    {
    case ISAKMP_EXCHANGE_ID: //Main mode
    case ISAKMP_EXCHANGE_AGGR: //Main mode
        *iNextPayload = ISAKMP_PAYLOAD_HASH;        //Fills the previous payload next field
        iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the adress.
        
        if (iNegotiation->iRole==INITIATOR)
        {
            DEBUG_LOG(_L("[HASH_I]"));
            iNegotiation->ComputeHash1L(hash); //Must be done after Isakmp_IDENT to have the correct Id payload
            hash_payload.SetLength((TUint16)(hash.Length() + sizeof(THashISAKMP)));
            iBuf.Append((TUint8 *)&hash_payload, sizeof(hash_payload));
            //iBuf.Append(iNegotiation->iHASH_I,iNegotiation->iHASH_ILen);
            iBuf.Append(hash);
            
        }
        else    //RESP
        {
            DEBUG_LOG(_L("[HASH_R]"));
            iNegotiation->ComputeHashrL(hash); //Must be done after Isakmp_IDENT to have the correct Id payload
            hash_payload.SetLength((TUint16)(hash.Length() + sizeof(THashISAKMP)));
            iBuf.Append((TUint8 *)&hash_payload, sizeof(hash_payload));
            //iBuf.Append(iNegotiation->iHASH_R,iNegotiation->iHASH_RLen);
            iBuf.Append(hash);
        }
        break;
		
    case IKE_QUICK_MODE:
        *iNextPayload = ISAKMP_PAYLOAD_HASH;        //Fills the previous payload next field
        iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the adress.
        switch (iNegotiation->iStage)
        {
        case 1://Require 2-stage hash payload construction
        case 2:
        case 4:
            iHash_pos=iBuf.Length();    //Saves the pos to put the payload later in Isakmp_HASH_cont
            //Leaves free space for the buffer
            iBuf.SetLength(iBuf.Length() + sizeof(THashISAKMP) + iNegotiation->HashLength());
        break;
        default: //stage 3
            DEBUG_LOG(_L("[HASH(3)]"));
            iNegotiation->ComputeHash2L(hash, iNegotiation->iStage);
            hash_payload.SetLength((TUint16)(hash.Length() + sizeof(THashISAKMP)));
            iBuf.Append((TUint8 *)&hash_payload, sizeof(hash_payload));
            iBuf.Append(hash);  //Puts the hash in the correct position. iHashPos fills by Isakmp_HASH()
        }
        break;
		
    case ISAKMP_EXCHANGE_INFO:
    case ISAKMP_EXCHANGE_TRANSACT:
		*iNextPayload = ISAKMP_PAYLOAD_HASH;        //Fills the previous payload next field
		iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.		
		iHash_pos = iBuf.Length();    //Saves the pos to put the payload later in Isakmp_HASH_cont
		//Leaves free space for the buffer
		iBuf.SetLength(iBuf.Length() + sizeof(THashISAKMP) + iNegotiation->HashLength());
        break;
		
    default:
        DEBUG_LOG(_L("ISAKMP_HASH payload build error"));
        iNegotiation->SetFinished();
    }
}

//Insert the hash in the designed position. Only Phase_II stage 1 or 2
void TIkev1IsakmpStream::IsakmpHashContL()
{
    THashISAKMP hash;
    TBuf8<ISAKMP_HASH_SIZE> tmp_hash;

    //to put the correct next_payload value we check in the buffer because the next 
    //payload will have updated the value there
    THashISAKMP *fake_hdr=(THashISAKMP *)(iBuf.Ptr() + iHash_pos);
    hash.SetPayload(fake_hdr->GetPayload());
    TInt hash_len=sizeof(THashISAKMP) + iNegotiation->HashLength();
    hash.SetLength((TUint16)hash_len);
    tmp_hash.Copy((TUint8 *)&hash,sizeof(hash));
    iBuf.Replace(iHash_pos, tmp_hash.Length(),tmp_hash);
    iHash_pos += tmp_hash.Length();
    
    TUint8 *hashMsg=((TUint8 *)iBuf.Ptr() + sizeof(ThdrISAKMP) + hash_len); //Msg to hash
    TInt hashMsgLen= iBuf.Length()- sizeof(ThdrISAKMP) - hash_len;
    switch (iNegotiation->iExchange) 
    {
    case IKE_QUICK_MODE:
        if (iNegotiation->iStage != 4)
            iNegotiation->ComputeHash2L(tmp_hash, iNegotiation->iStage, hashMsg, hashMsgLen);
        else //Send CONNECT message is informational although in Quick mode
            iNegotiation->ComputeHashInfL(tmp_hash, hashMsg, hashMsgLen);
        break;
    case ISAKMP_EXCHANGE_INFO:
    case ISAKMP_EXCHANGE_TRANSACT:              
        iNegotiation->ComputeHashInfL(tmp_hash, hashMsg, hashMsgLen);
        break;
    default:
        DEBUG_LOG(_L("ISAKMP_HASH_contL"));
        iNegotiation->SetFinished();
    }
    iBuf.Replace(iHash_pos,tmp_hash.Length(),tmp_hash); //Puts the hash in the correct position. iHashPos fills by Isakmp_HASH()
}

void TIkev1IsakmpStream::IsakmpSignatureL()
{
    TSignatureISAKMP sig;
    TBuf8<ISAKMP_HASH_SIZE> hash;

    DEBUG_LOG(_L("[SIG]"));
    if ( !iNegotiation->iPkiService )
        return;

    //DSS only allows SHA1 as hash
    TUint16 tmp = iNegotiation->iChosenProposal_I.iAttrList->iHashAlg;  //save the value to compute the hash with SHA1 if using DSS
    
    //First computes hash
    if (iNegotiation->iRole==INITIATOR)
    {
        iNegotiation->ComputeHash1L(hash); //Must be done after Isakmp_IDENT to have the correct Id payload
    }
    else
    {
        iNegotiation->ComputeHashrL(hash); //Must be done after Isakmp_IDENT to have the correct Id payload
    }
    iNegotiation->iChosenProposal_I.iAttrList->iHashAlg = tmp;  //Restore the value after computing the hash
    
    *iNextPayload = ISAKMP_PAYLOAD_SIG;     //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
                                                    //Will be filled by the next called function
    TSignatureISAKMP *p_sig=(TSignatureISAKMP *)(iBuf.Ptr() + iBuf.Length());   //To update the payload size later
    iBuf.Append((TUint8 *)&sig, sizeof(sig));
    
    TPtr8 sig_data_ptr((TUint8 *)(iBuf.Ptr() + iBuf.Length()), iBuf.MaxLength() - iBuf.Length());
    
    DEBUG_LOG1(_L("sig_data_ptr length=%d"), sig_data_ptr.Length());
    if ( iNegotiation->iPkiService->Ikev1SignatureL(iNegotiation->iPeerTrustedCA->Des(),
                                                    iNegotiation->iHostData,
                                                    hash, sig_data_ptr) == 0 )
    {
        DEBUG_LOG(_L("Signature Computation failed!"));
    }

    DEBUG_LOG(_L("Signature"));
    iBuf.SetLength(iBuf.Length() + sig_data_ptr.Length());  //updates the buffer size
    p_sig->SetLength((TUint16)(sizeof(sig) + sig_data_ptr.Length()));   //Puts the correct length in the buffer
}

//Adds the Nonce payload to the buffer
void TIkev1IsakmpStream::IsakmpNonce()
{
    TNonceISAKMP nonce_payload;

    //iNegotiation->ComputeNonce();         //Nonce to be sent
    nonce_payload.SetLength((TUint16)(sizeof(nonce_payload) + OAKLEY_DEFAULT_NONCE_SIZE));
    *iNextPayload = ISAKMP_PAYLOAD_NONCE;       //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
                                                    //Will be filled by the next called function
    iBuf.Append((TUint8 *)&nonce_payload, sizeof(nonce_payload));
    if (iNegotiation->iRole==INITIATOR)
    {
        DEBUG_LOG(_L("[NONCE_I]"));
        iBuf.Append(iNegotiation->iNONCE_I);    //variable size
    }
    else
    {
        DEBUG_LOG(_L("[NONCE_R]"));
        iBuf.Append(iNegotiation->iNONCE_R);    //variable size
    }

}

//Adds the Notification payload to the buffer
void TIkev1IsakmpStream::IsakmpNotification(TUint16 aType, TUint8 aProtocol, TUint8* aNotifData, TInt aDataLth)
{
    TNotificationISAKMP notif;
    
    TBuf8<2*ISAKMP_COOKIE_SIZE> spi;

    spi.Copy(iNegotiation->iCookie_I);
    spi.Append(iNegotiation->iCookie_R);

    DEBUG_LOG(_L("[NOT]"));
    *iNextPayload = ISAKMP_PAYLOAD_NOTIF;   //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    TUint16 length = (TUint16)(sizeof(notif) + spi.Length() + aDataLth);

    notif.SetLength(length);
    notif.SetDOI(iNegotiation->iDOI);
    notif.SetProtocol(aProtocol);
    notif.SetSPISize((TUint8)spi.Length());
    notif.SetMsgType(aType);
                                                    //Will be filled by the next called function
    iBuf.Append((TUint8 *)&notif, sizeof(notif));   //Header
    iBuf.Append(spi);       //insert the SPI

	if ( aNotifData && aDataLth )
	   iBuf.Append(aNotifData, aDataLth);   //Add Notification data
        
}

//Adds the Notification payload to the buffer
void TIkev1IsakmpStream::IsakmpReplayStatus(TUint8 aProtocol, TUint32 aSPI, TUint8 aReplayWindowLength)
{
    TNotificationISAKMP notif;
    TUint32 data;
    
    DEBUG_LOG(_L("[NOT]"));
    *iNextPayload = ISAKMP_PAYLOAD_NOTIF;   //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    notif.SetLength((TUint16)(sizeof(notif) + sizeof(aSPI) + sizeof(data))); //No Notification Data yet!!!
    notif.SetDOI(iNegotiation->iDOI);
    notif.SetProtocol(aProtocol);
    notif.SetSPISize(sizeof(aSPI)); //No SPI sent
    notif.SetMsgType(DOI_REPLAY_STATUS);
                                                    //Will be filled by the next called function
    iBuf.Append((TUint8 *)&notif, sizeof(notif));   //Header
    iBuf.Append((TUint8 *)&aSPI, sizeof(aSPI));     //Insert the SPI

    // Notification Data:   0 if replay detection disabled
    //                      1 if replay detection enabled

    if (aReplayWindowLength > 0)
        data = ByteOrder::Swap32(1);
    else
        data = 0;
    iBuf.Append((TUint8 *)&data, sizeof(data));

}

//Adds the Notification payload to the buffer
void TIkev1IsakmpStream::IsakmpResponderLifetime(TUint8 aProtocol, TUint32 aSPI, const TDesC8 &aLifetime, const TDesC8 &aLifesize)
{
    TNotificationISAKMP notif;
    TDataISAKMP attr;
    
    DEBUG_LOG(_L("[NOT]"));
    *iNextPayload = ISAKMP_PAYLOAD_NOTIF;   //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    
    //Notification payload + spi + attribs lifetype and life value
    TInt attrlen = 0;
    if (aLifetime.Length() > 0)
        attrlen = 2*sizeof(attr) + aLifetime.Length();
    if (aLifesize.Length() > 0)
        attrlen += 2*sizeof(attr) + aLifesize.Length();
    if (attrlen == 0)   //No lifevalues to send
        return;

    notif.SetLength((TUint16)(sizeof(notif) + sizeof(aSPI) + attrlen));
    notif.SetDOI(iNegotiation->iDOI);
    notif.SetProtocol(aProtocol);
    notif.SetSPISize(sizeof(aSPI)); //No SPI sent
    notif.SetMsgType(DOI_RESPONDER_LIFETIME);
                                                    //Will be filled by the next called function
    iBuf.Append((TUint8 *)&notif, sizeof(notif));   //Header
    iBuf.Append((TUint8 *)&aSPI, sizeof(aSPI));     //Insert the SPI
    
    //Lifetime
    if (aLifetime.Length() > 0)
    {
        //LifeType
        attr.SetBasic(ETrue);
        attr.SetType(DOI_ATTR_TYPE_LIFE_TYPE);
        attr.SetValue(SECONDS);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));

        //Life Value
        attr.SetBasic(EFalse);
        attr.SetType(DOI_ATTR_TYPE_LIFE_DUR);
        attr.SetLength((TUint16)(aLifetime.Length()));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(aLifetime);
    }

    //Lifesize
    if (aLifesize.Length() > 0)
    {
        //LifeType
        attr.SetBasic(ETrue);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_TYPE);
        attr.SetValue(KBYTES);
        iBuf.Append((TUint8 *)&attr, sizeof(attr));

        //Life Value
        attr.SetBasic(EFalse);
        attr.SetType(OAKLEY_ATTR_TYPE_LIFE_DUR);
        attr.SetLength((TUint16)(aLifesize.Length()));
        iBuf.Append((TUint8 *)&attr, sizeof(attr));
        iBuf.Append(aLifesize);
    }
}


//Only send 1 SPI for the ISAKMP SA, AH or ESP
void TIkev1IsakmpStream::IsakmpDelete(TDesC8 &aSPI, TUint8 aProtocol)
{
    TDeleteISAKMP delete_payload;

    DEBUG_LOG(_L("[DEL]"));

    *iNextPayload = ISAKMP_PAYLOAD_D;       //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    
    delete_payload.SetLength((TUint16)(sizeof(delete_payload) + aSPI.Length())); //No Notification Data yet!!!
    delete_payload.SetDOI(iNegotiation->iDOI);  
    delete_payload.SetProtocol(aProtocol);
    delete_payload.SetSPISize((TUint8)aSPI.Length());
    delete_payload.SetNumSPI(1);

    iBuf.Append((TUint8 *)&delete_payload, sizeof(delete_payload)); //Header
    iBuf.Append(aSPI);  //insert the SPI
}

//Adds the Vendor ID payload to the buffer
void TIkev1IsakmpStream::IsakmpVendorId(TInt aID_Type,
                                   TUint8 *aICOOKIE,
                                   TUint8 *aRCOOKIE,
                                   TInetAddr &aLocalAddr,
                                   TUint8 *aGenericVidData, TInt aGenericVidLth )
{
    
    *iNextPayload = ISAKMP_PAYLOAD_VID;    //Fills the previous payload next field
    iNextPayload  = (TUint8 *)(iBuf.Ptr() + iBuf.Length());

    if ( (aID_Type == IETF_NATT_VENDOR_ID || aID_Type == IETF_RFC_NATT_VENDOR_ID) && aGenericVidData ) {
       TVendorISAKMP vendor_payload;
       vendor_payload.SetLength((TUint16)(sizeof(vendor_payload) + aGenericVidLth));
       iBuf.Append((TUint8 *)&vendor_payload, sizeof(vendor_payload)); //Header
       iBuf.Append(aGenericVidData, aGenericVidLth);                   //Data
       return;
    }   
/*--------------------------------------------------------
 *
 *  If an expanded Vendor Id required build it, otherwise 
 *  use "old" style" short Vendor Id,             
 *
 *--------------------------------------------------------*/
    TBool ExpandedVID;
    
    if ( aID_Type == EXPANDED_VENDOR_ID )
         ExpandedVID = ETrue;       
    else ExpandedVID = EFalse;
    
    TInt payload_lth = ConstructVendorId(ExpandedVID,
                                         aICOOKIE,
                                         aRCOOKIE,                                   
                                         aLocalAddr,
                                        (TVendorISAKMP*)iNextPayload);
    iBuf.SetLength(iBuf.Length() + payload_lth);   //The new info just added
    
}


void TIkev1IsakmpStream::IsakmpChre(TUint16 aLAMType, TUint16 aAttr1, HBufC8 *aBfr1,
                                TUint16 aAttr2, HBufC8 *aBfr2, TUint16 aAttr3, HBufC8 *aBfr3)   
{
    TCHREISAKMP chre_payload;

    DEBUG_LOG(_L("[CHRE]"));

    *iNextPayload = ISAKMP_PAYLOAD_CHRE;     //Fills the previous payload next field
    iNextPayload = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    /*----------------------------------------------------
     * Fill generic payload header fields (except length)
     *----------------------------------------------------*/
    chre_payload.SetLAMtype(aLAMType);      
    chre_payload.SetCHREReserved();
    TInt pos = iBuf.Length(); //save the position to insert later
    iBuf.SetLength(pos + sizeof(chre_payload)); //leave room to insert later
    
    /*----------------------------------------------------
     * Store CHRE payload attribute data 
     *----------------------------------------------------*/
    if ( aAttr1 !=0 )
       IsakmpChreAttrib(aAttr1, aBfr1);
    if ( aAttr2 !=0 )
       IsakmpChreAttrib(aAttr2, aBfr2);   
    if ( aAttr3 !=0 )
       IsakmpChreAttrib(aAttr3, aBfr3);   
    /*----------------------------------------------------
     * Store correct payload length 
     *----------------------------------------------------*/
    chre_payload.SetLength((TUint16)(iBuf.Length() - pos));
    TBuf8<sizeof(chre_payload)> aux;
    aux.Append((TUint8 *)&chre_payload, sizeof(chre_payload));
    iBuf.Replace(pos, aux.Length(), aux);
}

void TIkev1IsakmpStream::IsakmpChreAttrib(TUint16 aType, HBufC8 *aBfr)
{
  /*----------------------------------------------------
   * Store CHRE payload attribute data (variable length) 
   *----------------------------------------------------*/
    TDataISAKMP attr;
    TUint       length;
    attr.SetBasic(EFalse);
    attr.SetType(aType);
    if ( aBfr ) 
         length = aBfr->Length(); 
    else length = 0;
    attr.SetLength((TUint16)length);       
    iBuf.Append((TUint8 *)&attr, sizeof(attr));
    if ( length )
       iBuf.Append(aBfr->Ptr(),length);
}   


void TIkev1IsakmpStream::IsakmpIntnet(TUint32 aIpv4Addr)
{
  /*----------------------------------------------------------
   * Build inititor (=client) private Internal Address payload
   * Payload contains only PRI_INTERNAL_ADDRESS attribute coded
   * as a basic attribute with value 0.
   *---------------------------------------------------------*/
    TINTNETISAKMP intnet_payload;
    TDataISAKMP   attr;

    DEBUG_LOG(_L("[IA]"));

    *iNextPayload = ISAKMP_INT_NETWORK;     //Fills the previous payload next field
    iNextPayload  = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.
    
    intnet_payload.SetLength((TUint16)(sizeof(intnet_payload) +
                                      (2 + 2)));  //basic attribute
    iBuf.Append((TUint8 *)&intnet_payload,
                          sizeof(intnet_payload));  //Payload Header
    attr.SetBasic(ETrue);
    attr.SetType(PRI_INTERNAL_ADDRESS);
    attr.SetValue((TUint16)aIpv4Addr);  
    iBuf.Append((TUint8 *)&attr, sizeof(attr));

}

void TIkev1IsakmpStream::IsakmpNatD(TBool aRfcNatt, TDesC8 &aHash)
{
  /*------------------------------------------------------------
   * Build NAT-D related to NAT discovery.
   * Payload data is a hash data defined as follows:
   * HASH = HASH(CKY-I | CKY-R | IP | Port)
   * using the negotiated HASH algorithm   
   *---------------------------------------------------------*/
    TNATDISAKMP nat_d_payload;

    DEBUG_LOG(_L("[NAT_D]"));
	
	if ( aRfcNatt )
    	*iNextPayload = IETF_RFC_NAT_DISCOVERY;     //Fills the previous payload next field
    else
    	*iNextPayload = IETF_NAT_DISCOVERY;     //Fills the previous payload next field
    iNextPayload  = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.

    nat_d_payload.SetLength((TUint16)(sizeof(nat_d_payload) + aHash.Length()));
    iBuf.Append((TUint8 *)&nat_d_payload, sizeof(nat_d_payload));   //Header
    iBuf.Append(aHash);                                             //Data

}

void TIkev1IsakmpStream::IsakmpAttributes(TUint8 aMsgType, TUint16 aIdentifier, TDesC8 &aAttributes)
{
  /*------------------------------------------------------------
   * Build Attributes payload. (used with config-mode and XAUTH)
   *---------------------------------------------------------*/
    TAttributeISAKMP attr_payload;
    attr_payload.SetCfgMsgType(aMsgType);      
    attr_payload.SetReservedField();
    attr_payload.SetIdentifier(aIdentifier);    

    DEBUG_LOG(_L("[ATTR]"));

    *iNextPayload = ISAKMP_PAYLOAD_ATTRIBUTES;     //Fills the previous payload next field
    iNextPayload  = (TUint8 *)(iBuf.Ptr() + iBuf.Length());//saves the address.

    attr_payload.SetLength((TUint16)(sizeof(attr_payload) + aAttributes.Length()));
    iBuf.Append((TUint8 *)&attr_payload, sizeof(attr_payload));   //Header
    iBuf.Append(aAttributes);                                     //Data
}
