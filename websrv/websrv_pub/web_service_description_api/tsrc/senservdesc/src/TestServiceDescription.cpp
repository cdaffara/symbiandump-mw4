/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:        ?Description
*
*/

#include <e32std.h>
#include <f32file.h>
#include "TestServiceDescription.h"


//TestServiceDescription//

CTestServiceDescription* CTestServiceDescription::NewL()
{
     return((CTestServiceDescription*)CSenXmlServiceDescription::NewL());
}
CTestServiceDescription* CTestServiceDescription::NewLC()
{
     return((CTestServiceDescription*)CSenXmlServiceDescription::NewLC());
  
}

EXPORT_C void CTestServiceDescription::StartElementL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                       const RAttributeArray& aAttributes)
{
		 
	return(CSenXmlServiceDescription::StartElementL(aNsUri,aLocalName,aQName,aAttributes));
    
}

EXPORT_C void CTestServiceDescription::EndElementL(const TDesC8& aNsUri,
                                                     const TDesC8& aLocalName,
                                                     const TDesC8& aQName )
{
	
	return(CSenXmlServiceDescription::EndElementL(aNsUri,aLocalName,aQName ));
    
}


///Credential///

CTestCredential* CTestCredential::NewL()
{
	_LIT8(KText, "text");
	RAttributeArray array;
    return((CTestCredential*)CSenCredential::NewL(KText, KText, KText, array));
}


CTestCredential* CTestCredential::NewLC()
{
	_LIT8(KText, "text");
	RAttributeArray array;
    return((CTestCredential*)CSenCredential::NewLC(KText, KText, KText, array));
  
}




EXPORT_C void CTestCredential::StartElementL(const TDesC8& aNsUri,
    											const TDesC8& aLocalName,
    											const TDesC8& aQName,
    											const RAttributeArray& aAttributes)
{
	return(CSenCredential::StartElementL(aNsUri,aLocalName,aQName,aAttributes));
}
