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











/*******************************************************
 *     SOAP 1.1    |             SOAP 1.2              *
 *-----------------------------------------------------*
 *  env:Fault      | env:Fault                         *
 *-----------------------------------------------------*
 *  faultcode      | env:Code, env:Subcode, env:Value  *
 *-----------------------------------------------------*
 *  faultstring    | env:Reason, env:Text              *
 *-----------------------------------------------------*
 *  faultactor     | env:Node, (env:Role)              *
 *-----------------------------------------------------*
 *  detail         | env:Detail                        *
 *******************************************************/
 
/****************************************************************************
 *                            SOAP 1.2 Example                              *
 ****************************************************************************
 *                                                                          *
 * <env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"        *
 *               xmlns:m="http://www.example.org/timeouts"                  *
 *               xmlns:xml="http://www.w3.org/XML/1998/namespace">          *
 *  <env:Body>                                                              *
 *   <env:Fault>                                                            *
 *    <env:Code>                                                            *
 *      <env:Value>env:Sender</env:Value>                                   *
 *      <env:Subcode>                                                       *
 *       <env:Value>m:MessageTimeout</env:Value>                            *
 *      </env:Subcode>                                                      *
 *    </env:Code>                                                           *
 *    <env:Reason>                                                          *
 *      <env:Text xml:lang="en">Sender Timeout</env:Text>                   *
 *    </env:Reason>                                                         *
 *    <env:Node>http://MyServer:80/sql</env:Node>                           *
 *    <env:Role>http://schemas.microsoft.com/sqlserver/2004/SOAP</env:Role> *
 *    <env:Detail>                                                          *
 *      <m:MaxTime>P5M</m:MaxTime>                                          *
 *    </env:Detail>                                                         *
 *   </env:Fault>                                                           *
 *  </env:Body>                                                             *
 * </env:Envelope>                                                          *
 *                                                                          *
 ****************************************************************************/ 

/****************************************************************************
 *                   SOAP (older than 1.2) Example                          *
 ****************************************************************************
 *                                                                          *
 * <env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/"      *
 *       env:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">     *
 *  <env:Body>                                                              *
 *   <env:Fault>                                                            *
 *    <faultcode>env:MustUnderstand</faultcode>                             *
 *    <faultstring>Mandatory Header error.</faultstring>                    *
 *    <faultactor>http://www.wrox.com/heroes/endpoint.asp</faultactor>      *
 *    <detail>                                                              *
 *     <w:source xmlns:w="http://www.wrox.com/">                            *
 *      <module>endpoint.asp</module>                                       *
 *      <line>203</line>                                                    *
 *     </w:source>                                                          *
 *    </detail>                                                             *
 *   </env:Fault>                                                           *
 *  </env:Body>                                                             *
 * </env:Envelope>                                                          *
 *                                                                          *
 ****************************************************************************/ 

// INCLUDE FILES
#include "SenSoapFault2.h"
#include "SenSoapEnvelope2.h"

#include <xml/dom/xmlengnodelist.h>

EXPORT_C CSenSoapFault2* CSenSoapFault2::NewL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aPrefix,
                                        const RAttributeArray& aAttributes,
                                        TXmlEngElement& aElement,
                                        RSenDocument& aDocument)
    {
    CSenSoapFault2* pNew = new (ELeave) CSenSoapFault2;
    CleanupStack::PushL(pNew);
	pNew->ConstructL(aNsUri, aLocalName, aPrefix, aAttributes,
	                 aElement, aDocument);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenSoapFault2* CSenSoapFault2::NewL(TXmlEngElement& aElement)
    {
    CSenSoapFault2* pNew = new (ELeave) CSenSoapFault2;
    CleanupStack::PushL(pNew);
	pNew->ConstructL(aElement);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenSoapFault2* CSenSoapFault2::NewL(TXmlEngElement& aElement,
                                              RSenDocument& aDocument)
    {
    CSenSoapFault2* pNew = new (ELeave) CSenSoapFault2;
    CleanupStack::PushL(pNew);
	pNew->ConstructL(aElement, aDocument);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenSoapFault2::~CSenSoapFault2()
    {
    }

EXPORT_C CSenSoapFault2::CSenSoapFault2()
	{
	}

EXPORT_C void CSenSoapFault2::ConstructL(TXmlEngElement& aElement)
	{
	BaseConstructL(aElement);
	}

EXPORT_C void CSenSoapFault2::ConstructL(TXmlEngElement& aElement,
                                         RSenDocument& aDocument)
	{
    iDocument = aDocument.Copy();
    iElement = aElement;
	}

EXPORT_C void CSenSoapFault2::ConstructL(const TDesC8& aNsUri,
							 const TDesC8& aLocalName,
							 const TDesC8& aPrefix,
							 const RAttributeArray& aAttributes,
							 TXmlEngElement& aElement,
							 RSenDocument& aDocument)
	{
	BaseConstructL(aNsUri, aLocalName, aPrefix, aAttributes,
	               aElement, aDocument);
	}
	
EXPORT_C TPtrC8 CSenSoapFault2::FaultCode()
	{
	TInt err(KErrNone);
	TXmlEngElement element;
	TXmlEngElement* pElement = NULL;
	TRAP( 
	    err, 
	    element = AsElementL();
	    pElement = &(element);
	    )
    if( !err && pElement )
        {
    	TXmlEngNamespace ns = pElement->NamespaceDeclaration();
        if ( ns.Uri() != KSenSoap12EnvelopeXmlns)
    		{
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL(list, KFaultCodeLocalname); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement child = list.Next();
   	            list.Close();
                return child.Text();
                }
            list.Close();
    		}
            
        else
    		{
            RXmlEngNodeList<TXmlEngElement> list1;
            TRAP( err, pElement->GetElementsByTagNameL( list1, KFault12CodeLocalname, NsUri() ); )
            if ( !err && list1.HasNext() )
                {
                TXmlEngElement child = list1.Next();
                RXmlEngNodeList<TXmlEngElement> list2;
                TRAP( err, child.GetElementsByTagNameL( list2, KFault12ValueLocalname, NsUri()); )
                if ( !err && list2.HasNext() )
                    {
                    TXmlEngElement value = list2.Next();
                    list2.Close();
                    list1.Close();
                    return value.Text();
                    }
                list2.Close();
    			}
    	    list1.Close();
    		}
        }		
        
    return KNullDesC8();

	}

EXPORT_C TPtrC8 CSenSoapFault2::FaultSubcode()
	{
	TInt err( KErrNone );
	TXmlEngElement element;
	TXmlEngElement* pElement = NULL;
	TRAP( 
	    err, 
	    element = AsElementL();
	    pElement = &(element);
	    )
	if( !err && pElement)
	    {
    	TXmlEngNamespace ns = pElement->NamespaceDeclaration();
        if ( ns.Uri() == KSenSoap12EnvelopeXmlns)
            {
            RXmlEngNodeList<TXmlEngElement> list1;
            TRAP( err, pElement->GetElementsByTagNameL( list1, KFault12CodeLocalname, NsUri()); )
            if ( !err && list1.HasNext() )
                {
                TXmlEngElement child = list1.Next();
                RXmlEngNodeList<TXmlEngElement> list2;
                TRAP( err, child.GetElementsByTagNameL(list2, KFault12SubcodeLocalname, NsUri()); )
                if ( !err && list2.HasNext() )
                    {
                    TXmlEngElement subcode = list2.Next();
                    RXmlEngNodeList<TXmlEngElement> list3;
                    TRAP( err, subcode.GetElementsByTagNameL(list3, KFault12ValueLocalname, NsUri()); )
                    if ( !err && list3.HasNext() )
                        {
                        TXmlEngElement value = list3.Next();
                        list3.Close();
                        list2.Close();
                        list1.Close();
                        return value.Text();
                        }
                    list3.Close();
                    }
                list2.Close();
                }
            list1.Close();
            }
        }   
    // else { // AsElementL() -method leaved(!) }
    return KNullDesC8();
	}


EXPORT_C TPtrC8 CSenSoapFault2::FaultString()
	{
	TInt err( KErrNone );
	TXmlEngElement element;
	TXmlEngElement* pElement = NULL;
	TRAP( 
	    err, 
	    element = AsElementL(); 
	    pElement = &(element); 
	    )
	if( !err && pElement ) 
	    {
    	TXmlEngNamespace ns = pElement->NamespaceDeclaration();
        if ( ns.Uri() != KSenSoap12EnvelopeXmlns)
            {
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL(list, KFaultStringLocalname); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement child = list.Next();
                list.Close();
                return child.Text();
                }
            list.Close();
            }
        else
            {
            RXmlEngNodeList<TXmlEngElement> list1;
            TRAP( err, pElement->GetElementsByTagNameL( list1, KFault12ReasonLocalname, NsUri()); )
            if ( !err && list1.HasNext() )
                {
                TXmlEngElement child = list1.Next();
                RXmlEngNodeList<TXmlEngElement> list2;
                TRAP( err, child.GetElementsByTagNameL( list2, KFault12TextLocalname, NsUri()); )
                if ( !err && list2.HasNext() )
                    {
                    TXmlEngElement value = list2.Next();
                    list2.Close();
                    list1.Close();
                    return value.Text();
                    }
                list2.Close();
    			}
            list1.Close();
            }
        }  
    // else { // AsElementL -method leaved(!) }      
	return KNullDesC8();
	}

EXPORT_C TPtrC8 CSenSoapFault2::FaultActor()
	{
	TInt err( KErrNone );
	TXmlEngElement element;
	TXmlEngElement* pElement = NULL;
	TRAP( 
	    err, 
	    element = AsElementL(); 
	    pElement = &(element);
	    )
    if( !err && pElement ) 
        {
    	TXmlEngNamespace ns = pElement->NamespaceDeclaration();
        if ( ns.Uri() != KSenSoap12EnvelopeXmlns)
            {
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL(list, KFaultActorLocalname); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement child = list.Next();
                list.Close();
                return child.Text();
                }
            list.Close();
            }
        else
            {
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL(list, KFault12NodeLocalname, NsUri()); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement value = list.Next();
                list.Close();
                return value.Text();
                }
            list.Close();
            }
        }
    // else { // AsElementL -method leaved(!) }            
	return KNullDesC8();
	}

EXPORT_C TPtrC8 CSenSoapFault2::Detail()
	{
    TInt err( KErrNone );
    TXmlEngElement element;
    TXmlEngElement* pElement = NULL;
	TRAP( 
	    err, 
	    element = AsElementL(); 
	    pElement = &(element);
	    )
	if( !err && pElement )
	    {
    	TXmlEngNamespace ns = pElement->NamespaceDeclaration();
        if ( ns.Uri() != KSenSoap12EnvelopeXmlns)
            {
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL(list, KDetailLocalname); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement child = list.Next();
                list.Close();
                return child.Text();
                }
            list.Close();
            }
        else
            {
            RXmlEngNodeList<TXmlEngElement> list;
            TRAP( err, pElement->GetElementsByTagNameL( list, KFault12DetailLocalname, NsUri() ); )
            if ( !err && list.HasNext() )
                {
                TXmlEngElement child = list.Next();
                list.Close();
                return child.Text();
                }
            list.Close();
            }
        }  
    // else { // AsElementL -method leaved(!) }                          
	return KNullDesC8();
	}

// End of File
