/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Key management daemon extra module.
*
*/


#include <e32math.h>

#include "ikev1extra.h"

//////////////////////////////////////////////////////////////7
//
// Global functions
//
//Puts the data in the descriptor into a TInt64
TInt Desc8ToTInt64(const TDesC8 &aLifetime, TInt64 &aTime)
{
    TUint32 high,low;
    
    //Seconds lifetime
    //TAttrib_II *attr_II = aProp->iAttrList->At(0);    //presume only 1 transform for each proposal
    TUint len = aLifetime.Length();
    TInt high_len = 0;
    if (len > 0)
    {       
        if (len > sizeof(aTime))
        {            
            //LogError(_L("Phase_II Lifetime(sec) Overflowed Setting to maximum value"));
            aTime = MAKE_TINT64(KMaxTInt,KMaxTUint);            
            return KErrOverflow;
        }
        else
        {
            if (len <= sizeof(TUint))
                high = 0;
            else
            {
                high_len = len - sizeof(TUint);
                Mem::Copy(&high, aLifetime.Ptr(), high_len); //The first total_length - sizeof(TUint) bytes
                high = ByteOrder::Swap32(high);
                high = high >> (sizeof(TUint)*8 - len*8);   //To set the correct value (shift in bits)
                len = sizeof(TUint); //remaining length
            }
        
            Mem::Copy(&low, aLifetime.Ptr() + high_len , len);
            low = ByteOrder::Swap32(low);
            low = low >> (sizeof(TUint)*8 - len*8); //To set the correct value (shift in bits)
            aTime = MAKE_TINT64(high,low );
        }
    }
    else
        aTime = 0;

    return KErrNone;
}

//Creates an addr. mask of aPrefixLen length
void PrefixMask(TInetAddr &anAddr, TInt aPrefixLen, TInt aFamily)
{
    if (aFamily == STATIC_CAST(TInt, KAfInet))
    {
		TUint32 addr = 0;   
		if ( aPrefixLen ) {
           addr = INET_ADDR(255,255,255,255);
           addr <<= 32 - aPrefixLen;
		}
        anAddr.SetAddress(addr);
        anAddr.SetFamily(KAfInet);
    }
    else    //KAfInet6
        anAddr.PrefixMask(aPrefixLen);  //This method only works for IPv6

}

TInt PrefixLen(TInetAddr& anAddr)
{
    TIp6Addr ip6addr = anAddr.Ip6Address();
    TInt length;
    TInt i;

    switch (anAddr.Family())
    {
        case KAfInet:
            TReal bit;
            TInt err;
            TInt mask8;
    
            length = 32;

            for (i = 0; i < length; i++)
            {
                err = Math::Pow(bit,2,i % 8);
                if (err != KErrNone)
                    return err;
                mask8 = ip6addr.u.iAddr8[(TInt)(i/8)] & (TInt)bit;
                if (mask8 == (TInt)bit) //The bit is 1
                    break;
            }

            return length - i;//length of the mask

        case KAfInet6:
            i = 3;          // index to a n-bit chunk of an address
            length = 128;   // first assumption on length of the prefix
    
            // start counting the 32 bit zero chunks from end
            while (ip6addr.u.iAddr32[i] == 0)
            {
                length -= 32;
                if (--i < 0)
                    return 0; // zero length prefix
            }
    
            // switch to 16 bit chunks
            i = i * 2 + 1;
            if (ip6addr.u.iAddr16[i] == 0)
            {
                length -= 16;
                i--;
            }
    
            // switch to 8 bit chunks
            i = i * 2 + 1;
            if (ip6addr.u.iAddr8[i] == 0)
            {
                length -= 8;
                i--;
            }
    
            // switch to 1 bit chunks
            {
                TUint8 octet = ip6addr.u.iAddr8[i];
                for (i = 1; (octet & i) == 0; i <<= 1)
                    length--;
            }
            
            return length;

        default:
            return KErrNotSupported;
    }
}



////////////////////////////////////////////////////////////////////////////////////
//
//class TAttrib
//
//aIsRelaxed indicates theat the lifetimes won't be compared
TInt TAttrib::Compare(TAttrib& aAttr, TBool aIsRelaxed)
{
    if (iTransformID != aAttr.iTransformID)
        return KErrTransformID;
    if (iEncrAlg != aAttr.iEncrAlg)
        return KErrEncrAlg;
    if (iHashAlg != aAttr.iHashAlg)
        return KErrHashAlg;
    if (iAuthMethod != aAttr.iAuthMethod)
        return KErrAuthMethod;    
    if (iGroupDesc != aAttr.iGroupDesc)
        return KErrGroupDesc;
    if ((iGroupType != aAttr.iGroupType) && 
        (!(iGroupType == 0) && (aAttr.iGroupType == MODP))) //MODP is the default type so is the same as 0
        return KErrGroupType;
    if (iGroupPrime.Compare(aAttr.iGroupPrime)!=0)
        return KErrGroupPrime;
    if (iGroupGen1.Compare(aAttr.iGroupGen1)!=0)
        return KErrGroupGen1;
    if (iGroupGen2.Compare(aAttr.iGroupGen2)!=0)
        return KErrGroupGen2;
    if (iGroupCurveA.Compare(aAttr.iGroupCurveA)!=0)
        return KErrGroupCurveA;
    if (iGroupCurveB.Compare(aAttr.iGroupCurveB)!=0)
        return KErrGroupCurveB;
    if (iPRF != aAttr.iPRF)
        return KErrPRF;
    if (iKeyLength != aAttr.iKeyLength)
    {
        if ( iEncrAlg != AES_CBC )
            return KErrKeyLength;
        else
        {
            if ( !( aIsRelaxed && (iKeyLength == 128) && (aAttr.iKeyLength == 0)) )
               return KErrKeyLength;    
        }
        
    }   
    if (iFieldSize != aAttr.iFieldSize)
        return KErrFieldSize;
    if (iGroupOrder.Compare(aAttr.iGroupOrder)!=0)
        return KErrGroupOrder;

    if (!aIsRelaxed)    //If relaxed lifetimes don't need to match
    {
        if (iLifeDurationSecs.Compare(aAttr.iLifeDurationSecs)!=0)
            return KErrLifeTime;
        if (iLifeDurationKBytes.Compare(aAttr.iLifeDurationKBytes)!=0)
            return KErrLifeSize;
    }
    return KErrNone;
}

//
//class CTransModifierList : public CArrayPtr<TTransModifier>
//
CTransModifierList::CTransModifierList(TInt aGranularity) : CArrayPtrFlat<TTransModifier>(aGranularity)
{

}

CTransModifierList::~CTransModifierList()
{
	ResetAndDestroy();
}

//
//class TAttrib_II
//
TAttrib_II::TAttrib_II()
{
        iTransformNum=0;
        iTransformID=0; 
        iGroupDesc=0;       
        iEncMode=0; 
        iAuthAlg=0;
        iKeyLength=0;
        iKeyRounds=0;
        iComprDicSize=0;
};

TInt TAttrib_II::Compare(TAttrib_II& aAttr, TBool aRelaxed)
{
    TBuf8<MAX_ALG_DATA> iComprPrivAlg;
    
//  if (iTransformNum != aAttr.iTransformNum)   //Transform number
//      return KErrTransformNum;                // Test retired
    if ( iTransformID != aAttr.iTransformID ) //Transform ID
        return KErrTransformID;

    if ((aAttr.iGroupDesc < iGroupDesc) &&
        (!(iGroupDesc == 0)))    
        return KErrGroupDesc;
    if (iEncMode != aAttr.iEncMode) //Encapsulation Mode
        return KErrEncMode;
    if (iAuthAlg != aAttr.iAuthAlg) //HMAC
        return KErrAuthAlg;

    if (iKeyLength != aAttr.iKeyLength)
    {
		if ( iTransformID != ESP_AES_CBC )
			 return KErrKeyLength;
		else
		{
			if (!( ((iKeyLength == 128) && (aAttr.iKeyLength == 0) )
				    ||
			      ((iKeyLength == 0) && (aAttr.iKeyLength == 128) ) ))				 
				return KErrKeyLength;    
		}
    }

    if (iKeyRounds != aAttr.iKeyRounds)
    {
        if ( iTransformID != ESP_AES_CBC )
           return KErrKeyRounds;
        if ( (iKeyRounds != 0  || aAttr.iKeyRounds != 10) // 10 AES default rounds
             &&  
             (iKeyRounds != 10 || aAttr.iKeyRounds != 0) )
           return KErrKeyRounds;
    }   
    if (iComprDicSize != aAttr.iComprDicSize)   //Compress Dictionary size
        return KErrComprDicSize;
    if (iComprPrivAlg.Compare(aAttr.iComprPrivAlg)!=0)
        return KErrComprPrivAlg;

    if (!aRelaxed)
    {
        if (iLifeDurationSecs.Compare(aAttr.iLifeDurationSecs)!=0)
            return KErrLifeTime;
        if (iLifeDurationKBytes.Compare(aAttr.iLifeDurationKBytes)!=0)
            return KErrLifeSize;
        if (iResponderLifetimeSecs.Compare(aAttr.iResponderLifetimeSecs)!=0)
            return KErrLifeTime;
        if (iResponderLifetimeKBytes.Compare(aAttr.iResponderLifetimeKBytes)!=0)
            return KErrLifeSize;        
    }
    return KErrNone;
}

void TAttrib_II::Copy(TAttrib_II &aAttr)
{
    iTransformNum = aAttr.iTransformNum;    //Transform number
    iTransformID = aAttr.iTransformID;  //Transform ID
    iLifeDurationSecs.Copy(aAttr.iLifeDurationSecs);
    iLifeDurationKBytes.Copy(aAttr.iLifeDurationKBytes);
    iResponderLifetimeSecs.Copy(aAttr.iResponderLifetimeSecs);
    iResponderLifetimeKBytes.Copy(aAttr.iResponderLifetimeKBytes);    
    iGroupDesc = aAttr.iGroupDesc;      //OAKLEY GROUP
    iEncMode = aAttr.iEncMode;      //Encapsulation Mode
    iAuthAlg = aAttr.iAuthAlg;      //HMAC
    iKeyLength = aAttr.iKeyLength;
    iKeyRounds = aAttr.iKeyRounds;
    iComprDicSize = aAttr.iComprDicSize;    //Compress Dictionary size
    iComprPrivAlg.Copy(aAttr.iComprPrivAlg);
}

//
//class CAttrib_IIList
//

CAttrib_IIList::CAttrib_IIList(TInt aGranularity) : CArrayPtrFlat<TAttrib_II>(aGranularity)
{

}

CAttrib_IIList::~CAttrib_IIList()
{
	ResetAndDestroy();	
}

//
//class CProposal_II
//

void CProposal_II::ConstructL(TInt aGranularity)
{
    iAttrList = new (ELeave) CAttrib_IIList(aGranularity);  //Default to granularity 1
}

CProposal_II::~CProposal_II()
{
    delete iAttrList;
}

//Compares that aProp matches with one of the sets of attributes of this proposal and the Protocol
//aRelaxed indicates if the comparison includes the lifetimes or not.
//Returns KErrNotFound if no match or the transf num (>=0) if match
TInt CProposal_II::Match(CProposal_II *aRemoteProp, TBool aRelaxed, TInt* aLocalNbr )
{
    
    if (iProtocol != aRemoteProp->iProtocol)
        return KErrPropProtocol;

    TInt ret = KErrNoTransforms;
    TInt i, j, count2;
    TInt count = iAttrList->Count();
    //
    // Compare all transforms in the peer proposal to the all local proposals defined
    //  
    for (i = 0; i < count ; i++)
    {
        j = 0;
        count2 = aRemoteProp->iAttrList->Count();
        
        for (j = 0; j < count2 ; j++)
        {
            ret = iAttrList->At(i)->Compare(*aRemoteProp->iAttrList->At(j), aRelaxed); 
            if (ret == KErrNone) {
               if ( aLocalNbr )
                  *aLocalNbr = i;
               return j;
            }    
        }   
    }
    //No matching set of attributes
    return ret; //return the last error
}



//
//class CProposal_IIList
//

CProposal_IIList::CProposal_IIList(TInt aGranularity) : CArrayPtrFlat<CProposal_II>(aGranularity)
{

}

CProposal_IIList::~CProposal_IIList()
{
	ResetAndDestroy();	
}

//IMPORTANT: assume all the proposals in this have the same number so they are AND'd
//If more than one proposal number in this the method won't work
//aTransArray contains the num of transform matching for each proposal
//Returns the Remote proposal num that matches (>=0) or an error (<0, see list)
TInt CProposal_IIList::MultiMatchL(CProposal_IIList *aRemoteProp, TBool aRelaxed, CTransModifierList *aTransArray)
{
    CProposal_II *rem_prop; //remote proposal
    TInt trans_num = KErrNoRemoteProposals;
    TInt local_num;
    TInt i1 = 0, i2;
    TInt prop_numII = aRemoteProp->At(0)->iProposalNum;  //Proposed by the peer

    if ( Count() == 0 )
        return KErrNoLocalProposals;
    
    CProposal_II *prop1 = At(0);    //First proposal in this
    TInt count2 = aRemoteProp->Count();
    TTransModifier *tmodif;

    //loop through the remote proposals list. The local is restarted for every new remote proposal num.
    for ( i2 = 0; i2 < count2 ; i2++ )
    {
        rem_prop = aRemoteProp->At(i2);
        
        if ( rem_prop->iProposalNum == prop_numII )
        {
            // Find matching transform from proposal
            trans_num = prop1->Match(rem_prop, aRelaxed, &local_num);
            
            if ( trans_num >= 0 ) //There's a match
            {
                tmodif = new (ELeave) TTransModifier();
                CleanupStack::PushL(tmodif);
                tmodif->iPropNum  = prop1->iProposalNum; // Store local proposal number     
                tmodif->iTransNum = trans_num; // Store remote transform index in proposal
                tmodif->iReplayWindowLength = prop1->iReplayWindowLength;   //to update SAD correctly
                tmodif->iReducedLifeSecs.Set(prop1->iAttrList->At(local_num)->iLifeDurationSecs);
                tmodif->iReducedLifeKBytes.Set(prop1->iAttrList->At(local_num)->iLifeDurationKBytes);
                tmodif->iResponderLifetimeSecs.Set(prop1->iAttrList->At(local_num)->iResponderLifetimeSecs);
                tmodif->iResponderLifetimeKBytes.Set(prop1->iAttrList->At(local_num)->iResponderLifetimeKBytes);                
                aTransArray->AppendL(tmodif);   //add to the array and go for the next
                CleanupStack::Pop();      //tmodif safe
                if ( (i1 + 1) < Count() ) //still proposals left in 'this' list
                {
                    prop1 = At(i1++);
                    if ( (i2 + 1) == count2 )   //last proposal
                        return KErrPropNumberMismatch;  //No match because more local than remote proposals
                    continue;
                }
   
                if (i2 < (count2 - 1))  
                {
                   //No more local proposals and still remote left                  
                    if (aRemoteProp->At(i2+1)->iProposalNum == prop_numII)
                    {   //num mismatch begin again the local proposals loop
                        i1 = 0;
                        prop1 = At(0);  //First proposal in this
                        aTransArray->Reset();   //Empties all the array because the current match is not valid
                        for (i2++; i2 < count2 ; i2++)  //go for next remote proposal and
                        {
                            rem_prop = aRemoteProp->At(i2);
                            if (rem_prop->iProposalNum != prop_numII)
                            {
                                prop_numII = rem_prop->iProposalNum;    //new proposal number to consider
                                i2--;   //To fetch the correct proposal at the begining of the external loop
                                break;  //next number found break the loop
                            }
                        }
                        continue;       //Main loop continues
                    }
                }
                
                break;  //Loop finished. Acceptable proposal found ! 
            }
            else    //No transform matches so proposals lists don't match
            {       //look for the next remote proposal group (number)
                i1 = 0;
                prop1 = At(0);  //First proposal in this
                aTransArray->Reset();   //Empties all the array because the current match is not valid
                i2++;   //next proposal
                if (i2 == count2)
                    return trans_num;   //Error in the last transform
                for (; i2 < count2 ; i2++)
                {
                    rem_prop = aRemoteProp->At(i2);
                    if ( rem_prop->iProposalNum != prop_numII )
                    {
                       prop_numII = rem_prop->iProposalNum;        //new proposal number to consider
                       i2--;   //To fetch the correct proposal at the begining of the external loop
                       break;  //next number found break the loop
                    }
                }
                continue; //Main loop continues             
            }
        }
        else    //New group of AND'd remote proposals
        {
            i1 = 0;
            prop1 = At(0);
            prop_numII = rem_prop->iProposalNum;    //new proposal num
            aTransArray->Reset();   //Empties all the array
            i2 --;          //To fetch the correct proposal at the begining of the external loop            
        }
        
    }
    
    if ( trans_num >= 0 )
    {
       return prop_numII;  //Remote proposal num that matches
    }
    //Otherwise fails the comparison
    return trans_num;   //No match. Returns last error in a transform
}
