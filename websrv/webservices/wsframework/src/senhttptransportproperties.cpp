/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#include <s32strm.h> // RWriteStream
#include <SenXmlReader.h>
#include "SenHttpTransportProperties.h"

namespace
    {
    _LIT8(KIapIdLocalName, "IapId"); 
    _LIT8(KProxyHostLocalName, "ProxyHost"); 
    _LIT8(KProxyPortLocalName, "ProxyPort"); 
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewL()
    {
    CSenHttpTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewLC()
    {
    CSenHttpTransportProperties* pNew = new (ELeave) CSenHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewL(const TDesC8& aXmlUtf8,
                                                               CSenXmlReader& aParser)
    {
    CSenHttpTransportProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewLC(const TDesC8& aXmlUtf8,
                                                                CSenXmlReader& aParser)
    {
    CSenHttpTransportProperties* pNew = new (ELeave) CSenHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenHttpTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenHttpTransportProperties* CSenHttpTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenHttpTransportProperties* pNew = new (ELeave) CSenHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }



/**
* Get an integer identifying properties class type.
* All subclasses MUST return use unique integer value.
* @return unique TInt identifying this properties class type
*/
TInt CSenHttpTransportProperties::PropertiesClassType() 
    { 
    return KSenPropertiesClassTypeSenHttpTransportProperties; 
    }

/**
* Writes this properties object as descriptor into a stream.
* UTF-8 charset encoding will be used.
* @since Series60
* @param aWriteStream The stream to write to.
*/
void CSenHttpTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenTransportProperties::WriteAsXMLToL(aWriteStream);
    /*
    WriteAsXMLToL(aWriteStream);
    */
    }

/**
* Get these properties as UTF-8 charset encoded descriptor.
* @since Series60
* @return a pointer to UTF-8 encoded descriptor. 
* Caller takes ownership.
*/
HBufC8* CSenHttpTransportProperties::AsUtf8L()
    {
    return CSenTransportProperties::AsUtf8L();
    /*
    return AsXmlL();
    */
    }


/**
* Get these properties as UTF-8 charset encoded descriptor.
* @since Series60
* @return a pointer to UTF-8 encoded descriptor, which is
* has been left on cleanup stack.
* Caller takes ownership.
*/
HBufC8* CSenHttpTransportProperties::AsUtf8LC()
    {
    return CSenTransportProperties::AsUtf8LC();
    /*
    HBufC8* pAsXml = AsXmlL();
    CleanupStack::PushL(pAsXml);
    return pAsXml;
    */
    }

/**
* Destructor.
*/
CSenHttpTransportProperties::~CSenHttpTransportProperties()
    {
    }

/**
* Constructs this fragment. Subclasses should call this
* function in their corresponding 2nd phase constructor.
*
* @param aLocalname is used as XML localname for this instance
* @param aXml is the XML document to be parsed into this DOM tree
* this properties instance represents.
* Parsing is omitted iff descriptor is zero-length: KNullDesC8.
*/
void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aLocalname, aXml, aParser);
    }

/**
* Constructs this fragment. Subclasses should call this
* function in their corresponding 2nd phase constructor.
*
* @param aNamespace is the XML namespace
* @param aLocalname is the XML localname
* @param aXml is the XML document to be parsed into this DOM tree
* this properties instance represents.
* Parsing is omitted iff descriptor is zero-length: KNullDesC8.
*/
void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aNamespace, aLocalname, aXml, aParser);
    }

/**
* Constructs this fragment. Subclasses should call this
* function in their corresponding 2nd phase constructor.
*
* @param aNamespace is the XML namespace
* @param aLocalname is the XML localname
* @param aLocalname is the XML qualified name
* @param aXml is the XML document to be parsed into this DOM tree
* this properties instance represents.
* Parsing is omitted iff descriptor is zero-length: KNullDesC8.
*/
void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aQualifiedName, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aNamespace, aLocalname, aQualifiedName, aXml, aParser);

    }

/**
* 2nd phase constructor:
*/
void CSenHttpTransportProperties::BaseConstructL(const CSenElement& aElement)
    {
    CSenTransportProperties::BaseConstructL(aElement);
    }




CSenHttpTransportProperties::CSenHttpTransportProperties()
    {
    }


// First versions of real HTTP spesific transport methods:

EXPORT_C TInt CSenHttpTransportProperties::IapId(TUint32& aCurrentIapId)
    {
    CSenElement* pIapIdElement = AsElement().Element(KIapIdLocalName);
    if(pIapIdElement && pIapIdElement->HasContent())
        {
        TPtrC8 content = pIapIdElement->Content();
        TLex8 lex(content);

        TInt retVal = lex.Val(aCurrentIapId, EDecimal);
        return retVal;
        }
    return KErrNotFound;
    }


EXPORT_C void CSenHttpTransportProperties::SetIapIdL(TUint32 aIapId)
    {
    CSenElement* pIapIdElement = AsElement().Element(KIapIdLocalName);
    if(!pIapIdElement)
        {
        pIapIdElement = &(AsElement().AddElementL(KIapIdLocalName));
        }
    TBuf8<128> buf;
    buf.AppendFormat(_L8("%u"), aIapId);
    pIapIdElement->SetContentL(buf);
    }

EXPORT_C TInt CSenHttpTransportProperties::ProxyPort()
    {
    CSenElement* pProxyPortElement = AsElement().Element(KProxyPortLocalName);
    if(pProxyPortElement && pProxyPortElement->HasContent())
        {
        TPtrC8 content = pProxyPortElement->Content();
        TLex8 lex(content);

        TUint proxyPort(0);
        TInt retVal = lex.Val(proxyPort, EDecimal);
        if(retVal==KErrNone)
            {
            return (TInt)proxyPort;
            }
        else
            {
            return retVal;
            }
        }
    return KErrNotFound;
    }

EXPORT_C void CSenHttpTransportProperties::SetProxyPortL(TInt aProxyPort)
    {
    CSenElement* pProxyPortElement = AsElement().Element(KProxyPortLocalName);
    if(!pProxyPortElement)
        {
        pProxyPortElement = &(AsElement().AddElementL(KProxyPortLocalName));
        }
    TBuf8<128> buf;
    buf.AppendFormat(_L8("%u"), aProxyPort);
    pProxyPortElement->SetContentL(buf);
    }

EXPORT_C TPtrC8 CSenHttpTransportProperties::ProxyHost()
    {
    CSenElement* pProxyHostElement = AsElement().Element(KProxyHostLocalName);
    if(pProxyHostElement && pProxyHostElement->HasContent())
        {
        return pProxyHostElement->Content();
        }
    else return KNullDesC8();
    }

EXPORT_C void CSenHttpTransportProperties::SetProxyHostL(const TDesC8& aProxyHost)
    {
    CSenElement* pProxyHostElement = AsElement().Element(KProxyHostLocalName);
    if(!pProxyHostElement)
        {
        pProxyHostElement = &(AsElement().AddElementL(KProxyHostLocalName));
        }
    pProxyHostElement->SetContentL(aProxyHost);
    }

// END OF FILE´


