/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include <http.h>

#include "senmtomtxnstate.h"
#include "senhttpchannel.h"
#include "sendebug.h"
#include "senlogger.h"


// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSenMtomTxnState::CSenMtomTxnState
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSenMtomTxnState::CSenMtomTxnState(MSenResponseObserver& aObserver)
    : CSenMultiPartTxnState(aObserver)
   	{
    }
    
CSenMtomTxnState* CSenMtomTxnState::NewL(MSenResponseObserver& aObserver,
                          		     	 const TDesC8* aUri,
                                   		 const TDesC8& aSoapAction,
                                   		 CSenSoapEnvelope2& aSoapEnvelope2)
                                   		 
    {
    CSenMtomTxnState* pNew = NewLC( aObserver, aUri, aSoapAction,
                                	aSoapEnvelope2);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenMtomTxnState* CSenMtomTxnState::NewLC(MSenResponseObserver& aObserver,
                                  		  const TDesC8* aUri,
                                  		  const TDesC8& aSoapAction,
                                  		  CSenSoapEnvelope2& aSoapEnvelope2)
   
    {
    CSenMtomTxnState* pNew = new (ELeave) CSenMtomTxnState(aObserver);;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, aSoapAction, aSoapEnvelope2);
    return pNew;
    }


void CSenMtomTxnState::ConstructL(const TDesC8* aUri,
                         	      const TDesC8& aSoapAction,
                         	      CSenSoapEnvelope2& aSoapEnvelope2)
    {
    
    __ASSERT_ALWAYS(aUri != NULL,
                    User::Panic(KRequestUriNullPanicText,
                                CSenHttpChannel::ERequestUriNull));
    iRequestUri = aUri->AllocL();
    iSoapAction.CreateL(aSoapAction.Length());
    iSoapAction.Copy(aSoapAction);
    
    if(!&aSoapEnvelope2)
		{
		iHasRequestBody = EFalse;
		User::Panic(KMultiPartNoRequestMessagePanicText, SenMultiPartUtils::EMultiPartNoRequestMessage);
		}
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMtomTxnState::ConstructL")));
      	
   	iMessage = &aSoapEnvelope2; // not owned
   	RSenDocument doc = aSoapEnvelope2.AsDocumentL();
 	doc.GetDataContainerList(iBinaryDataList);

 	iCountDataParts = 3 + iBinaryDataList.Count()*2; //how many parts there are in XOP 
	iHasRequestBody = ETrue;
    }
    

// ---------------------------------------------------------------------------
// creates HTTP content type header for XOP message
//
// HTTP content type header for SOAP v1.1 should be the next:
// Content-Type: Multipart/Related;boundary=MIME_boundary;type="application/xop+xml";
// start="<>"; start-info="text/xml"
//
// HTTP content type header for SOAP v1.2 should be the next:
// Content-Type: Multipart/Related;boundary=MIME_boundary;type="application/xop+xml";
// start="<>";start-info="application/soap+xml"
//
// Note: 1.it's one line without return carrige
//       2."boundary" is generated as <rundomNumber>
//       3."start" is generated as <randomNumber>@<homeTime>
//       4."actionVal12" = SoapAction from transport properties
// ---------------------------------------------------------------------------
//
void CSenMtomTxnState::SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders)
	{
	_LIT8(KMultipartRelated,"Multipart/Related");	//CodeScannerWarning
	_LIT8(KBoundry,"boundary");
	_LIT8(KType,"type");
	_LIT8(KStart,"start");
	_LIT8(KStartInfo,"start-info");
    _LIT8(KApplicationXop,"\"application/xop+xml\"");
    _LIT8(KApplicationSoap,"\"application/soap+xml\"");
    _LIT8(KTextXml,"\"text/xml\"");
    _LIT8(KSlashLessthan,"\"<");
	_LIT8(KSlashGreaterthan,">\"");
	SenMultiPartUtils::GenerateRandomRootCidL(iRootCid);
	RBuf8 rootCid(NULL);
	rootCid.ReAllocL(4+iRootCid.Length());
	CleanupClosePushL(rootCid);
    rootCid.Append(KSlashLessthan);
	rootCid.Append(iRootCid);
	rootCid.Append(KSlashGreaterthan);
	 
	SenMultiPartUtils::GenerateRandomBoundaryL(iMimeBoundaryOut);

	RStringF multipartRelated = aSession.StringPool().OpenFStringL(KMultipartRelated);
	RStringF boundary         = aSession.StringPool().OpenFStringL(KBoundry);
	RStringF boundaryVal      = aSession.StringPool().OpenFStringL(iMimeBoundaryOut);
	RStringF type             = aSession.StringPool().OpenFStringL(KType);
	RStringF typeVal          = aSession.StringPool().OpenFStringL(KApplicationXop);
	RStringF start            = aSession.StringPool().OpenFStringL(KStart);
	RStringF startVal         = aSession.StringPool().OpenFStringL(rootCid);
	RStringF startInfo        = aSession.StringPool().OpenFStringL(KStartInfo);
	RStringF startInfoVal11   = aSession.StringPool().OpenFStringL(KTextXml);
	RStringF startInfoVal12   = aSession.StringPool().OpenFStringL(KApplicationSoap);
	
	CleanupStack::PopAndDestroy(&rootCid);
	
	THTTPHdrVal contentTypeVal(multipartRelated);
	THTTPHdrVal boundVal(boundaryVal);
	THTTPHdrVal tVal(typeVal);
	THTTPHdrVal stVal(startVal);
	
	THTTPHdrVal stInfoVal;

	switch (((CSenSoapEnvelope2*)iMessage)->SoapVersion())
		{
 		case ESOAP11:
			{
			stInfoVal = startInfoVal11;
			break;
			}
		case ESOAP12:
			{
			stInfoVal = startInfoVal12;	
			break;
			}
		default:
			{
        	User::Panic(KMultiPartSoapVersionInvalidPanicText, SenMultiPartUtils::EMultiPartSoapVersionInvalid);			}			
		    }
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, boundary, boundVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
				       contentTypeVal, type, tVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, start, stVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, startInfo, stInfoVal);

					   
    if (((CSenSoapEnvelope2*)iMessage)->SoapVersion() == ESOAP12)
        {
      	RStringF action           = aSession.StringPool().OpenFStringL(_L8("action"));
    	RStringF actionVal12      = aSession.StringPool().OpenFStringL(iSoapAction);
    	
        aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
		    			   contentTypeVal, action, actionVal12);
    	action.Close();
	    actionVal12.Close();
        }

	multipartRelated.Close();
	boundary.Close();         
	boundaryVal.Close();      
	type.Close();             
	typeVal.Close();          
	start.Close();            
	startVal.Close();         
	startInfo.Close();        
	startInfoVal11.Close();   
	startInfoVal12.Close();
	}
// ---------------------------------------------------------------------
// creates SoapEnvelope in Request
// ---------------------------------------------------------------------
//    
void CSenMtomTxnState::MessageL()
    {
    // iCurrentPart == XOP envelope:
    SenMultiPartUtils::SerializeMessageL(*(CSenSoapEnvelope2*)iMessage, iCurrentPart);
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenMtomTxnState::GetNextDataPart - aDataPart (XOP Envelope):")));
    iCurPart = ETrue; 
    }

//  End of File
