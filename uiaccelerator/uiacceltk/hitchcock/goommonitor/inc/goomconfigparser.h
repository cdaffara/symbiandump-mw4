/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The parser for the GOOM configuration file
*
*/


#ifndef GOOMCONFIGPARSER_H_
#define GOOMCONFIGPARSER_H_

#include <xml/contenthandler.h>

using namespace Xml;

class CGOomConfig;
class CGOomRunPluginConfig;

NONSHARABLE_CLASS(CGOomConfigParser) : public CBase, public MContentHandler 
    {
public:
    CGOomConfigParser(CGOomConfig& aConfig, RFs& aFs);
    
    void ParseL();

    
    // From MContent handler
    virtual void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
    virtual void OnEndDocumentL(TInt aErrorCode);
    virtual void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
                                 TInt aErrorCode);
    virtual void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
    virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
                                       TInt aErrorCode);
    virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
                                          TInt aErrorCode);
    virtual void OnError(TInt aErrorCode);
    virtual TAny* GetExtendedInterface(const TInt32 aUid);
    
    
    
private:
    
    void ConfigError(TInt aError);
    
    
    
    TInt StartElementL(const TDesC8& aLocalName, 
                          const RAttributeArray& aAttributes);
    
    void SetGlobalSettings(const RAttributeArray& aAttributes);
    void SetForceCheckConfigL(const RAttributeArray& aAttributes);
    void SetAppConfigL(const RAttributeArray& aAttributes);
    void SetCloseAppConfigL(const RAttributeArray& aAttributes);
    void SetAppCloseIdlePriorityConfigL(const RAttributeArray& aAttributes);
    void SetForegroundAppPriorityL(const RAttributeArray& aAttributes);
    
    void SetSystemPluginConfigL(const RAttributeArray& aAttributes);
    void SetAppPluginConfigL(const RAttributeArray& aAttributes);
    void SetPluginIdlePriorityL(const RAttributeArray& aAttributes);
    void SetPluginForegroundAppPriorityL(const RAttributeArray& aAttributes);
    
    TInt GetValueFromAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TPtrC8& aValue);
    TInt GetValueFromHexAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TUint& aValue);
    TInt GetValueFromDecimalAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TUint& aValue);
    TInt GetValueFromDecimalAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TInt& aValue);
    
    void SetPluginSyncMode(const RAttributeArray& aAttributes, CGOomRunPluginConfig& aRunPluginConfig);

    enum TGOomParsingState
        {
        EGOomParsingStateNone,
        EGOomParsingStateRoot,
        EGOomParsingStateGlobalSettings,
        EGOomParsingStateAppSettings,
        EGOomParsingStateAppCloseSettings,
        EGOomParsingStateSystemPluginSettings,
        EGOomParsingStateAppPluginSettings
        };    
    
    // Check that the current state is as expected
    // If not then the specified config error is generated
    void CheckState(TGOomParsingState aExpectedState, TInt aError);
    
    // Check that the current state is as expected
    // If not then the specified config error is generated
    // This version checks to ensure that the current state matches either of the passed in states
    void CheckState(TGOomParsingState aExpectedState1, TGOomParsingState aExpectedState2, TInt aError);
    
    CGOomConfig& iConfig;
    
    RFs& iFs;
    
    void ChangeState(TGOomParsingState aState);    
    
    TGOomParsingState iState;
    
    TUint iParentUid;
    TUint iParentTargetApp;
    };

#endif /*GOOMCONFIGPARSER_H_*/
