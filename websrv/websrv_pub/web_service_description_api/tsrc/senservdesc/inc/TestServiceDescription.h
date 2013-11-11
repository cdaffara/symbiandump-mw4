



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

#ifndef TESTSERVICEDESCRIPTION_H
#define TESTSERVICEDESCRIPTION_H

#include <e32std.h>
#include <f32file.h>
#include "SenXmlServiceDescription.h"
#include "SenCredential.h"
#include "SenServicePattern.h"


class CTestServiceDescription :public CSenXmlServiceDescription
{
public:    
    static CTestServiceDescription* NewL();
    static CTestServiceDescription* NewLC();
    IMPORT_C void StartElementL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);
                                            
	IMPORT_C void EndElementL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                      		const TDesC8& aQName);                                            


};



class CTestCredential :public CSenCredential
{
public:    
    static CTestCredential* NewL();
    static CTestCredential* NewLC();
    IMPORT_C  void StartElementL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);

    


};


#endif