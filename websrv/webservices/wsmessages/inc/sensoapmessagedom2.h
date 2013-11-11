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
* Description:        CSenSoapMessageDom is an utility class offering capability to
*                parse XML SOAP envelope and manipulation methods to alter its
*                contents.
*
*/








#ifndef SEN_SOAP_MESSAGE_DOM2_H
#define SEN_SOAP_MESSAGE_DOM2_H

//  INCLUDES
#include <SenWsSecurityHeader2.h>
#include <SenSoapMessage2.h>

// CLASS DECLARATION

/**
* CSenSoapMessage extends basic SOAP envelope functionality by
* offering methods to set the security header and security token.
* @lib SenUtils.lib
* @since Series60 4.0
*/
class CSenSoapMessageDom2 : public CSenSoapMessage2
    {
    public:  // Constructors and destructor

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewL();

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewLC();

        /**
        * Two-phase constructor.
        */
	    IMPORT_C static CSenSoapMessageDom2* NewL(TSOAPVersion aVersion);

        /**
        * Two-phase constructor.
        */
	    IMPORT_C static CSenSoapMessageDom2* NewLC(TSOAPVersion aVersion);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewL(TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewLC(TSOAPVersion aVersion,
                                                   const TDesC8& aSecurityNs);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewL(MSenMessageContext& aContext);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewLC(MSenMessageContext& aContext);

        /**
        * Two-phase constructor.
        */
	    IMPORT_C static CSenSoapMessageDom2* NewL(MSenMessageContext& aContext,
	                                              TSOAPVersion aVersion);

        /**
        * Two-phase constructor.
        */
	    IMPORT_C static CSenSoapMessageDom2* NewLC(MSenMessageContext& aContext,
	                                               TSOAPVersion aVersion);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewL(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs);

        /**
        * Two-phase constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewLC(MSenMessageContext& aContext,
                                                   TSOAPVersion aVersion,
                                                   const TDesC8& aSecurityNs);
        /**
        * Two-phased copy constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewL(CSenSoapMessageDom2& aMessage);

        /**
        * Two-phased copy constructor.
        */
        IMPORT_C static CSenSoapMessageDom2* NewLC(CSenSoapMessageDom2& aMessage);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenSoapMessageDom2();

    public: // From MSenMessage
        IMPORT_C virtual TClass Type();
        IMPORT_C virtual MSenMessage* CloneL();       

    protected:

        /**
         *  C++ default constructor.
         */
        IMPORT_C CSenSoapMessageDom2::CSenSoapMessageDom2();

        IMPORT_C CSenFragmentBase* CreateBodyFragmentL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aPrefix,
                                                       const RAttributeArray& aAttributes,
                                                       TXmlEngElement& aParent,
                                                       RSenDocument& aOwnerDocument);
    };

#endif // SEN_SOAP_MESSAGE_DOM2_H

// End of File


