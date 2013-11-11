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
* Description: Header declaration
*
*/







#ifndef TESTXML_H
#define TESTXML_H

#include <e32std.h>
#include <f32file.h>
#include "SenBaseFragment.h"
#include "SenDomFragment.h"
#include "SenBaseElement.h"
#include "SenXmlElement.h"


class CTestBaseFragment:public CSenBaseFragment
{
public:    
    static CTestBaseFragment* NewL(const TDesC8& aLocalName);
    ~CTestBaseFragment();
    
	MSenElement& SetContentOfL(const TDesC8& aLocalName,const TDesC8& aContent);
	TPtrC8 ContentOf(const TDesC8& aLocalName);        
   void BaseConstructL(CSenXmlReader& aReader);

private:

    CTestBaseFragment();
    
};

class CTestDomFragment:public CSenDomFragment
{
public:    
   static CTestDomFragment* NewL(const TDesC8& aLocalName);
   void BaseConstructL(CSenXmlReader& aReader);
    ~CTestDomFragment();
    	

private:

    CTestDomFragment();
    
};

class CTestBaseElement:public CSenBaseElement
{
public:    
   static CTestBaseElement* NewL(const TDesC8& aLocalName);
   const TDesC8& AddAttributeL(const TDesC8& aAttrName, const TDesC8& aValue);
    ~CTestBaseElement();
    	

private:

    CTestBaseElement();
    
};

class CTestXmlElement:public CSenXmlElement
{
public:    
   static CTestXmlElement* NewL(const TDesC8& aLocalName);
   const TDesC8& AddAttributeL(const TDesC8& aAttrName, const TDesC8& aValue);
    ~CTestXmlElement();
    	

private:

    CTestXmlElement();
    
};
#endif