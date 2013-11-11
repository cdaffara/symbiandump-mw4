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
#include <s32strm.h>

#include <SenXmlElement.h>
#include <SenXmlUtils.h>
#include <SenDomFragment.h>

#include "SenSoapEnvelope.h"
#include "SenSoapFault.h"
#include "senlogger.h"

#ifdef _SENDEBUG
namespace
    {
     _LIT8(KVersionMismatchFormat8,  "Fault.SOAP1.2 : %S");
    }

#endif // _SENDEBUG

EXPORT_C CSenSoapEnvelope* CSenSoapEnvelope::NewL()
    {
    CSenSoapEnvelope* pNew = new (ELeave) CSenSoapEnvelope();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C void CSenSoapEnvelope::BaseConstructL(TSOAPVersion aVersion)
    {
    if (aVersion != ESOAP12)
        {
        CSenBaseFragment::BaseConstructL(KSenSoapEnvelopeXmlns, KSenSoapEnvelopeName, KSenSoapEnvelopeQName);
        }
    else
        {
        CSenBaseFragment::BaseConstructL(KSenSoap12EnvelopeXmlns, KSenSoapEnvelopeName, KSenSoapEnvelopeQName);
        }
    }

EXPORT_C void CSenSoapEnvelope::BaseConstructL()
    {
    BaseConstructL(ESOAP11);
    }

CSenSoapEnvelope::CSenSoapEnvelope()
:   ipBodyFragment(NULL),
    ipHeaderFragment(NULL),
    ipSoapAction(NULL),
    iFault(EFalse)
    {
    }

EXPORT_C CSenSoapEnvelope::~CSenSoapEnvelope()
    {
    if(ipBodyFragment)
        {
        ipBodyFragment->ExtractElement();
        delete ipBodyFragment;
        }
    if(ipHeaderFragment)
        {
        ipHeaderFragment->ExtractElement(); // // delegate's element is owned by us
        delete ipHeaderFragment;
        }
    delete ipSoapAction;
    }

EXPORT_C void CSenSoapEnvelope::StartElementL(const TDesC8& aNsUri,
                                              const TDesC8& aLocalName, 
                                              const TDesC8& aQName,
                                              const RAttributeArray& aAttributes)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            // check if we encounter the start of a soap:Envelope...
            if(!ipElement && aNsUri == NsUri() &&
                aLocalName == KSenSoapEnvelopeName)
                {
                ipElement = CSenXmlElement::NewL(aNsUri, aLocalName, aQName);
                }
            if (ipElement && aLocalName == KSenSoapEnvelopeName && aNsUri!=NsUri())
                {
                TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, KVersionMismatchFormat8, &KVersionMismatch()));
                delete ipElement;
                ipElement = NULL;
                CSenBaseFragment::BaseConstructL(aNsUri, aLocalName, aQName);
                }
            if(ipElement && ipElement->LocalName() == aLocalName &&
                            ipElement->NamespaceURI() == aNsUri)
                {
                if ( ipElement->NsPrefix() != SenXmlUtils::NsPrefix(aQName) )
                    {
                    delete ipElement;
                    ipElement = NULL;
                    CSenBaseFragment::BaseConstructL(aNsUri, aLocalName, aQName);
                    }
                else
                    {
                    ipElement->Set(aNsUri, aLocalName, aQName);
                    }

                SetAttributesL(aAttributes);    // results in
                }

            else if(aNsUri == NsUri())
                {
                // we are interested in SOAP Header
                if(aLocalName == KSenSoapEnvelopeHeaderName)
                    {
                    iState = KStateParsingSoapHeader;

                    // Make sure that header element exists:
                    // == this ensures that we add a "Header"
                    // element to the children:
                    CSenElement& header = HeaderL();
                    header.Set(aNsUri, aLocalName, aQName);
                    header.AddAttributesL(aAttributes);
                    }
                else if(aLocalName == KSenSoapEnvelopeBodyName)
                    {
                    iState = KStateParsingSoapBody;

                    CSenElement& body = BodyL();
                    body.Set(aNsUri, aLocalName, aQName);
                    body.AddAttributesL(aAttributes);
                    }
                //ESLI-6WEN9V
                //This case is to handle fault which lies outside the body or without body
                else if(aLocalName == KSenSoapFaultName)
                    {
                    iFault = ETrue;
                    // delegate parsing to a SOAPFaultFragment to keep
                    // a structured representation.
                    CSenSoapFault* pSOAPFault =
                        CSenSoapFault::NewL(aNsUri, aLocalName, aQName, aAttributes);
                    CleanupStack::PushL(pSOAPFault);
                    // use DOM fragment copy constructor to get correct type
                    // of element inside of it.
                    ipBodyFragment = CSenDomFragment::NewL(*pSOAPFault);
                    CleanupStack::PopAndDestroy(); // pSOAPFault
                    ipBodyFragment->SetOwner(*this);
                    BodyL().AddElementL(ipBodyFragment->AsElement());
                    ipBodyFragment->ParseWithL(*Reader());
                    }
                //ESLI-6WEN9V
                }
            break;
            }
        case KStateParsingSoapHeader:
            {
            ParseHeaderL(aNsUri, aLocalName, aQName, aAttributes);
            }
            break;
        case KStateParsingSoapBody:
            {
            if(ipBodyFragment)
                {
                ipBodyFragment->ExtractElement();
                delete ipBodyFragment;
                ipBodyFragment = NULL;
                }

            // check if we encounter a Fault
            if (aLocalName == KSenSoapFaultName)
                {
                iFault = ETrue;
                // delegate parsing to a SOAPFaultFragment to keep
                // a structured representation.
                CSenSoapFault* pSOAPFault =
                    CSenSoapFault::NewL(aNsUri, aLocalName, aQName, aAttributes);
                CleanupStack::PushL(pSOAPFault);
                // use DOM fragment copy constructor to get correct type
                // of element inside of it.
                ipBodyFragment = CSenDomFragment::NewL(*pSOAPFault);
                CleanupStack::PopAndDestroy(); // pSOAPFault
                ipBodyFragment->SetOwner(*this);
                BodyL().AddElementL(ipBodyFragment->AsElement());
                ipBodyFragment->ParseWithL(*Reader());
                }
            else
                {
                ipBodyFragment =
                    CSenBaseFragment::NewL( aNsUri,
                                            aLocalName,
                                            aQName,
                                            aAttributes);

                ipBodyFragment->SetOwner(*this);
                BodyL().AddElementL(ipBodyFragment->AsElement());
                ipBodyFragment->ParseWithL(*Reader());
                }
            }
            break;
        default:
            break;
        }
    }

EXPORT_C void CSenSoapEnvelope::EndElementL(const TDesC8& aNsUri, 
                                            const TDesC8& aLocalName,
                                            const TDesC8& /*aQName*/)
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            break;
            }
        case KStateParsingSoapHeader:
            {
            if (aNsUri == NsUri() && aLocalName == KSenSoapEnvelopeHeaderName)
                {
                iState = KStateIgnore;
                }
            break;
            }
        case KStateParsingSoapBody:
            {
            if (aNsUri == NsUri() && aLocalName == KSenSoapEnvelopeBodyName)
                {
                iState = KStateIgnore;
                }
/*
              else
                {
                if(ipBodyFragment)
                    {
                    //ipBodyFragment->EnsureNamespacesL();
                    }
                } 
*/
            }
            break;
        default:
            break;
        }
    }

EXPORT_C TPtrC8 CSenSoapEnvelope::SetBodyL(const TDesC8& aBody)
    {
    return BodyL().SetContentL(aBody);
    }

EXPORT_C CSenElement& CSenSoapEnvelope::BodyL()
    {
    CSenElement& envelope = this->AsElement();
    CSenElement* pBody = envelope.Element(NsUri(), KSenSoapEnvelopeBodyName);
    if (pBody == NULL)
        {
        pBody = &envelope.AddElementL(NsUri(), KSenSoapEnvelopeBodyName);
        }
    return *pBody;
    }

EXPORT_C CSenElement& CSenSoapEnvelope::HeaderL()
    {
    CSenElement& envelope = this->AsElement();
    CSenElement* pHeader = envelope.Element(NsUri(), KSenSoapEnvelopeHeaderName);
    if (!pHeader)
        {
        // add it
        CSenElement& body = BodyL();
        pHeader = envelope.CreateElementL(envelope.NsPrefix(), KSenSoapEnvelopeHeaderName);
        if (pHeader)
            {
            envelope.InsertElementL(*pHeader,body);
            }
        }
    return *pHeader; 
    }

EXPORT_C CSenElement& CSenSoapEnvelope::AddHeaderL(CSenElement& aHeaderElement)
    {
    return (HeaderL().AddElementL(aHeaderElement));
    }

EXPORT_C void CSenSoapEnvelope::ParseHeaderL(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttributes)
    {
    if(ipHeaderFragment)
        {
        ipHeaderFragment->ExtractElement();
        delete ipHeaderFragment;
        ipHeaderFragment = NULL;
        }
    ipHeaderFragment = CSenDomFragment::NewL(aNsUri, aLocalName, aQName, aAttributes);
    ipHeaderFragment->SetOwner(*this); // to ensure we will return to this class' endElement method
    
    HeaderL().AddElementL(ipHeaderFragment->AsElement()); // transfers ownership
    ipHeaderFragment->ParseWithL(*iXmlReader);
    }


EXPORT_C HBufC8* CSenSoapEnvelope::BodyAsStringL()
    {
    CSenElement& body = BodyL();


    HBufC8* bodyAsXML = NULL;

    // Note, that difference to Jave ref-implementation here is,
    // that in *EVERY* case, no matter if SOAP BODY has no child 
    // element at all OR if it has one or more childer, the whole
    // <S:Body> element is ALWAYS returned -- [never just the only
    // child, and never a of zero-length string, even if no children]
    CSenElement* pDetachedElement = body.DetachL();
    if(pDetachedElement)
        {
        CleanupStack::PushL(pDetachedElement);
        bodyAsXML = pDetachedElement->AsXmlL();
        CleanupStack::PopAndDestroy(); // pDetachedElement
        return bodyAsXML;
        }
        
    // Function now returns NULL only if SOAP-ENV does not contain
    // a body at all(!)
    return bodyAsXML;
    }

EXPORT_C TBool CSenSoapEnvelope::IsFault()
    {
    return iFault;
    }

// NOTE: caller takes ownership of the NEW fault element
EXPORT_C CSenSoapFault* CSenSoapEnvelope::DetachFaultL()
    {
    CSenElement* pNotOwned = this->BodyL().Element(NsUri(), KSenSoapFaultName);
    if(pNotOwned)
        {
        CSenElement* pDetached = pNotOwned->DetachL();
        if(pDetached)
            {
            CleanupStack::PushL(pDetached);
            CSenSoapFault* pAnswer = CSenSoapFault::NewL(*pDetached);
            CleanupStack::PopAndDestroy(); // pDetached
            return pAnswer;
            }
        }
    return NULL;
    }


EXPORT_C CSenSoapFault* CSenSoapEnvelope::FaultL()
    {
    CSenSoapFault* pFault = NULL;
    if(iFault)
        {
        // trust that the element is actually
        // CSenSoapFault added in StartElementL()

        pFault = (CSenSoapFault*)this->BodyL().Element(NsUri(), KSenSoapFaultName);

        }
    return pFault;
    }

EXPORT_C TPtrC8 CSenSoapEnvelope::SetSoapActionL(const TDesC8& aSoapAction)
    {

    _LIT8(KQuote, "\"");

    HBufC8* pTemp = HBufC8::NewLC(aSoapAction.Length()+(2*KQuote().Length()));  

    TPtr8 temp = pTemp->Des();

    if(aSoapAction != KNullDesC8()
     && !SenXmlUtils::StartsWith(aSoapAction, KQuote))
        {
        temp.Append(KQuote);
        }
        
    temp.Append(aSoapAction);

    if(aSoapAction != KNullDesC8() && !SenXmlUtils::EndsWith(*pTemp, KQuote))
        {
        temp.Append(KQuote);
        }

    if(ipSoapAction)
        {
        delete ipSoapAction;
        ipSoapAction = NULL;
        }
        
   	ipSoapAction = pTemp;
    
    CleanupStack::Pop(); // pTemp is now safe
    return SoapAction2();
    }


EXPORT_C TPtrC8 CSenSoapEnvelope::SoapAction()
    {
    if(ipSoapAction)
        {
        return ipSoapAction->Des();
        }
    else
        {
		return KSoapActionHeaderValueEmpty();
        }
    
    }


EXPORT_C void CSenSoapEnvelope::ParseL(const TDesC8& aXml)
    {
    CSenBaseFragment::ParseL(aXml);
//    CSenElement& envelope = this->AsElement();

    /*
    // SOAP Header is not mandatory:
    CSenElement* pHeader = envelope.Element(KSenSoapEnvelopeXmlns, KSenSoapEnvelopeHeaderName);
    if(!pHeader)
        {
        User::Leave(KErrSenNoSoapHeader);
        }
    */

    // We could check body, but then this class could not
    // be used in cases, where header is parsed from file
    // or some other source, prior setting the body via
    // SetBody()

    /*
    CSenElement* pBody = envelope.Element(KSenSoapEnvelopeXmlns, KSenSoapEnvelopeBodyName);
    if(!pBody)
        {
        User::Leave(KErrSenNoSoapBody);
        }
    */
    }

EXPORT_C TBool CSenSoapEnvelope::HasHeader()
    {
    CSenElement& envelope = this->AsElement();
    // True, if element exists, false otherwise:
    return (envelope.Element(NsUri(), KSenSoapEnvelopeHeaderName) != NULL);
    }

EXPORT_C TBool CSenSoapEnvelope::HasBody()
    {
    CSenElement& envelope = this->AsElement();
    // True, if element exists, false otherwise:
    return (envelope.Element(NsUri(), KSenSoapEnvelopeBodyName) != NULL);
    }

EXPORT_C TSOAPVersion CSenSoapEnvelope::SoapVersion()
    {
    if( NsUri() == KSenSoap12EnvelopeXmlns )
        {
        return ESOAP12;
        }
    else 
        {
        return ESOAP11;
        }
    }
    
    
EXPORT_C TPtrC8 CSenSoapEnvelope::SoapAction2()
    {
    if(ipSoapAction)
        {
        return ipSoapAction->Des();
        }
    else
        {
        // Note, that zero-length descriptor must be returned
        // if SetSoapActionL() has not been called. Otherwise,
        // when passing transport properties over IPC boundary,
        // code cannot determine whether the caller, who utilizes
        // CSenSoapEnvelope wishes to override any underlying
        // value of SOAPAction with "empty" value (== "").
        // If SOAPAction is set in CSenSoapEnvelope, it is stronger
        // than message level (SendL/SubmitL in CSenServiceConnection)
        // or session level (SetTransportPropertiesL) definitions.
        return KNullDesC8();
        }
        
    }
    
// End of File
