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
#include <SenXmlUtils.h>
#include <SenBaseAttribute.h>
#include <SenXmlConstants.h>

#include "senlogger.h"
#include "SenHttpTransportProperties.h"
#include "senpropertiesfragment.h"
#include "MSenProperty.h"

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

void CSenHttpTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenTransportProperties::SetReader(aReader);
    } 

MSenProperties::TSenPropertiesClassType CSenHttpTransportProperties::PropertiesClassType() 
    {
    return ESenHttpTransportProperties;
    }

void CSenHttpTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenTransportProperties::WriteToL(aWriteStream);
    }
    
void CSenHttpTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenTransportProperties::ReadFromL(aBuffer);
    }    

HBufC8* CSenHttpTransportProperties::AsUtf8L()
    {
    return CSenTransportProperties::AsUtf8L();
    }

HBufC8* CSenHttpTransportProperties::AsUtf8LC()
    {
    return CSenTransportProperties::AsUtf8LC();
    }
    
TBool CSenHttpTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenHttpTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenTransportProperties::IsSafeToCast(aClass);
        }
    }      
    
    
TInt CSenHttpTransportProperties::SetPropertyL(const TDesC8& aName,
                                               const TDesC8& aValue)
    {
    return CSenTransportProperties::SetPropertyL(aName, aValue);
    }
    
TInt CSenHttpTransportProperties::PropertyL(const TDesC8& aName,
                                            TPtrC8& aValue)
    {
    return CSenTransportProperties::PropertyL(aName, aValue);
    }
    
TInt CSenHttpTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                  const TInt aValue)
    {
    return CSenTransportProperties::SetIntPropertyL(aName, aValue);
    }

TInt CSenHttpTransportProperties::IntPropertyL(const TDesC8& aName,
                                               TInt& aValue)
    {
    return CSenTransportProperties::IntPropertyL(aName, aValue);
    }

TInt CSenHttpTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                   const TBool aValue)
    {
    return CSenTransportProperties::SetBoolPropertyL(aName, aValue);
    }

TInt CSenHttpTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                TBool& aValue)
    {
    return CSenTransportProperties::BoolPropertyL(aName, aValue);
    }  

TInt CSenHttpTransportProperties::SetOmittedL(const TDesC8& aName, TBool aValue)
    {
    return CSenTransportProperties::SetOmittedL(aName, aValue);
    }

TInt CSenHttpTransportProperties::RemovePropertyL(const TDesC8& aName)
    {
    return CSenTransportProperties::RemovePropertyL(aName);
    }

CSenHttpTransportProperties::~CSenHttpTransportProperties()
    {
    }

void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aLocalname, aXml, aParser);
    }

void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aNamespace, aLocalname, aXml, aParser);
    }

void CSenHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aQualifiedName, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aNamespace, aLocalname, aQualifiedName, aXml, aParser);
    }

void CSenHttpTransportProperties::BaseConstructL(const CSenElement& aElement)
    {
    CSenTransportProperties::BaseConstructL(aElement);
    }

CSenHttpTransportProperties::CSenHttpTransportProperties()
    {
    }


TInt CSenHttpTransportProperties::ContentTypeL(TPtrC8& aContentType)
    {
    return HttpHeaderL(KContentTypeLocalName, aContentType);
    }

void CSenHttpTransportProperties::SetContentTypeL(const TDesC8& aContentType)
    {
    SetPropertyL(KContentTypeLocalName, aContentType, KHttpHeaderType);
    }

TInt CSenHttpTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return HttpHeaderL(KUserAgentLocalName, aUserAgent);
    }

void CSenHttpTransportProperties::SetUserAgentL(const TDesC8& aUserAgent)
    {
    SetPropertyL(KUserAgentLocalName, aUserAgent, KHttpHeaderType);
    }

        
TInt CSenHttpTransportProperties::SoapActionL(TPtrC8& aSoapAction)
    {
    return CSenTransportProperties::SoapActionL(aSoapAction);
    }

void CSenHttpTransportProperties::SetSoapActionL(const TDesC8& aSoapAction)
    {
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

    SetPropertyL(KSoapActionLocalName, *pTemp, KHttpHeaderType);

    CleanupStack::PopAndDestroy(pTemp); // Destroy temporary SoapAction
	}

TInt CSenHttpTransportProperties::AcceptL(TPtrC8& aAccept)
    {
    return HttpHeaderL(KAcceptLocalName, aAccept);    
    }

void CSenHttpTransportProperties::SetAcceptL(const TDesC8& aAccept)
    {
    SetPropertyL(KAcceptLocalName, aAccept, KHttpHeaderType);
    }
        
TInt CSenHttpTransportProperties::HttpMethodL(TSenHttpMethod& aMethod)
    {
    TPtrC8 method;
    TInt retVal = PropertyL(KHttpMethodLocalName, method);
    if (retVal == KErrNone)
        {
        if (method == KHttpGet)
            {
            aMethod = ESenHttpGet;
            return KErrNone;
            }
        else if (method == KHttpPost)
            {
            aMethod = ESenHttpPost;
            return KErrNone;
            }
        else if (method == KHttpPut)
            {
            aMethod = ESenHttpPut;
            return KErrNone;
            }
        else if (method == KHttpDelete)
            {
            aMethod = ESenHttpDelete;
            return KErrNone;
            }
        else
            {
            return KErrUnknown;
            }
        }
    else
        {
        return retVal;
        }
    }

void CSenHttpTransportProperties::SetHttpMethodL(TSenHttpMethod aHttpMethod)
    {
    switch ( aHttpMethod )
        {
        case ESenHttpGet:
            SetPropertyL(KHttpMethodLocalName, KHttpGet);
            break;
        case ESenHttpPost:
            SetPropertyL(KHttpMethodLocalName, KHttpPost);
            break;
        case ESenHttpPut:
            SetPropertyL(KHttpMethodLocalName, KHttpPut);
            break;
        case ESenHttpDelete:
            SetPropertyL(KHttpMethodLocalName, KHttpDelete);
            break;
        default:
            // Not possible
            break;
        }
    }

TInt CSenHttpTransportProperties::HttpVersionL(TSenHttpVersion& aHttpVersion)
    {
    TPtrC8 method;
    TInt retVal = PropertyL(KHttpVersionLocalName, method);
    if (retVal == KErrNone)
        {
        if (method == KHttp10)
            {
            aHttpVersion = ESenHttp10;
            return KErrNone;
            }
        else if (method == KHttp11)
            {
            aHttpVersion = ESenHttp11;
            return KErrNone;
            }
        else
            {
            return KErrUnknown;
            }
        }
    else
        {
        return retVal;
        }
    }

void CSenHttpTransportProperties::SetHttpVersionL(TSenHttpVersion aHttpVersion)
    {
    switch ( aHttpVersion )
        {
        case ESenHttp10:
            SetPropertyL(KHttpVersionLocalName, KHttp10);
            break;
        case ESenHttp11:
            SetPropertyL(KHttpVersionLocalName, KHttp11);
            break;
        default:
            // Not possible
            break;
        }
    }

TInt CSenHttpTransportProperties::HttpHeaderL(const TDesC8& aHeaderName,
                                                TPtrC8& aValue)
    {
    TPtrC8 value;
    TPtrC8 type;
    TInt retVal = PropertyL(aHeaderName, value, type);
    if ( retVal == KErrNone )
        {
        if ( type == KHttpHeaderType )
            {
            aValue.Set(value);
            }
        else
            {
            retVal = KErrNotFound;
            }
        }
    return retVal;
    }
        
void CSenHttpTransportProperties::SetHttpHeaderL(const TDesC8& aHeaderName, const TDesC8& aValue)
    {
    SetPropertyL(aHeaderName, aValue, KHttpHeaderType);
    }

TInt CSenHttpTransportProperties::SetPropertyL(const TDesC8& aName,
                                               const TDesC8& aValue,
                                               const TDesC8& aType)
    {
    return CSenTransportProperties::SetPropertyL(aName, aValue, aType);
    }

TInt CSenHttpTransportProperties::PropertyL(const TDesC8& aName,
                                            TPtrC8& aValue,
                                            TPtrC8& aType)
    {
    return CSenTransportProperties::PropertyL(aName, aValue, aType);
    }

TInt CSenHttpTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    return CSenTransportProperties::IapIdL(aCurrentIapId);
    }

void CSenHttpTransportProperties::SetIapIdL(TUint32 aIapId)
    {
    CSenTransportProperties::SetIapIdL(aIapId);
    }

void CSenHttpTransportProperties::SetSnapIdL(TUint32 aSnapId)
    {
	CSenTransportProperties::SetSnapIdL(aSnapId);
    }


TInt CSenHttpTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
    {
    return CSenTransportProperties::SnapIdL(aCurrentSnapId);
    }
TInt CSenHttpTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    return CSenTransportProperties::ProxyPortL(aProxyPort);
    }

void CSenHttpTransportProperties::SetProxyPortL(TInt aProxyPort)
    {
    CSenTransportProperties::SetProxyPortL(aProxyPort);
    }

TInt CSenHttpTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return CSenTransportProperties::ProxyHostL(aProxyHost);
    }

void CSenHttpTransportProperties::SetProxyHostL(const TDesC8& aProxyHost)
    {
    CSenTransportProperties::SetProxyHostL(aProxyHost);
    }

TInt CSenHttpTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return CSenTransportProperties::ProxyUsageL(aValue);
    }

void CSenHttpTransportProperties::SetProxyUsageL(TBool aProxyUsage)
    {
    CSenTransportProperties::SetProxyUsageL(aProxyUsage);
    }
    
TInt CSenHttpTransportProperties::SecureDialogL(TBool& aValue)
    {
    return CSenTransportProperties::SecureDialogL(aValue);
    }

void CSenHttpTransportProperties::SetSecureDialogL(TBool aSecureDialog)
    {
    CSenTransportProperties::SetSecureDialogL(aSecureDialog);
    }
/*
TInt CSenHttpTransportProperties::IAPDialogL(TBool& aValue)
    {
    return CSenTransportProperties::IAPDialogL(aValue);
    }

void CSenHttpTransportProperties::SetIAPDialogL(TBool aIAPDialog)
    {
    CSenTransportProperties::SetIAPDialogL(aIAPDialog);
    }
*/
TInt CSenHttpTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return CSenTransportProperties::DeviceIDL(aDeviceID);
    }

void CSenHttpTransportProperties::SetDeviceIDL(const TDesC8& aDeviceID)
    {
    CSenTransportProperties::SetDeviceIDL(aDeviceID);
    }    
TInt CSenHttpTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return CSenTransportProperties::DownloadFolderL(aDownloadFolder);
    }

void CSenHttpTransportProperties::SetDownloadFolderL(const TDesC8& aDownloadFolder)
    {
    CSenTransportProperties::SetDownloadFolderL(aDownloadFolder);
    }  

TInt CSenHttpTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    return CSenTransportProperties::FileAttachmentL(aCid, aFileName);
    }

TInt CSenHttpTransportProperties::SetFileAttachmentL(const TDesC8& aCid, const TDesC8& aFileName)
    {
    return CSenTransportProperties::SetFileAttachmentL(aCid, aFileName);
    }

void CSenHttpTransportProperties::ApplyBindingL(TSOAPVersion aSoapVersion)
    {

    TLSLOG_FORMAT((KSenUtilsLogChannel, KMinLogLevel, _L("CSenHttpTransportProperties::ApplyBindingL(SOAP1.%d)"), aSoapVersion==ESOAP11?1:2));
    //TLSLOG_L(KSenUtilsLogChannel, KSenUtilsLogLevel, "CSenHttpTransportProperties::ApplyBindingL");
    TPtrC8 value;
    SoapActionL(value);
    if ( aSoapVersion == ESOAP11 )
        {
        TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "- Applying SOAP 1.1 binding.");
        HBufC8* dblQutoedValue = AdaptDblQutesLC(value);
        SetSoapActionL(*dblQutoedValue);
        CleanupStack::PopAndDestroy(dblQutoedValue);
        }
    else
        {
        TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "- Applying SOAP 1.2 binding.");
        const TInt KActionLength = 11; // comes from chars: '; action=""'
	    CSenElement* pElement = ipFragment->AsElement().Element(KSoapActionLocalName);
	    if ( pElement )
		    {
		    // Remove the type attribute, to prevent HTTP header to be added in HTTP transport plug-in(s)
	        CSenBaseAttribute* attr = SenXmlUtils::RemoveAttributeL(*pElement, KSenTypeAttributeName);
	        delete attr;
	        attr = NULL;
		    }
//        // (Re-)setting empty (zero-length) value for type attribute should do the same trick:
//        SetPropertyL(KSoapActionLocalName, value, KNullDesC8);

        HBufC8* dblQutoedValue = AdaptDblQutesLC(value);
        HBufC8* buf = HBufC8::NewLC(dblQutoedValue->Length() + KSenSoap12ContentTypeHeaderValue().Length() +
                KActionLength);
        TPtr8 ptr = buf->Des();
        ptr.Append(KSenSoap12ContentTypeHeaderValue);
        if ( ptr.Length() )
            {
            _LIT8(KColonSpace,"; ");
            ptr.Append(KColonSpace);	//CodeScannerWarnings
            }
        _LIT8(KAction,"action=");
        ptr.Append(KAction);	//CodeScannerWarnings
        ptr.Append( *dblQutoedValue );
        SetContentTypeL( *buf );
        CleanupStack::PopAndDestroy( buf );        
        CleanupStack::PopAndDestroy( dblQutoedValue );        
        SetAcceptL( KSenSoap12AcceptHeaderValue );
        }
    }    
HBufC8* CSenHttpTransportProperties::AdaptDblQutesLC(const TDesC8& aValue)
    {
    if((!SenXmlUtils::StartsWith(aValue, KSenDblQuot()))
       && (!SenXmlUtils::EndsWith(aValue, KSenDblQuot()))
       && (aValue.Length() >= 2 * KSenDblQuot().Length()))
        {
        HBufC8* buf = HBufC8::NewLC(aValue.Length()  + 2 * KSenDblQuot().Length());
        TPtr8 ptr = buf->Des();
        ptr.Append( KSenDblQuot);
        ptr.Append( aValue );
        ptr.Append( KSenDblQuot);
        return buf;
        }
     else
        {
        return aValue.AllocLC();
        }
    }
MSenProperties* CSenHttpTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenHttpTransportProperties* pProperties = this->NewLC(element);
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

TInt CSenHttpTransportProperties::MwsNamespaceL(TPtrC8& aMwsNamespace)
    {
    return CSenTransportProperties::MwsNamespaceL(aMwsNamespace);
    }

void CSenHttpTransportProperties::SetMwsNamespaceL(const TDesC8& aMwsNamespace)
    {
    CSenTransportProperties::SetMwsNamespaceL(aMwsNamespace);
    }  

// END OF FILE


