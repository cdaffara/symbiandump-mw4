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






 

#ifndef WSSTAR_POLICY_INTERSECTER_H
#define WSSTAR_POLICY_INTERSECTER_H

#include <SenDomFragmentBase.h>
#include <RSenDocument.h>
#include <XmlEngDDocument.h>


class CSenElement;
class CWsSecurityPolicy;

/**
 * Class:   CPolicyIntersecter
 * Description: This class can intersect two polcicies provided as fileNames or TElements
 * The Intersection take care of domain specific knowledge of the assertions and can resolve it. 
 * currently only Ws-SecurityPolicy domain is supported
 */

class CPolicyIntersecter : public CBase

{
  
  public:

  static CPolicyIntersecter* NewL();
  static CPolicyIntersecter* NewLC();
    
  virtual ~CPolicyIntersecter();

  TXmlEngElement IntersectL(TDesC& aFirstPolicy, TDesC& aSecondPolicy); //filenames with absolute path
  TXmlEngElement IntersectL(TXmlEngElement aFirstPolicy, TXmlEngElement aSecondPolicy); //root elements
private:

  void ConstructL();

  CPolicyIntersecter();
  
  void IntersectAllANDAssertionsL(TXmlEngElement aFirstElement, 
                    TXmlEngElement aSecondElement, RArray<TXmlEngElement>& aResult);
  TXmlEngElement IntersectCompareSecondAssertionsL(TXmlEngElement aFirstElement, 
                    TXmlEngElement aSecondElement, TXmlEngElement aIntersectedAND);
  
  TInt VerifyNamespace(TXmlEngElement aFirstElement, TXmlEngElement aSecondElement);
  
private:    
   CWsSecurityPolicy* iSecurityPolicyDomain;
   
   TXmlEngElement   irootAND;
   
   TXmlEngElement  iIntersectedPolicyElement;
   RXmlEngDocument iPolicy1; 
   RXmlEngDocument iPolicy2;
   
//   TString XORString;
//   TString ANDString;
//   TString policyNsUri;
};

#endif //WSSTAR_POLICY_INTERSECTER_H