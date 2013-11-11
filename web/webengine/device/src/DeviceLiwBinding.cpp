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
* Description:  Implementation of Liw Device binding
*
*/


#include <config.h>
#include <value.h>
#include <interpreter.h>
#include <date_object.h>
#include <operations.h>
#include <liwservicehandler.h>
#include <rtsecmanager.h>
#include <rtsecmgrutility.h>
#include <rtsecmgrscriptsession.h>
#include <rtsecmgrcommondef.h>
#include <WidgetRegistryClient.h>
#include <PropertyNameArray.h>
#include <internal.h>
#include <liwvariant.h>
#include <liwbufferextension.h>

#include "DeviceLiwBinding.h"
#include "DeviceLiwPeer.h"
#include "DeviceBinding.h"
#include "ServiceObject.h"
#include "ServiceEventHandler.h"
#include "DeviceLiwInterface.h"
#include "DeviceLiwMap.h"
#include "DeviceLiwIterable.h"
#include "DeviceLiwResult.h"

using namespace KJS;
using namespace LIW;

// CONSTANTS
#define SAPISECURITYPROMPTNEEDED 0x0001
#define SAPIPROMPTLESS           0x0002
#define SAPIACCESSDENIED         0x0003

// TBD: This needs to be substituted with LIW const in LIW header
const TInt KMaxKeySize = 255;
_LIT8( KWildChar, "*" );

const TInt KTimeBufSize = 255;
_LIT( KLocaleDateTime, "%E,%D%N%Y  %1 %2, %3%-B %:0%J%:1%T%:2%S%+B" );

// JS time epoch 1st January 1970 (needed to offset from Symbian epoch)
_LIT(KJsEpoch, "19700000:000000.000000");


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::NewL
// NewL
//
// ----------------------------------------------------------------------------
//
CDeviceLiwBinding* CDeviceLiwBinding::NewL()
    {
    CDeviceLiwBinding* bind = new (ELeave) CDeviceLiwBinding();
    CleanupStack::PushL(bind);
    bind->ConstructL();
    CleanupStack::Pop(bind);
    return bind;
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::ConstructL
// ConstructL sencond phace construction.
//
// ----------------------------------------------------------------------------
//
void CDeviceLiwBinding::ConstructL()
    {
        m_critArr = new (ELeave) RCriteriaArray();
        m_serviceHandler = CLiwServiceHandler::NewL();

        RWidgetRegistryClientSession widgetregistry;
        TInt ret = widgetregistry.Connect();
        if ( ret != KErrNone && ret != KErrAlreadyExists )
            {
            User::Leave( ret );
            }
        else
            {
            CleanupClosePushL( widgetregistry );
            }
        TInt aPolicyId = widgetregistry.SecurityPolicyId();
        if ( aPolicyId < 0 )
            {
            User::Leave( aPolicyId );
            }
        User::LeaveIfError(widgetregistry.Disconnect());
        CleanupStack::PopAndDestroy(); //widgetregistry

        // we need to keep main session allive, to keep sub session
        m_secMgr = CRTSecManager::NewL();

        CTrustInfo* trust = CTrustInfo::NewLC();

        // TODO: in future, to support blanket permission,
        // this function should be called at installation time.
        // TInt32 exId = secMgr->RegisterScript( aPolicyId, *trust );
        // m_scriptSession = secMgr->GetScriptSession( aPolicyId, exId );

        // check for script session validity.
        m_scriptSession = m_secMgr->GetScriptSessionL( aPolicyId, *trust );
        if ( !m_scriptSession )
            {
            User::Leave( KErrGeneral );
            }
#ifdef BRDO_SEC_MGR_PROMPT_ENHANCEMENT_FF
		m_scriptSession->SetPromptOption(RTPROMPTUI_PROVIDER);   //  This is for setting the new prompting method
#endif
        CleanupStack::PopAndDestroy( trust );
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::~CDeviceLiwBinding
// Default destructor
//
// ----------------------------------------------------------------------------
//
CDeviceLiwBinding::~CDeviceLiwBinding()
    {
    m_critArr->ResetAndDestroy();
    m_serviceHandler->DetachL(*m_critArr);
    m_critArr->ResetAndDestroy();
    delete m_critArr;
    delete m_serviceHandler;
    delete m_scriptSession;
    delete m_secMgr;
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::LoadServiceProvider
// Load service provider
//
// ----------------------------------------------------------------------------
//
TInt CDeviceLiwBinding::LoadServiceProvider( ExecState* exec, const List& args )
    {
    TInt error = KErrNone;
    CLiwCriteriaItem* item;
    HBufC8* svcName = NULL;
    HBufC8* interfaceName = NULL;
    int argcount = args.size();

    if ( argcount > 0 && args[0]->type() == StringType &&
        args[0]->toString( exec ).size() > 0 )
        {
            TRAP( error,
            {
            // Get service name
            svcName = KJS::GetAsciiBufferL( args[0]->toString( exec ) );
            CleanupStack::PushL( svcName );

            // Get optional supported interface
            if ( argcount > 1 && args[1]->type() == StringType &&
                args[1]->toString( exec ).size() > 0 )
                {
                interfaceName = KJS::GetAsciiBufferL( args[1]->toString( exec ) );
                CleanupStack::PushL( interfaceName );
                }

            if ( interfaceName && interfaceName->Length() > 0 )
                {
                item = CLiwCriteriaItem::NewLC( 1, *interfaceName, *svcName );
                }
            else
                {
                item = CLiwCriteriaItem::NewLC( 1, KNullDesC8(), *svcName );
                }

            item->SetServiceClass( TUid::Uid( KLiwClassBase ) );
            RCriteriaArray crit_arr;
            crit_arr.AppendL( item );

            CleanupStack::Pop(item); // item

            // "Attach" runtime to service
            if ( m_scriptSession && m_serviceHandler )
                {
                TInt load_err;
                RWidgetRegistryClientSession widgetregistry;
                TInt ret = widgetregistry.Connect();
                if ( ret != KErrNone && ret != KErrAlreadyExists )
                    {
                    User::Leave( ret );
                    }
                else
                   {
                   CleanupClosePushL( widgetregistry );
                   }

                switch ( widgetregistry.WidgetSapiAccessState(m_Uid))
                    {
                    case SAPISECURITYPROMPTNEEDED :
                        load_err = m_serviceHandler->AttachL( crit_arr, *m_scriptSession );
                        break;
                    case SAPIPROMPTLESS :
                        load_err = m_serviceHandler->AttachL( crit_arr );
                        break;
                    case SAPIACCESSDENIED :
                        load_err = KLiwSecurityAccessCheckFailed;
                        break;
                    default :
                        load_err = KLiwSecurityAccessCheckFailed;
                        break;
                    }

                User::LeaveIfError(widgetregistry.Disconnect());
                CleanupStack::PopAndDestroy(); //widgetregistry
                // hard coded for now since the definition of TLiwLoadStatus is not exposed to common dir yet
                if ( load_err == KLiwServiceLoadSuccess || load_err == KLiwServiceAlreadyLoaded )
                    {
                    error = KErrNone; // normalize the error
                    m_critArr->AppendL( item );
                    }
                else
                    {
                    error = load_err; // pass on the TLiwLoadStatus
                    delete item;
                    }
                }

            if ( interfaceName )
                {
                CleanupStack::PopAndDestroy( interfaceName );
                }

            CleanupStack::PopAndDestroy( svcName );
            });
        }
    else
        {
        error = KErrArgument;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// Convert Unload service provider
// return JSValue - javascript list
// ---------------------------------------------------------------------------
//
void CDeviceLiwBinding::UnloadServiceProvider(const TDesC8& aServiceName,const TDesC8& aInterfaceName)
    {
    TRAP_IGNORE(
        CLiwCriteriaItem* item = CLiwCriteriaItem::NewLC( 1, aInterfaceName, aServiceName );
        item->SetServiceClass( TUid::Uid( KLiwClassBase ) );
        RCriteriaArray critArray;
        critArray.AppendL( item );
        m_serviceHandler->DetachL( critArray );
        CleanupStack::Pop(item);
        critArray.ResetAndDestroy();
        for (int i = 0; i < m_critArr->Count(); i++)
            {
            if ( (*m_critArr)[i]->ServiceCmdStr() == aInterfaceName &&
                 (*m_critArr)[i]->ContentType() == aServiceName)
                m_critArr->Remove(i);
            }
        )
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::ServiceObjectPeer
// Factory method to create a peer with service name
//
//
// ----------------------------------------------------------------------------
//
MDevicePeer* CDeviceLiwBinding::ServiceObjectPeer(
    ExecState *exec,
    const List& args )
    {
    DeviceLiwPeer* peer = NULL;

    TRAPD( error,
        {
        peer = new ( ELeave ) DeviceLiwPeer(
            exec->lexicalInterpreter()->globalExec(), this, 0 );
        CleanupStack::PushL( peer );
        peer->SetServiceNameL( exec, args );
        CleanupStack::Pop(peer);
        });

    if ( error != KErrNone )
        {
        return 0;
        }

    return static_cast<MDevicePeer*>( peer );
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::ListProviders
// Call/Invoke the LIW function to list the providers
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::ListProviders(
    ExecState* exec,
    const List& args )
    {
    CLiwCriteriaItem* item = NULL;
    HBufC8* cmdName = NULL;
    HBufC8* contentName = NULL;
    int argcount = args.size();

    // Get content name
    if ( argcount > 0 && args[0]->type() == StringType &&
        args[0]->toString( exec ).size() > 0 )
        {
        contentName = KJS::GetAsciiBufferL( args[0]->toString( exec ) );
        CleanupStack::PushL( contentName );
        }

    if ( argcount > 1 && args[1]->type() == StringType &&
        args[1]->toString( exec ).size() > 0 )
        {
        // Get cmd name
        cmdName = KJS::GetAsciiBufferL( args[1]->toString( exec ) );
        CleanupStack::PushL( cmdName );
        }

    if ( cmdName && cmdName->Length() > 0 )
        {
        if ( contentName && contentName->Length() > 0 )
            {
            item = CLiwCriteriaItem::NewLC( 1, *cmdName, *contentName );
            }
        else
            {
            item = CLiwCriteriaItem::NewLC( 1, *cmdName, KWildChar() );
            }
        }
    else
        {
        if ( contentName && contentName->Length() > 0 )
            {
            item = CLiwCriteriaItem::NewLC( 1, KWildChar(), *contentName );
            }
        else
            {
            item = CLiwCriteriaItem::NewLC( 1, KWildChar(), KWildChar() );
            }
        }

    RCriteriaArray critArray, providerList;
    item->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    critArray.AppendL( item );
    m_serviceHandler->QueryImplementationL( critArray, providerList );
    CleanupStack::Pop();//item

    if ( contentName )
        {
        CleanupStack::PopAndDestroy();//contentName
        }

    if ( cmdName )
        {
        CleanupStack::PopAndDestroy();//cmdName
        }

    if ( providerList.Count() > 0 )
        {
        // convert to js value
        List jsList;
        jsList.append( jsNumber( providerList.Count() ) );
        JSObject * rval = exec->lexicalInterpreter()->builtinArray()->construct(
                                                            exec, jsList );

        for ( TInt i = 0; i < providerList.Count(); i++ )
            {
            List jsPair;
            jsPair.append( jsNumber( 2 ) );
            JSObject * pairVal = exec->lexicalInterpreter()->builtinArray()->construct(
                                                            exec, jsPair );

            HBufC* buf1 = HBufC::NewLC( providerList[i]->ServiceCmdStr().Length() + 1 ); // +1 for zero terminate
            buf1->Des().Copy( providerList[i]->ServiceCmdStr() );
            buf1->Des().ZeroTerminate();
            pairVal->put( exec, Identifier::from(0), jsString( UString( (UChar*) buf1->Des().Ptr(), buf1->Des().Length() ) ) );

            HBufC* buf2 = HBufC::NewLC( providerList[i]->ContentType().Length() + 1 ); // +1 for zero terminate
            buf2->Des().Copy( providerList[i]->ContentType() );
            buf2->Des().ZeroTerminate();
            pairVal->put( exec, Identifier::from(1),
            jsString( UString( (UChar*) buf2->Des().Ptr(), buf2->Des().Length() ) ) );

            rval->put( exec, Identifier::from(i), pairVal );
            CleanupStack::PopAndDestroy(2);//buf1,buf2
            }

        critArray.ResetAndDestroy();
        providerList.ResetAndDestroy();

        return( rval );
        }

    critArray.ResetAndDestroy();
    providerList.ResetAndDestroy();
    return jsUndefined();
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::InvokeOpInternal
// Call/Invoke the SAPI function
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::InvokeOpInternal(
    ExecState* exec,
    const Identifier& propertyName,
    const List& args,
    MDevicePeer* peer )
    {
    TUint cmdOption = 0;
    JSValue* rval = jsUndefined();
    int argcount = args.size();
    DeviceLiwPeer* callBack = NULL;

    if ( argcount > 1 )
        {
        cmdOption = KLiwOptASyncronous;
        if( args[1]->isObject() )
            {
            callBack = static_cast<DeviceLiwPeer*>( peer );
            }
        }
    if (propertyName == "Cancel")
        {
        cmdOption = KLiwOptCancel;
        }
    // Must have at least an operation name
    if ( propertyName.ustring().size() > 0 )
        {
        // Operation on a service interface object
        if ( peer && static_cast<DeviceLiwPeer*>( peer )->Interface() )
            {
            TRAPD( error,
                {
                CLiwGenericParamList* inps = CLiwGenericParamList::NewL(); // We manage this memory
                CleanupStack::PushL( inps );
                CLiwGenericParamList* outps = CLiwGenericParamList::NewL(); // And this
                CleanupStack::PushL( outps );

                // Collect args from first parameter. Note that args are
                // passed as an associative array.
                if ( argcount >= 1 )
                    {
                    JsList2LiwGenericParamListL( exec, args[0], *inps );
                    }

                // Get the operation name
                HBufC8* oper = KJS::GetAsciiBufferL( propertyName.ustring() );
                CleanupStack::PushL( oper );

                // Call the interface operation

                TTrapHandler* ____t = User::MarkCleanupStack();
                static_cast<DeviceLiwPeer*>( peer )->Interface()->ExecuteCmdL(
                                            *oper, *inps, *outps, cmdOption,
                                            callBack );
                User::UnMarkCleanupStack(____t);
                inps->Reset();
                rval = LiwGenericParamList2JsArray( exec, outps, ETrue );
                outps->Reset();
                CleanupStack::PopAndDestroy( oper ); // map
                CleanupStack::Pop();// outps
                CleanupStack::PopAndDestroy(  );// inps
               });

            if ( error != KErrNone )
                {
                return throwError(exec, GeneralError);
                }
            }
        }

    return rval;
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::InvokeOp
// Call/Invoke the SAPI function
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::InvokeOp(
    ExecState* exec,
    const Identifier& propertyName,
    const List& args,
    MDevicePeer* peer )
    {
    return InvokeOpInternal( exec, propertyName, args, peer );
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::CreateInterface
// Create SAPI interface
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::CreateInterface(
    ExecState* exec, HBufC8* serviceName, const Identifier& interfaceName )
    {
    JSValue* rval = jsUndefined();
    TInt error = KErrNone;

    if ( m_serviceHandler && serviceName->Length() > 0 && interfaceName.ustring().size() > 0 )
        {
        // Since this is called from JS context, set up our own trap harness
        TRAP( error,
            {
            CLiwGenericParamList& inps = m_serviceHandler->InParamListL(); // LIW manages this memory
            CleanupStack::PushL( &inps );
            CLiwGenericParamList& outps = m_serviceHandler->OutParamListL(); // LIW manages this memory
            CleanupStack::PushL( &outps );

            HBufC8* ifaceName = KJS::GetAsciiBufferL( interfaceName.ustring() );
            CleanupStack::PushL( ifaceName );

            const HBufC8* hsvc = serviceName;

            // Create LIW params
            CLiwCriteriaItem* item = CLiwCriteriaItem::NewLC( 1, *ifaceName,
                                        TPtrC8( hsvc->Ptr(), hsvc->Size() ) );

            item->SetServiceClass( TUid::Uid( KLiwClassBase ) );

            // Call the service command operation
            m_serviceHandler->ExecuteServiceCmdL(
                *item, inps, outps, NULL );

            // Convert results into JS array
            rval = LiwGenericParamList2JsArray( exec, &outps );
            CleanupStack::PopAndDestroy( 2 ); //item, ifaceName
            CleanupStack::Pop( 2 );//inps, outps
        });

        if ( error != KErrNone)
            {
            return throwError(exec, GeneralError);
            }
        }

    return rval;
    }


//TBD: replace this with a generic object type discovery method

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::IsAList
// Check if javascript list
//
//
// ----------------------------------------------------------------------------
//
TBool CDeviceLiwBinding::IsAList( ExecState* exec, JSValue* aValue )
    {
    if(!aValue->isObject())
        return EFalse;
    PropertyNameArray propertyNames;
    aValue->toObject( exec )->getPropertyNames( exec, propertyNames );
    unsigned size = static_cast<unsigned>(propertyNames.size());

    for (unsigned i = 0; i < size; i++)
    {
        bool rval;
        propertyNames[i].toArrayIndex( &rval );
        if ( rval )
            {
            return ETrue;
            }
    }

    return EFalse;
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::JsVal2LiwVariant
// Convert from javascript value to LiwVariant
//
//
// ----------------------------------------------------------------------------
//
TBool CDeviceLiwBinding::JsVal2LiwVariant(
    ExecState* exec,
    JSValue* aValue,
    TLiwVariant& variant )
    {
    TBool ret = ETrue;
    TInt error = KErrNone;

    switch ( aValue->type() )
        {
        case StringType:
            {
            // Pass as TPtrC and let SAPI do the conversion to 8 bit if needed.
            TRAP( error,
                {
                variant.SetL( TLiwVariant(
                    TPtrC16( ( unsigned short* ) aValue->toString( exec ).data(),
                    aValue->toString( exec ).size() ) ) );
                });
            break;
            }
        case NumberType:
            {
            if (isNaN(aValue->getNumber()) || isInf(aValue->getNumber()))
            {
                ret = EFalse;
                break;
            }
            TInt32 intVal = aValue->toInt32( exec );
            double doubleVal = aValue->toNumber( exec );
            if ( intVal != doubleVal )
            {
                variant.Set( (TReal)doubleVal );
            }
            else
            {
                // this is not fully correct due to the limitation of js number
                variant.Set( intVal );
            }
            break;
            }
        case ObjectType:
            {
            JSObject* anObj = aValue->toObject( exec );

            // Handle date
            if(anObj->className().find(UString("Date"),0) == 0)
                {
                double d = aValue->toNumber( exec );  // get UTC milli seconds
                if( isNaN(d) || isInf(d) )
                {
                    ret = EFalse;
                    break;
                }

                //Get the JS time in integer seconds
                const TInt64 dateVal = d;

                // Set JS epoch offset from Symbian epoch
                TTime sTime(KJsEpoch);
                TTimeIntervalMicroSeconds dateMicroSecs(dateVal*1000);  // convert to UTC micro seconds
                sTime += dateMicroSecs;

                // uncomment following to debug time
                //TBuf<100> date;
                //sTime.FormatL(date,KLocaleDateTime);

                // Set the LIW Variant time
                variant.Set( sTime );

                break;
            }
            // Boolean object
            else if(anObj->className().find(UString("Boolean"),0) == 0)
                {
                TBool val = aValue->toNumber( exec );
                variant.Set( val );
                break;
                }
            // Number object
            else if(anObj->className().find(UString("Number"),0) == 0)
                {
                double d = aValue->toNumber( exec );
                if (isNaN(d) || isInf(d))
                    {
                    ret = EFalse;
                    break;
                    }
                TInt32 intVal = aValue->toInt32( exec );
                if ( intVal != d )
                    {
                    variant.Set( (TReal)d );
                    }
                else
                    {
                    // this is not fully correct due to the limitation of js number
                    variant.Set( intVal );
                    }
                break;
                }
            // String object
            else if(anObj->className().find(UString("String"),0) == 0)
                {
                // Pass as TPtrC and let SAPI do the conversion to 8 bit if needed.
                TRAP( error,
                    {
                    variant.SetL( TLiwVariant(
                        TPtrC16( ( unsigned short* ) aValue->toString( exec ).data(),
                        aValue->toString( exec ).size() ) ) );
                    });
                break;
                }
            PropertyNameArray prop_names;
            aValue->toObject( exec )->getPropertyNames( exec, prop_names );
            unsigned size = static_cast<unsigned>(prop_names.size());

            // handle array
            if ( IsAList( exec, aValue ) )
                {
                CLiwList* liwList = NULL;

                TRAP( error, { liwList = CLiwDefaultList::NewL(); } );
                if ( error != KErrNone )
                    {
                    break;
                    }
                CleanupStack::PushL( liwList );

                for (unsigned i = 0; i < size; i++)
                    {
                    bool rval;
                    prop_names[i].toArrayIndex( &rval );

                    if ( rval )
                        {
                        TLiwVariant vv;
                        JSValue* v = aValue->toObject( exec )->get(exec, prop_names[i]);

                        if ( JsVal2LiwVariant( exec, v, vv ) )
                            {
                            TRAP( error, { liwList->AppendL( vv ); } );  // makes a copy
                            vv.Reset();

                            if ( error != KErrNone )
                                {
                                break; //from while
                                }
                            }
                        }
                    }//end of while

                if ( error == KErrNone )
                    {
                    if( liwList->Count() ) // do not set variant if list is empty
                        {
                        TRAP( error, { variant.SetL( liwList ); } );
                        }
                    }

                CleanupStack::Pop( liwList );
                liwList->DecRef();
                }
            // handle other objects
            else
                { // Map
                TRAP( error,
                    {
                    CLiwMap* liwMap = CLiwDefaultMap::NewL();
                    CleanupClosePushL( *liwMap );

                    for (unsigned i = 0; i < size; i++)
                        {
                        HBufC8* keyName = KJS::GetAsciiBufferL(
                                        prop_names[i].ustring() );
                        CleanupStack::PushL( keyName );

                        TLiwVariant vv;
                        JSValue* v = aValue->toObject( exec )->get(exec, prop_names[i]);

                        if (!keyName || !keyName->Length() )
                            continue;

                        if ( !JsVal2LiwVariant( exec, v, vv )  )
                            {
                            // error converting js value to liw variant, throw debugging error
                            UString err_txt( "Invalid value:[" );
                            err_txt.append( v->toString(exec) );
                            err_txt.append( "] for key:[" );
                            err_txt.append( (const char *)keyName->Des().PtrZ() );
                            err_txt.append( "].\0" );
                            throwError( exec, GeneralError, err_txt );
                            }
                            else if ( !vv.IsEmpty() )
                            {
                            liwMap->InsertL( *keyName, vv );//makes a copy
                            vv.Reset();
                            }

                        CleanupStack::PopAndDestroy( keyName );
                        }

                    if( liwMap->Count() ) //do notset variant if map is empty
                        {
                        variant.SetL( liwMap );
                        }
                    CleanupStack::PopAndDestroy(); // this will call liwMap->close
                    });
                }
            break;
            }
        case BooleanType:
            {
            TBool val = aValue->toNumber( exec );
            variant.Set( val );
            break;
            }
        case NullType:
        case UnspecifiedType:
        case UndefinedType:
        default:
            ret = EFalse;
            break;
        }

    if ( error != KErrNone )
        {
        ret = EFalse;
        }

    return ret;
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::DeviceLiwVariant2JsVal
// Convert from LiwVariant to javascript value of the same type
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::LiwVariant2JsVal(
    ExecState* exec,
    const TLiwVariant& variant )
    {
    switch ( variant.TypeId() )
        {
        case EVariantTypeTInt32:
            {
            return jsNumber( variant.AsTInt32() );
            }
        case EVariantTypeDesC:
            {
            return jsString( UString(
                (UChar*) variant.AsDes().Ptr(), variant.AsDes().Length() ) );
            }
        case EVariantTypeDesC8:// this is something wrong
            {
            JSValue* rval = jsUndefined();
            TRAPD( error,
                {
                HBufC* buf = HBufC::NewLC( variant.AsData().Length() + 1 ); // +1 for zero terminate
                TPtr bufDes = buf->Des();
                bufDes.Copy( variant.AsData() );
                bufDes.ZeroTerminate();
                rval = jsString( UString( (UChar*) bufDes.Ptr(), bufDes.Length() ) );
                CleanupStack::PopAndDestroy();//buf
                });
                return rval;
            }
        case EVariantTypeList:
            {
            TLiwVariant v;
            List jsList;
            jsList.append( jsNumber( variant.AsList()->Count() ) );
            JSObject * rval = exec->lexicalInterpreter()->builtinArray()->construct(
                exec, jsList );

            for ( TInt i = 0; i < variant.AsList()->Count(); i++ )
                {
                TRAPD( error, { variant.AsList()->AtL( i, v ); } );
                rval->put(
                    exec, Identifier::from(i), LiwVariant2JsVal( exec, v ) );
                }
            v.Reset();
            return rval;
            }
        case EVariantTypeMap:
            {
            TLiwVariant v;
            List jsList;
            jsList.append( jsNumber( variant.AsMap()->Count() ) );
            JSObject * rval = new DeviceLiwMap(exec->lexicalInterpreter()->builtinArray()->construct(
                exec, jsList ), variant.AsMap(), this);

            DevicePrivateBase* pMapData = (static_cast<DeviceLiwMap*> (rval))->getMapData();

            for ( TInt i = 0; i < variant.AsMap()->Count(); i++ )
                {
                TBuf8<KMaxKeySize> name;
                TRAPD( error,
                    {
                    if ( variant.AsMap()->AtL( i, name ) )
                        {
                        variant.AsMap()->FindL( name, v );
                        JSValue* jsval = LiwVariant2JsVal( exec, v );
                        rval->put( exec,
                                   Identifier( ( const char* ) name.PtrZ() ),
                                   jsval );
                        if ( v.TypeId() == EVariantTypeIterable )
                            {
                            DeviceLiwIterable* itObj = static_cast<DeviceLiwIterable*> (jsval);
                            DevicePrivateBase* itData = itObj->getIterableData();
                            itData->SetParent( pMapData );
                            pMapData->AddChild( itData );
                            }
                        else if ( v.TypeId() == EVariantTypeMap )
                            {
                            DeviceLiwMap* mapObj = static_cast<DeviceLiwMap*> (jsval);
                            DevicePrivateBase* mapData = mapObj->getMapData();
                            mapData->SetParent( pMapData );
                            pMapData->AddChild( mapData );
                            }
                        }
                    });
                // No error processing
                }
            v.Reset();
            return rval;
            }
        case EVariantTypeInterface:
            {
            // new DeviceLiwPeer() might fail, in which case the service JSObject * peer will be null
            DeviceLiwPeer* peer = new DeviceLiwPeer(
                                exec->lexicalInterpreter()->globalExec(),
                                this, variant.AsInterface() );
            if ( !peer )
                {
                return jsUndefined();
                }

            JSObject* impl = new DeviceLiwInterface( exec, this, peer );
            return impl;
            }
        case EVariantTypeIterable:
            {
            return new DeviceLiwIterable(exec, this, variant.AsIterable());
            }
        case EVariantTypeTReal:
            {
            TReal realValue;
            variant.Get( realValue );
            return jsNumber( realValue );
            }
        case EVariantTypeTBool:
            {
            TBool boolValue;
            variant.Get( boolValue );
            return jsBoolean( boolValue );
            }
            //Note that unmarshaling to String here is not symetrical with marshaling from Date
            //but JS application can easily construct a Date if it wants one
        case EVariantTypeTTime:
            {
            TTime UTCTime;
            TBuf<KTimeBufSize> buf;
            variant.Get( UTCTime );

            TTime utcNow;
            utcNow.UniversalTime();
            TTime localNow;
            localNow.HomeTime();
            // convert UTC time to local time
            TTimeIntervalMicroSeconds diff = localNow.MicroSecondsFrom(utcNow);
            TTime localTime = UTCTime + diff;
            // format the time to output string
            localTime.FormatL( buf, KLocaleDateTime );
            const UChar *p = (const UChar*)buf.Ptr();
            return jsString( UString( p, buf.Length() ) );
            }
        case EVariantTypeBuffer: // this is something i don't what to do
            {
            return jsUndefined();
                /*
            CLiwBuffer* buf = variant.AsBuffer();
            switch (buf->TypeID())
                {
                case KLiwBufferFile:
                    {
                    RFile file = ((CLiwFileBuffer*)buf)->AsFile();
                    TFileName fileName;
                    file.FullName(fileName);
                    return jsString( UString((UChar*) fileName.Ptr(), fileName.Length() ) );
                    }
                case KLiwBufferBitmap:
                    {
                    return jsUndefined();
                    }
                default:
                    return jsUndefined();
                }*/
            }
        case EVariantTypeTUid:
        case EVariantTypeFileHandle:
        default:
            return jsUndefined();
        }
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::JsList2LiwGenericParamListL
// Convert javascript array (list) to LiwGenericParamList
//
//
// ----------------------------------------------------------------------------
//
void CDeviceLiwBinding::JsList2LiwGenericParamListL(
    ExecState* exec,
    JSValue* aValue,
    CLiwGenericParamList& aLiwList )
    {
    if (!aValue->isObject())
        return;
    PropertyNameArray propertyNames;
    aValue->toObject( exec )->getPropertyNames( exec, propertyNames );
    int size = static_cast<unsigned>(propertyNames.size());

    for (unsigned i = 0; i < size; i++)
        {
        HBufC8* name = KJS::GetAsciiBufferL(propertyNames[i].ustring() );
        if ( !name->Length() )
            break;
        CleanupStack::PushL( name );
        JSValue* v = aValue->toObject( exec )->get( exec, propertyNames[i] );;
        TLiwVariant vv;

        if (  !JsVal2LiwVariant( exec, v, vv ) ) // error occurred
            {
            throwError(exec, GeneralError);
            }
        else if ( !vv.IsEmpty() )
            {
            aLiwList.AppendL( TLiwGenericParam( *name, vv ) );
            vv.Reset();
            }

        CleanupStack::PopAndDestroy(); //name
        }
    }


// ----------------------------------------------------------------------------
// CDeviceLiwBinding::LiwGenericParamList2JsArray
// Convert LiwGenericParamList to javascript array (list)
//
//
// ----------------------------------------------------------------------------
//
JSValue* CDeviceLiwBinding::LiwGenericParamList2JsArray(
    ExecState* exec,
    CLiwGenericParamList* aLiwList, TBool managed )
    {
    // the output param list should not be empty, if it is, return an undefined js obj.
    if ( aLiwList->Count() == 0 )
        return jsUndefined();

    List jsList;

    jsList.append( jsNumber( aLiwList->Count() ) );

    JSObject * rval = new DeviceLiwResult(exec->lexicalInterpreter()->builtinArray()->construct(
                exec, jsList ));

    DevicePrivateBase* retData =(static_cast<DeviceLiwResult*> (rval))->getResultData();

    for ( TInt i = 0; i < aLiwList->Count(); i++ )
        {
        TBuf8<KMaxKeySize> name( (*aLiwList)[i].Name() );
        JSValue* jsval = LiwVariant2JsVal( exec, (*aLiwList)[i].Value() );
        // connect DeviceLiwResult to DeviceLiwIterable
        if ( managed )
            {
            if ( (*aLiwList)[i].Value().TypeId() == EVariantTypeIterable )
                {
                DeviceLiwIterable* itObj = static_cast<DeviceLiwIterable*> (jsval);
                DevicePrivateBase* itData = itObj->getIterableData();
                itData->SetParent( retData );
                retData->AddChild( itData );
                }
            else if ( (*aLiwList)[i].Value().TypeId() == EVariantTypeMap )
                {
                DeviceLiwMap* mapObj = static_cast<DeviceLiwMap*> (jsval);
                DevicePrivateBase* mapData = mapObj->getMapData();
                mapData->SetParent( retData );
                retData->AddChild( mapData );
                }
            }
        rval->put( exec, Identifier( (const char*) name.PtrZ() ), jsval);  //??? should call AtL?
        }

    return rval;
    }

// ----------------------------------------------------------------------------
// CDeviceLiwBinding::SetUid
// Sets the widget uid
//
//
// ----------------------------------------------------------------------------
//
void CDeviceLiwBinding::SetUid( const TUint& aValue)
    {
    m_Uid.iUid = aValue;
#ifdef BRDO_SEC_MGR_PROMPT_ENHANCEMENT_FF
    SetAppName();
#endif
    }
#ifdef BRDO_SEC_MGR_PROMPT_ENHANCEMENT_FF
// ----------------------------------------------------------------------------
// CDeviceLiwBinding::SetAppName
// Sets the widget display name
//
// ----------------------------------------------------------------------------
//
void CDeviceLiwBinding::SetAppName()
    {
    RWidgetRegistryClientSession widgetregistry;
    TRAP_IGNORE(
    TInt ret = widgetregistry.Connect();
    if ( ret == KErrNone || ret == KErrAlreadyExists )
        {
        CleanupClosePushL( widgetregistry );
        }
    else
        {
        User::Leave( ret );
        }

    CWidgetPropertyValue* displayname = widgetregistry.GetWidgetPropertyValueL(m_Uid, EBundleDisplayName );
    User::LeaveIfError(widgetregistry.Disconnect());
    CleanupStack::PopAndDestroy(); //widgetregistry

    if ( displayname && displayname->iType == EWidgetPropTypeString )
        {
        m_scriptSession->SetApplicationNameL(*displayname);
        }
    );
    }
#endif

// ----------------------------------------------------------------------------
// KJS::GetAsciiBufferL
// Convert the Unicode string to plain ASCII chars chopping of any higher bytes
//
//
// ----------------------------------------------------------------------------
//
HBufC8* KJS::GetAsciiBufferL( const UString& aBuf )
    {
    int length = aBuf.size();
    HBufC8* buf = HBufC8::NewL( length + 1 ); // +1 for zero terminate
    TPtr8 bufDes8 = buf->Des();
    const UChar* p = aBuf.data();
    const UChar *limit = p + length;

    while ( p != limit )
        {
        TChar c = p->uc;
        bufDes8.Append( c );
        ++p;
        }

    bufDes8.ZeroTerminate();
    return buf;
    }

//END OF FILE
