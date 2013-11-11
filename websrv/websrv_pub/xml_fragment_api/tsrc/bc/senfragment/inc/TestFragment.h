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
* Description:  senfragment header declaration      
*
*/









#ifndef TESTFRAGMENT_H
#define TESTFRAGMENT_H

#include <e32std.h>
#include <f32file.h>
#include "SenFragmentBase.h"
#include "SenDomFragmentBase.h"



class CTestFragmentBase :public CSenFragmentBase
{
public:    
    static CTestFragmentBase* NewL(const TDesC8& aLocalName);
    IMPORT_C void SetContentL(const TDesC8& aContent);
	IMPORT_C void AddContentL(const TDesC8& aContent);    
};



class CTestDomFragmentBase :public CSenDomFragmentBase
{
public:    
    static CTestDomFragmentBase* NewL(const TDesC8& aLocalName);
    IMPORT_C TXmlEngElement SetContentOfL(const TDesC8& aLocalName,
                                        const TDesC8& aContent);
	IMPORT_C TPtrC8 ContentOf(const TDesC8& aLocalName);                                        

};


#endif