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

#include <SenXmlUtils.h>
#include <SenDomFragmentBase.h>
#include <xml/dom/xmlengnodelist.h>


#include "SenSoapEnvelope2.h"
#include "SenSoapFault2.h"
#include "senlayeredtransportproperties.h"
#include "senmessagecontext.h"
#include "msenmessagecontext.h"

#include "SenXmlProperties.h" // for de-alloc
#include "senlayeredxmlproperties.h" // for de-alloc

#include "sendebug.h"
#include "senlogger.h"
#include "wslibxml2utils.h"

EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewL()");    
    CSenSoapEnvelope2* pNew = CSenSoapEnvelope2::NewLC();
    CleanupStack::Pop( pNew ); // pNew
    return pNew;    
    }
    
EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewLC()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewLC()");    
    CSenSoapEnvelope2* pNew = new (ELeave) CSenSoapEnvelope2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewL(MSenMessageContext& aContext)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewL(aContext)");    
    CSenSoapEnvelope2* pNew = CSenSoapEnvelope2::NewLC(aContext);
    CleanupStack::Pop(pNew); // pNew
    return pNew;    
    }

EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewLC(MSenMessageContext& aContext)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewLC(aContext)");    
    CSenSoapEnvelope2* pNew = new (ELeave) CSenSoapEnvelope2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext);
    return pNew;
    }

EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewL(CSenSoapEnvelope2& aEnvelope)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewLC(aEnvelope)");    
    CSenSoapEnvelope2* pNew = CSenSoapEnvelope2::NewLC(aEnvelope);
    CleanupStack::Pop(pNew); // pNew
    return pNew;    
    }

EXPORT_C CSenSoapEnvelope2* CSenSoapEnvelope2::NewLC(CSenSoapEnvelope2& aEnvelope)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::NewLC(aEnvelope)");    
    CSenSoapEnvelope2* pNew = new (ELeave) CSenSoapEnvelope2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aEnvelope);
    return pNew;
    }

EXPORT_C void CSenSoapEnvelope2::BaseConstructL(TSOAPVersion aVersion)
    {
    if (aVersion != ESOAP12)
        {
        CSenFragmentBase::BaseConstructL(KSenSoapEnvelopeXmlns,
                                         KSenSoapEnvelopeName,
                                         KSenSoapEnvelopePrefix);
        }
    else
        {
        CSenFragmentBase::BaseConstructL(KSenSoap12EnvelopeXmlns,
                                         KSenSoapEnvelopeName,
                                         KSenSoapEnvelopePrefix);

        }
    }

EXPORT_C void CSenSoapEnvelope2::BaseConstructL(MSenMessageContext& aContext)
    {
    ipNotOwnedContext = &aContext;
    BaseConstructL(ESOAP11);
    }

EXPORT_C void CSenSoapEnvelope2::BaseConstructL(MSenMessageContext& aContext, TSOAPVersion aVersion)
    {
    ipNotOwnedContext = &aContext;
    BaseConstructL(aVersion);
    }

EXPORT_C void CSenSoapEnvelope2::BaseConstructL()
    {
    BaseConstructL(ESOAP11);
    }

EXPORT_C void CSenSoapEnvelope2::BaseConstructL(CSenSoapEnvelope2& aEnvelope)
    {
    CSenFragmentBase::BaseConstructL(aEnvelope.AsElementL());
    
    iTransactionId       = aEnvelope.iTransactionId;
    ipNotOwnedContext    = aEnvelope.ipNotOwnedContext;
    iFault               = aEnvelope.iFault;
    iDirection           = aEnvelope.iDirection;
    ipNotOwnedContext    = aEnvelope.ipNotOwnedContext;
    
    if ( aEnvelope.ipSoapAction )
        {
        ipSoapAction = aEnvelope.ipSoapAction->AllocL();
        }
        
    if ( aEnvelope.ipBodyFragment )
        {
        TXmlEngElement envelope = AsElementL();
        TXmlEngElement body;

        RXmlEngNodeList<TXmlEngElement> list;
        CleanupClosePushL(list);
        envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeBodyName,
                                             NsUri());
        if ( list.Count() > 0 )
            {
            body = list.Next();
            ipBodyFragment = CSenFragmentBase::NewL(body, AsDocumentL());
            }
        
        CleanupStack::PopAndDestroy(&list);
        }
        
    if ( aEnvelope.ipHeaderFragment )
        {
        TXmlEngElement envelope = AsElementL();
        TXmlEngElement header;
        
        RXmlEngNodeList<TXmlEngElement> list;
        CleanupClosePushL(list);
        envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeHeaderName,
                                             NsUri());
        if ( list.Count() > 0 )
            {
            ipHeaderFragment = CSenFragmentBase::NewL(header, AsDocumentL());
            }
        
        CleanupStack::PopAndDestroy(&list);
        }
        
    if ( aEnvelope.ipProperties )
        {
        ipProperties = aEnvelope.ipProperties->CloneL();
        }    
    }

CSenSoapEnvelope2::CSenSoapEnvelope2()
:   ipBodyFragment(NULL),
    ipHeaderFragment(NULL),
    ipSoapAction(NULL),
    iFault(EFalse),
    iTransactionId(KErrNotFound)
    {
    }

EXPORT_C CSenSoapEnvelope2::~CSenSoapEnvelope2()
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

    if( ipProperties )
        {
        
        if ( ipProperties->IsSafeToCast( MSenProperties::ESenXmlProperties ) )
            {
            CSenXmlProperties* pFlatProps = (CSenXmlProperties*)ipProperties;
            delete pFlatProps;
            ipProperties = NULL;
            }
        else if ( ipProperties->IsSafeToCast( MSenProperties::ESenLayeredXmlProperties ) )
            {
            CSenLayeredXmlProperties* pLayeredProps = (CSenLayeredXmlProperties*)ipProperties;
            delete pLayeredProps;
            ipProperties = NULL;
            }
        // else // TLS log as fatal..
        }
    }
    

EXPORT_C void CSenSoapEnvelope2::OnStartElementL(const RTagInfo& aElement,
    	                                         const RAttributeArray& aAttributes,
                                                 TInt /*aErrorCode*/)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::OnStartElementL");    
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
        
    switch (iState)
        {
        case KSenStateIgnore:
            {
            // check if we encounter the start of a soap:Envelope...
            if ( iElement.IsNull() && saxNsUri == NsUri() &&
                 saxLocalName == KSenSoapEnvelopeName )
                {
                if ( saxNsUri == KNullDesC8 && saxPrefix == KNullDesC8 )
                    {
                    iDocument.CreateDocumentElementL(saxLocalName);
                    }
                else
                    {
                    if ( saxPrefix == KNullDesC8 )
                        {
                        iDocument.CreateDocumentElementL(saxLocalName,
                                                         saxNsUri,
                                                         KNullDesC8);
                        }
                    else
                        {
                        iDocument.CreateDocumentElementL(saxLocalName,
                                                         saxNsUri,
                                                         saxPrefix);
                        }
                    }
                iElement = iDocument.DocumentElement();        
                }
                
            if ( iElement.NotNull() && saxLocalName == KSenSoapEnvelopeName && saxNsUri != NsUri() )
                {
                iElement.Remove();
                if ( saxNsUri == KNullDesC8 && saxPrefix == KNullDesC8 )
                    {
                    iDocument.CreateDocumentElementL(saxLocalName);
                    }
                else
                    {
                    if ( saxPrefix == KNullDesC8 )
                        {
                        iDocument.CreateDocumentElementL(saxLocalName,
                                                         saxNsUri,
                                                         KNullDesC8);
                        }
                    else
                        {
                        iDocument.CreateDocumentElementL(saxLocalName,
                                                         saxNsUri,
                                                         saxPrefix);
                        }
                    }
                iElement = iDocument.DocumentElement();        
                }
                

            if ( iElement.NotNull() && LocalName() == saxLocalName &&
                 NsUri() == saxNsUri )
                {
                TXmlEngElement element = AsElementL();
                WsXmlUtils domUtils;       
                domUtils.XmlEngRenameElementL(element, saxLocalName, saxNsUri, saxPrefix);
                AddAttributesToElementL(element, aAttributes);
                }

            else if ( saxNsUri == NsUri() )
                {
                // we are interested in SOAP Header
                if ( saxLocalName == KSenSoapEnvelopeHeaderName )
                    {
                    iState = KStateParsingSoapHeader;

                    // Make sure that header element exists:
                    // == this ensures that we add a "Header"
                    // element to the children:
                    TXmlEngElement header = HeaderL();
                    
                    WsXmlUtils domUtils;       
                    domUtils.XmlEngRenameElementL(header, saxLocalName, saxNsUri, saxPrefix);
                    AddAttributesToElementL(header, aAttributes);
                    }
                else if( saxLocalName == KSenSoapEnvelopeBodyName )
                    {
                    iState = KStateParsingSoapBody;

                    TXmlEngElement body = BodyL();
                    WsXmlUtils domUtils;
                    domUtils.XmlEngRenameElementL(body, saxLocalName, saxNsUri, saxPrefix);
                    AddAttributesToElementL(body, aAttributes);
                    }
                //ESLI-6WEN9V
                //This case is to handle fault which lies outside the body or without body
                else if( saxLocalName == KSenSoapFaultName)
	                 	{
	                 	iFault = ETrue;
	                	RSenDocument document = AsDocumentL();
	                	TXmlEngElement element = BodyL();
	                                
	                	// delegate parsing to a SOAPFaultFragment to keep
	                	// a structured representation.
	                	ipBodyFragment = CSenSoapFault2::NewL(saxNsUri,
	                                                      saxLocalName,
	                                                      saxPrefix,
	                                                      aAttributes,
	                                                      element,
	                                                      document);
	                                                      
	                	OnDelegateParsingL(*ipBodyFragment);	
	                 	
	                 	}
                //ESLI-6WEN9V
                }
            break;
            }
        case KStateParsingSoapHeader:
            {
            ParseHeaderL(aElement, aAttributes);
            }
            break;
        case KStateParsingSoapBody:
            {
            if ( ipBodyFragment )
                {
                ipBodyFragment->ExtractElement();
                delete ipBodyFragment;
                ipBodyFragment = NULL;
                }

            // check if we encounter a Fault
            if ( saxLocalName == KSenSoapFaultName )
                {
                iFault = ETrue;
                RSenDocument document = AsDocumentL();
                TXmlEngElement element = BodyL();
                                
                // delegate parsing to a SOAPFaultFragment to keep
                // a structured representation.
                ipBodyFragment = CSenSoapFault2::NewL(saxNsUri,
                                                      saxLocalName,
                                                      saxPrefix,
                                                      aAttributes,
                                                      element,
                                                      document);
                                                      
                OnDelegateParsingL(*ipBodyFragment);
                }
            else
                {
                RSenDocument document = AsDocumentL();
                TXmlEngElement element = BodyL();
                
                // Delegate uses same document
                // Parent for delegate will be body
                
				ipBodyFragment = CreateBodyFragmentL(saxNsUri, 
				                                     saxLocalName,
					                                 saxPrefix, 
					                                 aAttributes,
					                                 element, 
					                                 document);
				
				
                OnDelegateParsingL(*ipBodyFragment);
                }
            }
            break;
        default:
            break;
        }
    }

EXPORT_C void CSenSoapEnvelope2::OnEndElementL(
                                            const RTagInfo& aElement,
                                            TInt /*aErrorCode*/)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::OnEndElementL");    
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
    
    switch (iState)
        {
        case KSenStateIgnore:
            {
            break;
            }
        case KStateParsingSoapHeader:
            {
            if (saxNsUri == NsUri() && saxLocalName == KSenSoapEnvelopeHeaderName)
                {
                iState = KSenStateIgnore;
                }
            break;
            }
        case KStateParsingSoapBody:
            {
            if (saxNsUri == NsUri() && saxLocalName == KSenSoapEnvelopeBodyName)
                {
                iState = KSenStateIgnore;
                }
/*            else
                {
                if(ipBodyFragment)
                    {
                    //ipBodyFragment->EnsureNamespacesL();
                    }
                }*/
            }
            break;
        default:
            break;
        }
    }

EXPORT_C TXmlEngElement CSenSoapEnvelope2::SetBodyL(TXmlEngElement aBodyElement)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::SetBodyL(aBodyElement)");    
    if ( aBodyElement.Name() == KSenSoapFaultName &&  aBodyElement.NamespaceUri() == NsUri() )
        {
        iFault = ETrue;
        }
    
    TXmlEngElement body = BodyL();
    body.RemoveChildren();
    aBodyElement.MoveTo(body);
    return aBodyElement;
    }
    
EXPORT_C TPtrC8 CSenSoapEnvelope2::SetBodyL(const TDesC8& aBody)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::SetBodyL(aBody)");
    TLSLOG_ALL(KSenMessagesLogChannel, KMaxLogLevel, aBody);
    TXmlEngElement body = BodyL();
    body.SetTextNoEncL(aBody);
    return TPtrC8(aBody);
    }
    

EXPORT_C TXmlEngElement CSenSoapEnvelope2::BodyL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::BodyL()");
    TXmlEngElement envelope = AsElementL();
    TXmlEngElement body;

    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeBodyName,
                                         NsUri());
    if ( list.Count() > 0 )
        {
        body = list.Next();
        }
    else
        {
        body = envelope.AddNewElementL(KSenSoapEnvelopeBodyName);

        // Check if namespace declaration
        // is already defined in envelope.
        // => Should be!
        TXmlEngNamespace ns = envelope.LookupNamespaceByUriL(NsUri());
        if ( ns.IsUndefined() )
            {
            // Namespace declaration will be added
            body.AddNamespaceDeclarationL(NsUri(), KNullDesC8);
            }
        
        WsXmlUtils domUtils;       
        domUtils.XmlEngRenameElementL(body, KSenSoapEnvelopeBodyName, NsUri(), KNullDesC8);
        }    
    
    CleanupStack::PopAndDestroy(&list);
    return body;
    }

EXPORT_C TXmlEngElement CSenSoapEnvelope2::HeaderL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::HeaderL()");
    TXmlEngElement envelope = AsElementL();
    TXmlEngElement header;

    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeHeaderName,
                                         NsUri());
    if ( list.Count() > 0 )
        {
        header = list.Next();
        }
    else
        {
        header = envelope.AddNewElementL(KSenSoapEnvelopeHeaderName);

        // Check if namespace declaration
        // is already defined in envelope.
        // => Should be!
        TXmlEngNamespace ns = envelope.LookupNamespaceByPrefixL(NsPrefix());
        WsXmlUtils domUtils;       
        domUtils.XmlEngRenameElementL(header, KSenSoapEnvelopeHeaderName, ns.Uri(), ns.Prefix() );

        TXmlEngElement body = BodyL();
        if ( body.NotNull() )
            {
            header.SetAsFirstSibling();
            }
        }
    CleanupStack::PopAndDestroy(&list);
        
    return header;
    }

EXPORT_C TXmlEngElement CSenSoapEnvelope2::AddHeaderL(TXmlEngElement aHeaderElement)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::AddHeaderL(aHeaderElement)");
    aHeaderElement.MoveTo(HeaderL());
    return aHeaderElement;
    }

EXPORT_C void CSenSoapEnvelope2::ParseHeaderL(
                                        const RTagInfo& aElement,
                                        const RAttributeArray& aAttributes)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::ParseHeaderL");
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
    
    if ( ipHeaderFragment )
        {
        delete ipHeaderFragment;
        ipHeaderFragment = NULL;
        }
        
    TXmlEngElement header = HeaderL();
    RSenDocument document = AsDocumentL();
        
    ipHeaderFragment = CSenDomFragmentBase::NewL(saxNsUri,
                                                 saxLocalName,
                                                 saxPrefix,
                                                 aAttributes,
                                                 header,
                                                 document
                                                 );

    OnDelegateParsingL(*ipHeaderFragment);
    }


EXPORT_C HBufC8* CSenSoapEnvelope2::BodyAsStringL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::BodyAsStringL");
    TXmlEngElement body = BodyL();
    
    HBufC8* bodyAsXML = NULL;

    // Note, that difference to Jave ref-implementation here is,
    // that in *EVERY* case, no matter if SOAP BODY has no child 
    // element at all OR if it has one or more children, the whole
    // <S:Body> element is ALWAYS returned -- [never just the only
    // child, and never a of zero-length string, even if no children]

    body.Unlink(); // According to documentation RDocument which
                   // was holding the body TXmlEngElement remains owner
                   // until body is linked elsewhere or destroyed.
                   // => CleanupStack usage is not needed.
    body.ReconcileNamespacesL();
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    body.OuterXmlL(buffer);
    bodyAsXML = buffer.AllocL();
    CleanupStack::PopAndDestroy(&buffer);
    body.Remove(); // Destroy Body
        
    return bodyAsXML;
    }

EXPORT_C TBool CSenSoapEnvelope2::IsFault()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::IsFault()");
    return iFault;
    }

// NOTE: caller takes ownership of the NEW fault element
EXPORT_C CSenSoapFault2* CSenSoapEnvelope2::DetachFaultL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::DetachFaultL()");
    TXmlEngElement body = BodyL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    
    TXmlEngElement fault;
    body.GetElementsByTagNameL(list, KSenSoapFaultName, NsUri());

    if ( list.Count() > 0 )
        {
        fault = list.Next();
            
        fault.Unlink();

        fault.ReconcileNamespacesL();

        CSenSoapFault2* pAnswer = CSenSoapFault2::NewL(fault);
        CleanupStack::PopAndDestroy(&list);
        return pAnswer;
        }
        
    CleanupStack::PopAndDestroy(&list);
    return NULL;
    }

EXPORT_C CSenSoapFault2* CSenSoapEnvelope2::FaultL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::FaultL()");
    TXmlEngElement body = BodyL();
    TXmlEngElement fault;

    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    body.GetElementsByTagNameL(list, KSenSoapFaultName,  NsUri());
    if ( list.Count() > 0 )
        {
        fault = list.Next();

        CSenSoapFault2* pAnswer = CSenSoapFault2::NewL(fault,
                                                       AsDocumentL());
        CleanupStack::PopAndDestroy(&list);
        return pAnswer;
        }

    CleanupStack::PopAndDestroy(&list);
    return NULL;
    }

EXPORT_C TPtrC8 CSenSoapEnvelope2::SetSoapActionL(const TDesC8& aSoapAction)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::SetSoapActionL(aSoapAction)");

    _LIT8(KQuote, "\"");
    HBufC8* pTemp = HBufC8::NewLC(aSoapAction.Length()+(2*KQuote().Length()));

    TPtr8 temp = pTemp->Des();

    if(!SenXmlUtils::StartsWith(aSoapAction, KQuote))
        {
        temp.Append(KQuote);
        }
    temp.Append(aSoapAction);

    if(!SenXmlUtils::EndsWith(*pTemp, KQuote))
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
    return SoapAction();
    }


EXPORT_C TPtrC8 CSenSoapEnvelope2::SoapAction()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::SoapAction()");
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

EXPORT_C TBool CSenSoapEnvelope2::HasHeader()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::HasHeader()");
    TInt err( KErrNone );
    TInt count( 0 );
    RXmlEngNodeList<TXmlEngElement> list;
    TRAP(   
        err, 
        TXmlEngElement envelope = AsElementL(); 
        envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeHeaderName, NsUri() );    
         )
    if( !err ) 
        {
        count = list.Count();
        }
    list.Close();
    if ( count > 0 )
        {
        return ETrue;
        }
    else
        {
        // actually, there could be some header(s), but it could not be checked, if a leave occured.       
        return EFalse;
        }
    }

EXPORT_C TBool CSenSoapEnvelope2::HasBody()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::HasBody()");
    TInt err( KErrNone );
    TInt count( 0 );
    RXmlEngNodeList<TXmlEngElement> list;
    
    TRAP( 
        err, 
        TXmlEngElement envelope = AsElementL(); 
        envelope.GetElementsByTagNameL(list, KSenSoapEnvelopeBodyName, NsUri());
        )
    if( !err )
        {
        count = list.Count();
        }
    list.Close();            
    if ( count > 0 )
        {
        return ETrue;
        }
    else
        {
        // actually, there could be body, but it could not be checked, if a leave occured.                   
        return EFalse;
        }
    }
    
EXPORT_C TSOAPVersion CSenSoapEnvelope2::SoapVersion()
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
    
EXPORT_C MSenMessage::TClass CSenSoapEnvelope2::Type()
    {
    return MSenMessage::ESoapEnvelope2;
    }

EXPORT_C MSenMessage::TDirection CSenSoapEnvelope2::Direction()
    {
    return iDirection;
    }

/*
EXPORT_C MSenProperties& CSenSoapEnvelope2::Properties()
    {
    if ( ipContext == NULL )
        {
        return *ipNotOwnedContext->TransportProperties();
        }

    return *ipContext->TransportProperties();
    }
    
EXPORT_C MSenMessageContext& CSenSoapEnvelope2::Context()    
    {
    if ( ipContext == NULL )
        {
        return *ipNotOwnedContext;
        }

    return *ipContext;
    }
*/

EXPORT_C TInt CSenSoapEnvelope2::SetContext(MSenMessageContext* apNotOwnedContext)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::SetContext(MSenMessageContext)");
    if( !apNotOwnedContext )
        {
        return KErrArgument;
        }

    // switching to NEW context always resets any existing properties(!)
    if(ipProperties)
    	{
        if ( ipProperties->IsSafeToCast(MSenProperties::ESenXmlProperties) )
            {
            CSenXmlProperties* pFlatProps = (CSenXmlProperties*)ipProperties;
            delete pFlatProps;
            ipProperties = NULL;
            }
        else if ( ipProperties->IsSafeToCast(MSenProperties::ESenLayeredXmlProperties) )
            {
            CSenLayeredXmlProperties* pLayeredProps = (CSenLayeredXmlProperties*)ipProperties;
            delete pLayeredProps;
            ipProperties = NULL;
	        }
        }
    // else {  // TLS log as fatal  }
    ipNotOwnedContext = apNotOwnedContext;
    return KErrNone;
    }
    
EXPORT_C MSenMessageContext* CSenSoapEnvelope2::Context()
    {
    return ipNotOwnedContext;
    }

EXPORT_C TInt CSenSoapEnvelope2::SetProperties(MSenProperties* apOwnedProperties)
    {
    if( !apOwnedProperties ) 
        {
        return KErrArgument;
        }
    if( ipProperties )
        {
        // De-allocate existing, self-owned properties
        if ( ipProperties->IsSafeToCast(MSenProperties::ESenXmlProperties) )
            {
            CSenXmlProperties* pFlatProps = (CSenXmlProperties*)ipProperties;
            delete pFlatProps;
            ipProperties = NULL;
            }
        else if ( ipProperties->IsSafeToCast(MSenProperties::ESenLayeredXmlProperties) )
            {
            CSenLayeredXmlProperties* pLayeredProps = (CSenLayeredXmlProperties*)ipProperties;
            delete pLayeredProps;
            ipProperties = NULL;
            }
        // else { // Log as fatal }
        }

    if( ipNotOwnedContext )
        {
        // Store properties into context
        ipNotOwnedContext->SetProperties( apOwnedProperties, ETrue );
        }
    else
        {
        // Own the properties directly inside this message
        ipProperties = apOwnedProperties;
        }
    return KErrNone;
    }
    
EXPORT_C MSenProperties* CSenSoapEnvelope2::Properties()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::Properties()");
    if( ipNotOwnedContext )
        {
        return ipNotOwnedContext->Properties();
        }
    else if ( ipProperties )
        {
        return ipProperties;
        }
    else
        {
        return NULL;
        }        
    }


EXPORT_C TBool CSenSoapEnvelope2::IsSafeToCast(MSenMessage::TClass aType)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::IsSafeToCast(aType)");
    if ( aType == MSenMessage::ESoapEnvelope2 )
        {
        return ETrue;
        }

    return EFalse;
    }

EXPORT_C TInt CSenSoapEnvelope2::TxnId()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::TxnId()");
    return iTransactionId;
    }
    
EXPORT_C CSenFragmentBase* CSenSoapEnvelope2::CreateBodyFragmentL(const TDesC8& aNsUri,
                                                                  const TDesC8& aLocalName,
                                                                  const TDesC8& aPrefix,
                                                                  const RAttributeArray& aAttributes,
                                                                  TXmlEngElement& aParent,
                                                                  RSenDocument& aOwnerDocument)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::CreateBodyFragmentL");
	return CSenDomFragmentBase::NewL(aNsUri, aLocalName, aPrefix, aAttributes, aParent, aOwnerDocument);    
    }
    
EXPORT_C MSenMessage* CSenSoapEnvelope2::CloneL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapEnvelope2::CloneL()");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL(*this);
    return pEnvelope;
    }

// End of File
