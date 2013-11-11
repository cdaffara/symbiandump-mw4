/*
* Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKE policy parser main module
*
*/



#include <e32std.h>
#include <coeutils.h>
#include "ikepolparser.h"

//
// EXPORTed functions
//

//---------------------------------------------------------------------------
//
// IKE Policy Parsing
//
//---------------------------------------------------------------------------

//
//CIkeData -> contains the ike data
//
CIkeData::CIkeData() : iIkeVersion(1)
    {
    }

EXPORT_C CIkeData* CIkeData::NewL()
    {
    return new (ELeave) CIkeData();
    }

EXPORT_C CIkeData* CIkeData::NewL(const CIkeData *aData)
    {
    CIkeData* self = new (ELeave) CIkeData();
    CleanupStack::PushL(self);
    self->ConstructL(aData);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CIkeData::ConstructL(const CIkeData *aData)
    {
    CopyL(aData);
    }

EXPORT_C CIkeData::~CIkeData()
    {
    delete iRemoteIdentity;
    delete iEAPRealmPrefix;
    delete iEAPManualRealm;
    delete iEAPManualUserName;    
    delete iCRACKLAMUserName;
    delete iCRACKLAMPassword;
    delete iClientCertType;
    delete iFQDNAddr;
    Empty();  
    }

EXPORT_C void CIkeData::CopyL(const CIkeData* aData)
    {
    if ( aData == NULL )
        {
        User::Leave( KErrNoMemory );
        }
    
    Empty();    //Free the previous info
    iAddr = aData->iAddr;       //Peer host address
    iMask = aData->iMask;       //Peer host mask
    if (aData->iFQDNAddr)
    	iFQDNAddr=TStringData::NewL(aData->iFQDNAddr->GetData());
    iMode = aData->iMode;
    iNotify = aData->iNotify;
    iFQDN.Copy(aData->iFQDN);
    iIdType = aData->iIdType;
    iGroupDesc_II = aData->iGroupDesc_II;
    iAcceptPartialRemoteId = aData->iAcceptPartialRemoteId;
    iIkeVersion = aData->iIkeVersion;
    iCommit = aData->iCommit;
    iIpsecExpires = aData->iIpsecExpires;
    iAlwaysSendCert = aData->iAlwaysSendCert;
    iInitialContact = aData->iInitialContact;   //INITIAL-CONTACT sending
    iResponderLifetime = aData->iResponderLifetime; //RESPONDER_LIFETIME sending
    iReplayStatus = aData->iReplayStatus;   //RESPONDER_LIFETIME sending
    iCRACKLAMType = aData->iCRACKLAMType;   //CRACK LAM type
    if (aData->iCRACKLAMUserName)
    	iCRACKLAMUserName=TStringData::NewL(aData->iCRACKLAMUserName->GetData());
    if (aData->iCRACKLAMPassword)
    	iCRACKLAMPassword=TStringData::NewL(aData->iCRACKLAMPassword->GetData());
    iUseInternalAddr = aData->iUseInternalAddr;
    iUseNatProbing   = aData->iUseNatProbing;
    iEspUdpPort      = aData->iEspUdpPort;
    iNatKeepAlive    = aData->iNatKeepAlive;
	iDscp            = aData->iDscp;
    iDPDHeartBeat    = aData->iDPDHeartBeat;
    iRekeyingThreshold = aData->iRekeyingThreshold;
    iUseXauth   = aData->iUseXauth; 
    iUseCfgMode = aData->iUseCfgMode; 
    iUseMobIke  = aData->iUseMobIke;

    iRemoteIdType      = aData->iRemoteIdType;
    iSkipRemoteIdCheck = aData->iSkipRemoteIdCheck;
    if ( aData->iRemoteIdentity )
        iRemoteIdentity = TStringData::NewL(aData->iRemoteIdentity->GetData());
    iEAPProtocol     = aData->iEAPProtocol;
    iEAPHideIdentity = aData->iEAPHideIdentity;   
    if ( aData->iEAPRealmPrefix )
        iEAPRealmPrefix = TStringData::NewL(
            aData->iEAPRealmPrefix->GetData()); 
    if ( aData->iEAPManualRealm )
        iEAPManualRealm = TStringData::NewL(
            aData->iEAPManualRealm->GetData()); 
    if ( aData->iEAPManualUserName )
        iEAPManualUserName = TStringData::NewL(
            aData->iEAPManualUserName->GetData());
    if (aData->iClientCertType)
    		iClientCertType = TStringData::NewL(aData->iClientCertType->GetData());

    iNumProp = aData->iNumProp;
    TProposalData* prop = NULL;
    TProposalData* prev = NULL;
    TProposalData* tmp_prop = aData->iPropList;
    for (TInt i=0; i < iNumProp; i++)
        {
        prop = new (ELeave) TProposalData;
        *prop = *tmp_prop;
        if (prev)
            prev->iNext = prop;
        else    //first
            iPropList = prop;

        prop->iPrev = prev;
        prev = prop;
        tmp_prop = tmp_prop->iNext;
        }
    if (prop)
        prop->iNext = NULL;

    iPresharedKey = aData->iPresharedKey;
    iCA = aData->iCA;
    iOwnCert = aData->iOwnCert;
    iPrivKey = aData->iPrivKey;
    iPeerCert = aData->iPeerCert;

    if ( aData->iCAList ) 
        {
        TInt count( aData->iCAList->Count() );
        iCAList = new (ELeave) CArrayFixFlat<TCertInfo*>(count);

        for ( TInt x(0); x < count; x++ )
            {
            TCertInfo* ca_copy = new (ELeave) TCertInfo;
            CleanupStack::PushL(ca_copy);
            *ca_copy = *(aData->iCAList->At(x));
            iCAList->AppendL(ca_copy);
            CleanupStack::Pop(ca_copy);
            }
        }
    }

void CIkeData::Empty()
    {
    TProposalData* prop = iPropList;
    TProposalData* next_prop;
    while (prop)
        {
        next_prop = prop->iNext;
        delete prop;
        prop = next_prop;
        }
    if ( iCAList ) 
        {
        for (TInt i(0); i<iCAList->Count(); ++i)
        {
            delete iCAList->At(i);
            iCAList->At(i) = NULL;
        }
        iCAList->Reset();
        delete iCAList;
        iCAList = NULL;
        }           
    }


//
//  CIkeDataArray
//
CIkeDataArray::CIkeDataArray(TInt aGranularity) 
    :CArrayFixFlat<CIkeData*>(aGranularity)
    {
    }

EXPORT_C CIkeDataArray* CIkeDataArray::NewL(TInt aGranularity)
    {
    ASSERT(aGranularity);
    return new (ELeave) CIkeDataArray(aGranularity);
    }

CIkeDataArray::CIkeDataArray(CIkeDataArray* aData)
    :CArrayFixFlat<CIkeData*>(aData ? aData->Count() : 0)
    {
    }

EXPORT_C CIkeDataArray* CIkeDataArray::NewL(CIkeDataArray* aData)
    {
    User::LeaveIfNull(aData);
    ASSERT(aData->Count()); 
    CIkeDataArray* self = new (ELeave) CIkeDataArray(aData);
    CleanupStack::PushL(self);
    self->ConstructL(aData);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CIkeDataArray::ConstructL(CIkeDataArray *aData)
    {
    CopyL(aData);
    }

EXPORT_C CIkeDataArray::~CIkeDataArray()
    {
    Empty();
    }

//Constructs this from the data in aData
EXPORT_C void CIkeDataArray::CopyL(CIkeDataArray* aData)
    {
    User::LeaveIfNull(aData);
    CIkeData* ike_data(NULL);
    for (TInt i(0); i < aData->Count(); i++)
        {
        ike_data = CIkeData::NewL(aData->At(i));
        AppendL(ike_data);
        }
    iMaxLifetimeKB    = aData->iMaxLifetimeKB;
    iMaxLifetimeSec   = aData->iMaxLifetimeSec;
    iMaxRetrans       = aData->iMaxRetrans;
    iMaxTraceFileSize = aData->iMaxTraceFileSize;
    }

EXPORT_C void CIkeDataArray::Empty()
    {
    for (TInt i(0); i < Count(); i++)
        {
        delete At(i);
        }
    Reset();
    }



//
//TIKEParser  -> parses the ike data from a descriptor or writes it to a file
//
// Symbian change - start
#ifdef __VC32__
#pragma warning(disable : 4097) // typedef-name used as synonym for class-name
#endif
// Symbian change - end
EXPORT_C TIkeParser::TIkeParser() : TLex(), iVersion(0)
{

}

EXPORT_C TIkeParser::TIkeParser(const TDesC &aStr, TInt aVersion) : TLex(aStr), iVersion(aVersion)
{

}

//Parses the Main ike file
EXPORT_C void TIkeParser::MainParseL(CIkeDataArray *aIkeList)
{
    TPtrC ikeHostSeparator = IKE_HOST_SEPARATOR;
    DoMainParseL(aIkeList, ikeHostSeparator);    
}

EXPORT_C void TIkeParser::ParseIKESectionsL(CIkeDataArray *aIkeList)
{
    TPtrC ikeSeparator = IKE_SEPARATOR;
    DoMainParseL(aIkeList, ikeSeparator);
}

void TIkeParser::DoMainParseL(CIkeDataArray* aIkeList, TPtrC& aSectionName)
{
    ASSERT( aIkeList );

    TPtrC token(NULL,0);
    
    while (!Eos())
    {
        token.Set(NextToken());
        if (token.Compare(aSectionName)==0)
        {
            CIkeData* ike_piece = CIkeData::NewL();
            CleanupStack::PushL(ike_piece);
            ParseIkePieceL(ike_piece, SEC_PARSER_VERSION, //Always the latest because is generated by the program itself
                           aSectionName);    
            aIkeList->AppendL(ike_piece);
            CleanupStack::Pop(ike_piece);
        }
        else if (token.Compare(IKE_GENERAL_SEPARATOR)==0)
        {
            //Always the latest because is generated by the program itself
            User::LeaveIfError(ParseGeneralData(aIkeList));
        }
    }
}


//Only 2 sections of general data (max lifetimes)
TInt TIkeParser::ParseGeneralData(CIkeDataArray* aIkeList)
{
    ASSERT(aIkeList);
    
    TLex sub_num;   //Used for number conversion
    TInt err = KErrNone;
    TPtrC token(NULL,0);
    while (!Eos() && err==KErrNone)
    {
        Mark();
        token.Set(NextToken());
        if (token.CompareF(_L("ISAKMP_SA_MAX_LIFETIME_SEC:")) == 0) //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(aIkeList->iMaxLifetimeSec,EDecimal)!=KErrNone)
                err = KSecParserErrMaxLifetimeSec;
        }
        else if (token.CompareF(_L("ISAKMP_SA_MAX_LIFETIME_KB:"))==0)   //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(aIkeList->iMaxLifetimeKB,EDecimal)!=KErrNone)
                err = KSecParserErrMaxLifetimeKB;
        }
        else if (token.CompareF(_L("ISAKMP_MAX_RETRANS:"))==0)  //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(aIkeList->iMaxRetrans)!=KErrNone)
                err = KSecParserErrMaxRetrans;
        }
        else if (token.CompareF(_L("TRACE_FILE_SIZE:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(aIkeList->iMaxTraceFileSize)!=KErrNone)
                err = KSecParserErrMaxRetrans;
        }
        
        else if (token.Compare(IKE_HOST_SEPARATOR)==0)  //END of general section
        {
            UnGetToMark();  //Ungets the token
            return KErrNone;
        }
        else if (token.Compare(IKE_GENERAL_SEPARATOR)==0)//END of general section
        {
            UnGetToMark();  //Ungets the token
            return KErrNone;
        }
        else if (token.Compare(IKE_SEPARATOR)==0)//END of general section
        {
            UnGetToMark();  //Ungets the token
            return KErrNone;
        }
        //others ignored
    }
    
    return err;
}

void TIkeParser::ParseIkePieceL(CIkeData* aIkeData, TInt aVersion, TPtrC& aSectionName)
{
    TInt pos = Remainder().Find(aSectionName);
    if (pos != KErrNotFound)
    {
        TPtr ike_ptr((TUint16*)Remainder().Ptr(),pos, pos); //Until the next section

        TIkeParser parser(ike_ptr, aVersion);
        parser.ParseL(aIkeData);
        Assign(Remainder().Mid(pos));    //rest of the text to parse
    }
    else    //No more tags
    {
        TIkeParser parser(Remainder(), aVersion);
        parser.ParseL(aIkeData);
        Assign(Remainder().Mid(Remainder().Length()));   
    }
}

EXPORT_C void TIkeParser::ParseL(CIkeData* aConf)
{
    ASSERT(aConf);
    
    TInt err = KErrNone;
    TInt errCA= KErrNone;
    TPtrC token(NULL,0);
    TLex sub_num;   //Used for number conversion

    while (!Eos() && err==KErrNone)
    {
        //General Data
        token.Set(NextToken());
        if (token.CompareF(_L("ADDR:"))==0) //CompareF ignores case
        {
            token.Set(NextToken());

            err = aConf->iAddr.Input(token);
            if (err != KErrNone)
            {
            	err = KSecParserErrPeerAddr;
            	aConf->iFQDNAddr = TStringData::NewL(token);
            }            
            if (iVersion >= 2)  //Incorporated in version 2
            {
                token.Set(NextToken());
                err = aConf->iMask.Input(token);
                if (err != KErrNone)
                    err = KSecParserErrPeerMask;
            }
        }
        else if (token.CompareF(_L("DNS_SERVER:"))==0)
        {
            token.Set(NextToken());
            
            err = aConf->iDnsServer.Input(token);
            if (err != KErrNone)
            {
            	err = KSecParserErrUnknown;
            }            
        }        
        else if (token.CompareF(_L("MODE:"))==0)    //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("Main"))==0)
                aConf->iMode = IKE_PARSER_MAIN;
            else if (token.CompareF(_L("Aggressive"))==0)
                aConf->iMode = IKE_PARSER_AGGRESSIVE;
            else
                err = KSecParserErrMode;
        }
        else if (token.CompareF(_L("IKE_VERSION:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iIkeVersion, EDecimal) != KErrNone )
            {
            	err = KSecParserErrUnknown;
            }
            else if ( aConf->iIkeVersion != 2 )
                aConf->iIkeVersion = 1; // IKE version 1 is currently the default
        }
        else if (token.CompareF(_L("SEND_NOTIFICATION:"))==0)   //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iNotify = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iNotify = EFalse;
            else
                err = KSecParserErrNotify;
        }
        else if (token.CompareF(_L("FQDN:"))==0)    //CompareF ignores case
        {
            token.Set(GetRestOfLine());
            aConf->iFQDN.Copy(token);           
        }
        else if (token.CompareF(_L("ID_TYPE:"))==0) //CompareF ignores case
        {
            // Specifies IKE Phase 1 Identity type. Integer value specified in rfc2407
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iIdType, EDecimal)!= KErrNone )
                aConf->iIdType = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("REMOTE_ID_TYPE:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iRemoteIdType, EDecimal)!= KErrNone )
                aConf->iRemoteIdType = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("REMOTE_IDENTITY:"))==0)    //CompareF ignores case
        {
            token.Set(GetRestOfLine());
            aConf->iRemoteIdentity = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("ACCEPT_PARTIAL_REMOTE_ID:"))==0)    //CompareF ignores case
    	{
    		aConf->iAcceptPartialRemoteId = EFalse;            
    		token.Set(NextToken());
    		if (token.CompareF(_L("True"))==0)
    			aConf->iAcceptPartialRemoteId = ETrue;
    	}
        else if (token.CompareF(_L("SKIP_REMOTE_ID_CHECK:"))==0)    //CompareF ignores case
        {
            aConf->iSkipRemoteIdCheck = EFalse;            
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iSkipRemoteIdCheck = ETrue;
        }
        else if (token.CompareF(_L("GROUP_DESCRIPTION_II:"))==0)    //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("MODP_768"))==0)
                aConf->iGroupDesc_II = IKE_PARSER_MODP_768;
            else if (token.CompareF(_L("MODP_1024"))==0)
                aConf->iGroupDesc_II = IKE_PARSER_MODP_1024;
            else if (token.CompareF(_L("MODP_1536"))==0)
                aConf->iGroupDesc_II = IKE_PARSER_MODP_1536;
            else if (token.CompareF(_L("MODP_2048"))==0)
                aConf->iGroupDesc_II = IKE_PARSER_MODP_2048;
            else
            {
                err = KSecParserErrGroupDesc_II;
            }
        }
        else if (token.CompareF(_L("USE_COMMIT:"))==0)  //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iCommit = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iCommit = EFalse;
            else
                err = KSecParserErrCommit;
        }
        else if (token.CompareF(_L("IPSEC_EXPIRE:"))==0)    //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iIpsecExpires = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iIpsecExpires = EFalse;
            else
                err = KSecParserErrIpsecExpire;
        }
        else if (token.CompareF(_L("SEND_CERT:"))==0)   //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iAlwaysSendCert = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iAlwaysSendCert = EFalse;
            else
                err = KSecParserErrSendCert;
        }
        else if (token.CompareF(_L("INITIAL_CONTACT:"))==0) //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iInitialContact = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iInitialContact = EFalse;
            else
                err = KSecParserErrInitialContact;
        }
        else if (token.CompareF(_L("RESPONDER_LIFETIME:"))==0)  //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iResponderLifetime = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iResponderLifetime = EFalse;
            else
                err = KSecParserErrResponderLifetime;
        }
        else if (token.CompareF(_L("REPLAY_STATUS:"))==0)   //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iReplayStatus = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iReplayStatus = EFalse;
            else
                err = KSecParserErrReplayStatus;
        }
        else if (token.CompareF(_L("CRACK_LAM_TYPE:"))==0)   //CompareF ignores case
        {
            token.Set(NextToken());
            
            //There is no difference between SECUREID nowadays. So both values are 
            //mapped to IKE_PARSER_CRACK_PASSWORD
            if (token.CompareF(_L("PASSWORD"))==0 || token.CompareF(_L("SECURID"))==0)
                aConf->iCRACKLAMType = IKE_PARSER_CRACK_PASSWORD;
            else
            {
                err = KSecParserErrCRACKLAMType;
            }
        }
        else if (token.CompareF(_L("CRACK_LAM_USERNAME:"))==0)   //CompareF ignores case
        {
            token.Set(GetRestOfLine());
			aConf->iCRACKLAMUserName = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("CRACK_LAM_PASSWORD:"))==0)   //CompareF ignores case
        {
            token.Set(GetRestOfLine());
			aConf->iCRACKLAMPassword = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("OWN_CERT_TYPE:"))==0)   //CompareF ignores case
        {
            token.Set(GetRestOfLine());
			aConf->iClientCertType = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("USE_INTERNAL_ADDR:"))==0)  //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iUseInternalAddr = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iUseInternalAddr = EFalse;
            else
                err = KSecParserErrUseIntAddr;
        }
        else if (token.CompareF(_L("USE_NAT_PROBE:"))==0)  //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iUseNatProbing = ETrue;
            else if (token.CompareF(_L("False"))==0)
                aConf->iUseNatProbing = EFalse;
            else
                err = KSecParserErrUseNATProbe;
        }
        else if (token.CompareF(_L("ESP_UDP_PORT:"))==0) //CompareF ignores case
        {
            // Specifies port value for IPSEC ESP encapsulation
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iEspUdpPort) != KErrNone )
               aConf->iEspUdpPort = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("NAT_KEEPALIVE:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iNatKeepAlive) != KErrNone )
                aConf->iNatKeepAlive = 0; // In error situation, set id value to reserved (= Not defined)  
    	}
    	else if (token.CompareF(_L("DSCP_VALUE:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
        	if ( sub_num.Val(aConf->iDscp, EDecimal) != KErrNone )
                aConf->iDscp = 0; // In error situation, set id value to reserved (= Not defined) 
            aConf->iDscp <<= 2;     
        }
        else if (token.CompareF(_L("DPD_HEARTBEAT:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iDPDHeartBeat) != KErrNone )
                aConf->iDPDHeartBeat = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("REKEYING_THRESHOLD:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iRekeyingThreshold) != KErrNone )
                aConf->iRekeyingThreshold = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("USE_XAUTH:"))==0) //CompareF ignores case
        {
            aConf->iUseXauth = EFalse;          
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iUseXauth = ETrue;
        }
        else if (token.CompareF(_L("USE_MODE_CFG:"))==0) //CompareF ignores case
        {
            aConf->iUseCfgMode = EFalse;            
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iUseCfgMode = ETrue;
        }
        else if (token.CompareF(_L("USE_MOBIKE:"))==0) //CompareF ignores case
        {
            aConf->iUseMobIke = EFalse;            
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iUseMobIke = ETrue;
        }
        else if (token.CompareF(_L("EAP_PROTOCOL:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();          
            if ( sub_num.Val(aConf->iEAPProtocol, EDecimal)!= KErrNone )
                aConf->iEAPProtocol = 0; // In error situation, set id value to reserved (= Not defined)  
        }
        else if (token.CompareF(_L("EAP_REALM_PREFIX:"))==0)    //CompareF ignores case
        {
            token.Set(GetRestOfLine());
            aConf->iEAPRealmPrefix = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("EAP_MANUAL_REALM:"))==0)    //CompareF ignores case
        {
            token.Set(GetRestOfLine());
            aConf->iEAPManualRealm = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("EAP_MANUAL_USERNAME:"))==0)    //CompareF ignores case
        {
            token.Set(GetRestOfLine());
            aConf->iEAPManualUserName = TStringData::NewL(token);
        }
        else if (token.CompareF(_L("EAP_HIDE_IDENTITY:"))==0)    //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("True"))==0)
                aConf->iEAPHideIdentity = ETrue;
            else aConf->iEAPHideIdentity = EFalse;
        }
        
        else if (token.CompareF(_L("PROPOSALS:"))==0)   //CompareF ignores case
            err = ParseProposals(aConf);
        //Preshared Keys List
        else if (token.CompareF(_L("PRESHARED_KEYS:"))==0)  //CompareF ignores case
            err = ParsePresharedKeys(aConf);
        //CAs Data
        else if (token.CompareF(_L("CAs:"))==0) //CompareF ignores case
            {
           	err = ParseCAs(aConf);
        	if (!err)
            	aConf->iCAFound=ETrue;
            }        //Own Certs List
        else if (token.CompareF(_L("OWN_CERTS:"))==0)   //CompareF ignores case
            err = ParseOwnCerts(aConf);
        //PeerCerts List
        else if (token.CompareF(_L("PEER_CERTS:"))==0)  //CompareF ignores case
            err = ParsePeerCerts(aConf);
    }
	if ( err == KErrNone )
		errCA=CheckPolicy(aConf);
    if (errCA)
    	err=errCA;
    User::LeaveIfError(err);
}

TInt TIkeParser::CheckPolicy(CIkeData* aConf) 
{
	TInt errno=KErrNone;
	if (aConf->iCARequired && !aConf->iCAFound)
		errno=KSecParserErrCA;
	return errno;
}

EXPORT_C TInt TIkeParser::BufferAppend(HBufC8*& aPolBfr, const TDesC8& aText)
{
    TInt err = KErrNone;
    // Make sure that we have enough space for the new text

    TInt spaceLeft = aPolBfr->Des().MaxLength() - aPolBfr->Des().Length();
    if (aText.Length() > spaceLeft)
        {
        // Allocate enough space for the new text + some additional
        // free space so that allocations are not too frequent

        TInt newMaxLength = aPolBfr->Des().MaxLength() 
                            + aText.Length() + KIkePolicyBufferSizeIncrement;
        HBufC8* tempBfr = aPolBfr->ReAlloc(newMaxLength);
        if (tempBfr)
            {
            aPolBfr = tempBfr;
            }
        else
            {
            return KErrNoMemory;
            }
        }
    aPolBfr->Des().Append(aText);
    return err;
}


//Writes one Host Info.
EXPORT_C TInt TIkeParser::MainWrite(CIkeData *aConf, HBufC8*& aPolBfr)
{

    TBuf8<IKE_HOST_SEPARATOR_LEN> buf;
    buf.Copy(IKE_HOST_SEPARATOR);
    TInt err = BufferAppend(aPolBfr, buf);

    if (err != KErrNone)
        return err;
    err = BufferAppend(aPolBfr, (_L8("\n")));
    if (err != KErrNone)
        return err;
    return Write(aConf, aPolBfr);
}

//Writes some IKE General Data. ("Old Version" no trace file size)
EXPORT_C TInt TIkeParser::GeneralInfoWrite(TUint32 aMaxLifetimeSec, 
    TUint32 aMaxLifetimeKB, TInt aMaxRetrans, HBufC8*& aPolBfr)
{
    return GeneralInfoWrite(aMaxLifetimeSec, aMaxLifetimeKB, 
                            aMaxRetrans, 0, aPolBfr);
}

//Writes some IKE General Data. ("New Version")
EXPORT_C TInt TIkeParser::GeneralInfoWrite(TUint32 aMaxLifetimeSec, 
    TUint32 aMaxLifetimeKB, TInt aMaxRetrans, 
    TInt aMaxTraceFileSize, HBufC8*& aPolBfr)
{
    TBuf8<40> line;

    TBuf8<IKE_GENERAL_SEPARATOR_LEN> buf;
    buf.Copy(IKE_GENERAL_SEPARATOR);
    TInt err = BufferAppend(aPolBfr, buf);
    
    if (err != KErrNone)
        return err;
    err = BufferAppend(aPolBfr, (_L8("\n")));
    if (err != KErrNone)
        return err;

    line.Format(_L8("ISAKMP_SA_MAX_LIFETIME_SEC: %u\n"),aMaxLifetimeSec);   
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    
    line.Format(_L8("ISAKMP_SA_MAX_LIFETIME_KB: %u\n"),aMaxLifetimeKB); 
    err = BufferAppend(aPolBfr, line);

    line.Format(_L8("ISAKMP_MAX_RETRANS: %u\n"),aMaxRetrans);   
    err = BufferAppend(aPolBfr, line);

    line.Format(_L8("TRACE_FILE_SIZE: %u\n"), aMaxTraceFileSize);   
    err = BufferAppend(aPolBfr, line);
    
    return err;

}


EXPORT_C TInt TIkeParser::Write(CIkeData* aConf, HBufC8*& aPolBfr)
{
    ASSERT(aConf);
    
    TBuf8<1024> line;
    TInt err = KErrNone;
    TBuf<39> addr_buf;
    TBuf<39> mask_buf;

    TBuf8<39> addr8;
    TBuf8<39> mask8;

    line.Append(_L8("ADDR: "));
    if (!aConf->iAddr.IsUnspecified())
        aConf->iAddr.OutputWithScope(addr_buf);
    else
        addr_buf.Copy(_L("0.0.0.0"));   
    if (!aConf->iMask.IsUnspecified())
        aConf->iMask.OutputWithScope(mask_buf);
    else    //only happens with version 1 (no mask)
        mask_buf.Copy(_L("255.255.255.255"));   

    addr8.Copy(addr_buf);
    line.Append(addr8);
    line.Append(_L(" "));
    mask8.Copy(mask_buf);
    line.Append(mask8);
    line.Append(_L8("\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    

    
    if (!aConf->iDnsServer.IsUnspecified())
    {            
        aConf->iDnsServer.OutputWithScope(addr_buf);        
        line = _L8("DNS_SERVER: ");
        addr8.Copy(addr_buf);
        line.Append(addr8);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
                
        
    }

            


    if ( (aConf->iMode == IKE_PARSER_MAIN) || (aConf->iMode == IKE_PARSER_AGGRESSIVE) )
    { 
        line.Copy(_L8("MODE: "));
        line.Append(TextMode(aConf->iMode));
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }   

    if ( aConf->iIkeVersion != 0 )
    { 
        line.Format(_L8("IKE_VERSION: %d\n"), aConf->iIkeVersion);          
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
  
    line.Copy(_L8("SEND_NOTIFICATION: "));
    if (aConf->iNotify)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    if (aConf->iFQDN.Length() > 0)
    {
        line.Copy(_L8("FQDN: "));
        line.Append(aConf->iFQDN);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    if (aConf->iIdType != 0)
    {
        line.Format(_L8("ID_TYPE: %d\n"), aConf->iIdType);          
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }

    if ( aConf->iRemoteIdentity )
    {
        line.Copy(_L8("REMOTE_IDENTITY: "));
        line.Append(aConf->iRemoteIdentity->GetData());
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    if (aConf->iRemoteIdType != 0) 
    {
        line.Format(_L8("REMOTE_ID_TYPE: %d\n"), aConf->iRemoteIdType);          
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
	
	line.Copy(_L8("ACCEPT_PARTIAL_REMOTE_ID: "));          	
	if (aConf->iAcceptPartialRemoteId) 
		line.Append(_L("TRUE\n"));
	else
		line.Append(_L("FALSE\n"));
	err = BufferAppend(aPolBfr, line);
	if (err != KErrNone)
		return err;
	
    line.Copy(_L8("SKIP_REMOTE_ID_CHECK: "));           
    if (aConf->iSkipRemoteIdCheck) 
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    if (aConf->iGroupDesc_II != 0) 
        {
        line.Copy(_L8("GROUP_DESCRIPTION_II: "));
        line.Append(TextGroupDesc(aConf->iGroupDesc_II));
        line.Append(_L("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
        }

    line.Copy(_L8("USE_COMMIT: "));
    if (aConf->iCommit)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("IPSEC_EXPIRE: "));
    if (aConf->iIpsecExpires)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("SEND_CERT: "));
    if (aConf->iAlwaysSendCert)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("INITIAL_CONTACT: "));
    if (aConf->iInitialContact)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("RESPONDER_LIFETIME: "));
    if (aConf->iResponderLifetime)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("REPLAY_STATUS: "));
    if (aConf->iReplayStatus)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    if ( aConf->iCRACKLAMType == IKE_PARSER_CRACK_PASSWORD )
    {
        line.Copy(_L8("CRACK_LAM_TYPE: "));
        line.Append(TextLAMType(aConf->iCRACKLAMType));
        line.Append(_L("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    
    //IKE Private (Nokia VPN specific) extensions
    line.Copy(_L8("USE_INTERNAL_ADDR: "));
    if (aConf->iUseInternalAddr)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    
    line.Copy(_L8("USE_NAT_PROBE: "));
    if (aConf->iUseNatProbing)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    if (aConf->iEspUdpPort != 0)
    {
        line.Format(_L8("ESP_UDP_PORT: %d\n"), aConf->iEspUdpPort);          
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }

    line.Format(_L8("NAT_KEEPALIVE: %d\n"), aConf->iNatKeepAlive);          
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
  
	line.Format(_L8("DSCP_VALUE: %d\n"), aConf->iDscp >> 2);          
	err = BufferAppend(aPolBfr, line);
	if (err != KErrNone)
		return err;
	
    line.Format(_L8("DPD_HEARTBEAT: %d\n"), aConf->iDPDHeartBeat);          
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Format(_L8("REKEYING_THRESHOLD: %d\n"), aConf->iRekeyingThreshold);
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
  
    line.Copy(_L8("USE_XAUTH: "));
    if (aConf->iUseXauth)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    
    line.Copy(_L8("USE_MODE_CFG: "));
    if (aConf->iUseCfgMode)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    line.Copy(_L8("USE_MOBIKE: "));
    if (aConf->iUseMobIke)
        line.Append(_L("TRUE\n"));
    else
        line.Append(_L("FALSE\n"));
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;
    if ( aConf->iCRACKLAMUserName )
	{
		line.Copy(_L8("CRACK_LAM_USERNAME: "));
		line.Append(aConf->iCRACKLAMUserName->GetData());
		line.Append(_L8("\n"));
		err = BufferAppend(aPolBfr, line);
		if (err != KErrNone)
			return err;
	}
	if ( aConf->iCRACKLAMPassword )
	{
		line.Copy(_L8("CRACK_LAM_PASSWORD: "));
		line.Append(aConf->iCRACKLAMPassword->GetData());
		line.Append(_L8("\n"));
		err = BufferAppend(aPolBfr, line);
		if (err != KErrNone)
			return err;
	}
	if ( aConf->iClientCertType )
	{
		line.Copy(_L8("OWN_CERT_TYPE: "));
		line.Append(aConf->iClientCertType->GetData());
		line.Append(_L8("\n"));
		err = BufferAppend(aPolBfr, line);
		if (err != KErrNone)
			return err;
	}

    if ( aConf->iEAPProtocol != 0 ) 
    {
        line.Format(_L8("EAP_PROTOCOL: %d\n"), aConf->iEAPProtocol);          
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
        line.Copy(_L8("EAP_HIDE_IDENTITY: "));
        if (aConf->iEAPHideIdentity)
            line.Append(_L("TRUE\n"));
        else
            line.Append(_L("FALSE\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    if ( aConf->iEAPRealmPrefix )
    {
        line.Copy(_L8("EAP_REALM_PREFIX: "));
        line.Append(aConf->iEAPRealmPrefix->GetData());
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    if ( aConf->iEAPManualRealm )
    {
        line.Copy(_L8("EAP_MANUAL_REALM: "));
        line.Append(aConf->iEAPManualRealm->GetData());
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    if ( aConf->iEAPManualUserName )
    {
        line.Copy(_L8("EAP_MANUAL_USERNAME: "));
        line.Append(aConf->iEAPManualUserName->GetData());
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }

    //Proposals
    line.Format(_L8("PROPOSALS: %d\n"), aConf->iNumProp);
    err = BufferAppend(aPolBfr, line);
    if (err != KErrNone)
        return err;

    TProposalData* prop = aConf->iPropList;
    for (TInt i=0; i<aConf->iNumProp; i++)
    {
        line.Copy(_L8("ENC_ALG: "));
        line.Append(TextEncrAlg(prop->iEncrAlg, prop->iEncrKeyLth));
        line.Append(_L8("\nAUTH_METHOD: "));
        line.Append(TextAuthMethod(prop->iAuthMeth));
        line.Append(_L8("\nHASH_ALG: "));
        line.Append(TextHashAlg(prop->iHashAlg));
        line.Append(_L8("\nGROUP_DESCRIPTION: "));
        line.Append(TextGroupDesc(prop->iGroupDesc));
        line.Append(_L8("\nGROUP_TYPE: "));
        line.Append(TextGroupType(prop->iGroupType));
        line.AppendFormat(_L8("\nLIFETIME_KBYTES: %d"),prop->iLifetimeKb);
        line.AppendFormat(_L8("\nLIFETIME_SECONDS: %d"),prop->iLifetimeSec);
        line.Append(_L8("\nPRF: "));
        line.Append(TextPRF(prop->iPRF));
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
        prop = prop->iNext;
    }

    if (aConf->iPresharedKey.iKey.Length() > 0)
    {
        line.Copy(_L8("PRESHARED_KEYS:\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
        line.Copy(_L("FORMAT: "));
        line.Append(TextFormat(aConf->iPresharedKey.iFormat));
        line.AppendFormat(_L8("\nKEY: %d "),aConf->iPresharedKey.iKey.Length());
        line.Append(aConf->iPresharedKey.iKey);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    //
    //CAS
    //Support for multiple CAs
    //
    if ( aConf->iCAList )
    {   
        TInt ca_count = aConf->iCAList->Count();
        line.Copy(_L("CAs: "));
        line.AppendNum(ca_count);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
     
        for (TInt i=0; i < ca_count; i++)
        {
            line.Copy(_L("  FORMAT: "));
            line.Append(CertFormat(aConf->iCAList->At(i)->iFormat));
            line.Append(_L("\n  DATA: "));
            line.Append(aConf->iCAList->At(i)->iData);
            line.Append(_L8("\n"));
            err = BufferAppend(aPolBfr, line);
            if (err != KErrNone)
                return err;
        }
    }   

    //Own Certs
    if (aConf->iOwnCert.iOwnCertExists)
        {
        line.Copy(_L8("OWN_CERTS:\n"));
        if (aConf->iOwnCert.iData.Length() > 0)
            {
            line.Append(_L8("  FORMAT: "));
            line.Append(CertFormat(aConf->iOwnCert.iFormat));
            line.Append(_L8("\n  DATA: "));
            line.Append(aConf->iOwnCert.iData);
            line.Append(_L8("\n  PRIVATE_KEY_FORMAT: "));
            line.Append(CertFormat(aConf->iPrivKey.iFormat));
            line.Append(_L8("\n  PRIVATE_KEY_DATA: "));
            line.Append(aConf->iPrivKey.iData);
            line.Append(_L8("\n"));
            line.AppendFormat(_L8("\n  PRIVATE_KEY_LENGTH: %d"), aConf->iOwnCert.iPrivateKeyLength);
            }

        // Always included  
        line.Append(_L8("\n  SUBJECT_DN_SUFFIX: "));
        line.Append(aConf->iOwnCert.iSubjectDnSuffix);
        TInt identityType;
        if (aConf->iOwnCert.iIdentityAsRfc822Name == EIdentityAsRfc822NameUndefined)
            {
            identityType = 2;
            }
        else if (aConf->iOwnCert.iIdentityAsRfc822Name == EIdentityAsRfc822NameNo)
            {
            identityType = 0;
            }
        else
            {
            identityType = 1;
            }
        line.AppendFormat(_L8("\n  IDENTITY_AS_RFC822NAME: %d"), identityType);

        line.Append(_L8("\n  RFC822NAME_FQDN: "));
        line.Append(aConf->iOwnCert.iRfc822NameFqdn);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }

    //Peer Certs
    if (aConf->iPeerCert.iData.Length() > 0)
    {
        line.Format(_L8("PEER_CERTS:"));
        line.Append(_L8("  FORMAT: "));
        line.Append(CertFormat(aConf->iPeerCert.iFormat));
        line.Append(_L8("\n  DATA: "));
        line.Append(aConf->iPeerCert.iData);
        line.Append(_L8("\n"));
        err = BufferAppend(aPolBfr, line);
        if (err != KErrNone)
            return err;
    }
    
    return KErrNone;
}

TInt TIkeParser::ParseProposals(CIkeData* aConf)
{
    ASSERT(aConf);
    
    TLex sub_num = NextToken(); //Used for number conversion
    if (sub_num.Val(aConf->iNumProp) != KErrNone)
        return KSecParserErrProposal;

    if (aConf->iNumProp == 0)
    {
        aConf->iPropList = NULL;
        return KErrNone;
    }

    TPtrC token(NULL,0);
    TProposalData* prev_prop = NULL;
    TInt err = KErrNone;
    TInt authMethodFound=0;
    //At least one proposal!
    for (TInt i=0; i<aConf->iNumProp; i++)   //All the proposals
    {
        TProposalData* prop = NULL;
        prop = new TProposalData;
        if (!prop)
            return KErrNoMemory;
        
        prop->iNext = NULL;
        if (i==0)   //First proposal
            aConf->iPropList = prop;
        else
            prev_prop->iNext = prop;

        prop->iPrev = prev_prop;
		authMethodFound=0;
        token.Set(NextToken());

        if (token.CompareF(_L("ENC_ALG:"))==0)  //CompareF ignores case
        {
            prop->iEncrKeyLth = 0;    // = Use Default key length
            token.Set(NextToken());
            if (token.CompareF(_L("DES-CBC"))==0)
               	prop->iEncrAlg = IKE_PARSER_DES_CBC;
            else if (token.CompareF(_L("3DES-CBC"))==0)
                prop->iEncrAlg = IKE_PARSER_DES3_CBC;
            else if (token.CompareF(_L("AES128-CBC"))==0)
            {
                prop->iEncrKeyLth = 128;                    
                prop->iEncrAlg    = IKE_PARSER_AES_CBC;
            }
            else if (token.CompareF(_L("AES192-CBC"))==0)
            {
                prop->iEncrKeyLth = 192;                    
                prop->iEncrAlg    = IKE_PARSER_AES_CBC;
            }
            else if (token.CompareF(_L("AES256-CBC"))==0)
            {
                prop->iEncrKeyLth = 256;                    
                prop->iEncrAlg    = IKE_PARSER_AES_CBC;
            }
            else
            {
                err = KSecParserErrEncrAlg;
                break;
            }
        }
        else
        {
            err = KSecParserErrEncrAlg;
            break;
        }

        token.Set(NextToken());
        if (token.CompareF(_L("AUTH_METHOD:"))==0)  //CompareF ignores case
        {
            authMethodFound=1;
            token.Set(NextToken());
            if (token.CompareF(_L("RSA_SIGNATURES"))==0)
            {
            	prop->iAuthMeth = IKE_PARSER_RSA_SIG;
            	aConf->iCARequired=ETrue;
            }
            else if (token.CompareF(_L("DSS_SIGNATURES"))==0)
            {
            	prop->iAuthMeth = IKE_PARSER_DSS_SIG;
            	aConf->iCARequired=ETrue;
            }
            else if (token.CompareF(_L("RSA_ENCRYPT"))==0)
            {
            	prop->iAuthMeth = IKE_PARSER_RSA_ENCR;
            	aConf->iCARequired=ETrue;
            }
            else if (token.CompareF(_L("RSA_REV_ENCRYPT"))==0)
            {
            	prop->iAuthMeth = IKE_PARSER_RSA_REV_ENCR;
            	aConf->iCARequired=ETrue;
            }
            else if (token.CompareF(_L("PRE-SHARED"))==0)
                prop->iAuthMeth = IKE_PARSER_PRE_SHARED;
            else if (token.CompareF(_L("IKE-CRACK"))==0)
            {
            	prop->iAuthMeth = IKE_PARSER_CRACK;
            	aConf->iCARequired=ETrue;
            }
            else
            {
                err = KSecParserErrAuthMethod;
                break;
                //ErrorL(R_PARSE_ERROR_AUTH_METHOD);
            }
        }
        else
        {
            if (aConf->iEAPProtocol<1)
            {
            	err = KSecParserErrAuthMethod;
            	break;	
            }
        }
		if (aConf->iEAPProtocol>0)
        		prop->iAuthMeth=IKE_PARSER_PRE_SHARED;
        if (authMethodFound==1)
        	token.Set(NextToken());
        if (token.CompareF(_L("HASH_ALG:"))==0) //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("MD5"))==0)
                prop->iHashAlg = IKE_PARSER_MD5;
            else if (token.CompareF(_L("SHA1"))==0)
                prop->iHashAlg = IKE_PARSER_SHA1;
            else
            {
                err = KSecParserErrHashAlg;
                break;
            }
        }
        else
        {
            err = KSecParserErrHashAlg;
            break;
        }
        token.Set(NextToken());
        if (token.CompareF(_L("GROUP_DESCRIPTION:"))==0)    //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("MODP_768"))==0)
                prop->iGroupDesc = IKE_PARSER_MODP_768;
            else if (token.CompareF(_L("MODP_1024"))==0)
                prop->iGroupDesc = IKE_PARSER_MODP_1024;
            else if (token.CompareF(_L("MODP_1536"))==0)
                prop->iGroupDesc = IKE_PARSER_MODP_1536;
            else if (token.CompareF(_L("MODP_2048"))==0)
                prop->iGroupDesc = IKE_PARSER_MODP_2048;
            else
            {
                err = KSecParserErrGroupDesc;
                break;
            }
        }
        else
        {
            err = KSecParserErrGroupDesc;
            break;
        }

        token.Set(NextToken());
        if (token.CompareF(_L("GROUP_TYPE:"))==0)   //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("MODP"))==0)
                prop->iGroupType = IKE_PARSER_MODP;
            else if(token.CompareF(_L("DEFAULT"))==0)
                prop->iGroupType = IKE_PARSER_DEFAULT;
            else
            {
                err = KSecParserErrGroupType;
                break;
            }
        }
        else
        {
            err = KSecParserErrGroupType;
            break;
        }

        token.Set(NextToken());
        if (token.CompareF(_L("LIFETIME_KBYTES:"))==0)  //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(prop->iLifetimeKb,EDecimal)!=KErrNone)
            {
                err = KSecParserErrLifeBytes;
                break;
            }
            token.Set(NextToken());
        }
		else
        {
            prop->iLifetimeKb=0; 
        }
        if (token.CompareF(_L("LIFETIME_SECONDS:"))==0) //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(prop->iLifetimeSec,EDecimal)!=KErrNone)
            {
                err = KSecParserErrLifeSecs;
                break;
            }
        }
        else
        {
           	err = KSecParserErrLifeSecs;
            break;
        }

        Mark();

        token.Set(NextToken());

        // PRF is an optional parameter, so if it doesn't exist, just use 
        // the default value and unget to mark (so that the rest of the parsing
        // won't fail).
        if (token.CompareF(_L("PRF:"))==0)  //CompareF ignores case
        {
            token.Set(NextToken());
            if (token.CompareF(_L("3DES-CBC-MAC"))==0)
                prop->iPRF = IKE_PARSER_DES3_CBC_MAC;
            else if (token.CompareF(_L("NONE"))==0)
                prop->iPRF = IKE_PARSER_NONE;
            else
            {
                err = KSecParserErrPRF;
                break;
            }
        }
        else
        {
            prop->iPRF = IKE_PARSER_NONE;
            UnGetToMark();
        }

        prev_prop = prop;   //Save as previous proposal
    }   //Proposals loop

    return err;
}


TInt TIkeParser::ParsePresharedKeys(CIkeData* aConf)
{
    ASSERT(aConf);
    
    TPtrC token(NULL, 0);
    TLex sub_num;   //Used for number conversion
    TInt j = 0;
    TInt num=0;

    TPresharedKeyData *key = &aConf->iPresharedKey;
    token.Set(NextToken());

    if (token.CompareF(_L("FORMAT:"))==0)   //CompareF ignores case
    {
        token.Set(NextToken());
        if (token.CompareF(_L("HEX_FORMAT"))==0)
            key->iFormat = HEX_KEY;
        else if (token.CompareF(_L("STRING_FORMAT"))==0)
            key->iFormat = STRING_KEY;
        else
            return KSecParserErrPreFormat;
    }
    else
        return KSecParserErrPreKey;

    token.Set(NextToken());
    if (token.CompareF(_L("KEY:"))==0)  //CompareF ignores case
    {
        sub_num = NextToken();
        if (sub_num.Val(num)!=KErrNone)
            return KSecParserErrPreKey;
        
        if (num > KMaxPresharedKeyLength)
        {
            //Key is too long for our buffer.
            return KSecParserErrPreKey;
        }
        
        if (key->iFormat == HEX_KEY && (num % 2) != 0)
        {
            //HEX encoded keys must contains even number of characters.
            //(Because each byte is expressed by two characters.)
            return KSecParserErrPreKey;
        }
    }
    else
        return KSecParserErrPreKey;

    SkipSpaceAndMark();
    
    if (key->iFormat == STRING_KEY)
    {        
        for (j = 0; j < num && (!Eos()) ; j++)  //Parse the key char to char because it contains blanks
            key->iKey.Append(Get());
    }
    else if (key->iFormat == HEX_KEY)
    {
        //Make sure that key contains a valid hex decim number.
        for (j = 0; j < num && (!Eos()) ; j++)
        {
            TChar c = Get();
            if (c >= TChar('0') && c <= TChar('9') ||
                c >= TChar('a') && c <= TChar('f') ||
                c >= TChar('A') && c <= TChar('F'))
            {
            key->iKey.Append(c);
            }
            else
            {
            return KSecParserErrPreKey;
            }
            
        }
    }
        
    if (j<num)
        return KSecParserErrPreKey;

    return KErrNone;
}


TInt TIkeParser::ParseCAItem(CIkeData* /*aConf*/, TCertInfo* aCA, TPtrC& aToken)
{
    ASSERT(aCA);
    
    if (aToken.CompareF(_L("FORMAT:")) == 0 )   //CompareF ignores case
        {
        aToken.Set(NextToken());
        if (aToken.CompareF(_L("PEM"))==0)
            aCA->iFormat = PEM_CERT;
        else if (aToken.CompareF(_L("BIN"))==0)
            aCA->iFormat = BIN_CERT;
        else if (aToken.CompareF(_L("NAME"))==0)
            aCA->iFormat = CA_NAME;
        else if (aToken.CompareF(_L("KEYID"))==0)
            aCA->iFormat = KEY_ID;
        else if (aToken.CompareF(_L("APPLUID"))==0)
            aCA->iFormat = APPL_UID;
        else            
            return KSecParserErrCA;
        }
    else 
        {
        return KSecParserErrCA;
        }   

    aToken.Set(NextToken());
    if ( aToken.CompareF(_L("DATA:")) == 0)  //CompareF ignores case
        {
        switch(aCA->iFormat)
            {
            case CA_NAME: //falls through
            case APPL_UID:
                {
                aCA->iData = GetRestOfLine();   //ASCII format CA name or Appl uid list
                // Check if data contains empty attributes
                if( KErrNotFound != aCA->iData.FindF(_L("=,")) || KErrNotFound != aCA->iData.FindF(_L("=\"\"")))
                    {
                    return KSecParserErrCA;
                    }
                }
                break;
            case BIN_CERT:                
                return ParseFileName(aCA->iData);                               
                //No break needed
            default:
                aCA->iData = NextToken();       //PEM cert or  Key identifier      
                break;
            }

        return KErrNone;
        }   
    else
        {
        return KSecParserErrCA;
        }   
}

void TIkeParser::ParseCAListL(CIkeData* aConf, TInt aCACount, TPtrC& aToken)
{
    ASSERT(aConf);
    
    aConf->iCAList = new (ELeave) CArrayFixFlat<TCertInfo*>(aCACount);

    while ( aCACount ) 
        {
        TCertInfo* ca_name = new (ELeave) TCertInfo; 
        CleanupStack::PushL(ca_name);
        User::LeaveIfError(ParseCAItem(aConf, ca_name, aToken));
        aConf->iCAList->AppendL(ca_name);
        CleanupStack::Pop(ca_name);
        
        aCACount--;
        if ( aCACount )
           aToken.Set(NextToken()); // Enter to next "FORMAT" tag               
        }
}

/**---------------------------------------------------------------
 *  
 * Parse CAs section in host (=IKE) configuration.
 * There is two possibilities:
 * CAs:
 *   FORMAT: BIN
 *   DATA: jps-SecurID-1_0-ca.cer
 * This supports only one CA (This format may be removed later)
 * CAs: 2
 *   FORMAT: BIN
 *   DATA: jps-SecurID-1_0-ca.cer
 *   FORMAT: BIN
 *   DATA: jps-SecurID-1_1-ca.cer
 * This supports several CA:s. Exact CA count is in line CAs: x
 *
 *----------------------------------------------------------------*/
TInt TIkeParser::ParseCAs(CIkeData* aConf)
{
    TInt ca_count = 0;
    TPtrC token(NULL, 0);
    token.Set(NextToken());

    if ( token.CompareF(_L("FORMAT:")) == 0 )  //CompareF ignores case
        {
        ca_count = 1;
        }
    else 
        {
        TLex sub_num = token;
        if ( (sub_num.Val(ca_count) != KErrNone) || (ca_count == 0) ) 
            {
            return KSecParserErrCA;
            }
        token.Set(NextToken()); // Enter to the first "FORMAT" tag              
        }
    
    TInt err = KErrNone;
    TRAP(err, ParseCAListL(aConf, ca_count, token);)
    return err;
}

TInt TIkeParser::ParseOwnCerts(CIkeData* aConf)
{   
    ASSERT(aConf);
    
    TPtrC token(NULL,0);

    TInt nMandatoryFields=0;
    TInt err=KErrNone;
    TBool endOfSection=EFalse;
    TOwnCertInfo *own_cert = &aConf->iOwnCert;
    own_cert->iOwnCertExists = 0;
    TCertInfo *priv_key = &aConf->iPrivKey;

    while (!endOfSection && err==KErrNone)
    {
        Mark();
        token.Set(NextToken());

        if (token.CompareF(_L("FORMAT:"))==0)   //CompareF ignores case
        {
            nMandatoryFields++;
            token.Set(NextToken());
            if (token.CompareF(_L("BIN"))==0)
                own_cert->iFormat = BIN_CERT;
            else if (token.CompareF(_L("NAME"))==0)
                own_cert->iFormat = CA_NAME;
            else
                 err=KSecParserErrOwnCerts;
        }
        
        else if (token.CompareF(_L("DATA:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            err = ParseFileName(own_cert->iData);
        }

        //Private key part
        else if (token.CompareF(_L("PRIVATE_KEY_FORMAT:"))==0)   //CompareF ignores case
        {
            nMandatoryFields++;
            token.Set(NextToken());
            if (token.CompareF(_L("PEM"))==0)
                priv_key->iFormat = PEM_CERT;
            else if (token.CompareF(_L("BIN"))==0)
                priv_key->iFormat = BIN_CERT;
            else
                err=KSecParserErrOwnCerts;
        }

        else if (token.CompareF(_L("PRIVATE_KEY_DATA:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            err = ParseFileName(priv_key->iData);
        }
        // new stuff // error codes?
        else if (token.CompareF(_L("SUBJECT_DN_SUFFIX:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            own_cert->iSubjectDnSuffix = GetRestOfLine();                          
            // Check if data contains empty attributes
        	if( KErrNotFound != own_cert->iSubjectDnSuffix.FindF(_L("=,")) || 
        	    KErrNotFound != own_cert->iSubjectDnSuffix.FindF(_L("=\"\"")))
        		{
        		return KSecParserErrOwnCerts;
        		}
        }
 
        else if (token.CompareF(_L("IDENTITY_AS_RFC822NAME:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            token.Set(NextToken());
            if (token.CompareF(_L("1"))==0)
                own_cert->iIdentityAsRfc822Name=EIdentityAsRfc822NameYes;
            else if (token.CompareF(_L("0"))==0)
                own_cert->iIdentityAsRfc822Name=EIdentityAsRfc822NameNo;
            else
                own_cert->iIdentityAsRfc822Name=EIdentityAsRfc822NameUndefined;
        }
        else if (token.CompareF(_L("RFC822NAME_FQDN:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            own_cert->iRfc822NameFqdn = NextToken();  
        }

        else if (token.CompareF(_L("PRIVATE_KEY_LENGTH:"))==0) //CompareF ignores case
        {
            nMandatoryFields++;
            TLex sub_num = NextToken();
            if (sub_num.Val(own_cert->iPrivateKeyLength)!=KErrNone)
                err= KSecParserErrOwnCerts;
        }
        else
        {
            UnGetToMark(); // current token didn't belong to this section
            endOfSection=ETrue;
        }
    }
    if (nMandatoryFields<1) // Some mandatory fields are missing
        {
        err= KSecParserErrOwnCerts;
        }
    else
        {
        own_cert->iOwnCertExists = 1;                             // Write Own Certs 
        }
    return err;
}


TInt TIkeParser::ParsePeerCerts(CIkeData* aConf)
{   
    ASSERT(aConf);
    
    TPtrC token(NULL, 0);

    TCertInfo *peer_cert = &aConf->iPeerCert;
    token.Set(NextToken());
    if (token.CompareF(_L("FORMAT:"))==0)   //CompareF ignores case
    {
        token.Set(NextToken());
        if (token.CompareF(_L("PEM"))==0)
            peer_cert->iFormat = PEM_CERT;
        else if (token.CompareF(_L("BIN"))==0)
            peer_cert->iFormat = BIN_CERT;
        else
            return KSecParserErrPreFormat;
    }
    else
        return KSecParserErrPreKey;

    token.Set(NextToken());
    if (token.CompareF(_L("DATA:"))==0) //CompareF ignores case
        peer_cert->iData = NextToken(); //Filename or PEM cert
    else
        return KSecParserErrPreKey;

    return KErrNone;
}

TBufC<16> TIkeParser::TextMode(const TInt aMode)
{
    TBufC<16> ret;

    switch (aMode)
    {
        case IKE_PARSER_MAIN:
            ret=_L("Main");
            break;
        case IKE_PARSER_AGGRESSIVE:
            ret=_L("Aggressive");
            break;
        default:
            break;
    }
    
    return ret;
}

TBufC<16> TIkeParser::TextEncrAlg(const TInt aAlg, const TInt aKeyLth)
{
    TBufC<16> ret;

    switch (aAlg)
    {
        case IKE_PARSER_DES_CBC:
            ret=_L("DES-CBC");
            break;
        case IKE_PARSER_DES3_CBC:
            ret=_L("3DES-CBC");
            break;
        case IKE_PARSER_AES_CBC:
            if ( aKeyLth == 256 )
                 ret=_L("AES256-CBC");
            else if ( aKeyLth == 192 )
                 ret=_L("AES192-CBC");
            else ret=_L("AES128-CBC");
            break;
        default:
            break;
    }
    
    return ret;
}

TBufC<16> TIkeParser::TextAuthMethod(const TInt aMethod)
{
    TBufC<16> ret;

    switch (aMethod)
    {
        case IKE_PARSER_RSA_SIG:
            ret=_L("RSA_SIGNATURES");
            break;
        case IKE_PARSER_DSS_SIG:
            ret=_L("DSS_SIGNATURES");
            break;
        case IKE_PARSER_RSA_ENCR:
            ret=_L("RSA_ENCRYPT");
            break;
        case IKE_PARSER_RSA_REV_ENCR:
            ret=_L("RSA_REV_ENCRYPT");
            break;
        case IKE_PARSER_PRE_SHARED:
            ret=_L("PRE-SHARED");
            break;
        case IKE_PARSER_CRACK:
            ret=_L("IKE-CRACK");
            break;
        default:
            break;
    }
    
    return ret;
}

TBufC<16> TIkeParser::TextHashAlg(const TInt aAlg)
{
    TBufC<16> ret;

    switch (aAlg)
    {
        case IKE_PARSER_MD5:
            ret=_L("MD5");
            break;
        case IKE_PARSER_SHA1:
            ret=_L("SHA1");
            break;
        default:
            break;
    }
    
    return ret;
}

TBufC<16> TIkeParser::TextGroupDesc(const TInt aDesc)
{
    TBufC<16> ret;

    switch (aDesc)
    {
        case IKE_PARSER_MODP_768:
            ret=_L("MODP_768");
            break;
        case IKE_PARSER_MODP_1024:
            ret=_L("MODP_1024");
            break;
        case IKE_PARSER_MODP_1536:
            ret=_L("MODP_1536");
            break;
        case IKE_PARSER_MODP_2048:
            ret=_L("MODP_2048");
            break;
        default:
            break;
    }   
    return ret;
}

TBufC<16> TIkeParser::TextGroupType(const TInt aType)
{
    TBufC<16> ret;

    switch (aType)
    {
        case IKE_PARSER_DEFAULT:
            ret=_L("DEFAULT");
            break;
        case IKE_PARSER_MODP:
            ret=_L("MODP");
            break;
        default:
            break;
    }
    return ret;
}

TBufC<16> TIkeParser::TextPRF(const TInt aPRF)
{
    TBufC<16> ret;

    switch (aPRF)
    {
        case IKE_PARSER_NONE:
            ret=_L("NONE");
            break;
        case IKE_PARSER_DES3_CBC_MAC:
            ret=_L("3DES-CBC-MAC");
            break;
        default:
            break;
    }
    return ret;
}

TBufC<16> TIkeParser::TextFormat(const TKeyFormat aFormat)
{
    TBufC<16> ret;

    switch (aFormat)
    {
        case HEX_KEY:
            ret=_L("HEX_FORMAT");
            break;
        case STRING_KEY:
        default:
            ret=_L("STRING_FORMAT");
            break;
    }
    return ret;
}

TBufC<16> TIkeParser::CertFormat(TCertFormat aFormat)
{
    TBufC<16> ret;

    switch (aFormat)
    {
        case PEM_CERT:
            ret=_L("PEM");
            break;
        case BIN_CERT:
            ret=_L("BIN");
            break;
        case CA_NAME:
            ret=_L("NAME");
            break;
        case KEY_ID:
            ret=_L("KEYID");
            break;
        case APPL_UID:
            ret=_L("APPLUID");
            break;
        default:
            break;
    }
    
    return ret;
}

TBufC<16> TIkeParser::TextLAMType(const TUint8 aLAMType)
{
    TBufC<16> ret;
    
    switch (aLAMType)
    {
        case IKE_PARSER_CRACK_PASSWORD:
            ret=_L("PASSWORD");
            break;
        default:
            break;
    }
    
    return ret;
}


TPtrC TIkeParser::GetRestOfLine()
{
    TChar chr=Get();
    while(chr.IsSpace() && chr!='\n' && chr!='\r')
        chr=Get();
    UnGet();
    Mark();
        chr=Get(); // Fix for empty rest of line
    while(!Eos() && chr!='\n' && chr!='\r')
        chr=Get();
    if (!Eos())
    	UnGet();
    return MarkedToken();
}


TInt TIkeParser::ParseFileName(TDes& aFileName)
{
    //Get the rest of the line                
    //Trim trailing white spaces from the file name
    TPtrC fileName = GetRestOfLine(); 
    HBufC* fileNameCopy = fileName.Alloc();
    if (fileNameCopy == NULL)
        {
        return KErrNoMemory;
        }
    fileNameCopy->Des().TrimRight();
    fileName.Set(fileName.Left(fileNameCopy->Length()));
    delete fileNameCopy;
    
    aFileName = fileName;     
    
    return KErrNone;
}


TBool TSecParser::CheckVersion()
{
    TPtrC token(NULL, 0);
    TLex version_num;

    token.Set(NextToken());
    if (token.Compare(_L("SECURITY_FILE_VERSION:"))==0)
    {
        version_num = NextToken();
        if (version_num.Val(iVersion)!=KErrNone)
            return EFalse;
        if ((iVersion < FIRST_SEC_PARSER_VERSION) || (iVersion > SEC_PARSER_VERSION))
            return EFalse;
    }
    else
        return EFalse;

    return ETrue;
}

void TSecParser::ParseInfoL(CSecurityPiece* aPiece_data)
{
    ASSERT(aPiece_data);
    
    HBufC* buf = HBufC::NewLC(MAX_INFO_SIZE);
    TPtr ptr = buf->Des();
    TChar ch = Get();
    TInt i=0;

    ch = Get();
    while (((ch==' ') || (ch=='\n')) && (!Eos()))
        ch = Get();
    while ((ch!='[') && (!Eos()) && i < MAX_INFO_SIZE)
    {
        ptr.Append(ch);
        i++;
        ch = Get();
    }

    if (i == MAX_INFO_SIZE) //The rest is ignored
    {
        ch = Get();
        while ( (ch != '[') && (!Eos()) )
            ch = Get();
    }

    if (ch =='[')
    {
        UnGet();    // the '['
        if (ptr.Length() > 0)   //If empty no \n
            ptr.SetLength(ptr.Length() - 1);    //eliminates the \n at the end
    }
    aPiece_data->SetInfoL(ptr);
    CleanupStack::PopAndDestroy(buf);
}

TInt TSecParser::ParseGeneral(CGeneralData *aGeneralData)
{
    TInt err;
    TInt pos = Remainder().Find(_L("["));   //The segment is until the next tag or Eos()
    if (pos!=KErrNotFound)
    {

        TPtr gen_ptr((TUint16 *)Remainder().Ptr(),pos, pos); //Until the next section

        TGeneralParser parser(gen_ptr);
        err = parser.Parse(aGeneralData);
        Assign(Remainder().Mid(pos));    //rest of the text to parse
    }
    else    //No more tags
    {
        TGeneralParser parser(Remainder());
        err = parser.Parse(aGeneralData);
    }   

    return (err);
}


void TSecParser::NextTag()
{
    while (!Eos())
        if (Get() == '[' )
        {
            UnGet();    //Next tag found
            return;
        }

}

//Puts the security file data into string format to be saved to disk.
EXPORT_C TInt TSecParser::Write(CSecurityPiece *aPiece_data, HBufC8*& aPolBfr)
{
    TInt err;

    err = WriteVersion(aPolBfr);
    if (err != KErrNone)
        return err;

    err = WriteInfo(aPiece_data, aPolBfr);
    if (err != KErrNone)
        return err;

    return err = WriteIke(aPiece_data, aPolBfr);
}

TInt TSecParser::WriteVersion(HBufC8*& aPolBfr)
{
    TBuf8<32> buf;
    buf.Format(_L8("SECURITY_FILE_VERSION: %d\n"), SEC_PARSER_VERSION);
    return TIkeParser::BufferAppend(aPolBfr, buf);    
}

TInt TSecParser::WriteInfo(CSecurityPiece* aPiece_data, HBufC8*& aPolBfr)
{
    ASSERT(aPiece_data);
    
    TInt err;

    TBuf8<MAX_INFO_SIZE> buf = _L8("[INFO]\n");
    err = TIkeParser::BufferAppend(aPolBfr, buf);
    if (err != KErrNone)
        return err;

    buf.Copy(aPiece_data->Info()->Des());
    err = TIkeParser::BufferAppend(aPolBfr, buf);
    if (err != KErrNone)
        return err;
    return TIkeParser::BufferAppend(aPolBfr, (_L8("\n")));
}

TInt TSecParser::WriteIke(CSecurityPiece* aPiece_data, HBufC8*& aPolBfr)
{
    ASSERT(aPiece_data);

    TBuf8<6> buf = _L8("[IKE]\n");
    TInt err = TIkeParser::BufferAppend(aPolBfr, buf);
    if (err != KErrNone)
        return err;
    return TIkeParser::Write(aPiece_data->IkeData(), aPolBfr);
}


//
//CSecurityPiece
//

EXPORT_C void CSecurityPiece::ConstructL(TInt aSize)
{
    iInfo = HBufC::NewL(aSize);
    iIkeData = CIkeData::NewL();
    iGeneralData = new (ELeave) CGeneralData();
}

EXPORT_C void CSecurityPiece::SetInfoL(const TDesC &aDes)
{
    if (aDes.Length() > iInfo->Des().MaxLength())
        iInfo = iInfo->ReAllocL(aDes.Length()); //ReAllocs if needed
    
    iInfo->Des().Copy(aDes);
}

EXPORT_C CSecurityPiece::~CSecurityPiece()
{
    delete iInfo;
    delete iIkeData;
    delete iGeneralData;
}




EXPORT_C CGeneralData::CGeneralData()
{
}

EXPORT_C CGeneralData::CGeneralData(CGeneralData* aData)
{
    ASSERT(aData);
    iDeactivationTimeout = aData->iDeactivationTimeout;
}

EXPORT_C TGeneralParser::TGeneralParser(const TDesC &aStr) : TLex(aStr)
    {
    }

EXPORT_C TInt TGeneralParser::Parse(CGeneralData* aData)
{
    ASSERT(aData);

    TLex sub_num;   //Used for number conversion
    TInt err = KErrNone;
    TPtrC token(NULL,0);
    while (!Eos() && err==KErrNone)
    {
        Mark();
        token.Set(NextToken());
        if (token.CompareF(_L("POLICY_AUTO_DEACTIVATION_TIMEOUT:")) == 0) //CompareF ignores case
        {
            sub_num = NextToken();
            if (sub_num.Val(aData->iDeactivationTimeout)!=KErrNone)
                err = KErrGeneral;  //change ??
        }
    }
    return err;
}


EXPORT_C TInt TSecParser::ParseGeneralData(CGeneralData* aData)
{
    TPtrC token(NULL, 0);
    TInt ret;

    if (!CheckVersion())
        return KErrNotSupported;    //Invalid file or version
    while (!Eos())
    {
        token.Set(NextToken());
        if (token.Compare(_L("[GENERAL]"))==0)
        {
            ret = ParseGeneral(aData);
            if (ret !=KErrNone)
                return ret;
        }
        else    //Unknown Tag Ignored
            NextTag();

    }

    return KErrNone;
}

//
// IkeParser
//
EXPORT_C RArray<TUid>* IkeParser::GetApplUidListL(
    const TDesC16& aApplUidString)
    {
    //
    // Build application UID array from Application Uid text string
    //
    RArray<TUid>* applUidList = new (ELeave) RArray<TUid>(1);
    CleanupStack::PushL( applUidList );
    TLex ApplUids(aApplUidString);
    TUint32 Uid;
    TUid ApplUid; 
    IkeParser::NextHexaDigit(ApplUids);

    while ( !ApplUids.Eos() )
        {
        if ( ApplUids.Val(Uid, EHex) != KErrNone )
            {
            break;   // Error 
            }
        ApplUid.iUid = Uid;
        User::LeaveIfError( applUidList->Append(ApplUid) );
        IkeParser::NextHexaDigit(ApplUids);   
        }
    CleanupStack::Pop( applUidList );
    return applUidList;
    } 

EXPORT_C HBufC8* IkeParser::TextToHexOctetsL(const TDesC16& aTextString)
{
    TInt Lth = (aTextString.Length() + 1)/2;
    HBufC8* HexOctets = HBufC8::NewL((Lth | 3) + 1);
    if ( DeHex(aTextString.Ptr(), aTextString.Length(), (TUint8*)HexOctets->Ptr()) )
        HexOctets->Des().SetLength(Lth);   

    return HexOctets;
}

EXPORT_C TBool IkeParser::TextToHexOctets(const TDesC16& aTextString, TDes8& aHexOctets)
{
    TBool Status;
    TInt Lth = ((aTextString.Length() + 1)/2);
    if ( aHexOctets.MaxLength() >=  Lth )
    {
        Status = DeHex(aTextString.Ptr(), aTextString.Length(), (TUint8*)aHexOctets.Ptr());
        if ( !Status )
            Lth = 0; 
    }
    else
    {
        Status = EFalse;
        Lth = 0;
    } 
    aHexOctets.SetLength(Lth);
  
    return Status;
}

TBool IkeParser::DeHex(const TUint16* aUcStr, TInt aStrLen, TUint8* aDstBfr)
{
    TBool Status = ETrue;
    TUint8 d1, d2;

    while ( (aStrLen > 0) && Status )
    {
        d1 = (TUint8)(*aUcStr & 0xff);
        Status &= HexVal(d1);
        aUcStr ++;
        if ( aStrLen > 1 )
        { 
           d2 = (TUint8)(*aUcStr & 0xff);
           Status &= HexVal(d2);
        }   
        else d2 = 0;
        aUcStr ++;    
        aStrLen -= 2;
        *aDstBfr++ = (TUint8)(d1 * 16 + d2);
    }
    return Status;
}

TBool IkeParser::HexVal(TUint8& c)
{
    TBool Status = ( c >= 'a' && c <= 'f');
    if ( Status )
        c = (TUint8)(c - 'a' + 10);
    else
    {
        Status = (c >= 'A' && c <= 'F');
        if ( Status )
            c = (TUint8)(c - 'A' + 10);
        else
        {
            Status = (c >= '0' && c <= '9');
            if ( Status )
                c = (TUint8)(c - '0'); 
        } 
    } 
    return Status;
}

void IkeParser::NextHexaDigit(TLex& aUidString)
{
    TChar ch;

    while ( !aUidString.Eos() )
    {
        ch = aUidString.Peek();
        if ( ch.IsHexDigit() )
        {
            break;
        }
        aUidString.Inc();   
    }
}
