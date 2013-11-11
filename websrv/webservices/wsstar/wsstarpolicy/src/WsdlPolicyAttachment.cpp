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




#include "wsdlpolicyattachment.h"
#include "wspolicyreader.h"
#include "wspolicywriter.h"
#include "policyassertion.h"
#include "wspolicy.h"
using namespace WsdlPolicyAttachment;

EXPORT_C CWsdlPolicyAttachment* CWsdlPolicyAttachment::NewL(const TDesC8& aWsdlFileName)
    {
    CWsdlPolicyAttachment* pSelf = CWsdlPolicyAttachment::NewLC(aWsdlFileName);
    CleanupStack::Pop(pSelf);
    return pSelf;  
    }
    
EXPORT_C CWsdlPolicyAttachment* CWsdlPolicyAttachment::NewLC(const TDesC8& aWsdlFileName)
    {
    CWsdlPolicyAttachment* pSelf = new (ELeave) CWsdlPolicyAttachment();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aWsdlFileName);
    return pSelf;  
    }

CWsdlPolicyAttachment::~CWsdlPolicyAttachment()
    {
    iOriginalPolicies.Reset();
    iNormalizedPolicies.Reset();
    if( iRegistry )
        {
        iRegistry->ResetRegistry();
        }
    delete iRegistry;
    }

void CWsdlPolicyAttachment::ConstructL(const TDesC8& aWsdlFileName)
    {
    iRegistry = CPolicyRegistry::NewL();
    CreateWsdlPolicyL( aWsdlFileName );
    }

CWsdlPolicyAttachment::CWsdlPolicyAttachment() :
iOriginalPolicies(ETrue,ETrue),
iNormalizedPolicies(ETrue, ETrue), 
iParsed(EFalse)
    {
    }

void CWsdlPolicyAttachment::CreateWsdlPolicyL( const TDesC8& aWsdlContent )
    {
    TInt size = aWsdlContent.Length();
    if(size > 0)
        {
        iXmlReader = CSenXmlReader::NewL( KXmlParserMimeType ); // use libxml2 sax parser + CodeScannerWarnings     
        TRAPD (err, ParseL( aWsdlContent )); 
        delete iXmlReader;
        iXmlReader = NULL;
        
        if(err == KErrNone)
            {
            iParsed = ETrue;
            iPolicy = &AsElement(); 
            iRegistry->PopulateRegistryL(iPolicy);   
            }
        }   
}

TPtrC8 CWsdlPolicyAttachment::GetAttributeValueL(const TDesC8& aAttribute, CSenElement* aElement)
{
    RPointerArray<CSenBaseAttribute>& attrs = aElement->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(aAttribute) == 0)
            {
             return bs->Value();
            }
         }
    return KNullDesC8();
}

EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveServicePolicyL( const TDesC8& aService )
    {
    //wsdl:service
    CSenElement* pServicePolicy = GetServicePolicyL( aService );
    RPointerArray<CSenElement> array;
    CleanupClosePushL( array );
    if( pServicePolicy )
        {
        array.Append( pServicePolicy );
        }
    CWSPolicy* pol = GetEffectivePolicyL( array, iRegistry );
    CleanupStack::PopAndDestroy( &array ); //array.Close();    
    //CSenElement* policy = NULL;
    //return policy;
    return NULL;
    }


EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveEndpointPolicyL( 
    const TDesC8& aService, 
    const TDesC8& aPort )
{
    //wsdl:port
    //wsdl:portType
    //wsdl:binding
    CSenElement* portPolicy = GetPortPolicyL(aService, aPort);
    RPointerArray<CSenElement> array;
    CleanupClosePushL( array );
    if( portPolicy )
        {
        array.Append(portPolicy);
        }
    CSenElement* portTypePolicy = GetPortTypePolicyL(aService, aPort);
    if(portTypePolicy)
        {
        array.Append(portTypePolicy);    
        }
        
    CSenElement* bindingPolicy = GetBindingPolicyL(aService, aPort);
    if(bindingPolicy)
        {
        array.Append(bindingPolicy);
        }

    // (!) Merge these policies to get effective one:
    CWSPolicy* pol = GetEffectivePolicyL(array, iRegistry); //merge these policies to get effective one
    if( pol )
        {
        // Serialize into file: 
        RFs fss;
        User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);
        
        CSenElement* poli = pol->PolicyL();
        _LIT( KFileNameOut, "c:\\logs\\WsSecurity10Sign.svc.xml");        
        HBufC8* temp = poli->AsXmlL();
        CleanupStack::PushL(temp);
        TFileName iFileName12(KFileNameOut);
        RFile  xmlFilew;
        xmlFilew.Replace(fss , iFileName12, EFileWrite );
        xmlFilew.Write(temp->Des());
        xmlFilew.Close();
        CleanupStack::PopAndDestroy(temp);    
        CleanupStack::PopAndDestroy(&fss);
        }    
    CleanupStack::PopAndDestroy( &array ); // array.Close();
    return NULL;    
    }

EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveOperationPolicyL( const TDesC8& aService, 
                                                                           const TDesC8& aPortName, 
                                                                           const TDesC8& aOpName )
    {
    // wsdl:portType/wsdl:operation
    // wsdl:binding/wsdl:operation
    CSenElement* portTypePolicy = GetPortTypeOperationPolicyL(aService, aPortName, aOpName);
    RPointerArray<CSenElement> array;
    CleanupClosePushL( array );
    if(portTypePolicy)
        {
        array.Append(portTypePolicy);    
        }
    CSenElement* bindingPolicy = GetBindingOperationPolicyL(aService, aPortName, aOpName);
    if(bindingPolicy)
        {
        array.Append(bindingPolicy);    
        }
    // (!) Merge these policies to get effective one:
    GetEffectivePolicyL(array, iRegistry);
    CleanupStack::PopAndDestroy( &array ); // array.Close();
    // CSenElement* policy = NULL;
    // return policy;  
    return NULL;
    }

EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveInputPolicyL(const TDesC8& aService, const TDesC8& aPortName, const TDesC8& aOpName)
    {
    // wsdl:message
    // wsdl:portType/wsdl:operation/wsdl:input
    // wsdl:binding/wsdl:operation/wsdl:input
    CSenElement* messagePolicy = GetMessagePolicyL(aService, aPortName, aOpName, ETrue);
    RPointerArray<CSenElement> array;
    CleanupClosePushL( array );
    if( messagePolicy )
        {
        array.Append(messagePolicy);
        }
    CSenElement* portTypePolicy = GetPortTypeOperationInputOutputPolicyL(aService, aPortName, aOpName, ETrue);
    if( portTypePolicy )
        {
        array.Append(portTypePolicy);        
        }
    CSenElement* bindingPolicy = GetBindingOperationInputOutputPolicyL(aService, aPortName, aOpName, ETrue);
    if( bindingPolicy )
        {
        array.Append(bindingPolicy);    
        }
    // (!) Merge these policies to get effective one:
    GetEffectivePolicyL(array, iRegistry); 
    CleanupStack::PopAndDestroy( &array ); // array.Close();
    //CSenElement* policy = NULL;
    //return policy;  
    return NULL;
    }

EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveOutputPolicyL(const TDesC8& aService, const TDesC8& aPortName, const TDesC8& aOpName)
    {
    // wsdl:message
    // wsdl:portType/wsdl:operation/wsdl:output
    // wsdl:binding/wsdl:operation/wsdl:output
    CSenElement* messagePolicy = GetMessagePolicyL(aService, aPortName, aOpName, EFalse);
    RPointerArray<CSenElement> array;
    if( messagePolicy )
        {
        array.Append(messagePolicy);        
        }
    CSenElement* portTypePolicy = GetPortTypeOperationInputOutputPolicyL(aService, aPortName, aOpName, EFalse);
    if( portTypePolicy )
        {
        array.Append(portTypePolicy);    
        }
    CSenElement* bindingPolicy = GetBindingOperationInputOutputPolicyL(aService, aPortName, aOpName, EFalse);
    if( bindingPolicy )
        {
        array.Append(bindingPolicy);    
        }
    // (!) Merge these policies to get effective one:
    GetEffectivePolicyL( array, iRegistry );
    CleanupStack::PopAndDestroy( &array ); // array.Close();
    // CSenElement* policy = NULL;
    // return policy;  
    return NULL;
}

EXPORT_C CSenElement* CWsdlPolicyAttachment::GetEffectiveFaultPolicyL( /*QName aService, */
                                                                        const TDesC8& /*aPortName*/, 
                                                                        const TDesC8& /*aOpName*/ )
    { 
        
    // wsdl:message
    // wsdl:portType/wsdl:operation/wsdl:fault
    // wsdl:binding/wsdl:operation/wsdl:fault
      return NULL;
    }
    
////////////////////////////////////////////////////////////////////////
//PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
CSenElement* CWsdlPolicyAttachment::GetServicePolicyL( const TDesC8& aServiceName )
    {
    //    RPointerArray<CSenElement>& list = iPolicy->ElementsL(); //get the definitions elements
    //    TInt count = list.Count();
    //    for(TInt i = 0; i< count; i++)
    //    {
    //        CSenElement* definitions = list[i];
    //        TPtrC8 localName = definitions->LocalName();
                
    RPointerArray<CSenElement> serviceList;     // get the wsdl:service
    iPolicy->ElementsL(serviceList, KService); //KWsdlNsUri, KService);
    CleanupClosePushL( serviceList );
    TInt count2 = serviceList.Count();
    for( TInt j = 0; j< count2; j++ )
        {
        CSenElement* service = serviceList[j];
        
        TPtrC8 localServiceName = service->LocalName();

        TPtrC8 serviceName  = GetAttributeValueL(KName, service);
        if(serviceName != KNullDesC8())
            {
            if (serviceName.Compare(aServiceName) == 0)
                { //we have right service tag, search its children for PolicyReference
                    serviceList.Close();
                    return GetPolicyReferenceL(service);
                }
            }
        }
    CleanupStack::PopAndDestroy( &serviceList ); // serviceList.Close();
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetPortPolicyL(const TDesC8& aServiceName, const TDesC8& aPort)
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if (result)
        {
        result = GetPortElementL(aPort, result);
        if (result)
            {
            return GetPolicyReferenceL(result);
            }
        
        }
    return NULL;  
    }

CSenElement* CWsdlPolicyAttachment::GetPortTypePolicyL( const TDesC8& aServiceName, 
                                                        const TDesC8& aPort )
    {
    //from service get port
    //from port get binding
    //from binding get porttype
    CSenElement* result = GetServiceElementL(aServiceName);
    if (result)
        {
        result = GetPortElementL(aPort, result);
        if (result)
            {   
            // get binding from port 
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);

            result = GetBindingElementL(bindingName);
            if(result)    
                {
                //we got the right binding, now get the portType
                TPtrC8 portTypeQName = GetAttributeValueL(KType, result); 
                //actually this is buinding type
                TPtrC8 portTypePrefix = portTypeQName.Left(portTypeQName.Find(KColon));
                TPtrC8 portTypeName = portTypeQName.Right(portTypeQName.Length() - portTypePrefix.Length()-1);
                //here we have port Name
                //now look for this port type in wsdl
                result = GetPortTypeElementL(portTypeName);
                if(result)
                    {
                    return GetPolicyReferenceL(result);
                    }
                else
                    {
                    return NULL;
                    }
                }
            }
        }
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetBindingPolicyL( const TDesC8& aServiceName, 
                                                       const TDesC8& aPort )
    {
    //the aBindingName is obtained from the port operation. 
    //so Use that Name and then jsut search for that particualr binding
    CSenElement* result = GetServiceElementL(aServiceName);
    if (result)
        {
        result = GetPortElementL(aPort, result);
        if (result)
            {   
            // get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);
            result = GetBindingElementL(bindingName);
            if( result )    
                {
                //we got the right binding, now get the policy
                result = GetPolicyReferenceL( result );
                //if ( result ) 
                //    //we got a policy TAG for the binding
                // { 
                return result; 
                }
            }
        }
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetPortTypeOperationPolicyL( const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName )
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if ( result )
        {
        result = GetPortElementL(aPort, result);
        if ( result )
            {
            // get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);
            result = GetBindingElementL(bindingName);
            if( result )
                {
                // we got the right binding, now get the portType
                TPtrC8 portTypeQName = GetAttributeValueL(KType, result); 
                //actually this is buinding type
                TPtrC8 portTypePrefix = portTypeQName.Left(portTypeQName.Find(KColon));
                TPtrC8 portTypeName = portTypeQName.Right(portTypeQName.Length() - portTypePrefix.Length()-1);
                // here we have port Name
                // now look for this port type in wsdl
                result = GetPortTypeElementL( portTypeName );
                if( result ) //we got the portType
                    {
                    // now get the operation in this element
                    result = GetOperationPolicyL( aOpName, result );
                    return result;
                    }
                }
            }
        }
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetBindingOperationPolicyL( const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName )
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if ( result )
        {
        result = GetPortElementL(aPort, result);
        if ( result )
            {   
            //get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);
            result = GetBindingElementL(bindingName);
            if(result)    //we got the right binding, 
                // now get the OperationPolicy from this binding
                {
                result = GetOperationPolicyL(aOpName, result);   
                return result;
                }
            }
        }
    return NULL;
    }
    
CSenElement* CWsdlPolicyAttachment::GetOperationPolicyL( const TDesC8& aOpName, 
                                                         CSenElement* aElement )
    {
    RPointerArray<CSenElement> operationList;        
    aElement->ElementsL(operationList, KWsdlNsUri, KOperation);
    CleanupClosePushL( operationList );
    TInt count = operationList.Count();
    CSenElement* result = NULL;
    for(TInt i = 0; i< count; i++)
        {
        CSenElement* operation = operationList[i];
        TPtrC8 operationName = GetAttributeValueL(KName, operation);
        if(operationName.Compare(aOpName) == 0)
            {
            //we got the rigth operation element
            //now retrieve policy from this element
            result = GetPolicyReferenceL(operation);
            //if( result ) 
            //    {
            //    //we got a policy TAG for the binding
            //    
            //    }
            }
        }
    CleanupStack::PopAndDestroy( &operationList ); // operationList.Close();
    return result;
    }

CSenElement* CWsdlPolicyAttachment::GetMessagePolicyL( const TDesC8& aServiceName, 
                                                       const TDesC8& aPort, 
                                                       const TDesC8& aOpName, TBool aInput )
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if ( result )
        {
        result = GetPortElementL(aPort, result);
        if ( result )
            {   
            //get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);
            result = GetBindingElementL(bindingName);
            if( result )    
                {
                // we got the right binding, now get the Operation from this binding                     
                // if aInput == ETrue then its Input policy else its output policy
                // we get binding now find the input/output for that aOpName
                RPointerArray<CSenElement> operationList;
                result->ElementsL(operationList, KWsdlNsUri, KOperation);
                CleanupClosePushL(operationList);
                TInt count = operationList.Count();
                for(TInt i = 0; i< count; i++)
                    {
                    CSenElement* operation = operationList[i];
                    TPtrC8 operationName = GetAttributeValueL(KName, operation);
                    if(operationName.Compare(aOpName) == 0)
                        {
                        //we got the rigth operation element
                        //now retrieve input/output element
                        RPointerArray<CSenElement> operationInOutList;
                        CleanupClosePushL(operationInOutList);
                        if(aInput)
                            {
                            operation->ElementsL(operationInOutList, KWsdlNsUri, KInput);
                            }
                        else
                            {
                            operation->ElementsL(operationInOutList, KWsdlNsUri, KOutput);
                            }
                        TInt operationListCount = operationInOutList.Count();
                        for( TInt j = 0; j< operationListCount; j++ )
                            {
                            //should be only one, no more then one input. 
                            //if there are more, then weired but the fucntion 
                            //will return on first instance of input/output
                            
                            CSenElement* inputoutput = operationInOutList[j];
                            //get the message attribute of this input/output operation
                            if(inputoutput)
                                {
                                TPtrC8 messageName = GetAttributeValueL(KMessage, inputoutput);
                                if(messageName != KNullDesC8())
                                    { 
                                    
                                    // if Qname then retrieve the name from Qname
                                
                                    CSenElement* result = GetMessageElementL(messageName);
                                    if( result )
                                        {
                                        result = GetPolicyReferenceL(result);
                                        CleanupStack::PopAndDestroy(&operationInOutList);
                                        CleanupStack::PopAndDestroy(&operationList);                    
                                        return result;
                                        }
                                    else
                                        {
                                        CleanupStack::PopAndDestroy(&operationInOutList);
                                        CleanupStack::PopAndDestroy(&operationList);                    
                                        return NULL;
                                        }
                                    }
                                }
                            }
                        CleanupStack::PopAndDestroy(&operationInOutList);
                        }
                    }
                CleanupStack::PopAndDestroy(&operationList);                    
                }
            }
        }
    return NULL;
    }


CSenElement* CWsdlPolicyAttachment::GetPortTypeOperationInputOutputPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName, TBool aInput)
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if (result)
        {
        result = GetPortElementL(aPort, result);
        if (result)
            {
            // get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);
            result = GetBindingElementL( bindingName );
            if( result )
                {
                // we got the right binding, now get the portType
                TPtrC8 portTypeQName = GetAttributeValueL(KType, result); 
                // actually this is buinding type
                TPtrC8 portTypePrefix = portTypeQName.Left(portTypeQName.Find(KColon));
                TPtrC8 portTypeName = portTypeQName.Right(portTypeQName.Length() - portTypePrefix.Length()-1);
                //here we have port Name
                //now look for this port type in wsdl
                result = GetPortTypeElementL(portTypeName);
                if( result ) //we got the portType
                    {
                    // now get the operation input output policy from this element                        
                    return GetOperationInputOutputPolicyL(aOpName, result, aInput);
                    }
                else
                    {
                    return NULL;
                    }
                }
            }
        }
    return NULL;
    }
    
CSenElement* CWsdlPolicyAttachment::GetBindingOperationInputOutputPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName, TBool aInput)
    {
    CSenElement* result = GetServiceElementL(aServiceName);
    if (result)
        {
        result = GetPortElementL(aPort, result);
        if (result)
            {   
            // get binding from port and
            TPtrC8 bindingQName = GetAttributeValueL(KBinding, result);
            
            TPtrC8 bindingPrefix = bindingQName.Left(bindingQName.Find(KColon));
            TPtrC8 bindingName = bindingQName.Right(bindingQName.Length() - bindingPrefix.Length()-1);

            result = GetBindingElementL(bindingName);
            if(result)    //we got the right binding, 
            
                {
                // now get the Operation from this binding                 
                // if aInput == ETrue then its Input policy else its output policy
            
                result = GetOperationInputOutputPolicyL(aOpName, result, aInput);
                if(result)
                    {
                    return result;
                    }
                else
                    {
                    return NULL;
                    }
                }
            }
        }
    return NULL;  
    }

CSenElement* CWsdlPolicyAttachment::GetOperationInputOutputPolicyL(const TDesC8& aOpName, CSenElement* aElement, TBool aInput)
{
    RPointerArray<CSenElement> operationList;
    aElement->ElementsL( operationList, KWsdlNsUri, KOperation );
    CleanupClosePushL( operationList );
    TInt count = operationList.Count();
    for( TInt i = 0; i< count; i++ )
        {
        CSenElement* operation = operationList[i];
        TPtrC8 operationName = GetAttributeValueL(KName, operation);
        if(operationName.Compare(aOpName) == 0)
            {
            // we got the rigth operation element
            // now retrieve input/output element
            RPointerArray<CSenElement> operationInOutList;
            if(aInput)
                operation->ElementsL(operationInOutList, KWsdlNsUri, KInput);
            else
                operation->ElementsL(operationInOutList, KWsdlNsUri, KOutput);
            
            CleanupClosePushL(operationInOutList);
            TInt operationInOutListCount = operationInOutList.Count();
            for(TInt j = 0; j< operationInOutListCount; j++)
                { 
                //should be only one, no more then one input. 
                //if there are then weired but the fucntion will return on first instance of input/output
                CSenElement* inputoutput = operationInOutList[j];
                if(inputoutput)
                    {
                    CleanupStack::PopAndDestroy(&operationInOutList);
                    CleanupStack::PopAndDestroy(&operationList);
                    return GetPolicyReferenceL(inputoutput);
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(&operationInOutList);
                    CleanupStack::PopAndDestroy(&operationList);
                    return NULL;
                    }
                }
            CleanupStack::PopAndDestroy(&operationInOutList);            
            }
        }
    CleanupStack::PopAndDestroy(&operationList);
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetPolicyReferenceL(CSenElement* aElement)
    {
    RPointerArray<CSenElement> policyRef; //find the policyReference from input child aElement
    aElement->ElementsL(policyRef, KWspNsUri, KWspPolicyReference);
    //now we have PolicyReference tag in our PolicyRef List
    //get its URI and get teh hell out of here
    CleanupClosePushL(policyRef);
    //there shud be only ONE count here otherwise seems to me a problem
    TInt count = policyRef.Count();
    for(TInt i = 0; i< count; i++)
        {
        CSenElement* policyReference = policyRef[i];
        TPtrC8 policyUri  = GetAttributeValueL(KUri, policyReference);
        //may be only uri is enough at the moment.
        CSenElement* resultPolicy = GetPolicyL(policyUri);
        if(resultPolicy) //it should exist
            {
            CleanupStack::PopAndDestroy(&policyRef);
            return resultPolicy;
            }
        }
    CleanupStack::PopAndDestroy(&policyRef);    
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetPolicyL(const TDesC8& aUri)
    {
    //only take an input URI and return corresponding policy Element from wsdl file
    if(aUri.Length() < 1)
        return NULL;
   
    TPtrC8 reqPolicy = aUri.Right(aUri.Length()-1);
            
    RPointerArray<CSenElement> policyList; //get the wsp:policy elements
    iPolicy->ElementsL(policyList, KWspNsUri, KPolicy);
    CleanupClosePushL(policyList);
    TInt policyListcount = policyList.Count();
    
    CSenElement* requiredPolicy;
    for (TInt j = 0; j< policyListcount; j++)
        {
        requiredPolicy = policyList[j];
        if(requiredPolicy)
            {
            TPtrC8 policyId  = GetAttributeValueL(KWsuId, requiredPolicy);
            if(policyId.Compare(reqPolicy) == 0)
                {
                CleanupStack::PopAndDestroy(&policyList);                
                return requiredPolicy;
                }
            }
        }
    CleanupStack::PopAndDestroy(&policyList);        
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetServiceElementL(const TDesC8& aServiceName)
    {
    return GetElementL(aServiceName, KService);
    }

CSenElement* CWsdlPolicyAttachment::GetPortElementL(const TDesC8& aPortName, CSenElement* aElement)
    {
    return GetElementL(aPortName, KPort, aElement);
    }

CSenElement* CWsdlPolicyAttachment::GetPortTypeElementL(const TDesC8& aPortTypeName)
    {
    return GetElementL(aPortTypeName, KPortType);
    }

CSenElement* CWsdlPolicyAttachment::GetBindingElementL(const TDesC8& aBindingName)
    {
    return GetElementL(aBindingName, KBinding); 
    }

CSenElement* CWsdlPolicyAttachment::GetMessageElementL(const TDesC8& aMessageName)
    {
    return GetElementL(aMessageName, KMessage); 
    }

CSenElement* CWsdlPolicyAttachment::GetElementL(const TDesC8& aName, const TDesC8& aType)
    {
    RPointerArray<CSenElement> list; 
    //get the wsdl:definitions child elements with aType name
    iPolicy->ElementsL(list, KWsdlNsUri, aType);
    CleanupClosePushL(list);
    TInt count = list.Count();
    CSenElement* ele = NULL;
    for (TInt j = 0; j< count; j++)
        {
        ele = list[j];
        TPtrC8 localName = ele->LocalName();
        TPtrC8 name  = GetAttributeValueL(KName, ele);
        if(name.Compare(aName) == 0) //We got the right name 
            {
            CleanupStack::PopAndDestroy(&list);
            return ele;
            }
        ele = NULL;            
        }
    CleanupStack::PopAndDestroy(&list);        
    return NULL;
    }

CSenElement* CWsdlPolicyAttachment::GetElementL(const TDesC8& aName, const TDesC8& aType, CSenElement* aElement)
    {
    RPointerArray<CSenElement> list; //get the wsdl:definitions child elements with aType name
    aElement->ElementsL(list, KWsdlNsUri, aType);
    CleanupClosePushL(list);
    
    TInt count = list.Count();
    CSenElement* ele = NULL;
    for (TInt j = 0; j< count; j++)
        {
        ele = list[j];
        TPtrC8 localName = ele->LocalName();
        
        TPtrC8 name  = GetAttributeValueL(KName, ele);
        if(name.Compare(aName) == 0) //We got the right name 
            {
            CleanupStack::PopAndDestroy(&list);
            return ele;
            }
        ele = NULL;            
        }
    CleanupStack::PopAndDestroy(&list);        
    return NULL;
    }

CWSPolicy* CWsdlPolicyAttachment::GetPolicyToSenL(CPolicyAssertion* aPolicy)
    {
    _LIT8(KN, "name");
    TPtrC8 nimi(KN);
    CWSPolicyWriter* iWriter = CWSPolicyWriter::NewLC();
    CWSPolicy* policy = NULL;
    if(aPolicy)
        {
        CBufFlat *pBuf = CBufFlat::NewL(128);
        CleanupStack::PushL(pBuf);
        TPtrC8 p = iWriter->WriteToBufL(*pBuf, aPolicy);
        policy = CWSPolicy::NewL(nimi, p);                        
        if(policy)
            policy->NormalizePolicyL(NULL);
        
        CleanupStack::PopAndDestroy(pBuf);
        }
    CleanupStack::PopAndDestroy(iWriter);	
    return policy;
    }

CPolicyAssertion* CWsdlPolicyAttachment::GetSenToPolicyL(CSenElement* aPolicy, CPolicyRegistry* aReg)
    {
    CWSPolicyReader* iReader = CWSPolicyReader::NewLC();    
    CPolicyAssertion* policyTerm = iReader->GetPolicyL(aPolicy);
    CPolicyAssertion* policy = NULL;
    if(policyTerm)
        {
        CleanupStack::PushL(policyTerm);
        policy = (CPolicyAssertion*)policyTerm->NormalizeL(aReg);
        CleanupStack::PopAndDestroy(policyTerm);    
        }    
    CleanupStack::PopAndDestroy(iReader);  
    return policy;
    }

CWSPolicy* CWsdlPolicyAttachment::GetEffectivePolicyL(RPointerArray<CSenElement>& aPolicyArray, CPolicyRegistry* aReg)
    {
    TInt count = aPolicyArray.Count(); 
    

	CPolicyAssertion* policyTerm = NULL;
	CPolicyAssertion* policyTerm2 = NULL;
    CSenElement* policy = NULL;
    
    for(TInt i = 0; i< count; i++)
        {
        policy = aPolicyArray[i];
        if(policy && policy->LocalName().Compare(WSPolicy::KWsPolicy) == 0)
            {
            policyTerm2 = GetSenToPolicyL(policy, aReg);
            
			if(policyTerm == NULL)
			    {
			    policyTerm = policyTerm2; 
			    }
			else
			    {
			    CleanupStack::PushL(policyTerm);
			    CleanupStack::PushL(policyTerm2);
			    CPolicyAssertion* temp = (CPolicyAssertion*)policyTerm->MergeL(policyTerm2, NULL);
			    CleanupStack::PopAndDestroy(policyTerm2);
			    CleanupStack::PopAndDestroy(policyTerm);
			    policyTerm = temp;
			    }
            }
        }

	if (policyTerm && !policyTerm->IsNormalized()) 
    	{
	    CleanupStack::PushL(policyTerm);
	    CPolicyAssertion* temp = (CPolicyAssertion*) policyTerm->NormalizeL();
	    CleanupStack::PopAndDestroy(policyTerm);
	    policyTerm = temp;        
	    }
    return GetPolicyToSenL(policyTerm);    
    }
    
CSenElement* CWsdlPolicyAttachment::GetEffectivePolicyL(const TDesC8& /*aUri*/)
    {
    // will be a merged from all policies
    // input should be array of URIs or array of CSenElement*::PolicyElements 
    return NULL;
    }

// END OF FILE


