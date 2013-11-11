/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/






 

#ifndef WSSTAR_WSDL_POLICY_ATTACHMENT_H
#define WSSTAR_WSDL_POLICY_ATTACHMENT_H

#include "SenElement.h"
#include "SenBaseAttribute.h"
#include "senpointermap.h"
#include "SenDomFragment.h"

//class CWsdlService;
//class CWsdlPort;
//class CWsdlBinding;
//class CWsdlOperation;
//class CWsdlInputOperation;
//class CWsdlOutputOperation;
//class CWsaEndpointReference;
class CPolicyRegistry;
class CWSPolicy;
class CPolicyAssertion;
typedef RSenPointerMap<TDesC8, CSenElement> RWsdlPolicyMap;

namespace WsdlPolicyAttachment
{
//from wsdl
    _LIT8(KDefinitions, "definitions");
    _LIT8(KOperation, "operation");
    _LIT8(KMessage, "message");
    _LIT8(KPortType, "portType");
    _LIT8(KPort, "port");
    _LIT8(KBinding, "binding");
    _LIT8(KService, "service");
    _LIT8(KImport, "import");
    _LIT8(KName, "name");
    _LIT8(KElement, "element");
    _LIT8(KType, "type");
    _LIT8(KLocation, "location");
    _LIT8(KInput, "input");
    _LIT8(KOutput, "output");
    _LIT8(KFault, "fault");
    _LIT8(KRequired, "required");
    _LIT8(KTargetNamespace, "targetNamespace");
    _LIT8(KWsdlNsPrefix, "wsdl");
    _LIT8(KWsdlNsUri, "http://schemas.xmlsoap.org/wsdl/");
    
//from policy    
    _LIT8(KWspPolicyReference, "PolicyReference");
    _LIT8(KUri, "URI");
    _LIT8(KId, "Id");
    _LIT8(KWsuId, "wsu:Id");
    _LIT8(KPolicy, "Policy");
    _LIT8(KWspNsUri, "http://schemas.xmlsoap.org/ws/2004/09/policy");
    _LIT8(KWsuNsUri, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");
    _LIT8(KColon, ":");
//from soap    
    _LIT8(KAddress, "address");
    _LIT8(KSoap12NsUri,"http://schemas.xmlsoap.org/wsdl/soap12/");
//  {"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/soap-envelope"},
//  {"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/soap-encoding"},
//  {"xsi", "http://www.w3.org/2001/XMLSchema-instance"},
//  {"xsd", ""}, // http://www.w3.org/2001/XMLSchema"}, // don't use this, it might conflict with xs
//  {"xml", "http://www.w3.org/XML/1998/namespace"},
//  {"xs", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/XMLSchema" },
//  {"http", "http://schemas.xmlsoap.org/wsdl/http/"},
//  {"soap", "http://schemas.xmlsoap.org/wsdl/soap/", "http://schemas.xmlsoap.org/wsdl/soap*/"},
//  {"mime", "http://schemas.xmlsoap.org/wsdl/mime/"},
//  {"dime", "http://schemas.xmlsoap.org/ws/2002/04/dime/wsdl/", "http://schemas.xmlsoap.org/ws/*/dime/wsdl/"},
//  {"wsdl", "http://schemas.xmlsoap.org/wsdl/"},
//  {"gwsdl", "http://www.gridforum.org/namespaces/2003/03/gridWSDLExtensions"},
//  {"sd", "http://www.gridforum.org/namespaces/2003/03/serviceData"},
}

class CWsdlPolicyAttachment :public CSenDomFragment
{
  
  public:
    IMPORT_C static CWsdlPolicyAttachment* NewL(const TDesC8& aWsdlFileName);
    IMPORT_C static CWsdlPolicyAttachment* NewLC(const TDesC8& aWsdlFileName);

    virtual ~CWsdlPolicyAttachment();
//Service policy    
    IMPORT_C CSenElement* GetEffectiveServicePolicyL(const TDesC8& aService); //codescannerwarnings
//Endpoint policy    
    IMPORT_C CSenElement* GetEffectiveEndpointPolicyL(const TDesC8& aService, const TDesC8& aPort); //codescannerwarnings
//Operation Policy    
    IMPORT_C CSenElement* GetEffectiveOperationPolicyL(const TDesC8& aService, const TDesC8& aPortName, const TDesC8& aOpName); //codescannerwarnings
//Message Policies    
    IMPORT_C CSenElement* GetEffectiveInputPolicyL(const TDesC8& aService, const TDesC8& aPortName, const TDesC8& aOpName);  //codescannerwarnings
    IMPORT_C CSenElement* GetEffectiveOutputPolicyL(const TDesC8& aService, const TDesC8& aPortName, const TDesC8& aOpName);  //codescannerwarnings
    IMPORT_C CSenElement* GetEffectiveFaultPolicyL(/*QName aService, */const TDesC8& aPortName, const TDesC8& aOpName);
	

private:

    void ConstructL(const TDesC8& aWsdlFileName);

    CWsdlPolicyAttachment();
    void CreateWsdlPolicyL(const TDesC8& aWsdlContent);  //codescannerwarnings
    TPtrC8 GetAttributeValueL(const TDesC8& aAttribute, CSenElement* aElement);

    CSenElement* GetPolicyL(const TDesC8& aUri); //search the wsdl for policy tag with specific URI    
    
    CSenElement* GetServicePolicyL(const TDesC8& aServiceName);
    CSenElement* GetServiceElementL(const TDesC8& aServiceName);
    
    CSenElement* GetPortPolicyL(const TDesC8& aServiceName, const TDesC8& aPort);
    CSenElement* GetPortElementL(const TDesC8& aPortName, CSenElement* aElement);    
//    CSenElement* GetPortElementL(const TDesC8& aPortName);
    
    CSenElement* GetPortTypePolicyL(const TDesC8& aServiceName, const TDesC8& aPort);
    CSenElement* GetPortTypeElementL(const TDesC8& aPortTypeName);
    
    CSenElement* GetBindingPolicyL(const TDesC8& aServiceName, const TDesC8& aPort);
    CSenElement* GetBindingElementL(const TDesC8& aBindingName);
   
    CSenElement* GetPortTypeOperationPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName);
    CSenElement* GetBindingOperationPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName); 
    CSenElement* GetOperationPolicyL(const TDesC8& aOpName, CSenElement* aElement); 

    CSenElement* GetMessagePolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName, TBool aInput);
    CSenElement* GetMessageElementL(const TDesC8& aMessageName);
    
    CSenElement* GetPortTypeOperationInputOutputPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName, TBool aInput);
    CSenElement* GetBindingOperationInputOutputPolicyL(const TDesC8& aServiceName, const TDesC8& aPort, const TDesC8& aOpName, TBool aInput);
    CSenElement* GetOperationInputOutputPolicyL(const TDesC8& aOpName, CSenElement* aElement, TBool aInput);
    
    CSenElement* GetElementL(const TDesC8& aName, const TDesC8& aType);
    CSenElement* GetElementL(const TDesC8& aName, const TDesC8& aType, CSenElement* aElement);
    CSenElement* GetPolicyReferenceL(CSenElement* aElement);
    CSenElement* GetEffectivePolicyL(const TDesC8& aUri);
    CWSPolicy* GetEffectivePolicyL(RPointerArray<CSenElement>& aPolicyArray, CPolicyRegistry* aReg);
    
    CWSPolicy* GetPolicyToSenL(CPolicyAssertion* aPolicy);
    CPolicyAssertion* GetSenToPolicyL(CSenElement* aPolicy, CPolicyRegistry* aReg);
    
private:
//    CWsdlService* iService;
    CSenElement* iPolicy;
    RWsdlPolicyMap iOriginalPolicies;
    RWsdlPolicyMap iNormalizedPolicies;
    TFileName iWsdlFileName;
    TBool iParsed;
    CPolicyRegistry* iRegistry;
        
};



#endif