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
* Description:   IKEv1 Crack authentication
*
*/


#include "ikev1crack.h"
#include "ikedebug.h"
#include "ikev1pluginsession.h"
#include "ikev1timeout.h"
#include "ikev1negotiation.h"
#include "ikev1payload.h"
#include "ikev1isakmpstream.h"
#include "ikepolparser.h"


//
// Class that implements IKE CRACK authentication method
//

CIKECRACKNegotiation::CIKECRACKNegotiation( MIkeDebug& aDebug )
 : iState( 0 ),
   iDebug( aDebug )
    {
    }

CIKECRACKNegotiation::~CIKECRACKNegotiation()
{
/*-------------------------------------------------------------------
 *
 *  Delete pending dialog - and dialog info objects
 *
 *-------------------------------------------------------------------*/
#ifdef _DEBUG
	if ( iNegotiation ) DEBUG_LOG(_L("CRACK object deleted"));
#endif // _DEBUG	
            
    delete iDialog;
    delete iDialogInfo;
	delete iUserName;
	delete iDomain;
}


TInt CIKECRACKNegotiation::ConstructL(TInt aLAMType, CIkev1Negotiation *aNegotiation, const TDesC &aDomain)
{
/*------------------------------------------------------------------------
 *
 *  This method initializes actions to get authencation information from user.
 *  The authentication information is requested from user with LAM type
 *  specific dialog.
 *  
 *------------------------------------------------------------------------*/
    if ( aLAMType != CRACK_PASSWORD || !aNegotiation ) 
    {
#ifdef _DEBUG    
	   if (	aNegotiation )
		   DEBUG_LOG(_L("CRACK object construction failed, unsupported LAM type"));
#endif // _DEBUG		   
       return CRACK_FAILED;
    }
    iNegotiation = aNegotiation;
    iPluginSession      = aNegotiation->iPluginSession;   
    iLAMType     = aLAMType;
	if ( aDomain.Length() > 0 )
	{
		//
		// Allocate buffer for domain name attribute 
		// (to convey Group Name information)
		// 
		iDomain = HBufC8::NewL(aDomain.Length()); 
		iDomain->Des().Copy(aDomain);
		DEBUG_LOG(_L("CRACK Domain attribute saved"));		
	}	

    DEBUG_LOG(_L("CRACK authentication started"));
    
    return GetDataL(NULL); /* No challenge data */
}


TInt CIKECRACKNegotiation::ExecuteCRACKMsgL(const ThdrISAKMP &aHdr)
{
/*---------------------------------------------------------------------------
 *
 *  IKE message received during CRACK authentication phase:
 *  <--- HDRx*, CHRE or  <--- HDRx*, NOTIFICATION
 *  Process CHRE/Notification payload in IKE message
 *  
 *--------------------------------------------------------------------------*/
    if ( ( iState & WAITING_PEER_RSP ) == 0 ) {
       /*--------------------------------------------------------
        * Not waiting a response from gateway, ignore packet
        *--------------------------------------------------------*/ 
       return CRACK_IGNORE_MSG;  
    }
    iState &= ~WAITING_PEER_RSP;

	CIkev1Payloads* payload = CIkev1Payloads::NewL(aHdr, *iNegotiation, iDebug);
	if (!payload)
	{	
		return CRACK_FAILED;    
	}
	CleanupStack::PushL(payload);
	TInt status;
	
	if ( payload->iChre )
	{
		TInt i = 0;
		while ( i < payload->iNotifs->Count() )
		{
    	   /*----------------------------------------------------------------------
            *
            * A Notification payload received in IKE main/aggressive/information
            * exchange. If this is not a INITIAL-CONTACT notification it is
            * interpreted as a CRACK authentication failure indicated by the gateway.
            *
            *----------------------------------------------------------------------*/
			if ( !iNegotiation->ProcessNotificationL(payload->iNotifs->At(i)) ) {
			   CleanupStack::PopAndDestroy();  //payload
			   return CrackAuthenticationFailedL(payload->iNotifs->At(i)); 
			}      
			i ++;
		}
		
		if ( payload->iIaddr ) {
    	   /*----------------------------------------------------------------------
            *
            * An Internal Address payload received in IKE main/aggressive exchange. 
            *
            *----------------------------------------------------------------------*/
			iNegotiation->ProcessIntAddrL(payload->iIaddr);       			
		}	
	   /*----------------------------------------------------------------------
        *
        * Process attributes in CHRE payload
        *
        *----------------------------------------------------------------------*/
		status = ProcessCHREAttibutesL(payload->iChre);
	}
	else {
	   status = CRACK_CONTINUE;		
	   DEBUG_LOG(_L("No CHRE payload in IKE CRACK message")); 	  
	}        

    CleanupStack::PopAndDestroy();  //payload		
    return status;  

}

TInt CIKECRACKNegotiation::ProcessUserResponseL(CAuthDialogInfo *aDialogInfo )
{
/*---------------------------------------------------------------------------
 *  
 *  A response received from client user (through asynchronous dialog)
 *  Build an IKE message with an appropriate CHRE payload attributes
 *  and send message it to gateway.
 *  
 *-------------------------------------------------------------------------*/
    if ( iState & WAITING_USER_RSP ) 
    {    
       iState &= ~(WAITING_USER_RSP + SECURID_NEXT_PIN_MODE);
       delete iDialog;  /* delete dialog object */
       iDialog = NULL;
       
       TUint16 attr1 = 0;
       TUint16 attr2 = 0; 
       TUint16 attr3 = 0;
       HBufC8* bfr1 = NULL;
       HBufC8* bfr2 = NULL;
       HBufC8* bfr3 = NULL;
       
       /*--------------------------------------------------------
        *
        *  Store attributes according to LAM type
        *
        *--------------------------------------------------------*/
       switch ( iLAMType ) 
       {    
           case CRACK_PASSWORD:
               /*--------------------------------------------------
                *  Possible attributes: User name, Secret, Domain
                *-------------------------------------------------*/
                attr1 = CRACK_T_USERNAME;
                bfr1  = aDialogInfo->iUsername;
                attr2 = CRACK_T_SECRET;
                bfr2  = aDialogInfo->iSecret;
                bfr3  = iDomain;
                if ( bfr3 )
                   attr3 = CRACK_T_DOMAIN;                  
                break;

           default:
                break;
       }       

       SendCredentialsL(attr1, attr2, attr3, bfr1, bfr2, bfr3);
    }
    
    delete aDialogInfo;  /* release dialog info object */
    iDialogInfo = NULL;  /* reset dialog info pointer  */
     
    return CRACK_CONTINUE;
}

TInt CIKECRACKNegotiation::GetDataL(HBufC8* aChallenge)
{
    if ( iLAMType == CRACK_PASSWORD &&
         iNegotiation->iHostData->iCRACKLAMUserName &&
         iNegotiation->iHostData->iCRACKLAMPassword)
        {
        return GetUNPWDFromPolicyL();
        }
    
    else if ( iLAMType == CRACK_PASSWORD &&
              iNegotiation->iCRACKLAMUserName &&
              iNegotiation->iCRACKLAMPassword)
        {
        return GetUNPWDFromNegotiationL();
        }
    
    else
        {
        return GetDatafromUserL(aChallenge); 
        }
}

TInt CIKECRACKNegotiation::GetDatafromUserL(HBufC8* /*aChallenge*/)
{
    TInt status = CRACK_CONTINUE;
    /*---------------------------------------------------------------
     *
     *  Get CRACK authentication information from user according to
     *  current LAM type
     *
     *---------------------------------------------------------------*/
    iDialog     = CIkev1Dialog::NewL( iPluginSession, iPluginSession->DialogAnchor(), iDebug );
    iDialogInfo = new(ELeave) CAuthDialogInfo(iPluginSession, DIALOG_INFO_ID, iNegotiation->SAId(), 0);
    iNegotiation->iTimer->Cancel();   //Cancel previous timer because reply received & processed
    DEBUG_LOG(_L("Timer Cancelled!"));
    iNegotiation->iRetryNum = 0;
                  
    switch ( iLAMType ) 
    {
        case CRACK_PASSWORD:
             /*--------------------------------------------------
              * Request User name and password (domain) from user
              *-------------------------------------------------*/
             iDialog->GetAsyncUNPWDialogL(iDialogInfo, (MIkeDialogComplete*)this);
             break;

        default:     
             status = CRACK_FAILED;
             break;
    }

    iState |= WAITING_USER_RSP + SHOW_ERROR_DIALOG;

    return status;
}

TInt CIKECRACKNegotiation::GetUNPWDFromPolicyL()
{
    ASSERT(iLAMType == CRACK_PASSWORD);
    
    iNegotiation->iTimer->Cancel();   //Cancel previous timer because reply received & processed
    DEBUG_LOG(_L("Timer Cancelled!"));
    iNegotiation->iRetryNum = 0;

    /*--------------------------------------------------------
    *
    *  Store attributes: User name, Secret, Domain
    *
    *--------------------------------------------------------*/

    TUint16 attr1 = CRACK_T_USERNAME;
    HBufC8* bfr1  = iNegotiation->iHostData->iCRACKLAMUserName->GetAsciiDataL();
    CleanupStack::PushL(bfr1);
    TUint16 attr2 = CRACK_T_SECRET;
    HBufC8* bfr2  = iNegotiation->iHostData->iCRACKLAMPassword->GetAsciiDataL();
    CleanupStack::PushL(bfr2);
    HBufC8* bfr3  = iDomain;
    TUint16 attr3 = 0;
    if ( bfr3 )
        {
        attr3 = CRACK_T_DOMAIN;                  
        }

    SendCredentialsL(attr1, attr2, attr3, bfr1, bfr2, bfr3);
    
	CleanupStack::PopAndDestroy(2); // bfr1, bfr2
	
    return CRACK_CONTINUE;
}


TInt CIKECRACKNegotiation::GetUNPWDFromNegotiationL()
{
    ASSERT(iLAMType == CRACK_PASSWORD);
    
    iNegotiation->iTimer->Cancel();   //Cancel previous timer because reply received & processed
    DEBUG_LOG(_L("Timer Cancelled!"));
    iNegotiation->iRetryNum = 0;

    /*--------------------------------------------------------
    *
    *  Store attributes: User name, Secret, Domain
    *
    *--------------------------------------------------------*/

    TUint16 attr1 = CRACK_T_USERNAME;
    HBufC8* bfr1  = iNegotiation->iCRACKLAMUserName;
    TUint16 attr2 = CRACK_T_SECRET;
    HBufC8* bfr2  = iNegotiation->iCRACKLAMPassword;
    HBufC8* bfr3  = iDomain;
    TUint16 attr3 = 0;
    if ( bfr3 )
        {
        attr3 = CRACK_T_DOMAIN;                  
        }

    SendCredentialsL(attr1, attr2, attr3, bfr1, bfr2, bfr3);

    delete iNegotiation->iCRACKLAMUserName;
    iNegotiation->iCRACKLAMUserName = NULL;
    delete iNegotiation->iCRACKLAMPassword;
    iNegotiation->iCRACKLAMPassword = NULL;
    
    return CRACK_CONTINUE;
}

void CIKECRACKNegotiation::SendCredentialsL(TUint16 aAttr1, TUint16 aAttr2, TUint16 aAttr3,
                                            HBufC8* aBfr1, HBufC8* aBfr2, HBufC8* aBfr3)
{
    TIkev1IsakmpStream* msg = iNegotiation->SaveIkeMsgBfr( new (ELeave) TIkev1IsakmpStream(iDebug) );
    msg->IsakmpInit(iNegotiation);
    msg->IsakmpOwnIdentL();    /* Dummy ID for Crypto Cluster */
    msg->IsakmpChre((TUint16)iLAMType, aAttr1, aBfr1, aAttr2, aBfr2, aAttr3, aBfr3);

    if ( iNegotiation->iFamiliarPeer && iNegotiation->iHostData->iUseInternalAddr )
    {
        //
        // Request Internal address from gateway
        //
        msg->IsakmpIntnet(0);  /* null IPV4 address as parameter */
    }

    if ( iNegotiation->iHostData->iInitialContact ) 
    {
        //
        // Initial contact notification added as the last payload into IKE  message
        //
        if (!iPluginSession->FindIkev1SADataWithAddr(iNegotiation->iRemoteAddr))  //Only sent if no ISAKMP SA established
        {
            DEBUG_LOG(_L("Constructing INITIAL-CONTACT"));
            msg->IsakmpNotification(DOI_INITIAL_CONTACT, PROTO_ISAKMP); 
        }
    }
   
    iNegotiation->SendL(*msg);

    //
    // Take a copy of user name buffer in dialog info. This user name
    // is cached into user name file if current CRACK negotiation is
    // succeeded
    //
    if ( aBfr1 ) 
    {
	    delete iUserName;  // Delete old user name buffer for sure
	    iUserName = NULL;  
	    iUserName = HBufC8::New(aBfr1->Length() + 16); // 16 bytes space for padding
	    if ( iUserName ) 
	    {
		    iUserName->Des().Copy(aBfr1->Des()); 
	    } 	   
    }
   
    iState |= WAITING_PEER_RSP;
    iMsgCount++;
}
    
TInt CIKECRACKNegotiation::ProcessCHREAttibutesL(const TCHREISAKMP *aCHRE)
{
/*---------------------------------------------------------------------------
 *
 *  CHRE payload received from gateway. Process attributes in payload
 *  according to current LAM type.
 *  Assure first that LAM type in payload corresponds configured LAM type
 *  in CRACK object
 *  
 *--------------------------------------------------------------------------*/
    TInt length = (TInt)aCHRE->GetLength();
    if ( STATIC_CAST(TUint, length) < sizeof(TCHREISAKMP) ) {
       return CRACK_FAILED; 
    }
    
    length -= sizeof(TCHREISAKMP);  /* Attribute data lengt in payload */
    if ( (aCHRE->GetCHREReserved() != 0) || (aCHRE->GetLAMtype() != iLAMType )) {
       return CRACK_FAILED;         
    }
    
    TDataISAKMP *attr     = aCHRE->CHREAttrib();
    HBufC8  *challenge    = NULL;
    TInt    status        = CRACK_CONTINUE;
    TBool   get_user_data = EFalse;
    TUint16 fin;
    
    while ( length > 0 ) {
        
        length = length - attr->Size();
        if ( length < 0 ) {
           DEBUG_LOG(_L("BAD_PROPOSAL_SYNTAX (Length mismatch in the attibutes)"));
           return CRACK_FAILED;
        }
        switch ( attr->Type() ) {
            
           case CRACK_T_MESSAGE:
                if ( attr->IsBasic() ) {   /* MUST be variable */
                   return CRACK_FAILED;                 
                }
                break;
        
           case CRACK_T_FIN:
                if ( !attr->IsBasic() ) {   /* MUST be basic */
                   return CRACK_FAILED;                 
                }
                fin = attr->Value();
                if ( fin == CRACK_FIN_SUCCESS ) {
                   DEBUG_LOG(_L("CRACK authentication OK"));                     
                   status = CRACK_SUCCESS;
				   if ( iUserName ) {
					  //
					  // Cache user name into user name file
					  //
				      CIkev1Dialog* Dialog = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);		  			 
					  CleanupStack::PushL(Dialog);
					  TInt err(KErrNone);	 
					  TRAP(err, Dialog->StoreUserNameL(iUserName->Des()));
#ifdef _DEBUG					  
					  if (err == KErrNone)
						  DEBUG_LOG(_L("User Name caching succeeded"));
						  DEBUG_LOG(_L("User Name caching failed"));
#endif						  
					  CleanupStack::PopAndDestroy();		  
				   }	   
                }    
                else {
                   if ( fin == CRACK_FIN_MORE ) {
                      iState |= SECURID_NEXT_PIN_MODE; 
                      DEBUG_LOG(_L("CRACK SecurID Next pin mode entered"));                                            
                      get_user_data = ETrue;     /* SecurID "Next code2  */
                   }        
                   else {
                      status = CRACK_FAILED;     /* Illegal FIN value */
                   }      
                }   
                break;
            
           default:
                DEBUG_LOG(_L("ATTRIBUTES_NOT_SUPPORTED (Invalid attribute in CHRE)"));
                return CRACK_FAILED;
        }
        
        attr = attr->Next();
    }

    if ( get_user_data ) {
       /*---------------------------------------------------
        * Get information from user
        *---------------------------------------------------*/
       status = GetDatafromUserL(challenge);
       if ( challenge )
          CleanupStack::PopAndDestroy();  /* delete challenge */       
    }

    return status;
    
}   


TInt CIKECRACKNegotiation::CrackAuthenticationFailedL(const TNotificationISAKMP *aNotifPayload)
{
	(void)aNotifPayload;		
/*---------------------------------------------------------------------------
 *
 *  The gateway has sent a Notification payload which indicates that CRACK 
 *  authentication is failed.
 *  Display proper error dialog and return CRACK_FAILED status
 *  
 *--------------------------------------------------------------------------*/
    iNegotiation->iTimer->Cancel();   //Cancel timer because authentication failed
    DEBUG_LOG(_L("CRACK authentication failed!"));

    if ( (iState & (CRACK_AUTHENTICATED + WAITING_USER_RSP + SHOW_ERROR_DIALOG)) ==
                    SHOW_ERROR_DIALOG ) {       
       // Dialog object shall be delete in Dialog->RunL when dialog completed
       CIkev1Dialog* Dialog = CIkev1Dialog::NewL(iPluginSession, iPluginSession->DialogAnchor(), iDebug);
       Dialog->ShowErrorDialogL(TVpnNoteDialog::EKmdAuthenticationFailed, NULL, NULL);
       iState &= ~SHOW_ERROR_DIALOG;
    }
    
    return CRACK_FAILED;

}

//
// The implementation for class MIkeDialogComplete virtual function
//
TInt CIKECRACKNegotiation::DialogCompleteL(CIkev1Dialog* /*aDialog*/, TAny* aUserInfo,
                                            HBufC8* aUsername, HBufC8* aSecret, HBufC8* aDomain)
{
/*---------------------------------------------------------------------------
 *  
 *  A response received from client user (through asynchronous dialog)
 *  This method is introduced as a TUserCallback for CGetIKEPassword dialog
 *  object is created. When the dialog is completed this callback function
 *  is called to deliver Credentials data for CHRE payload attributes.
 *  Store credential buffers to CAuthDialogInfo object and call engine
 *  entry  
 *  
 *-------------------------------------------------------------------------*/
    TUint32 obj_id = 1;
    CAuthDialogInfo* info = (CAuthDialogInfo*)aUserInfo;
	DEBUG_LOG1(_L("CIKECRACKNegotiation::DialogCompleteL(), aUserInfo =  %x"), aUserInfo);
			
    if ( info )
	{
       obj_id = info->GetObjId();
	   DEBUG_LOG1(_L("Preparing to call AuthDialogCompletedL(), ObjId = %x"), obj_id);
       if ( obj_id == DIALOG_INFO_ID )
	   {
          info->iUsername = aUsername;
          info->iSecret   = aSecret;
          info->iDomain   = aDomain;
          obj_id = info->PluginSession()->AuthDialogCompletedL(info);
       }   
    }

    return obj_id;
}


