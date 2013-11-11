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

#ifndef IKEV1EXTRA_H
#define IKEV1EXTRA_H

//
// Structures to manage proposal and attribute lists
//

#include <in_sock.h>
#include <e32base.h>
#include "ikev1isakmpct.h"

//Maximum size of the SPI. 16 is ISAKMP SPI, others SPI are defined by its DOI 
//but the supported ones have a smaller size (AH=?  , ESP=? )
#define MAX_SPI_SIZE    ISAKMP_SPI_SIZE
#define ISAKMP_SPI_SIZE 16

#define MAX_LIFE_DURATION   16 //up to 2^16 secs. or KBytes
#define MAX_PRIME_SIZE  256 //the biggest is Oakley group 2 prime is 256 hex digits
#define MAX_GEN_SIZE    2   //the biggest is Oakley group 3 generator is 2 hex digits. (Could be fix size)
#define MAX_CURVE_SIZE  6   //the biggest is Oakley group 3 curve B is 6 hex digits.
#define MAX_ORDER_SIZE  48  //the biggest is Oakley group 4 order is 48 hex digits.
//
// Start numbers for Proposal-and Transform payload numbering 
//
#define FIRST_ISAKMP_PROPOSAL  0
#define FIRST_IPSEC_PROPOSAL   0
#define FIRST_ISAKMP_TRANSFORM 0
#define FIRST_IPSEC_TRANSFORM  0


// Attribute comparison errors
// Leave -1 free to use KErrNotFound for empty lists
const TInt KErrTransformID  = -2;   //different transform IDs
const TInt KErrEncrAlg      = -3;   //different encr algs.
const TInt KErrHashAlg      = -4;   //different hash algs.
const TInt KErrAuthMethod   = -5;   //different auth. methods.
const TInt KErrGroupDesc    = -6;   
const TInt KErrGroupType    = -7;
const TInt KErrGroupPrime   = -8;
const TInt KErrGroupGen1    = -9;
const TInt KErrGroupGen2    = -10;
const TInt KErrGroupCurveA  = -11;
const TInt KErrGroupCurveB  = -12;
const TInt KErrPRF          = -13;
const TInt KErrKeyLength    = -14;
const TInt KErrFieldSize    = -15;
const TInt KErrGroupOrder   = -16;
const TInt KErrLifeTime     = -17;
const TInt KErrLifeSize     = -18;
//const TInt KErrTransfNum      = -19;
const TInt KErrEncMode      = -20;
const TInt KErrAuthAlg      = -21;
const TInt KErrKeyRounds    = -22;
const TInt KErrComprDicSize = -23;
const TInt KErrComprPrivAlg = -24;
const TInt KErrTransformNum = -25;
const TInt KErrPropProtocol = -26;      //Proposals have different protocol.
const TInt KErrNoTransforms = -27;      //Proposal has no transforms
const TInt KErrNoRemoteProposals= -28;  //Remote Proposals list is empty
const TInt KErrNoLocalProposals = -29;  //Local Proposals list is empty
const TInt KErrPropNumberMismatch= -30; //The proposals lists have fiferent number of AND'd proposals

              
//Global functions 
TInt Desc8ToTInt64(const TDesC8 &aLifetime, TInt64 &aTime); //Puts the data in the descriptor into a TInt64
TInt PrefixLen(TInetAddr& anAddr);   //returns the prefix length of a mask
void PrefixMask(TInetAddr &anAddr, TInt aPrefixLen, TInt aFamily);  //Creates a addr. mask of aPrefixLen length

enum TLifeType {LIFETIME, LIFESIZE, NOLIFE};
//Mantains SA attrib info for Phase I negotiation. Defined by IKE RFC 2409
class TAttrib
{
public:
    TAttrib()
    {
        iTransformNum=0;
        iTransformID=0; 
        iEncrAlg=0;
        iHashAlg=0;
        iAuthMethod=0;
        iGroupDesc=0;
        iGroupType=0;
        iPRF=0;
        iKeyLength=0;
        iFieldSize=0;
        iXauthUsed=EFalse;
        iRole=0;
        
        iNext=NULL;
    };
    TInt Compare(TAttrib& aAttr, TBool aIsRelaxed);

public:
    TUint8 iTransformNum;   //Transform number
    TUint8 iTransformID;    //Transform ID
    TUint16 iEncrAlg;       //OAKLEY encryption function    (part of EHAO/EHAS)
    TUint16 iHashAlg;       //OAKLEY hash function          (part of EHAO/EHAS)
    TUint16 iAuthMethod;    //OAKLEY authentication function(part of EHAO/EHAS)
    TUint16 iGroupDesc;     //OAKLEY GROUP
    TUint16 iGroupType;
    TBuf8<MAX_PRIME_SIZE>   iGroupPrime;
    TBuf8<MAX_GEN_SIZE> iGroupGen1;
    TBuf8<MAX_GEN_SIZE> iGroupGen2;
    TBuf8<MAX_CURVE_SIZE> iGroupCurveA;
    TBuf8<MAX_CURVE_SIZE> iGroupCurveB;
    //TUint16 iLifeType;    //Not needed
    TBuf8<MAX_LIFE_DURATION> iLifeDurationSecs;
    TBuf8<MAX_LIFE_DURATION> iLifeDurationKBytes;
    TUint16 iPRF;
    TUint16 iKeyLength;
    TUint16 iFieldSize;
    TBuf8<MAX_ORDER_SIZE> iGroupOrder;

    TBool   iXauthUsed;     // Used for iAuthMethod value modification
    TInt    iRole;          // Used for iAuthMethod value modification
    
    TAttrib *iNext; 
    
};

class TProposal
{
public: //To ensure correct initialization
    TProposal()
    {
        iProposalNum=0;
        iProtocol=0;
        iNumTransforms=0;
        iAttrList=NULL;
    }
public:
    TUint8 iProposalNum;
    TUint8 iProtocol;           // Protocol ID in use
    TBuf8<MAX_SPI_SIZE> iSPI;   // SPI in the current exchange. SPI is in network order
    TUint8 iNumTransforms;

    TAttrib *iAttrList;     //Contains the list of transforms and attribs
    //Only ONE proposal in Phase I
    //TProposal *iNext;
};

class TTransModifier
{
public:
    TTransModifier() : iReducedLifeSecs(NULL,0), iReducedLifeKBytes(NULL,0) {} ;
public:
    TInt   iPropNum;            //Local proposal number selected
    TInt   iTransNum;           //Remote transform number selected
    TPtrC8 iReducedLifeSecs;    //Own lifetime (in case is smaller then received)
    TPtrC8 iReducedLifeKBytes;  //Own lifesize (in case is smaller then received)
    TPtrC8 iResponderLifetimeSecs;    //Responder lifetime
    TPtrC8 iResponderLifetimeKBytes;  //Responder lifetime size    
    TUint8 iReplayWindowLength;
};

NONSHARABLE_CLASS(CTransModifierList) : public CArrayPtrFlat<TTransModifier>
{
public:
    CTransModifierList(TInt aGranularity);
    ~CTransModifierList();
};

//#define MAX_ALG_DATA    1024    //4 + ?? vendor data
#define MAX_ALG_DATA    16        //Feature not supported, size reduced
//Mantains SA attrib info for Phase II negotiation. Defined by IPSEC DOI (RFC 2407)
class TAttrib_II
{
public:
    TAttrib_II();
    TInt Compare(TAttrib_II& aAttr, TBool aRelaxed);
    void Copy(TAttrib_II &aAttr);
public:
    TUint8 iTransformNum;   //Transform number
    TUint8 iTransformID;    //Transform ID
    TBuf8<MAX_LIFE_DURATION> iLifeDurationSecs;
    TBuf8<MAX_LIFE_DURATION> iLifeDurationKBytes;
    TBuf8<MAX_LIFE_DURATION> iResponderLifetimeSecs;
    TBuf8<MAX_LIFE_DURATION> iResponderLifetimeKBytes;        
    TUint16 iGroupDesc;     //OAKLEY GROUP
    TUint16 iEncMode;       //Encapsulation Mode
    TUint16 iAuthAlg;       //HMAC
    TUint16 iKeyLength;
    TUint16 iKeyRounds;
    TUint16 iComprDicSize;  //Compress Dictionary size
    TBuf8<MAX_ALG_DATA> iComprPrivAlg;

    //TAttrib_II *iNext;    //to make a list
};

class TChosenAttrib_II : public TAttrib_II
{
public:
    TChosenAttrib_II() : TAttrib_II(), iReducedLifeSecs(NULL,0), iReducedLifeKBytes(NULL,0) {} ;
public:
    TPtrC8 iReducedLifeSecs;
    TPtrC8 iReducedLifeKBytes;
};

NONSHARABLE_CLASS(CAttrib_IIList) : public CArrayPtrFlat<TAttrib_II>
{
public:
    CAttrib_IIList(TInt aGranularity);
    ~CAttrib_IIList();
};

NONSHARABLE_CLASS(CProposal_II) : public CBase
{
public: //To ensure correct initialization

    void ConstructL(TInt aGranularity = 1);
    ~CProposal_II();
//  TInt Match(CProposal_II *aRemoteProp, TBool aRelaxed);
    TInt Match(CProposal_II *aRemoteProp, TBool aRelaxed, TInt* aLocalNbr);    

public:
    TUint8 iProposalNum;
    TUint8 iProtocol;           // Protocol ID in use
    TUint8 iReplayWindowLength; // replay window size in bits
    TBuf8<MAX_SPI_SIZE> iSPI;   // SPI in the current exchange. SPI is in network order
    TUint8 iNumTransforms;

    //TAttrib_II *iAttrList;        //Contains the list of transforms and attribs
    CAttrib_IIList *iAttrList;      //Contains the list of transforms and attribs
    //CProposal_II *iNext;
};


NONSHARABLE_CLASS(CProposal_IIList) : public CArrayPtrFlat<CProposal_II>
{
public:
    CProposal_IIList(TInt aGranularity);
    TInt MultiMatchL(CProposal_IIList *aRemoteProp, TBool aRelaxed, CTransModifierList *aTransArray);
    ~CProposal_IIList();
};
#endif
