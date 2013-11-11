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
* Description:   Device lock handling related classes
*
*/




#ifndef C_VPNDEVLOCKHANDLER_H
#define C_VPNDEVLOCKHANDLER_H

#include <e32base.h>
#include <TerminalControl3rdPartyAPI.h>
#include <TerminalControl.h>

class CVpnTcStub;

/**
 * Process:
 * 1. Instantiate devlock policy handler
 * 2. Make sure devlock is available and VPN can use it
 *    If TerminalControlUsableL() leaves, device lock can't be used.
 * 3. (do other required stuff, e.g. pkcs12 deciphering and
 *    policy installation)
 * 4. Set the desired terminal control device lock policy by
 *    calling SetTerminalControlPolicyL(). TInt parameter
 *    dictates what kind of a policy will be set.
 *    If there are mutual attributes which exist in both
 *    current TC dev lock settings and in the intended
 *    VPN-imported settings, always use the stricter
 *    value for the attribute (i.e. if VPN wants to set devlock
 *    timeout to 50 and current policy has a timeout 20, stick
 *    with the current policy).
 */

/**
 * CVpnDevLockAttribute class defines a single devicelock related
 * attribute and its value. E.g. "ETimeout" with value "30" (minutes).
 *
 * ID range comes from TerminalControl3rdPartyAPI.h.
 *
 */
class CVpnDevLockAttribute : public CBase 
    {
public:
    /**
     * Instantiate a single attribute, with given ID and value
     *
     * @param aId The ID of the attribute, from TerminalControl3rdPartyAPI
     *        defined range
     * @param aVal The string value of the attribute. New memory will be allocated
     *        locally for the value.
     *
     * @return CVpnDevLockAttribute* the instantiated object.
     *
     * @leave Leaves if OOM
     */
    static CVpnDevLockAttribute* NewL(const TInt aId, const TDesC8& aVal);

    /**
     * Instantiate a single attribute, with given ID and value
     *
     * @param aId The ID of the attribute, from TerminalControl3rdPartyAPI
     *        defined range
     * @param aVal The TInt value of the attribute. It will be converted to a HBufC8
     *        internally, and new memory will be allocated locally for the new string.
     *
     * @return CVpnDevLockAttribute* the instantiated object.
     *
     * @leave Leaves if OOM
     */
    static CVpnDevLockAttribute* NewL(const TInt aId, const TInt aVal);

    /**
     * Instantiate a single attribute, with given ID and value.
     * The instantiated object will remain in CleanupStack.
     *
     * @return CVpnDevLockAttribute* the instantiated object.
     *
     * @leave Leaves if OOM
     */
    static CVpnDevLockAttribute* NewLC(const TInt aId, const TDesC8& aVal);

    /**
     * Destructor deletes the attribute's locally controlled resources.
     *
     */
    ~CVpnDevLockAttribute();

private:
    CVpnDevLockAttribute();

protected:
    /**
     * Instantiate a single attribute, with given ID and value
     *
     * @param aId The ID of the attribute, from TerminalControl3rdPartyAPI
     *        defined range
     * @param aVal The string value of the attribute. New memory will be 
     *        allocated locally for the new string.
     *
     * @leave Leaves if OOM
     */
    void ConstructL(const TInt aId, const TDesC8& aVal);

    /**
     * Instantiate a single attribute, with given ID and value
     *
     * @param aId The ID of the attribute, from TerminalControl3rdPartyAPI
     *        defined range
     * @param aVal The TInt value of the attribute. It will be converted to a HBufC8
     *        internally, and new memory will be allocated locally for the new string.
     *
     * @leave Leaves if OOM
     */
    void ConstructL(const TInt aId, const TInt aVal);

public:

    /**
     * Returns the TPtrC8 to the attribute value
     * 
     * @return KNullDesC8 if no attrib value set, the value otherwise
     */
    TPtrC8    AttributeValue() const;

    /**
     * Returns the ID of the attribute. IDs are defined in 
     *
     * @return TInt ID of the attribute
     *
     */
    TInt      AttributeId() const;
    
    /**
     * Returns the attribute value converted to TInt
     *
     * @param aVal The TInt version of the parameter
     *
     * @leave Leaves if the value is not convertible to TInt
     */
    void      AttributeValueAsIntL(TInt& aVal) const;

private:
    TInt        iAttributeId;
    HBufC8*     iAttributeVal;
    };

/**
 * CVpnDevLockPolicy is a container class for device lock related
 * attributes. Attributes are stored as CVpnDevLockAttribute
 * objects in the private member variable iPolicyArray.
 *
 * Attributes can be accessed using public accessors.
 *
 */
class CVpnDevLockPolicy : public CBase 
    {
public:
    /**
     * Instantiates a new policy with a set of related attributes.
     * Policy levels are defined in SUB SUB 415-467. Currently,
     * policy levels 0..3 are supported.
     *
     * @return CVpnDevLockPolicy* the instantiated object.
     *
     * @leave Leaves if OOM, or if the requested policy level
     *        is not supported.
     */
    static CVpnDevLockPolicy* NewL(const TInt aPolLevel);

    /**
     * Instantiates a new policy with a set of related attributes.
     * Policy levels are defined in SUB SUB 415-467. Currently,
     * policy levels 0..3 are supported. The instantiated object
     * will remain in CleanupStack.
     *
     * @return CVpnDevLockPolicy* the instantiated object.
     *
     * @leave Leaves if OOM, or if the requested policy level
     *        is not supported.
     */
    static CVpnDevLockPolicy* NewLC(const TInt aPolLevel);

    /**
     * Instantiates a new "empty" policy. The instantiated object
     * will remain in CleanupStack.
     *
     * @return CVpnDevLockPolicy* the instantiated object.
     *
     * @leave Leaves if OOM.
     */
    static CVpnDevLockPolicy* NewLC();

    /**
     * Destructor cleans up the locally allocated/controlled resources
     */
    ~CVpnDevLockPolicy();

private:
    CVpnDevLockPolicy();

protected:
    void ConstructL(const TInt aPolLevel);
    void ConstructL();

public:

    /**
     * Returns the attribute count for the policy.
     *
     * @return TInt Total count of all defined 
     * attributes in the container
     */
    TInt                    AttributeCount();

    /**
     * Returns the Policy attribute at the given attribute array 
     * index, if one exists.
     *
     * @param aIdx The index of the attribute requested. Must be
     *        between 0 and AttributeCount() - 1, inclusively
     *
     * @return The requested attribute or NULL if one didn't exist
     *
     */
    CVpnDevLockAttribute*   GetPolicyAttributeAt(const TInt aIdx);

    /**
     * Add an individual attribute for policy.
     *
     * @param aAttrib Attribute ID from device lock attribute range
     * @param aVal The value to assign. Internally converted to a string.
     *
     * @leave Leaves if there is not enough memory; may leave,
     * if the attribute id is out of range.
     * 
     */
    void    SetPolicyAttributeL(const TInt aAttrib, const TInt aVal);

    /**
     * Add an individual attribute for policy
     *
     * @param aAttrib Attribute ID from device lock attribute range
     * @param aVal The value to assign.
     *
     * @leave Leaves if there is not enough memory; may leave,
     * if the attribute id is out of range.
     * 
     */
    void    SetPolicyAttributeL(const TInt aAttrib, const TDesC8& aVal);

private:

    /**
     * Sets pre-defined attribute values to the policy according to
     * policy level.
     *
     * @param aPolLevel The requested policy level. Valid values
     *        range is 0..3.
     *
     * @leave Leaves if there is not enough memory to allocate
     *        the attribute strings, or if the requested
     *        policy level is not supported (KErrNotSupported)
     */
    void SetPolicyLevelL(const TInt aPolLevel);

private:
    CArrayFixFlat<CVpnDevLockAttribute*>*   iPolicyArray;
    };

/**
 * The main class for device lock operations.
 */
class CVpnDevLockHandler : public CBase 
    {
public:
    
    static CVpnDevLockHandler* NewL();
    static CVpnDevLockHandler* NewLC();
    
    /**
     * Standard destructor
     *
     */
    ~CVpnDevLockHandler();

protected:
    void ConstructL();

private:
    CVpnDevLockHandler();

public:

    /**
     * Checks whether VPN has access to terminal control device lock
     *
     * @leave Leaves when OOM. Leaves if VPN can't access device
     *        lock settings properly
     */
    void    TerminalControlSupportsVpnL();

    /**
     * Sets the terminal control's device lock policy to desired level.
     * If there are some existing device lock attributes that are 
     * "stricter" than the new ones proposed by the policy level, 
     * those will be left intact. Otherwise, the new requested
     * values will be applied.
     *
     */
    void SetTerminalControlPolicyL(const TInt aPolLevel);

private:

    /**
     * Checks whether there is a need to override the old existing
     * device lock parameter value. Returns ETrue iff the new
     * suggested value must be applied.
     *
     * @param aNew The suggested new attribute
     *
     * @ret TBool ETrue, if the old setting was more lenient than the
     *            new one, and therefore needs to be overridden.
     *
     * @leave Leaves when OOM
     */
    TBool   OverrideExistingL(const CVpnDevLockAttribute& aNew);

    /**
     * Obtains from the device the current setting for the given 
     * attribute.
     *
     * @param aAttribId The ID of the attribute for which the value is 
     *        to be fetched.
     *
     * @return A HBufC8* string containing the requested attribute
     *         value. Memory control of the object passes to the 
     *         caller.
     *
     * @leave Leaves if OOM or if the device doesn't support
     *        the given attribute.
     */
    HBufC8*     GetDevAttribValueL(const TInt aAttribId);
    
    /**
     * Sets the device's attribute (indicated by parameter's attribute id)
     * to the value contained by the parameter.
     *
     * @param aNew A CVpnDevLockAttribute object containing the desired 
     *        new settings
     *
     * @leave Leaves if OOM or if the device doesn't support
     *        the given attribute.
     */
    void        SetDevAttributeL(const CVpnDevLockAttribute& aNew);

    /**
     * Dynamically load the device lock library (if it's available)
     *
     * @return KErrNone if the library was found and is usable
     * @return KErrNotFound if lib wasn't found or is not usable.
     */
    void        LoadTCLibraryL();

private:

    RLibrary                        iTcLib;

    TBool                           iDevLockAvailable;
    
    CVpnTcStub*                     iTcStub;
    };

#endif // C_VPNPOLICYINSTALLER_H

