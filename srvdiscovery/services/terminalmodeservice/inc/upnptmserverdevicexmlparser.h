/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmServerDeviceXmlParser class declaration
*
*/

#ifndef __UPNPTMSERVERDEVICEXMLPARSER_H__
#define __UPNPTMSERVERDEVICEXMLPARSER_H__

// System Includes
#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <f32file.h>
#include <etelmm.h>
#include <ctsy/mmtsy_names.h>
#include <upnpcons.h>

// User Include
#include "upnptmserverdeviceinfo.h"

// Constants
const TUint KBufSize      = 20480;
const TUint KMaxBufLength = 255;
const TUint KDirectorySeparator = '\\';

using namespace Xml;

/**
 *  This class takes the root device content as xml formatted buffer which serves as placeholders 
 *  for many element values as an input and then constructs the complete device description 
 *  xml file and service xml files which are published over the network
 */
class CUpnpTmServerDeviceXmlParser: public CBase,
						            public MContentHandler						
	{
public:
	static CUpnpTmServerDeviceXmlParser* NewL( CUpnpTmServerDeviceInfo& aDeviceInfo );
	~CUpnpTmServerDeviceXmlParser();
	void StartL();
	inline const TDesC& DevicePath();
	inline const TDesC& AppServerSrvPath();
	inline const TDesC& ClientProfileSrvPath();
	inline const TDesC8& DescriptionUri();
	
private:
	CUpnpTmServerDeviceXmlParser( CUpnpTmServerDeviceInfo& aDeviceInfo );
	void ConstructL( );
	void DeleteDirectory();
	
private: // From MContentHandler
	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	inline void OnError(TInt aErrorCode);
	inline TAny* GetExtendedInterface(const TInt32 aUid);
		
private:
	RBuf8                                iAppServerSrvDescription;
	RBuf8                                iClientProfSrvDescription;
	RBuf8		                         iReadBuf;
	RBuf8		                         iDeviceDescription;
	CParser* 	                         iParser;
	RBuf		                         iDescFilePath;
	RBuf		                         iAppServerSrvFilePath;
	RBuf                                 iClientProfSrvFilePath;
	RBuf8                                iDescriptionUri;
	RFs                                  iFs;
	RBuf                                 iDeviceDir;
	RBuf                                 iIconDirectory;
    RMobilePhone                         iPhone;
    RTelServer                           iEtelSrv;
    RMobilePhone::TMobilePhoneIdentityV1 iPhoneId;
    CUpnpTmServerDeviceInfo&             iDeviceInfo;
    CFileMan*                            iFileMan; 
	};	

#include "upnptmserverdevicexmlparser.inl"

#endif  //__UPNPTMSERVERDEVICEXMLPARSER_H__
