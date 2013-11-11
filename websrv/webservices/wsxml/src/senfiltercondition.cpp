/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDES

#include "SenFilterCondition.h"

#include "SenDebug.h"
#include "SenLogger.h"


_LIT(KLogFileDir, "WsLog");
_LIT(KLogFileName, "SenXml.log");

// -------------------------------------------
//
// Implementation of class CSenFilterCondition
//
// -------------------------------------------


EXPORT_C CSenFilterCondition* CSenFilterCondition::NewL(TInt aIndex, 
                                                        const TDesC8& aElement, 
                                                        const TDesC8& aAttribute)
{
    CSenFilterCondition* pNew = new (ELeave) CSenFilterCondition;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aIndex, 
                     aElement, 
                     aAttribute);
    CleanupStack::Pop(); // pNew;
    return pNew;
}


void CSenFilterCondition::ConstructL(TInt aIndex, 
                                     const TDesC8& aElement, 
                                     const TDesC8& aAttribute)
{
    iIndex = aIndex;
    ipElement = aElement.AllocL();
    ipAttribute = aAttribute.AllocL();
}


CSenFilterCondition::CSenFilterCondition()
: ipElement(NULL),
  ipAttribute(NULL)
{
		TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterCondition::CSenFilterCondition(): Log opened"));

    iCurrentIndex = -1;
    iIndex = 0;

		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterCondition::CSenFilterCondition(): End"));
}


CSenFilterCondition::CSenFilterCondition(const CSenFilterCondition& aFilterCondition)
{
    iIndex = aFilterCondition.iIndex;
    iCurrentIndex = aFilterCondition.iCurrentIndex;
    
    if (NULL == aFilterCondition.ipElement) {
        ipElement = NULL;
    } else {
        ipElement = aFilterCondition.ipElement->Alloc();
    }
    
    if (NULL == aFilterCondition.ipAttribute) {
        ipAttribute = NULL;
    } else {
        ipAttribute = aFilterCondition.ipAttribute->Alloc();
    }
}


EXPORT_C CSenFilterCondition::~CSenFilterCondition()
{
    delete ipElement;
    delete ipAttribute;

    // Close the log file and the connection to the server.
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L("Log file closed."));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


EXPORT_C TBool CSenFilterCondition::Test()
{
    TBool test = EFalse;
    if ( ++iCurrentIndex == iIndex )
    {
        test = ETrue;
    }

    return test;
}


RFileLogger* CSenFilterCondition::Log() const
    {
    return (RFileLogger*) &iLog;
    }



