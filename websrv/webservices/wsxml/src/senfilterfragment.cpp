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








#include "SenXmlUtils.h"

#include "SenFilterFragment.h"

#include "SenDebug.h"
#include "SenLogger.h"


_LIT(KLogFileDir, "WsLog");
_LIT(KLogFileName, "SenXml.log");

// ------------------------------------------
//
// Implementation of class CSenFilterFragment
//
// ------------------------------------------


EXPORT_C CSenFilterFragment::CSenFilterFragment()
: ipCollector(NULL),
  //iRoot(NULL),
  ipFsm(new CSenFilterStateMachine()),
  ipPropertyName(NULL),
  ipPropertyValue(NULL),
  ipNewP(NULL),
  ipPutP(NULL)
{
		TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterFragment::CSenFilterFragment(): Log opened"));

    //ipPropertyValue = NULL;

		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterFragment::CSenFilterFragment(): End"));
}


EXPORT_C CSenFilterFragment::~CSenFilterFragment()
{
    delete ipFsm;
    delete ipPropertyName;
    delete ipPropertyValue;
    delete ipNewP;
    delete ipPutP;

    // Close the log file and the connection to the server.
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L("Log file closed."));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


EXPORT_C void CSenFilterFragment::SetCollector(MSenFilterCollector& aCollector)
{
    ipCollector = &aCollector;

    if ( ipNewP != NULL ) delete ipNewP;
    ipNewP = new CSenFilterNewProperty(aCollector);

    if ( ipPutP != NULL ) delete ipPutP;
    ipPutP = new CSenFilterPutProperty(aCollector);
}


EXPORT_C void CSenFilterFragment::StartElementL(const TDesC8& aNamespaceURI, 
                                                const TDesC8& aLocalName, 
                                                const TDesC8& aQName,
                                                const RAttributeArray& aAttributes)
{
    ipFsm->StartElementL(aLocalName, aAttributes);
}


EXPORT_C void CSenFilterFragment::CharactersL(const TDesC8& aCh, 
                                              TInt aStart, 
                                              TInt aLength)
{
    HBufC8* buf;
    TInt bufLen;

    if ( NULL == ipPropertyValue )
    {
        bufLen = (KBufferAllocationBlocks > aLength) ? KBufferAllocationBlocks : aLength;
        buf = HBufC8::NewLC(bufLen);
    } else {

        bufLen = ipPropertyValue->Length() + ((KBufferAllocationBlocks > aLength) ? KBufferAllocationBlocks : aLength);           
        buf = ipPropertyValue->ReAllocL(bufLen);
        CleanupStack::PushL(buf); 
        //User::LeaveIfError(buf->Append(ipPropertyValue->Ptr(), ipPropertyValue->Length()));
        //CleanupStack::Pop(buf);      
        //delete ipPropertyValue;
    }

    ipPropertyValue = buf;

    CleanupStack::Pop(); // buf

    TPtr8 bufPtr = (ipPropertyValue->Des());
    bufPtr.Append(aCh.Mid(aStart, aLength));

}


EXPORT_C void CSenFilterFragment::EndElementL(const TDesC8& aNamespaceURI, 
                                              const TDesC8& aLocalName, 
                                              const TDesC8& aQName)
{
    ipFsm->EndElementL(aLocalName);
}


EXPORT_C TInt CSenFilterFragment::BuildFrom(const TDesC8& aString)
{
    ipCollector->Initialize();
    return CSenBaseFragment::BuildFrom(aString);
}


EXPORT_C CSenFilterStateNode& 
CSenFilterFragment::AddFilterLC(CSenFilterStateNode& aNode, 
                               const TDesC8& aLocalName)
{
    iRoot = ipFsm->AddChildLC(aNode, 
                            aLocalName,
                            *CSenFilterCondition::NewL(0, KNullDesC8(), KNullDesC8()), 
                            (MSenFilterAction &)ipNewP,
                            (MSenFilterAction &)ipPutP);
    return *iRoot;
}


EXPORT_C CSenFilterStateNode& 
CSenFilterFragment::AddRootFilterLC(const TDesC8& aLocalName)
{
    iRoot = ipFsm->AddParentLC(aLocalName,
                            *CSenFilterCondition::NewL(0, KNullDesC8(), KNullDesC8()), 
                            (MSenFilterAction &)ipNewP,
                            (MSenFilterAction &)ipPutP);
    return *iRoot;
}


EXPORT_C void CSenFilterFragment::CompileL()
{
    ipFsm->CompileL(*iRoot);
}


RFileLogger* CSenFilterFragment::Log() const
    {
    return (RFileLogger*) &iLog;
    }


// ---------------------------------------------
//
// Implementation of class CSenFilterNewProperty
//
// ---------------------------------------------

EXPORT_C CSenFilterNewProperty::CSenFilterNewProperty(MSenFilterCollector& aCollector)
:ipCol(&aCollector)
{
		TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterNewProperty::CSenFilterNewProperty(): Log opened"));

		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterNewProperty::CSenFilterNewProperty(): End"));
}


EXPORT_C CSenFilterNewProperty::~CSenFilterNewProperty()
{

    // Close the log file and the connection to the server.
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , (_L("Log file closed.")));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


EXPORT_C void CSenFilterNewProperty::PerformL(const TDesC8& aEvent, 
                                              const RAttributeArray& aAttributes)
{
    //ipPropertyValue = new StringBuffer();
    //System.out.println("Selected new property: " + aEvent);
    ipCol->PutElementL(aEvent, aAttributes);
}


RFileLogger* CSenFilterNewProperty::Log() const
    {
    return (RFileLogger*) &iLog;
    }


// ---------------------------------------------
//
// Implementation of class CSenFilterPutProperty
//
// ---------------------------------------------

CSenFilterPutProperty::CSenFilterPutProperty(MSenFilterCollector& aCollector)
: ipCol(&aCollector),
  ipPropertyValue(NULL)
{
		TLSLOG_OPEN(KSenXmlLogChannel, KSenXmlLogLevel, KSenXml, KSenXmlLog);
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterPutProperty::CSenFilterPutProperty(): Log opened"));

		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , _L8("CSenFilterPutProperty::CSenFilterPutProperty(): End"));
}


CSenFilterPutProperty::~CSenFilterPutProperty()
{
    delete ipPropertyValue;

    // Close the log file and the connection to the server.
		TLSLOG(KSenXmlLogChannel  , KSenXmlLogLevel , (_L("Log file closed.")));
		TLSLOG_CLOSE(KSenXmlLogChannel);
}


void CSenFilterPutProperty::SetPropertyValueL(const TDesC8& aPropertyValue)
{
    ipPropertyValue = aPropertyValue.AllocL();
} 


void CSenFilterPutProperty::PerformL(const TDesC8& aEvent, 
                                     const RAttributeArray& aAttributes)
{
    //System.out.println("Putting property: " + aEvent);
    //TPtrC8 valueString = NULL;
    //        if ( ipVal->Length() > 0 )
    //{
    //    valueString = ipVal->Des();
    //  System.out.println("  Value is: " +  valueString);
    //}

    ipCol->PutL(aEvent, *ipPropertyValue);

    if ( ipPropertyValue != NULL )
    {
        delete ipPropertyValue;
        ipPropertyValue = NULL;
    }
} 


RFileLogger* CSenFilterPutProperty::Log() const
    {
    return (RFileLogger*) &iLog;
    }



