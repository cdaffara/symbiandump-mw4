/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility classes for device lock handling
*
*/



#include <e32std.h>

#include "vpndevlockhandler.h"
#include "policyinstaller_constants.h"
#include "logvpncommon.h"
#include "vpntcstub.h"


// 64bit integer's string representation can be at 
// most 20 bytes (+ sign) long...
#define MAX_INT_STR_LEN 21


//////////////
// CVpnDevLockPolicy implementation

CVpnDevLockPolicy* CVpnDevLockPolicy::NewL(const TInt aPolicy) 
    {
    LOG_("-> CVpnDevLockPolicy::NewL()");
    
    CVpnDevLockPolicy* self = new (ELeave) CVpnDevLockPolicy();
    CleanupStack::PushL(self);
    self->ConstructL(aPolicy);
    CleanupStack::Pop(self);
    
    LOG_("<- CVpnDevLockPolicy::NewL()");
    
    return self;
    }

CVpnDevLockPolicy* CVpnDevLockPolicy::NewLC(const TInt aPolicy) 
    {
    LOG_("-> CVpnDevLockPolicy::NewLC()");
    
    CVpnDevLockPolicy* self = new (ELeave) CVpnDevLockPolicy();
    CleanupStack::PushL(self);
    self->ConstructL(aPolicy);
    
    LOG_("<- CVpnDevLockPolicy::NewLC()");
    
    return self;
    }

CVpnDevLockPolicy* CVpnDevLockPolicy::NewLC() 
    {
    LOG_("-> CVpnDevLockPolicy::NewLC()");
    
    CVpnDevLockPolicy* self = new (ELeave) CVpnDevLockPolicy();
    CleanupStack::PushL(self);
    self->ConstructL();
    
    LOG_("<- CVpnDevLockPolicy::NewLC()");
    
    return self;
    }

CVpnDevLockPolicy::CVpnDevLockPolicy()
    {
    }

CVpnDevLockPolicy::~CVpnDevLockPolicy() 
    {
    LOG_("-> CVpnDevLockPolicy::CVpnDevLockPolicy()");

    // Delete each entry from the policy attributes array, 
    // and then reset the array itself
    if (iPolicyArray) 
        {
        for (TInt i = iPolicyArray->Count() - 1; i >= 0; i--) 
            {
            delete iPolicyArray->At(i);
            }
        iPolicyArray->Reset();
        }

    delete iPolicyArray;

    LOG_("<- CVpnDevLockPolicy::CVpnDevLockPolicy()");
    }

void CVpnDevLockPolicy::ConstructL(const TInt aPolicy) 
    {
    LOG_("-> CVpnDevLockPolicy::ConstructL()");
    ConstructL();
    SetPolicyLevelL(aPolicy);
    LOG_("<- CVpnDevLockPolicy::ConstructL()");
    }

void CVpnDevLockPolicy::ConstructL()
    {
    LOG_("-> CVpnDevLockPolicy::ConstructL()");
    iPolicyArray = new (ELeave) CArrayFixFlat<CVpnDevLockAttribute*>(3);
    LOG_("<- CVpnDevLockPolicy::ConstructL()");
    }

void CVpnDevLockPolicy::SetPolicyAttributeL(const TInt aAttrib, const TInt aVal) 
    {
    LOG_("-> CVpnDevLockPolicy::SetPolicyAttributeL() int variant");

    CVpnDevLockAttribute* attrib = CVpnDevLockAttribute::NewL(aAttrib, aVal);
    CleanupStack::PushL(attrib);
    iPolicyArray->AppendL(attrib);

    // ownership's been transferred to array
    CleanupStack::Pop(attrib); 


    // Remove this (or rather flag for debug builds only)
    TInt val(0);
    attrib->AttributeValueAsIntL(val);

    LOG_1(" Items in array: %d", iPolicyArray->Count());
    LOG8_1("  Attrib  ID: %d", attrib->AttributeId());
    LOG8_1("  Attrib val: %d", val);

    LOG_("<- CVpnDevLockPolicy::SetPolicyAttributeL() int variant");
    }


void CVpnDevLockPolicy::SetPolicyAttributeL(const TInt aAttrib, const TDesC8& aVal) 
    {
    LOG_("-> CVpnDevLockPolicy::SetPolicyAttributeL() str variant");

    // Create a new attribute and add it to the existing array of attributes
    CVpnDevLockAttribute* attrib = CVpnDevLockAttribute::NewL(aAttrib, aVal);
    CleanupStack::PushL(attrib);
    iPolicyArray->AppendL(attrib);
    
    // ownership's been transferred to array
    CleanupStack::Pop(attrib); 

    LOG8_1("  Attrib  ID: %d", attrib->AttributeId());

    LOG_("<- CVpnDevLockPolicy::SetPolicyAttributeL() str variant");
    }

/**
 * Description of device lock levels (from SUB 415-467)
 *
 * 0. Do not enforce device lock
 *   - installing this policy has no impact on the device lock settings
 * 1. Enforce device lock with policy 1
 *   - no special requirements on lock code
 *   - autolock timeout 30 minutes
 *   - wipe after 20 unsuccessful unlocking attempts
 * 2. Enforce device lock with policy 2
 *   - lock code minimum length 5 characters
 *   - autolock timeout 10 minutes
 *   - wipe after 10 unsuccessful unlocking attempts
 * 3. Enforce device lock with policy 3
 *   - lock code mimimum length 5 characters
 *   - both characters and numbers required
 *   - both upper and lower case letters required
 *   - passcode expires every 30 days and must be changed
 *   - new passcode must not match any of the 5 previous passcodes 
 *   - autolock timeout 5 minutes
 *   - wipe after 10 unsuccessful unlocking attempts
 *
 */
void CVpnDevLockPolicy::SetPolicyLevelL(const TInt aPolLevel) 
    {
    LOG_("-> CVpnDevLockPolicy::SetPolicyLevelL()");

    LOG_1(" Setting policy level: %d", aPolLevel);

    // Setting the policy level
    // See VPN device lock SUB for exact device lock policy level descriptions
    switch (aPolLevel) 
        {
        case 0:
            // No changes to existing devlock policy, 
            // when policy level is 0
            break;
        case 1:
            // Only enforce timeout and max number of passcode attempts
            SetPolicyAttributeL(RTerminalControl3rdPartySession::ETimeout, 30);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeMaxAttempts, 20);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EMaxTimeout, 30);
            break;
        case 2:
            // Enforce stricter timeout as well, and a minimum length for passcode
            SetPolicyAttributeL(RTerminalControl3rdPartySession::ETimeout, 10);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeMinLength, 5);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeMaxAttempts, 10);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EMaxTimeout, 10);
            break;
        case 3:
            // Enforce even more password-related rules
            SetPolicyAttributeL(RTerminalControl3rdPartySession::ETimeout, 5);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeMinLength, 5);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeMaxAttempts, 10);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers,
                                1);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower,
                                1);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeExpiration, 30);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EPasscodeHistoryBuffer, 5);
            SetPolicyAttributeL(RTerminalControl3rdPartySession::EMaxTimeout, 5);
            break;
        default:
            // Policy levels 4+ not supported, leave (tried to instantiate an illegal policy)
            LOG_("<- CVpnDevLockPolicy::SetPolicyLevelL() LEAVE: illegal policy level");
            User::Leave(KErrNotSupported);
            break;
        }

    LOG_("<- CVpnDevLockPolicy::SetPolicyLevelL()");
    }

TInt CVpnDevLockPolicy::AttributeCount() 
    {
    LOG_("-> CVpnDevLockPolicy::AttributeCount()");

    TInt ret(0);
    if (iPolicyArray) 
        {
        ret = iPolicyArray->Count();
        }

    LOG_1("<- CVpnDevLockPolicy::AttributeCount() ret: %d", ret);
    return ret;
    }

CVpnDevLockAttribute* CVpnDevLockPolicy::GetPolicyAttributeAt(const TInt aIdx) 
    {
    LOG_("-> CVpnDevLockPolicy::GetPolicyAttributeAt()");
    CVpnDevLockAttribute* ret(NULL);

    if (iPolicyArray && (aIdx < iPolicyArray->Count()))
        {
        LOG_1(" Found valid attribute at index %d", aIdx);
        ret = iPolicyArray->At(aIdx);
        }

    LOG_("<- CVpnDevLockPolicy::GetPolicyAttributeAt()");
    return ret;
    }


//////////////
// CVpnDevLockHandler implementation

/**
 * Constructor, and
 *
 */
CVpnDevLockHandler::CVpnDevLockHandler()
    {
    LOG_("-> CVpnDevLockHandler::CVpnDevLockHandler()");
    
    LOG_("<- CVpnDevLockHandler::CVpnDevLockHandler()");
    }

void CVpnDevLockHandler::ConstructL() 
    {
    // Connect to terminal control (if available)
    LoadTCLibraryL();
    }

void CVpnDevLockHandler::LoadTCLibraryL()
    {
    LOG_("-> CVpnDevLockHandler::LoadTCLibraryL()");

    TInt status = iTcLib.Load(KVpnDevLockWrapperDll, KNullUid);

    LOG_1(" DLL load status: %d", status);

    if (status == KErrNone)
        {
        LOG_(" Suitable library found, instantiating dev lock object");

        TLibraryFunction entry = iTcLib.Lookup(1);
        
        // Entry point function (NewTcStubL) might leave,
        // although it isn't readily obvious from the syntax
        iTcStub = (CVpnTcStub*)entry();

        LOG_(" Dev lock object instantiated");
        iDevLockAvailable = ETrue;
        }
    else 
        {
        LOG_(" No suitable library found!");
        iDevLockAvailable = EFalse;
        }
    
    LOG_("<- CVpnDevLockHandler::LoadTCLibraryL()");
    }

CVpnDevLockHandler::~CVpnDevLockHandler() 
    {
    LOG_("-> CVpnDevLockHandler::CVpnDevLockHandler()");

    LOG_(" Deleting TC stub");
    delete iTcStub;

    LOG_(" Closing TC library");
    iTcLib.Close();
    
    LOG_("<- CVpnDevLockHandler::CVpnDevLockHandler()");
    }

void CVpnDevLockHandler::TerminalControlSupportsVpnL() 
    {
    LOG_("-> CVpnDevLockHandler::TerminalControlSupportsVpnL()");

    HBufC8* attrib(NULL);

    // If the lib stub is available, try to read a device lock attribute
    // (read may fail))
    if (iTcStub) 
        {
        LOG_(" TC stub is valid");
        attrib = iTcStub->GetTCAttributeL(RTerminalControl3rdPartySession::ETimeout);

        if (attrib) 
            {
            LOG8_1(" Attribute: '%S'", attrib);
            delete attrib;
            }
        else 
            {
            LOG_(" Attribute couldn't be read");
            User::Leave(KErrAccessDenied);
            }
        }
    else 
        {
        LOG_(" No stub available, terminating");
        User::Leave(KErrAccessDenied);
        }

    LOG_("<- CVpnDevLockHandler::TerminalControlSupportsVpnL()");
    }

void CVpnDevLockHandler::SetTerminalControlPolicyL(const TInt aPolLevel) 
    {
    LOG_("-> CVpnDevLockHandler::SetTerminalControlPolicyL()");
    LOG_1(" Setting policy level %d", aPolLevel);

    CVpnDevLockPolicy* devpolicy = CVpnDevLockPolicy::NewL(aPolLevel);
    CleanupStack::PushL(devpolicy);

    // Iterate through all the attributes that VPN policy requires.
    // For each one found, check if it's stricter than the old device lock setting,
    // and only if that is the case, update the actual value on device.

    TInt attribs = devpolicy->AttributeCount();
    LOG_1(" Iterating through %d attributes", attribs);
    
    TBool forceLock(EFalse);

    for (TInt i = 0; i < attribs; i++) 
        {
        CVpnDevLockAttribute* attrib = devpolicy->GetPolicyAttributeAt(i);
        if (!attrib) 
            {
            LOG_("<- CVpnDevLockHandler::SetTerminalControlPolicyL() LEAVE: Not found");
            User::Leave(KErrNotFound);
            }

        LOG_1(" Attribute    ID: %d", attrib->AttributeId());

        // Only set the attribute if the existing one was not as strict
        // (definition of "strict" depends on attribute's semantics)
        TBool override(EFalse);
        TRAPD(err, override = OverrideExistingL(*attrib));

        if ((err == KErrNone && override) || err == KErrNotFound)
            {
            SetDevAttributeL(*attrib);
            forceLock = ETrue;
            }

        }

    // If at least one lock parameter was (succesfully) changed, force device lock after
    // import
    if (forceLock) 
        {
        LOG_(" Forcing device lock activation");

        // Force lock
        CVpnDevLockAttribute* attrib = CVpnDevLockAttribute::NewL(RTerminalControl3rdPartySession::ELock,
                                                                  1);
        CleanupStack::PushL(attrib);

        // Expire password (force password change)
        CVpnDevLockAttribute* attrib2 = 
                                CVpnDevLockAttribute::NewL(RTerminalControl3rdPartySession::EPasscodeExpiration, -1);
        CleanupStack::PushL(attrib2);

        LOG_(" Expiring old passcode");
        SetDevAttributeL(*attrib2);
        CleanupStack::PopAndDestroy(attrib2);

        LOG_(" Forcing device lock");
        SetDevAttributeL(*attrib);
        CleanupStack::PopAndDestroy(attrib);
        }
    
    
    CleanupStack::PopAndDestroy(devpolicy);

    LOG_("<- CVpnDevLockHandler::SetTerminalControlPolicyL()");
    }

TBool CVpnDevLockHandler::OverrideExistingL(const CVpnDevLockAttribute& aNew) 
    {
    LOG_("-> CVpnDevLockHandler::OverrideExisting()");
    TBool ret(EFalse);

    HBufC8* existingAttrib = GetDevAttribValueL(aNew.AttributeId());
    CleanupStack::PushL(existingAttrib);

    // At the moment, we can rely on the fact that all relevant
    // attributes are numerical... This may change in future
    TInt newVal(-1);
    TInt oldVal(-1);
    TLex8 lexer(*existingAttrib);
    User::LeaveIfError(lexer.Val(oldVal));
    aNew.AttributeValueAsIntL(newVal);

    switch (aNew.AttributeId()) 
        {
        case RTerminalControl3rdPartySession::ETimeout:
            // If device lock timeout is set to lower value than the old one,
            // then we need to update the policy
            LOG_(" Case ETimeout");
            if (newVal < oldVal || oldVal == 0) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeMinLength:
            // If passcode minimum length requirement is longer than the
            // old one, then we need to update the policy
            LOG_(" Case EPasscodeMinLength");
            if (newVal > oldVal) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeMaxAttempts:
            // If max attempts lower, override
            LOG_(" Case EPasscodeMaxAttempts");
            if (newVal < oldVal || oldVal == 0) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers:
            // If alphanum requirement didn't exist before, override
            LOG_(" Case EPasscodeRequireCharsAndNumbers");
            if (newVal > oldVal)
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower:
            // If UC/LC requirement didn't exist before, override
            LOG_(" Case EPasscodeRequireUpperAndLower");
            if (newVal > oldVal) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeExpiration:
            // If less days before passcode expiration, override
            LOG_(" Case EPasscodeExpiration");
            if (newVal < oldVal || oldVal == 0) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EPasscodeHistoryBuffer:
            // If the proposed passcode history is longer, override
            LOG_(" Case EPasscodeHistoryBuffer");
            if (newVal > oldVal) 
                {
                ret = ETrue;
                }
            break;
        case RTerminalControl3rdPartySession::EMaxTimeout:
            // If the proposed max timeout is shorter, override
            LOG_(" Case EMaxTimeout");
            if ((oldVal == 0) || (newVal < oldVal)) 
                {
                ret = ETrue;
                }
            break;
        default:
            LOG_(" ERROR: Case default, unknown attribute!");
            User::Leave(KErrNotFound);
            break;
        }
    CleanupStack::PopAndDestroy(existingAttrib);

    LOG_1("  Old value: %d", oldVal);
    LOG_1("  New value: %d", newVal);
    LOG( if (ret) 
        {
        LOG_("   => Old value needs to be updated");
        }
    );

    LOG_1("<- CVpnDevLockHandler::OverrideExisting() ret: %d", ret);
    return ret;
    }

HBufC8* CVpnDevLockHandler::GetDevAttribValueL(const TInt aAttribId) 
    {
    LOG_("-> CVpnDevLockHandler::GetDevAttribValueL()");

    HBufC8* buffer(NULL);

    if (iTcStub) 
        {
        buffer = iTcStub->GetTCAttributeL(aAttribId);
        if (buffer) 
            {
            LOG8_1(" Buffer: '%S'", buffer);
            }
        }

    LOG_("<- CVpnDevLockHandler::GetDevAttribValueL()");

    return buffer;
    }

void CVpnDevLockHandler::SetDevAttributeL(const CVpnDevLockAttribute& aNew) 
    {
    LOG_("-> CVpnDevLockHandler::SetDevAttribValueL()");

    if (iTcStub) 
        {
        iTcStub->SetTCAttributeL(aNew.AttributeId(), aNew.AttributeValue());
        }

    LOG_("<- CVpnDevLockHandler::SetDevAttribValueL()");
    }


//////////////
// CVpnDevLockAttribute implementation

CVpnDevLockAttribute* CVpnDevLockAttribute::NewL(const TInt aId, const TDesC8& aVal) 
    {
    LOG_("-> CVpnDevLockAttribute::NewL()");
    
    CVpnDevLockAttribute* self = new (ELeave) CVpnDevLockAttribute();
    CleanupStack::PushL(self);
    self->ConstructL(aId, aVal);
    CleanupStack::Pop(self);
    
    LOG_("<- CVpnDevLockAttribute::NewL()");
    return self;
    }

CVpnDevLockAttribute* CVpnDevLockAttribute::NewL(const TInt aId, const TInt aVal) 
    {
    LOG_("-> CVpnDevLockAttribute::NewL()");
    
    CVpnDevLockAttribute* self = new (ELeave) CVpnDevLockAttribute();
    CleanupStack::PushL(self);
    self->ConstructL(aId, aVal);
    CleanupStack::Pop(self);
    
    LOG_("<- CVpnDevLockAttribute::NewL()");
    return self;
    }

CVpnDevLockAttribute* CVpnDevLockAttribute::NewLC(const TInt aId, const TDesC8& aVal) 
    {
    LOG_("-> CVpnDevLockAttribute::NewLC()");
    
    CVpnDevLockAttribute* self = new (ELeave) CVpnDevLockAttribute();
    CleanupStack::PushL(self);
    self->ConstructL(aId, aVal);
    
    LOG_("<- CVpnDevLockAttribute::NewLC()");
    
    return self;
    }

CVpnDevLockAttribute::~CVpnDevLockAttribute() 
    {
    LOG_("-> CVpnDevLockAttribute::~CVpnDevLockAttribute()");
    delete iAttributeVal;
    LOG_("<- CVpnDevLockAttribute::~CVpnDevLockAttribute()");
    }

CVpnDevLockAttribute::CVpnDevLockAttribute() 
    {
    LOG_("-> CVpnDevLockAttribute::CVpnDevLockAttribute()");
    LOG_("<- CVpnDevLockAttribute::CVpnDevLockAttribute()");
    }

void CVpnDevLockAttribute::ConstructL(const TInt aId, const TDesC8& aVal) 
    {
    iAttributeId = aId;
    iAttributeVal = aVal.AllocL();
    }

void CVpnDevLockAttribute::ConstructL(const TInt aId, const TInt aVal) 
    {
    // Convert int to a string
    iAttributeId = aId;
    TBuf8<MAX_INT_STR_LEN> buf;
    buf.Num(aVal);
    iAttributeVal = buf.AllocL();
    }

TPtrC8 CVpnDevLockAttribute::AttributeValue() const
    {
    return *iAttributeVal;
    }

void CVpnDevLockAttribute::AttributeValueAsIntL(TInt& aVal) const
    {
    if (iAttributeVal)
        {
        TLex8 lexer(*iAttributeVal);
        User::LeaveIfError(lexer.Val(aVal));
        }
    }

TInt CVpnDevLockAttribute::AttributeId() const
    {
    return iAttributeId;
    }



CVpnDevLockHandler* CVpnDevLockHandler::NewL() 
    {
    LOG_("-> CVpnDevLockHandler::NewL()");
    
    CVpnDevLockHandler* self = new (ELeave) CVpnDevLockHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    LOG_("<- CVpnDevLockAttribute::NewL()");
    return self;
    }

CVpnDevLockHandler* CVpnDevLockHandler::NewLC() 
    {
    LOG_("-> CVpnDevLockAttribute::NewLC()");
    
    CVpnDevLockHandler* self = new (ELeave) CVpnDevLockHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    
    LOG_("<- CVpnDevLockAttribute::NewLC()");
    
    return self;
    }

