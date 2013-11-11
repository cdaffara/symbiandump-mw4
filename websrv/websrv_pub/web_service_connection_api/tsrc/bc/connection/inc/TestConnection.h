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
* Description:  Test connection header declaration      
*
*/








#ifndef TESTCONNECTION_H
#define TESTCONNECTION_H

#include <SenServiceConnection.h>



class CTestConnection : public CBase //:public CSenServiceConnection
{
    public: // Constructors and destructor

        static CTestConnection* NewL();//MSenServiceConsumer& aConsumer);
        void ConstructL();//MSenServiceConsumer& aConsumer);
		 ~CTestConnection();
		 CTestConnection();
		 TInt SendL(const TDesC8& aRequest);
		 TInt SubmitL(const TDesC8& aRequest, HBufC8*& aResponse) ;
         TInt IsReady(TBool& aReady) ;
         TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet) ;
         TInt CompleteServerMessagesOnOff(const TBool& aCompleteOnOff) ;
         TInt SendL(CSenSoapEnvelope& aMessage) ;
         TInt SubmitL(CSenSoapEnvelope& aMessage, HBufC8*& aResponseTo) ;
         TInt ServiceDescriptionL(HBufC8*& aServiceDescription) ;
         TInt StartTransaction() ;
         TInt TransactionCompleted() ;
         TInt TxnId() ;
         TInt SetTransportPropertiesL(const TDesC8& aProperties) ;
         TInt TransportPropertiesL(HBufC8*& aProperties) ;
         TInt SendL(const TDesC8& aRequest, const TDesC8& aProperties) ;
         TInt SubmitL(const TDesC8& aRequest, const TDesC8& aProperties, HBufC8*& aResponse) ;
         TInt SendL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties) ;
         TInt SubmitL(CSenSoapEnvelope& aMessage, const TDesC8& aProperties, HBufC8*& aResponseTo) ;
         TInt SendL(MSenMessage& aMessage) ;
      	 TInt SubmitL(MSenMessage& aMessage, CSenSoapEnvelope2*& aResponseTo) ;
      	 MSenMessage* Response() ;
    	 TInt CancelTransaction(TInt aTransactionID) ;
    	 TInt IdentityProviderL(CSenIdentityProvider*& apIdentityProvider) ;
    	 TInt Identifier() ;
    	 void RegisterFilesObserverL(MSenFilesObserver& aFilesObserver) ;
    		
    		
    		
    		
    		
    		
    	//MSENFRAGMENT
//         CSenElement& AsElement() ;
         CSenElement* ExtractElement() ;
         void SetReader(CSenXmlReader& aReader) ;
         CSenXmlReader* Reader() ;
         void ParseL(const TDesC8& aBuf) ;
         TInt BuildFrom(const TDesC8& aBuf) ;
         void DelegateParsingL(MSenFragment& aDelegate) ;
         void DelegateParsingL(  const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs) ;

         void ParseWithL(CSenXmlReader& aReader) ;
         void SetOwner(MSenFragment& aFragment) ;
         void ResumeParsingFromL(const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName) ;

         void SetAttributesL(const RAttributeArray& aAttrs) ;

         void WriteStartElementL(const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs) ;

         void WriteEndElementL(const TDesC8& aNsUri,
                                      const TDesC8& aLocalName,
                                      const TDesC8& aQName) ;

         const TDesC8& LocalName() const ;
        
         const TDesC8& NsUri() const ;

         const TDesC8& NsPrefix() const ;

         void WriteAsXMLToL(RWriteStream& aWriteStream) ;

         HBufC* AsXmlUnicodeL() ;
         HBufC8* AsXmlL() ;

         TBool ConsistsOfL(MSenFragment& aCandidate) ;
    	
    	//CActive
    	 void DoCancel() ;
    	
    	 void RunL() ;
    		
    
    		
    		

        
//        CTestProperty* iProp;
	//	CSenServiceConnection* iConn;
    private:
	//	CSenServiceConnection* iConn;
};



#endif