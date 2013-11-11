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
#include "TestFragment.h"


//CSenFragmentBase//

CTestFragmentBase* CTestFragmentBase::NewL(const TDesC8& aLocalName)
{	
    return((CTestFragmentBase*)CSenFragmentBase::NewL(aLocalName));
}

EXPORT_C void CTestFragmentBase::SetContentL(const TDesC8& aContent)
{
	return(CSenFragmentBase::SetContentL(aContent));
}

EXPORT_C void CTestFragmentBase::AddContentL(const TDesC8& aContent)
{
	return(CSenFragmentBase::AddContentL(aContent));
}


///CSenDomFragmentBase///

CTestDomFragmentBase* CTestDomFragmentBase::NewL(const TDesC8& aLocalName)
{
    return((CTestDomFragmentBase*)CSenDomFragmentBase::NewL(aLocalName));
}

EXPORT_C TXmlEngElement CTestDomFragmentBase::SetContentOfL(const TDesC8& aLocalName,
                                        const TDesC8& aContent)
{
	return(CSenDomFragmentBase::SetContentOfL(aLocalName, aContent));
}


EXPORT_C TPtrC8 CTestDomFragmentBase::ContentOf(const TDesC8& aLocalName)
{
	return(CSenDomFragmentBase::ContentOf(aLocalName));
}



