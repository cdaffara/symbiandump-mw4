/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*/



// INCLUDE FILES

#include "WrtPlatform.h"
#include <sisregistryentry.h>
#include <sisregistrysession.h>
#include <sysutil.h> 
#include <etel.h>
#include <etelmm.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TUid KBrowserAppUid = { 0x10008d39 };
_LIT(KNotAvailable, "N/A");
_LIT(KPlatformId, "S60");
_LIT(KDefManufacturer, "N/A");
_LIT(KDefModel, "N/A");
_LIT(KDefSerialNumber, "N/A");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
using namespace KJS;
const ClassInfo JSWrtPlatform::info = { "JSWrtPlatform", 0, &WrtPlatformTable, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin WrtPlatformTable 4
    id JSWrtPlatform::id DontDelete|ReadOnly
    romVersion JSWrtPlatform::romVersion DontDelete|ReadOnly
    packageVersion JSWrtPlatform::packageVersion DontDelete|ReadOnly
    manufacturer JSWrtPlatform::manufacturer DontDelete|ReadOnly
    model JSWrtPlatform::model DontDelete|ReadOnly
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// JSWrtPlatform::JSWrtPlatform
// C++ constructor
//
//
// ----------------------------------------------------------------------------
JSWrtPlatform::JSWrtPlatform( ExecState* exec )
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()), 
      pltPriv(new WrtPlatformPrivate())
    {
    TRAP_IGNORE( pltPriv->m_sisVersion = SISVersionL() );
    TRAP_IGNORE( GetPhoneInfoL() );
    }

// ----------------------------------------------------------------------------
// JSWrtPlatform::~JSWrtPlatform
// Destructor
//
//
// ----------------------------------------------------------------------------
JSWrtPlatform::~JSWrtPlatform()
    {
    delete pltPriv;
    }

// ----------------------------------------------------------------------------
// JSWrtPlatform::type
//
//
//
// ----------------------------------------------------------------------------
JSType JSWrtPlatform::type() const 
{ 
    return ObjectType; 
}

// ----------------------------------------------------------------------------
// JSWrtPlatform::toString
// Returns string representation of the object
//
//
// ----------------------------------------------------------------------------
UString JSWrtPlatform::toString(ExecState *exec) const
    {
    return "[object JSWrtPlatform]";
    }

// ----------------------------------------------------------------------------
// JSWrtPlatform::getValueProperty
//
//
// ----------------------------------------------------------------------------
JSValue* JSWrtPlatform::getValueProperty(ExecState *exec, int token) const
    {
    switch( token )
        {
        case id: {                
            TPtrC platformId;
            platformId.Set( KPlatformId );
            return jsString( UString((UChar*) platformId.Ptr(), platformId.Length()) );
        }
        case romVersion: {                
            TPtrC versionName;
            TBuf<KSysUtilVersionTextLength> version;
            if ( SysUtil::GetSWVersion( version ) == KErrNone )
                {
                versionName.Set( version );
                }
            else
                {
                versionName.Set( KNotAvailable );
                }

            return jsString( UString((UChar*) versionName.Ptr(), versionName.Length()) );
        }
        case packageVersion: {                
            return jsString( pltPriv->m_sisVersion );        
        }
        case manufacturer: {                
            return jsString( pltPriv->m_manufacturer );        
        }
        case model: {                
            return jsString( pltPriv->m_model );        
        }
/*        case revision: {                
            return jsString( pltPriv->m_revision );        
        }
          case serialNumber: {                
            return jsString( pltPriv->m_serialNumber );        
        }
*/
        default:
            return throwError(exec, GeneralError);
        }
    }

// ----------------------------------------------------------------------------
// JSWrtPlatform::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool JSWrtPlatform::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = Lookup::findEntry(&WrtPlatformTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<JSWrtPlatform>);
        return true;
        }
   
    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

// ----------------------------------------------------------------------------
// JSWrtPlatform::SISVersionL
//
//
// ----------------------------------------------------------------------------
UString JSWrtPlatform::SISVersionL()
{
    Swi::RSisRegistrySession regSession;
    CleanupClosePushL(regSession);
    User::LeaveIfError( regSession.Connect() );

    Swi::RSisRegistryEntry entry;
    CleanupClosePushL(entry);

    TPtrC versionName;
    TInt err = entry.Open( regSession, KBrowserAppUid );
    if( err == KErrNone )
        {
        TVersion version;
        version = entry.VersionL();
        versionName.Set( version.Name() );
        }
    else
        {
        versionName.Set( KNotAvailable );
        }
        
    CleanupStack::PopAndDestroy( &entry );               
    CleanupStack::PopAndDestroy( &regSession );

    return UString( (UChar*) versionName.Ptr(), versionName.Length() );
}

// ----------------------------------------------------------------------------
// JSWrtPlatform::GetPhoneInfoL
//
//
// ----------------------------------------------------------------------------
void JSWrtPlatform::GetPhoneInfoL()
{
    TPtrC manufacturer;
    manufacturer.Set( KDefManufacturer );
    pltPriv->m_manufacturer = UString( (UChar*) manufacturer.Ptr(), manufacturer.Length() );

    TPtrC model;
    model.Set( KDefModel );
    pltPriv->m_model = UString( (UChar*) model.Ptr(), model.Length() );

//    TPtrC revision;
//    revision.Set( KDefRevision );
//    pltPriv->m_revision = UString( (UChar*) revision.Ptr(), revision.Length() );

//    TPtrC serialNumber;
//    serialNumber.Set( KDefSerialNumber );
//    pltPriv->m_serialNumber = UString( (UChar*) serialNumber.Ptr(), serialNumber.Length() );

    // Read IMEI on HW, use dummy under WINS
#if !defined( __WINS__ )
    RTelServer server;
    User::LeaveIfError(server.Connect()); 
    CleanupClosePushL(server);
    TInt numPhones;
    User::LeaveIfError(server.EnumeratePhones(numPhones));
    if( numPhones > 0 )
    {
        RTelServer::TPhoneInfo phoneInfo;
        User::LeaveIfError(server.GetPhoneInfo(0, phoneInfo)); // Query first phone
        RMobilePhone phone;
        User::LeaveIfError(phone.Open(server, phoneInfo.iName));
        CleanupClosePushL(phone);
        User::LeaveIfError(phone.Initialise()); 
        TUint32 identityCaps;
        phone.GetIdentityCaps( identityCaps );
        RMobilePhone::TMobilePhoneIdentityV1 id;
        TRequestStatus status;
        phone.GetPhoneId( status, id );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );
        if (identityCaps & RMobilePhone::KCapsGetManufacturer)
            {
            manufacturer.Set( id.iManufacturer );
            pltPriv->m_manufacturer = UString( (UChar*) manufacturer.Ptr(), manufacturer.Length() );
            }
        if (identityCaps & RMobilePhone::KCapsGetModel)
            {
            model.Set( id.iModel );
            pltPriv->m_model = UString( (UChar*) model.Ptr(), model.Length() );
            }
/*        if (identityCaps & RMobilePhone::KCapsGetRevision)
            {
            revision.Set( id.iRevision );
            pltPriv->m_revision = UString( (UChar*) revision.Ptr(), revision.Length() );
            }
        if (identityCaps & RMobilePhone::KCapsGetSerialNumber)
            {
            serialNumber.Set( id.iSerialNumber );
            pltPriv->m_serialNumber = UString( (UChar*) serialNumber.Ptr(), serialNumber.Length() );
            }
*/
        CleanupStack::PopAndDestroy(); // phone
     }
     CleanupStack::PopAndDestroy(); // server
#else

#endif
}

//END OF FILE




