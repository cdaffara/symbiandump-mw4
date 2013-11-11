/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "senatomtxnstate.h"
#include "senhttpchannel.h"
#include "sendebug.h"
#include "senlogger.h"



// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSenAtomTxnState::CSenAtomTxnState
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSenAtomTxnState::CSenAtomTxnState(MSenResponseObserver& aObserver)
    : CSenMultiPartTxnState(aObserver)
    {
    }
    
CSenAtomTxnState* CSenAtomTxnState::NewL(MSenResponseObserver& aObserver,
                          		         const TDesC8* aUri,
                                   		 CSenAtomEntry& aAtomEntry)
                                   		 
    {
    CSenAtomTxnState* pNew = NewLC( aObserver, aUri, 
                                	aAtomEntry);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenAtomTxnState* CSenAtomTxnState::NewLC(MSenResponseObserver& aObserver,
                                  		  const TDesC8* aUri,
                                  		  CSenAtomEntry& aAtomEntry)
   
    {
    CSenAtomTxnState* pNew = new (ELeave) CSenAtomTxnState(aObserver);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, aAtomEntry);
    return pNew;
    }


void CSenAtomTxnState::ConstructL(const TDesC8* aUri,
                         	      CSenAtomEntry& aAtomEntry)
    {
    
    __ASSERT_ALWAYS(aUri != NULL,
                    User::Panic(KRequestUriNullPanicText,
                                CSenHttpChannel::ERequestUriNull));
    iRequestUri = aUri->AllocL();
    
    if(!&aAtomEntry)
		{
		iHasRequestBody = EFalse;
		User::Panic(KMultiPartNoRequestMessagePanicText, SenMultiPartUtils::EMultiPartNoRequestMessage);
		}
   	TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenAtomTxnState::ConstructL")));  	
   	iMessage = &(MSenMessage&)aAtomEntry; // not owned
   	RSenDocument doc = aAtomEntry.AsDocumentL();
 	doc.GetDataContainerList(iBinaryDataList);

 	iCountDataParts = 3 + iBinaryDataList.Count()*2; //how many parts there are in MultiPart message 
	iHasRequestBody = ETrue;
    }
    

// ---------------------------------------------------------------------------
// creates HTTP content type header for Atom multipart message
//
// HTTP content type header should be the next:
// Content-Type: Multipart/Related;boundary=MIME_boundary;type="application/atom+xml";
// start="<>"; start-info="text/xml"
//
//
// Note: 1.it's one line without return carrige
//       2."boundary" is generated as <rundomNumber>
//       3."start" is generated as <randomNumber>@<homeTime>
// ---------------------------------------------------------------------------
//
void CSenAtomTxnState::SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders)
	{
	_LIT8(KMultipartRelated,"Multipart/Related");
	_LIT8(KBoundry,"boundary");
	_LIT8(KType,"type");
	_LIT8(KStart,"start");
	_LIT8(KStartInfo,"start-info");
    _LIT8(KApplicationAtom,"\"application/atom+xml\"");
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
	RStringF typeVal          = aSession.StringPool().OpenFStringL(KApplicationAtom);
	RStringF start            = aSession.StringPool().OpenFStringL(KStart);
	RStringF startVal         = aSession.StringPool().OpenFStringL(rootCid);
	RStringF startInfo        = aSession.StringPool().OpenFStringL(KStartInfo);
	RStringF startInfoVal   = aSession.StringPool().OpenFStringL(KTextXml);
	
	CleanupStack::PopAndDestroy(&rootCid);
	
	THTTPHdrVal contentTypeVal(multipartRelated);
	THTTPHdrVal boundVal(boundaryVal);
	THTTPHdrVal tVal(typeVal);
	THTTPHdrVal stVal(startVal);
	
	THTTPHdrVal stInfoVal;

	stInfoVal = startInfoVal;
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, boundary, boundVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
				       contentTypeVal, type, tVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, start, stVal);
	aHeaders.SetFieldL(aSession.StringPool().StringF(HTTP::EContentType,RHTTPSession::GetTable()), 
					   contentTypeVal, startInfo, stInfoVal);

					   
    multipartRelated.Close();
	boundary.Close();         
	boundaryVal.Close();      
	type.Close();             
	typeVal.Close();          
	start.Close();            
	startVal.Close();         
	startInfo.Close();        
	startInfoVal.Close();   
	}

// ---------------------------------------------------------------------
// creates AtomEntry in Request
// ---------------------------------------------------------------------
//    
void CSenAtomTxnState::MessageL()
    {
    SenMultiPartUtils::SerializeMessageL(*(CSenAtomEntry*)iMessage, iCurrentPart, ESerializerDefault);
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenAtomTxnState::GetNextDataPart - aDataPart (Atom Message):")));
    iCurPart = ETrue; 
    }


//  End of File
