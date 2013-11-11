/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifdef __ENABLE_ALR__
#include <comms-infras/cs_mobility_apiext.h>
#endif //__ENABLE_ALR__


#include "senconnagentsession.h"
#include "senconnagentserver.h"
#include "senservicemanagerdefines.h"
#include "MSenServiceConsumer.h"

#include "MSenAuthenticationProvider.h"
#include "SenIdentityProvider.h"
#include "msenservicemanager.h"

#include "msencobrandinginterface.h"
#include "senchunk.h"
#include "senxmldebug.h"

CSenConnAgentSession::~CSenConnAgentSession()
    {
    
    }

void CSenConnAgentSession::ServiceL(const RMessage2& aMessage)
    {
    switch (aMessage.Function())
        {
        case ESenCliServInitialize:
            {
            iObserver = static_cast<MSenFilesObserver*>(
					const_cast<TAny*>(aMessage.Ptr0()));
            aMessage.Complete(KErrNone);
            ((CActiveSchedulerWait*)aMessage.Ptr1())->AsyncStop();
            }
            break;
        case ESenCliServTransferProgress:
            {
            TTransferProgress transfer;
            TPckg<TTransferProgress> transferPckg(transfer);
            aMessage.ReadL(0, transferPckg);
            HBufC8* buf = HBufC8::NewLC(aMessage.GetDesLengthL(1));
            TPtr8 bufPtr = buf->Des();
            aMessage.ReadL(1, bufPtr);
            aMessage.Complete(KErrNone);
            if (transfer.iSoap)
                {
                iObserver->TransferProgress(transfer.iTxnId,
                        transfer.iIncoming, *buf, KNullDesC8, 0);
                }
            else
                {
                iObserver->TransferProgress(transfer.iTxnId,
                        transfer.iIncoming, KNullDesC8, *buf, transfer.iProgress);
                }
            CleanupStack::PopAndDestroy(buf);
          }
            break;
        case ESenCliServAuthInitialize:
            {
	        iAuthObserver = static_cast<MSenAuthenticationProvider*>(
					const_cast<TAny*>(aMessage.Ptr0()));
	        ((CActiveSchedulerWait*)aMessage.Ptr1())->AsyncStop();
	        aMessage.Complete(KErrNone);
            }
	        break;
        case ESenCliServCoBrandingInitialize:
            {
	        ipCoBrandingObserver = static_cast<MSenCoBrandingObserver*>(
					const_cast<TAny*>(aMessage.Ptr0()));
	        ((CActiveSchedulerWait*)aMessage.Ptr1())->AsyncStop();
	        aMessage.Complete(KErrNone);
            }
            break;
	    case ESenCliServReAuthNeeded:
	        {
	    	TInt retVal(KErrNone);
	    	TRAPD(error, retVal = ReauthenticationNeededL(aMessage));
	    	if(error)
	    		{
	    		retVal = error;
	    		}
	    	aMessage.Complete(retVal);
	    	CActiveSchedulerWait* wait = static_cast<CActiveSchedulerWait*>(
					const_cast<TAny*>(aMessage.Ptr1()));
	    	if(wait && wait->IsStarted())
	    		{
	    		wait->AsyncStop();
	    		}
	    		}
        	break;
        case ESenCliServGoNoGo:
            {
	    	TInt retValue(KErrNone);
	    	TRAPD(CBerror, retValue = CoBrandingGoNoGoL(aMessage));
	    	if(CBerror)
	    		{
	    		retValue = CBerror;
	    		}
	    	aMessage.Complete(retValue);
            }
        	break;

#ifdef __ENABLE_ALR__
        case ESenCliServMobilityInitialize:
            {
            iMobiltyObserver = static_cast<MMobilityProtocolResp*>(
					const_cast<TAny*>(aMessage.Ptr0()));
            aMessage.Complete(KErrNone);
            ((CActiveSchedulerWait*)aMessage.Ptr1())->AsyncStop();                
            }
            break;
        case ESenCliServPrefferedCarrierAvailable:
            {
            TInt oldap = aMessage.Int0();
            TInt newap= aMessage.Int1();
            
            TAccessPointInfo oldAPInfo ;
            oldAPInfo.SetAccessPoint( oldap );
            TAccessPointInfo newAPInfo ;
            newAPInfo.SetAccessPoint( newap );

            HBufC8* upgradeBuf = HBufC8::NewLC(10);
            TPtr8 upgradeBuffer = upgradeBuf->Des();
            aMessage.ReadL(2, upgradeBuffer); 
            TBool isUpgrade = (TBool)(*upgradeBuffer.Ptr()) ;
            CleanupStack::PopAndDestroy(upgradeBuf); 
            
            HBufC8* seemlessBuf = HBufC8::NewLC(10);
            TPtr8 seemlessBuffer = seemlessBuf->Des();
            aMessage.ReadL(3, seemlessBuffer); 
            TBool isSeamless = (TBool)(*seemlessBuffer.Ptr()) ;
            CleanupStack::PopAndDestroy(seemlessBuf); 
            
            aMessage.Complete(KErrNone);
            // check for mobility observer and forward the call to the observer (application)
            if(iMobiltyObserver)
                {
                iMobiltyObserver->PreferredCarrierAvailable(oldAPInfo, newAPInfo, isUpgrade, isSeamless) ;
                }
            }
            break;
        case ESenCliServNewCarrierActive:
            {
            TInt newap = aMessage.Int0();
            TAccessPointInfo newAPInfo ;
            newAPInfo.SetAccessPoint( newap );
            HBufC8* inBuf = HBufC8::NewLC(10);
            TPtr8 inBuffer = inBuf->Des();
            aMessage.ReadL(1, inBuffer); 
            TBool isSeamless = (TBool)(*inBuffer.Ptr()) ;
            CleanupStack::PopAndDestroy(inBuf); 
            aMessage.Complete(KErrNone);
            // check for mobility observer and forward the call to the observer (application)
            if(iMobiltyObserver)
                {
                iMobiltyObserver->NewCarrierActive(newAPInfo, isSeamless) ;
                }
            }
            break;
        case ESenCliServMobilityError:
            {
        	TInt mobilityError = aMessage.Int0();
            aMessage.Complete(KErrNone);
            // check for mobility observer and forward the call to the observer (application)
            if(iMobiltyObserver)
                {
                iMobiltyObserver->Error(mobilityError);
                }
            }
            break;
#endif //__ENABLE_ALR__
        default:
            {
            aMessage.Complete(KErrNotSupported);
            }
            break;
        }
    }

void CSenConnAgentSession::Disconnect(const RMessage2& aMessage)
    {
    CSession2::Disconnect(aMessage);
    CSenConnAgentServer* server = reinterpret_cast<CSenConnAgentServer*>
            (Dll::Tls());
    ASSERT(server);
    server->DestroyIfInactive();
    }
TInt CSenConnAgentSession::CoBrandingGoNoGoL(const RMessage2& aMessage)
    {
    TBool* goNoGo = static_cast<TBool*>(
					const_cast<TAny*>(aMessage.Ptr0()));    

	HBufC8* buf = HBufC8::NewLC(aMessage.GetDesLengthL(1));
	TPtr8 bufPtr = buf->Des();
	aMessage.ReadL(1, bufPtr);
    
    RBuf8* selectedbrandID = static_cast<RBuf8*>(
					const_cast<TAny*>(aMessage.Ptr2()));

    if (ipCoBrandingObserver)
        {
        *goNoGo = ipCoBrandingObserver->OnGetBrandIdL(*buf,*selectedbrandID);  
				CleanupStack::PopAndDestroy(buf);        
        return KErrNone;
        }
        
		CleanupStack::PopAndDestroy(buf);        
    return KErrGeneral;
    }
TInt CSenConnAgentSession::ReauthenticationNeededL(const RMessage2& aMessage)
	{
	TInt retVal = KErrNone;
	TBool providerCreated = EFalse;
	
	if(iAuthObserver)
		{
		CSenChunk* pSenChunk = CSenChunk::NewLC(KNullDesC());
		retVal = pSenChunk->OpenChunkFromRMsgL(aMessage,0);
		
		if(retVal == KErrNone)
			{
	    	CSenIdentityProvider* pIdP = 
	    		(CSenIdentityProvider*)iAuthObserver->IdentityProviderL();
	    	
	    	if(!pIdP)
	    		{
	    		TPtrC8 identityProvider;
	    		retVal = pSenChunk->DescFromChunk(identityProvider);
	    		
	    		if(retVal == KErrNone)
	    			{
			    	pIdP = CSenIdentityProvider::NewLC(KNullDesC8());
			    	providerCreated = ETrue;
			    	CSenXmlReader* pXmlReader = CSenXmlReader::NewLC(KXmlParserMimeType);
			    	pXmlReader->SetContentHandler( *pIdP );
			    	pIdP->SetReader(*pXmlReader);
			    	
			    	pIdP->ParseL( identityProvider );
			    	CleanupStack::PopAndDestroy(pXmlReader);
			    		
			    	TPtrC8 userName = iAuthObserver->UsernameL();
			        TPtrC8 password = iAuthObserver->PasswordL();
			        if(pIdP && userName != KNullDesC8 && password != KNullDesC8 )
			        	{
				        retVal = pIdP->SetUserInfoL( userName ,
											   		 userName ,
											   		 password );
			        	}
	    			}
	    		}
	    	
	    	if(pIdP)
	    		{
	    		// Identity provider data retrieved from callback, return it
	    		HBufC8* desIdP = pIdP->AsXmlL();
	    		CleanupStack::PushL(desIdP);
	    		pSenChunk->DescToChunk(*desIdP);
	    		CleanupStack::PopAndDestroy(desIdP);
	    		}
	    	else
	    		{
	    		// Identity provider was not passed
	    		retVal = KErrNotFound;
	    		}
	    	
	    	if(providerCreated)
	    		{
	    		CleanupStack::PopAndDestroy(pIdP);
	    		}
			}
		CleanupStack::PopAndDestroy(pSenChunk);
		}
	else
		{
		retVal = KErrNotReady;
		}
	return retVal;
	}
