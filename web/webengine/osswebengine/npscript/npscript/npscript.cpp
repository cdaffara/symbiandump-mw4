/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <npruntime.h>
#include <Browser_platform_variant.hrh>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__S60_32__)
EXPORT_C void NPN_ReleaseVariantValue (NPVariant *variant)
{
    _NPN_ReleaseVariantValue (variant);
}
#endif // __S60_32__

EXPORT_C NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name)
{
    return _NPN_GetStringIdentifier(name);
}

EXPORT_C void NPN_GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
    _NPN_GetStringIdentifiers(names, nameCount, identifiers);
}

EXPORT_C NPIdentifier NPN_GetIntIdentifier(int32_t intid)
{
    return _NPN_GetIntIdentifier(intid);
}

EXPORT_C bool NPN_IdentifierIsString(NPIdentifier identifier)
{
    return _NPN_IdentifierIsString(identifier);
}

EXPORT_C NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
    return _NPN_UTF8FromIdentifier(identifier);
}

EXPORT_C int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
    return _NPN_IntFromIdentifier(identifier);
}

EXPORT_C NPObject *NPN_CreateObject(NPP npp, NPClass *aClass)
{
    return _NPN_CreateObject(npp, aClass);
}

EXPORT_C NPObject *NPN_RetainObject (NPObject *obj)
{
    return _NPN_RetainObject (obj);
}

EXPORT_C void NPN_ReleaseObject (NPObject *obj)
{
    return _NPN_ReleaseObject (obj);
}

EXPORT_C bool NPN_Invoke(NPP npp, NPObject *npobj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return _NPN_Invoke(npp, npobj, methodName, args, argCount, result);
}

EXPORT_C bool NPN_InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return _NPN_InvokeDefault(npp, npobj, args, argCount, result);
}

EXPORT_C bool NPN_Evaluate(NPP npp, NPObject *npobj, NPString *script, NPVariant *result)
{
    return _NPN_Evaluate(npp, npobj, script, result);
}

EXPORT_C bool NPN_GetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName, NPVariant *result)
{
    return _NPN_GetProperty(npp, npobj, propertyName, result);
}

EXPORT_C bool NPN_SetProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName, const NPVariant *value)
{
    return _NPN_SetProperty(npp, npobj, propertyName, value);
}

EXPORT_C bool NPN_RemoveProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
    return _NPN_RemoveProperty(npp, npobj, propertyName);
}

EXPORT_C bool NPN_HasProperty(NPP npp, NPObject *npobj, NPIdentifier propertyName)
{
    return _NPN_HasProperty(npp, npobj, propertyName);
}

EXPORT_C bool NPN_HasMethod(NPP npp, NPObject *npobj, NPIdentifier methodName)
{
    return _NPN_HasMethod(npp, npobj, methodName);
}

EXPORT_C bool NPN_Enumerate(NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
    return _NPN_Enumerate(npp, npobj, identifier, count);
}

EXPORT_C void NPN_SetException (NPObject *obj, const NPUTF8 *message)
{
    _NPN_SetException (obj, message);
}


#ifdef __cplusplus
}
#endif


//  End of File
