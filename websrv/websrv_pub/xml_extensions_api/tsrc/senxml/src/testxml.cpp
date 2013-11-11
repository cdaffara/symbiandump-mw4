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








#include <e32std.h>
#include <f32file.h>
#include "Testxml.h"

CTestBaseFragment* CTestBaseFragment::NewL(const TDesC8& aLocalName)
{
	return((CTestBaseFragment*)CSenBaseFragment::NewL(aLocalName));
}

CTestBaseFragment::~CTestBaseFragment()
{
	
}

MSenElement& CTestBaseFragment::SetContentOfL(const TDesC8& aLocalName,const TDesC8& aContent)
{
	return(CSenBaseFragment::SetContentOfL(aLocalName,aContent));
}

TPtrC8 CTestBaseFragment::ContentOf(const TDesC8& aLocalName)
{
	return(CSenBaseFragment::ContentOf(aLocalName));
}

void CTestBaseFragment::BaseConstructL(CSenXmlReader& aReader)
{
	return(CSenBaseFragment::BaseConstructL(aReader));
}


CTestDomFragment* CTestDomFragment::NewL(const TDesC8& aLocalName)
{
	return((CTestDomFragment*)CSenDomFragment::NewL(aLocalName));
}


void CTestDomFragment::BaseConstructL(CSenXmlReader& aReader)
{
	return(CSenDomFragment::BaseConstructL(aReader));
}


CTestBaseElement* CTestBaseElement::NewL(const TDesC8& aLocalName)
{
	return((CTestBaseElement*)CSenBaseElement::NewL(aLocalName));
}


const TDesC8& CTestBaseElement::AddAttributeL(const TDesC8& aAttrName, const TDesC8& aValue)
{
	return(CSenBaseElement::AddAttributeL(aAttrName, aValue));
}


CTestBaseElement::~CTestBaseElement()
{
	
}

CTestXmlElement* CTestXmlElement::NewL(const TDesC8& aLocalName)
{
	return((CTestXmlElement*)CSenXmlElement::NewL(aLocalName));
}


const TDesC8& CTestXmlElement::AddAttributeL(const TDesC8& aAttrName, const TDesC8& aValue)
{
	return(CSenXmlElement::AddAttributeL(aAttrName, aValue));
}


CTestXmlElement::~CTestXmlElement()
{
	
}
