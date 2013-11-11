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









#include "TestConnection.h"
#include <SenElement.h>
#include <SenXmlReader.h>

CTestConnection::CTestConnection()//:CSenServiceConnection(iProp)
{
	
}
  
void CTestConnection::ConstructL()//MSenServiceConsumer& aConsumer)
{
//	iConn = CSenServiceConnection::NewL();//aConsumer,_L8(""));
}

CTestConnection::~CTestConnection()
{
//	delete iConn;
}
/*
CTestConnection* CTestConnection::NewL(MSenServiceConsumer& aConsumer)
{
	CTestConnection* pTest = new(ELeave) CTestConnection();
	CleanupStack::PushL(pTest);
	pTest->ConstructL(aConsumer);
	CleanupStack::Pop(pTest);
	return pTest;
}
*/
CTestConnection* CTestConnection::NewL()//MSenServiceConsumer& aConsumer)
{
	CTestConnection* pTest = new(ELeave) CTestConnection();
	CleanupStack::PushL(pTest);
	//pTest->ConstructL(aConsumer);
	CleanupStack::Pop(pTest);
	return pTest;
}

TInt CTestConnection::SendL(const TDesC8& aRequest)
{
	TInt retval(0);
//	retval = iConn->SendL(aRequest);
	return retval;
}
TInt CTestConnection::SubmitL(const TDesC8& aRequest, HBufC8*& aResponse) 
{
	TInt retval(0);
//	retval = iConn->SubmitL(aRequest, aResponse) ;
	return retval;
}
TInt CTestConnection::IsReady(TBool& aReady) 
{
	TInt retval(0);
//	retval = iConn->IsReady(aReady);
	return retval;
}
TInt CTestConnection::HasFacetL(const TDesC8& aURI, TBool& aHasFacet) 
{
	TInt retval(0);
//	retval = iConn->HasFacetL(aURI, aHasFacet) ;
	return retval;
}
TInt CTestConnection::CompleteServerMessagesOnOff(const TBool& aCompleteOnOff) 
{
	TInt retval(0);
//	retval = iConn->CompleteServerMessagesOnOff(aCompleteOnOff);
	return retval;
}
TInt CTestConnection::SendL(CSenSoapEnvelope& aMessage) 
{
	TInt retval(0);
//	retval = iConn->SendL(aMessage);
	return retval;
}
TInt CTestConnection::SubmitL(CSenSoapEnvelope& aMessage, HBufC8*& aResponseTo) 
{
	TInt retval(0);
//	retval = iConn->SubmitL(aMessage, aResponseTo);
	return retval;
}
TInt CTestConnection::ServiceDescriptionL(HBufC8*& aServiceDescription) 
{
	TInt retval(0);
//	retval = iConn->ServiceDescriptionL(aServiceDescription) ;
	return retval;
}
TInt CTestConnection::StartTransaction() 
{
	TInt retval(0);
//	retval = iConn->StartTransaction();
	return retval;
}
TInt CTestConnection::TransactionCompleted() 
{
	TInt retval(0);
//	retval = iConn->TransactionCompleted() ;
	return retval;
}
TInt CTestConnection::TxnId() 
{
	TInt retval(0);
//	retval = iConn->TxnId() ;
	return retval;
}
TInt CTestConnection::SetTransportPropertiesL(const TDesC8& aProperties) 
{
	TInt retval(0);
//	retval = iConn->SetTransportPropertiesL(aProperties);
	return retval;
}
TInt CTestConnection::TransportPropertiesL(HBufC8*& aProperties) 
{
	TInt retval(0);
//	retval = iConn->TransportPropertiesL(aProperties) ;
	return retval;
}
TInt CTestConnection::SendL(const TDesC8& aRequest, const TDesC8& aProperties) 
{
	TInt retval(0);
//	retval = iConn->SendL(aRequest, aProperties) ;
	return retval;
}
TInt CTestConnection::SubmitL(const TDesC8& aRequest, const TDesC8& aProperties, HBufC8*& aResponse) 
{
	TInt retval(0);
//	retval = iConn->SubmitL(aRequest, aProperties, aResponse) ;
	return retval;
}
TInt CTestConnection::SendL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties) 
{
	TInt retval(0);
//	retval = iConn->SendL(aMessage, aProperties) ;
	return retval;
}
TInt CTestConnection::SubmitL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties, HBufC8*& aResponseTo) 
{
	TInt retval(0);
//	retval = iConn->SubmitL(aMessage, aProperties, aResponseTo) ;
	return retval;
}
TInt CTestConnection::SendL(MSenMessage& aMessage) 
{
	TInt retval(0);
//	retval = iConn->SendL(aMessage) ;
	return retval;
}
TInt CTestConnection::SubmitL(MSenMessage& aMessage, CSenSoapEnvelope2*& aResponseTo) 
{
	TInt retval(0);
//	retval = iConn->SubmitL(aMessage, aResponseTo) ;
	return retval;
}
MSenMessage* CTestConnection::Response() 
{
	TInt retval(0);
//	TRAP(retval, iConn->Response());
	return NULL;
}
TInt CTestConnection::CancelTransaction(TInt aTransactionID) 
{
	TInt retval(0);
//	retval = iConn->CancelTransaction(aTransactionID) ;
	return retval;
}
TInt CTestConnection::IdentityProviderL(CSenIdentityProvider*& apIdentityProvider) 
{
	TInt retval(0);
//	retval = iConn->IdentityProviderL(apIdentityProvider);
	return retval;
}
TInt CTestConnection::Identifier() 
{
	TInt retval(0);
//	retval = iConn->Identifier() ;
	return retval;
}

void CTestConnection::RegisterFilesObserverL(MSenFilesObserver& aFilesObserver) 
{}


//MSenFragment

CSenElement& CTestConnection::AsElement() 
{
	
}
CSenElement* CTestConnection::ExtractElement() 
{
	
}
void CTestConnection::SetReader(CSenXmlReader& aReader) 
{
	
}
CSenXmlReader* CTestConnection::Reader() 
{
	
}
void CTestConnection::ParseL(const TDesC8& aBuf) 
{
	
}
TInt CTestConnection::BuildFrom(const TDesC8& aBuf) 
{
	
}
void CTestConnection::DelegateParsingL(MSenFragment& aDelegate) 
{
	
}
void CTestConnection::DelegateParsingL(  const TDesC8& aNsUri,
                        const TDesC8& aLocalName,
                        const TDesC8& aQName,
                        const RAttributeArray& aAttrs) 
{
	
}

void CTestConnection::ParseWithL(CSenXmlReader& aReader) 
{
	
}
void CTestConnection::SetOwner(MSenFragment& aFragment) 
{
	
}
void CTestConnection::ResumeParsingFromL(const TDesC8& aNsUri,
                        const TDesC8& aLocalName,
                        const TDesC8& aQName) 
{
	
}

void CTestConnection::SetAttributesL(const RAttributeArray& aAttrs) 
{
	
}
void CTestConnection::WriteStartElementL(const TDesC8& aNsUri,
                        const TDesC8& aLocalName,
                        const TDesC8& aQName,
                        const RAttributeArray& aAttrs) 
{
	
}

void CTestConnection::WriteEndElementL(const TDesC8& aNsUri,
                      const TDesC8& aLocalName,
                      const TDesC8& aQName) 
{
	
}

const TDesC8& CTestConnection::LocalName()  const
{
	
}

const TDesC8& CTestConnection::NsUri()  const
{
	
}

const TDesC8& CTestConnection::NsPrefix()  const
{
	
}

void CTestConnection::WriteAsXMLToL(RWriteStream& aWriteStream) 
{
	
}

HBufC* CTestConnection::AsXmlUnicodeL() 
{
	
}
HBufC8* CTestConnection::AsXmlL() 
{
	
}

TBool CTestConnection::ConsistsOfL(MSenFragment& aCandidate) 
{
	
}

//CACTIVE


void CTestConnection::DoCancel() 
{
	
}

void CTestConnection::RunL() 
{
	
}

//Msenserviceconsumer
