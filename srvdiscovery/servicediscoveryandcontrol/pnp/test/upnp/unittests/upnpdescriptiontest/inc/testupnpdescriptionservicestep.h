// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains declaration of CTestXmlServiceTestStep class
// @internalAll
// 
//

#ifndef __TEST_HTTP_BUFFER_SIZE_STEP_H__
#define __TEST_HTTP_BUFFER_SIZE_STEP_H__

// User Include
#include "testupnpdescriptionserver.h"

// Literal constant defined for identifying step name
_LIT(KTestXmlServiceStep, "TestXmlServiceStep");

/**
This is the base test step from which all other test steps are derived
to test the HTTP Library.
@internalTechnology
@test
*/
class CTestXmlServiceTestStep : public CTestStep
	{
public:
    CTestXmlServiceTestStep();
	virtual ~CTestXmlServiceTestStep();

private:

public:
	// TEF virtuals
	virtual TVerdict doTestStepPreambleL();
	// TEF pure virtual
	virtual TVerdict doTestStepL();

	};

#endif		// __TEST_HTTP_BUFFER_SIZE_STEP_H__
