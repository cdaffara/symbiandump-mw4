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

#include "cupnpdevicecomposer.h"
#include <upnpdescriptionschema.h>
#include "inetprottextutils.h"
#include "tattributeiter.h"
#include "cattribute.h"

#define COMPOSE_BUFLENGTH 1024

_LIT8(KStringXmlStart,"<?xml version=\"1.0\" encoding=\"utf-8\"?>");
// Below hard coding should be removed
_LIT8(KStringRootStart,"\n<root xmlns=\"urn:schemas-upnp-org:device-1-0\">");
_LIT8(KDlnaNameSpace," xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">");
_LIT8(KDlna,"dlna:");

_LIT8(KBeginOpen,"\n<");
_LIT8(KBeginClose,"</"); 
_LIT8(KEnd,">");


const TStringTable& CUPnPDeviceXmlComposer::GetTable()
	{
	return UPNPDESCRIPTIONXMLTAGS::Table;
	}

void CUPnPDeviceXmlComposer::AppendLiteralL(TInt aTag, RBuf8& aBuf, TInt& count, TBool aIsStart )
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
void CUPnPDeviceXmlComposer::AppendLiteralL(const RString& aString, RBuf8& aBuf, TInt& count, TBool aIsStart )
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

void CUPnPDeviceXmlComposer::AppendLiteralL(const TDesC8& aLiteral, RBuf8& aBuf, TInt& count)
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
Allocates and constructs a CUPnPDeviceXmlComposer object.
Initialises all member data to their default values.
*/
CUPnPDeviceXmlComposer* CUPnPDeviceXmlComposer::NewL( const RStringPool& aStringPool)
    {
    CUPnPDeviceXmlComposer* self = new (ELeave) CUPnPDeviceXmlComposer( aStringPool );
    return self;
    }

CUPnPDeviceXmlComposer::CUPnPDeviceXmlComposer( const RStringPool& aStringPool): iStringPool (aStringPool)
    {
    iError = KErrNone;
    }
/**
Destructor
*/
CUPnPDeviceXmlComposer::~CUPnPDeviceXmlComposer()
    {
    }

void CUPnPDeviceXmlComposer::AppendDeviceAttributesL(const CUPnPDevice*	aDeviceAttribute, RBuf8 &aXmlData, TInt &count)
	{
	TAttributeIter it(aDeviceAttribute);
	const CAttribute* nextParam = NULL;
	if ( aDeviceAttribute->Dlna())
		{
		if (aDeviceAttribute->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaDoc, GetTable()))!= KNullDesC8 )
			{
			RBuf8 dlnaValue;
			dlnaValue.CreateL( 100 );
			dlnaValue.Append(KBeginOpen);
			dlnaValue.Append(KDlna);
			dlnaValue.Append(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaDoc, GetTable()).DesC());
			dlnaValue.Append( KDlnaNameSpace );
			AppendLiteralL(dlnaValue, aXmlData,count );
			dlnaValue.Close();
			AppendLiteralL(aDeviceAttribute->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaDoc, GetTable())),aXmlData,count);
			dlnaValue.CreateL(50);
			dlnaValue.Append(KBeginClose);
			dlnaValue.Append(KDlna);
			dlnaValue.Append(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaDoc, GetTable()).DesC());
			dlnaValue.Append( KEnd );	
			AppendLiteralL(dlnaValue, aXmlData,count );
			dlnaValue.Close();
			}
		if (aDeviceAttribute->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaCap, GetTable()))!= KNullDesC8 )
			{
			RBuf8 dlnaValue;
			dlnaValue.CreateL( 100 );
			dlnaValue.Append(KBeginOpen);
			dlnaValue.Append(KDlna);
			dlnaValue.Append(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaCap, GetTable()).DesC());
			dlnaValue.Append( KDlnaNameSpace );
			dlnaValue.Append( KEnd );
			AppendLiteralL(dlnaValue, aXmlData,count );
			dlnaValue.Close();
			AppendLiteralL(aDeviceAttribute->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaCap, GetTable())),aXmlData,count);
			dlnaValue.CreateL(50);
			dlnaValue.Append(KBeginClose);
			dlnaValue.Append(KDlna);
			dlnaValue.Append(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDlnaCap, GetTable()).DesC());
			dlnaValue.Append( KEnd );	
			AppendLiteralL(dlnaValue, aXmlData,count );
			dlnaValue.Close();
			}
		
		}
	while (!it.AtEnd())
		{
		nextParam = it();
		TInt index = nextParam->AttributeName().Index(GetTable());
		if (index != UPNPDESCRIPTIONXMLTAGS::EDlnaDoc && index != UPNPDESCRIPTIONXMLTAGS::EDlnaCap )
			{
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
			AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
			AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
			++it;
			}
		else
			++it;
		}
	}

void CUPnPDeviceXmlComposer::AppendDeviceIconL(const CUPnPIconInfo* aIconInfo, RBuf8 &aXmlData, TInt &count)
	{
	TAttributeIter it(aIconInfo);
	const CAttribute* nextParam = NULL;
	while (!it.AtEnd())
		{
		nextParam = it();
		AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
		AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
		AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
		++it;
		}
	}
	
void CUPnPDeviceXmlComposer::AppendServiceL(const CUPnPServiceInfo* aServInfo, RBuf8 &aXmlData, TInt &count)
	{
	TAttributeIter it(aServInfo);
	const CAttribute* nextParam = NULL;
	while (!it.AtEnd())
		{
		nextParam = it();
		AppendLiteralL(nextParam->AttributeName(),aXmlData,count,ETrue);
		AppendLiteralL(nextParam->AttributeValue(),aXmlData,count);
		AppendLiteralL(nextParam->AttributeName(),aXmlData,count,EFalse);
		++it;
		}
	}

void CUPnPDeviceXmlComposer::AppendDeviceXmlL(const CUPnPDevice* devObj, RBuf8 &aXmlData, TInt &count)
	{

    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EDevice,aXmlData,count,ETrue);

	if(devObj->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EPresentationUrl, GetTable())).Length()!=0)
		{
        AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EPresentationUrl,aXmlData,count,ETrue);
	    AppendLiteralL(devObj->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EPresentationUrl, GetTable())),aXmlData,count);
        AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EPresentationUrl,aXmlData,count,EFalse);
		}

    AppendDeviceAttributesL(devObj,aXmlData,count);
    if(devObj->CountOfIconInfoTable()!=0)
    	{
    	const CUPnPIconInfo* aIconInfo = NULL;
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EIconList,aXmlData,count,ETrue);
        for(TInt k=0;k<devObj->CountOfIconInfoTable();k++)
        	{
            AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EIcon,aXmlData,count,ETrue);
            aIconInfo = devObj->AtIconInfoTable(k);
            AppendDeviceIconL(aIconInfo, aXmlData, count);
            AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EIcon,aXmlData,count,EFalse);
        	}
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EIconList,aXmlData,count,EFalse);
    	}
    
    // service info goes here
    
    if(devObj->CountOfServiceInfoTable()!=0)
    	{
    	const CUPnPServiceInfo* aServInfo = NULL;
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EServiceList,aXmlData,count,ETrue);
    	
    	for(TInt m=0;m<devObj->CountOfServiceInfoTable();m++)
    		{
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EService,aXmlData,count,ETrue);
    		aServInfo = devObj->AtServiceInfoTable(m);
    		AppendServiceL(aServInfo,aXmlData, count);
    		AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EService,aXmlData,count,EFalse);
    		}
    	
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EServiceList,aXmlData,count,EFalse);
    	}

    
    if(devObj->CountOfEmbeddedDeviceInfoTable()!= 0 )
    	{
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EDeviceList,aXmlData,count,ETrue);
    	
    	for(TInt n = 0; n < devObj->CountOfEmbeddedDeviceInfoTable(); n++)
    		AppendDeviceXmlL(devObj->AtEmbeddedDeviceInfoTable(n),aXmlData,count);
    	
    	AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EDeviceList,aXmlData,count,EFalse);
    	}

    
    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EDevice,aXmlData,count,EFalse);

	}

void CUPnPDeviceXmlComposer::ComposeDeviceXmlL(const CUPnPDeviceDescription *aDeviceDescObj, RBuf8 &aXmlData)
	{
	RBuf	unicode;
    TInt	count = 0;
 

    if(!(aDeviceDescObj->Validate(iStringPool,GetTable())))
    	User::Leave(KErrCorrupt);
 
    User::LeaveIfError(aXmlData.Create(COMPOSE_BUFLENGTH));
        
    AppendLiteralL(KStringXmlStart,aXmlData,count);
    AppendLiteralL(KStringRootStart,aXmlData,count);

    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ESpecVersion,aXmlData,count,ETrue);

    HBufC8* versionMajor = NULL;
    HBufC8* versionMinor = NULL;
    InetProtTextUtils::ConvertIntToDescriptorL(aDeviceDescObj->MajorNumber(), versionMajor);
    InetProtTextUtils::ConvertIntToDescriptorL(aDeviceDescObj->MinorNumber(), versionMinor);
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
    
    CUPnPDevice* devObj =  aDeviceDescObj->DeviceObject();
    if(aDeviceDescObj->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EUrlBase, GetTable())).Length()!=0)
    	{
        AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EUrlBase,aXmlData,count,ETrue);
        AppendLiteralL(aDeviceDescObj->Property(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::EUrlBase, GetTable())),aXmlData,count);
        AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::EUrlBase,aXmlData,count,EFalse);
    	}
    
    // Add device specific info to the xml buffer.
    AppendDeviceXmlL(devObj,aXmlData,count);

    AppendLiteralL(UPNPDESCRIPTIONXMLTAGS::ERoot,aXmlData,count,EFalse);
	
 	}

