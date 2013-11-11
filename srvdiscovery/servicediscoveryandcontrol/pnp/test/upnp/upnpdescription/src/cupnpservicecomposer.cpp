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
//

#include "cupnpservicecomposer.h"
#include "upnpdescriptionschema.h"
#include "inetprottextutils.h"
#include "tattributeiter.h"
#include "cattribute.h"

_LIT8(KBeginOpen,"\n<");
_LIT8(KBeginClose,"</");
_LIT8(KEnd,">");

_LIT8(KStringXmlStart,"<?xml version=\"1.0\" encoding=\"utf-8\"?>");

_LIT8(KStringScpdStart,"\n<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");

_LIT8(KStringStVarStart,"\n\t\t<stateVariable sendEvents=\"");
_LIT8(KStringStVarStart1,"\">");
#define COMPOSE_BUFLENGTH 1024

void CUPnPServiceXmlComposer::AppendLiteralL(TInt aTag, RBuf8& aBuf, TInt& count, TBool aIsStart )
	{
	
    RString  str = iStringPool.String(aTag,UPNPDESCRIPTIONXMLTAGS::Table );
    TInt litSize = str.DesC().Size();
    
    if(aIsStart)
    	{
    	litSize+= KBeginOpen.iTypeLength + KEnd.iTypeLength;
    	}
    else
    	{
    	litSize+= KBeginClose.iTypeLength + KEnd.iTypeLength;
    	}
	
	if((count+litSize)>= aBuf.MaxSize()) 
		{ 
		aBuf.ReAllocL(((aBuf.MaxSize()+COMPOSE_BUFLENGTH)<(aBuf.MaxSize()+litSize))?(aBuf.MaxSize()+litSize):(aBuf.MaxSize()+COMPOSE_BUFLENGTH));
		}

		if(aIsStart)
			{
			aBuf.Append(KBeginOpen);
			}
		else
			{
			aBuf.Append(KBeginClose);
			}
		aBuf.Append(str.DesC()); 
		aBuf.Append(KEnd);
		count+=litSize;
	}

void CUPnPServiceXmlComposer::AppendLiteralL(const RString& aString, RBuf8& aBuf, TInt& count, TBool aIsStart )
	{
	TInt litSize = aString.DesC().Size();
    if(aIsStart)
    	{
    	litSize+= KBeginOpen.iTypeLength + KEnd.iTypeLength;
    	}
    else
    	{
    	litSize+= KBeginClose.iTypeLength + KEnd.iTypeLength;
    	}
	
	if((count+litSize)>= aBuf.MaxSize()) 
		{ 
		aBuf.ReAllocL(((aBuf.MaxSize()+COMPOSE_BUFLENGTH)<(aBuf.MaxSize()+litSize))?(aBuf.MaxSize()+litSize):(aBuf.MaxSize()+COMPOSE_BUFLENGTH));
		}

		if(aIsStart)
			{
			aBuf.Append(KBeginOpen);
			}
		else
			{
			aBuf.Append(KBeginClose);
			}
		aBuf.Append(aString.DesC()); 
		aBuf.Append(KEnd);
		count+=litSize;
	}
void CUPnPServiceXmlComposer::AppendLiteralL(const TDesC8& aLiteral, RBuf8& aBuf, TInt& count)
	{
    TInt litSize = aLiteral.Length();
    
	if((count+litSize)>= aBuf.MaxSize()) 
		{ 
		aBuf.ReAllocL(((aBuf.MaxSize()+COMPOSE_BUFLENGTH)<(aBuf.MaxSize()+litSize))?(aBuf.MaxSize()+litSize):(aBuf.MaxSize()+COMPOSE_BUFLENGTH));
		}

		aBuf.Append(aLiteral); 
		count+=litSize;
	}
/**
Allocates and constructs a CUPnPServiceXmlComposer object.
Initialises all member data to their default values.
*/
CUPnPServiceXmlComposer* CUPnPServiceXmlComposer::NewL(  const RStringPool& aStringPool )
    {
    CUPnPServiceXmlComposer* self = new (ELeave) CUPnPServiceXmlComposer( aStringPool );
     return self;
    }

CUPnPServiceXmlComposer::CUPnPServiceXmlComposer(  const RStringPool& aStringPool ):iStringPool ( aStringPool )
    {
	iError = KErrNone;
    }
/**
Destructor
*/
CUPnPServiceXmlComposer::~CUPnPServiceXmlComposer()
    {
    }


const TStringTable& CUPnPServiceXmlComposer::GetTable()
	{
	return UPNPDESCRIPTIONXMLTAGS::Table;
	}

void CUPnPServiceXmlComposer::AppendActionL(const CUPnPAction* aUPnPAction, RBuf8 &aXmlData, TInt &count)
	{
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAction,aXmlData,count,ETrue);
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EName,aXmlData,count,ETrue);
	AppendLiteralL(aUPnPAction->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EName, GetTable())),aXmlData,count);
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EName,aXmlData,count,EFalse);
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EArgumentList,aXmlData,count,ETrue);

	for(TInt j =0;j< aUPnPAction->Count();j++ )
		{
		CUPnPArgument *uPnPArgument = aUPnPAction->At(j);
		if (!uPnPArgument->Validate(iStringPool, GetTable()))
			User::Leave(KErrCorrupt);
		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EArgument,aXmlData,count,ETrue);
		TAttributeIter it(uPnPArgument);
		const CAttribute* nextParam = NULL;
		while (!it.AtEnd())
			{
			nextParam = it();
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
			AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
			++it;
			}
    	if ( uPnPArgument->RetVal() )
    		{
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ERetValue,aXmlData,count,ETrue);
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ERetValue,aXmlData,count,EFalse);
    		}
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EArgument,aXmlData,count,EFalse);
		}
	
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EArgumentList,aXmlData,count,EFalse);
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAction,aXmlData,count,EFalse);
	}

void CUPnPServiceXmlComposer::AppendStateVariableL(const CUPnPStateVariable* aUPnPStateVariable, RBuf8 &aXmlData, TInt &count)
	{
   	
	AppendLiteralL(KStringStVarStart,aXmlData,count);
	AppendLiteralL(aUPnPStateVariable->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::ESendEvents, GetTable())),aXmlData,count);
	AppendLiteralL(KStringStVarStart1,aXmlData,count);
	
	TAttributeIter it(aUPnPStateVariable);
	const CAttribute* nextParam = NULL;
	while (!it.AtEnd())
		{
		nextParam = it();
		if ( nextParam->AttributeName() == iStringPool.String(UPNPDESCRIPTIONXMLTAGS::ESendEvents, GetTable()))
			{
			++it;
			continue;
			}
		else
			{
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
			AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
			++it;
			}
		}
	
	if(aUPnPStateVariable->AllowedValues())
		{
		if(aUPnPStateVariable->AllowedValueType())
    		{
    		const CUPnPArgValueList* aTempArgValueList = static_cast<const CUPnPArgValueList*>(aUPnPStateVariable->AllowedValues());
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValueList,aXmlData,count,ETrue);
    		for(TInt k = 0; k< aTempArgValueList->Count();k++)
    			{
	    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValue,aXmlData,count,ETrue);
	    		AppendLiteralL(aTempArgValueList->At(k),aXmlData,count);
	    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValue,aXmlData,count,EFalse);
    			}
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValueList,aXmlData,count,EFalse);
    		}
    	else 
    		{
    		const CUPnPValueRange* aTempValueRange = static_cast<const CUPnPValueRange*>(aUPnPStateVariable->AllowedValues());	
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValueRange,aXmlData,count,ETrue);
    		TAttributeIter it(aTempValueRange);
    		const CAttribute* nextParam = NULL;
    		while (!it.AtEnd())
				{
				nextParam = it();
				AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
				AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
				AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
				++it;
				}
    		
	    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EAllowedValueRange,aXmlData,count,EFalse);
    		}
    	}
	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EStateVariable,aXmlData,count,EFalse);
	}


void CUPnPServiceXmlComposer::ComposeServiceXmlL(const CUPnPServiceDescription *aServDescObj, RBuf8 &aXmlData)
	{
	RBuf	unicode;
    TInt	count = 0;


   if(!(aServDescObj->Validate(iStringPool, GetTable())))
    	User::Leave(KErrCorrupt);
  
    User::LeaveIfError(aXmlData.Create(COMPOSE_BUFLENGTH));
    
    
    AppendLiteralL(KStringXmlStart,aXmlData,count);
    AppendLiteralL(KStringScpdStart,aXmlData,count);
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ESpecVersion,aXmlData,count,ETrue);
    
    HBufC8* versionMajor = NULL;
    HBufC8* versionMinor = NULL;
    InetProtTextUtils::ConvertIntToDescriptorL(aServDescObj->MajorNumber(), versionMajor);
    InetProtTextUtils::ConvertIntToDescriptorL(aServDescObj->MinorNumber(), versionMinor);
    CleanupStack::PushL(versionMajor);
    CleanupStack::PushL(versionMinor);
    
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EMajorNumber,aXmlData,count,ETrue);
    AppendLiteralL(versionMajor->Des(),aXmlData,count);
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EMajorNumber,aXmlData,count,EFalse);
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EMinorNumber,aXmlData,count,ETrue);
    AppendLiteralL(versionMinor->Des(),aXmlData,count);
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EMinorNumber,aXmlData,count,EFalse);
    
    CleanupStack::PopAndDestroy(versionMinor);
    CleanupStack::PopAndDestroy(versionMajor);
    
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ESpecVersion,aXmlData,count,EFalse);

    
    if(aServDescObj->CountOfActionList()!= 0)
    	{
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EActionList,aXmlData,count,ETrue);
    	}
   
    for(TInt i =0;i< aServDescObj->CountOfActionList();i++ )
    	{
    	const CUPnPAction	*uPnPAction = aServDescObj->AtActionList(i);
    	AppendActionL(uPnPAction,aXmlData,count);
    	}
    if(aServDescObj->CountOfActionList()!= 0)
    	{
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EActionList,aXmlData,count,EFalse);
    	}
	
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EServiceStateTable,aXmlData,count,ETrue);
    
    for(TInt i =0;i< aServDescObj->CountOfServiceStateTable();i++ )
    	{
    	const CUPnPStateVariable *uPnPStateVariable = aServDescObj->AtServiceStateTable(i);
    	AppendStateVariableL(uPnPStateVariable,aXmlData, count);
    	}
    
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EServiceStateTable,aXmlData,count,EFalse);
    
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EScpd,aXmlData,count,EFalse);

	
    
    unicode.CreateL(aXmlData.Length()); 
    unicode.Copy(aXmlData);
    unicode.Close();
	}
