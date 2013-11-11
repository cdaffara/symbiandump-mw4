/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include "sencontextitem.h" 


#include "msencoreservicemanager.h"     // internal: Core\inc
#include "msenremotehostlet.h"

#include "SenXmlElement.h"              // Xml\inc
#include "SenXmlReader.h"               // Xml\inc
#include "senwsdescription.h"   // ServiceDescription\inc
#include "SenHttpTransportProperties.h" // Utils\inc
#include "senlayeredhttptransportproperties.h"

#include "senmessagebase.h"
#include "SenSoapMessage2.h"
#include "senatomentry.h"
#include "SenSoapMessage.h"

#include "MSenMessage.h"

#include "senchunk.h"

// From MSenContextItem:
MSenContextItem::TClass CSenContextItem::Type() const
    { 
    return MSenContextItem::EContextItem; 
    }

CSenContextItem::CSenContextItem(TAny* apData, MSenContextItem::TData aType, TInt* apFlags)
: ipData(apData), iType(aType), ipFlags(apFlags) 
// NOTE: currently only "flag" is ownership: **if ipFlags == NULL, data is owned**
    {
    }

EXPORT_C CSenContextItem::~CSenContextItem()
    {
    if(ipFlags == NULL)
        {
        // Destroy the owned data
        switch(iType)
            {
            case ETDesC8:
                {
                TDesC8* p = (TDesC8*) ipData;
                delete p; 
                ipData = NULL;
                }
                break;
            case ETInt:
                {
                TInt* p = (TInt*) ipData;
                delete p; 
                ipData = NULL;
                }
                break;
            case ETUint:
                {
                TUint* p = (TUint*) ipData;
                delete p; 
                ipData = NULL;
                }
                break;
            case EMSenProperties:
                {
                MSenProperties* pM = (MSenProperties*) ipData;
                MSenProperties::TSenPropertiesClassType t = pM->PropertiesClassType();
                if( t == MSenProperties::ESenXmlProperties           ||
                    t == MSenProperties::ESenTransportProperties     ||
                    t == MSenProperties::ESenHttpTransportProperties ||
                    t == MSenProperties::ESenVtcpTransportProperties )
                    {
                    CSenXmlProperties* p = (CSenXmlProperties*)ipData; 
                    delete p;
                    ipData = NULL;
                    }
                else if( t == MSenProperties::ESenLayeredXmlProperties          ||
                         t == MSenProperties::ESenLayeredTransportProperties     ||
                         t == MSenProperties::ESenLayeredHttpTransportProperties ||
                         t == MSenProperties::ESenLayeredVtcpTransportProperties )
                    {
                    CSenLayeredXmlProperties* p = (CSenLayeredXmlProperties*)ipData; 
                    delete p;
                    ipData = NULL;
                    }
                }
                break;
            case ECSenWSDescription:
                {
                CSenWSDescription* p = (CSenWSDescription*) ipData;
                delete p;
                ipData = NULL;
                }
                break;
            case ECSenXmlReader:
                {
                CSenXmlReader* p = (CSenXmlReader*) ipData;
                delete p;
                ipData = NULL;
                }
                break;
            case ECSenElement:
                {
                CSenElement* p = (CSenElement*) ipData;
                delete p;
                ipData = NULL;
                }
                break;
            case ECSenSoapMessage:
                    {
                    CSenSoapEnvelope* p = (CSenSoapEnvelope*) ipData;
                    delete p;
                    ipData = NULL;
                    }
                break;
            case ECSenSoapEnvelope2:
                    {
                    CSenSoapEnvelope2* p = (CSenSoapEnvelope2*) ipData;
                    delete p;
                    ipData = NULL;
                    }
                break;
            case ECSenAtomMessage:
                    {
                    CSenAtomEntry* p = (CSenAtomEntry*) ipData;
                    delete p;
                    ipData = NULL;
                    }
                break;
            case ECSenMessageBase:
                    {
                    CSenMessageBase* p = (CSenMessageBase*) ipData;
                    delete p;
                    ipData = NULL;
                    }
                break;
            case ETAny: // delete void* is not defined(!)
            case EMSenCoreServiceManager:
            case EMSenRemoteServiceConsumer:
            case EMSenRemoteHostlet:
            case EMSenMessage:
            default:
                {
                #ifdef _SENDEBUG
                User::Panic(KErrUnknown, _LIT("~SenCtxItem"));
                #endif
                }
            }
        }
    else delete ipFlags; // onlyg ipFlags were owned..
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(TAny* apNotOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apNotOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(TAny* apNotOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apNotOwned, MSenContextItem::ETAny, new (ELeave) TInt(1)); // not owned
    CleanupStack::PushL(pNew);
    return pNew;
    }


/*
EXPORT_C CSenContextItem* CSenContextItem::NewL(TAny* apValue, TBool aIsOwnedOrNot)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apValue, aIsOwnedOrNot);
    // REMOVED, REASON: if owned: deleting of void* pointer is undefined => 
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(TAny* apValue, TBool aIsOwnedOrNot)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apValue, MSenContextItem::ETAny, aIsOwnedOrNot?NULL:(new (ELeave) TInt(1)));
    CleanupStack::PushL(pNew);
    return pNew;
    }
*/

EXPORT_C CSenContextItem* CSenContextItem::NewL(const TDesC8& aRefToValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aRefToValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(const TDesC8& aRefToValue)
    {
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&(TDesC8&)aRefToValue, MSenContextItem::ETDesC8, new (ELeave) TInt(1)); // not owned
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(TDesC8* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(TDesC8* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ETDesC8, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(const TInt apValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(const TInt apValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(new (ELeave) TInt(apValue), MSenContextItem::ETInt, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }


EXPORT_C CSenContextItem* CSenContextItem::NewL(TInt* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(TInt* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ETInt, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }


EXPORT_C CSenContextItem* CSenContextItem::NewL(const TUint aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(const TUint aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(new (ELeave) TUint(aValue), MSenContextItem::ETUint, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(TUint* apOwned)
    {
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(TUint* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ETInt, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(MSenCoreServiceManager& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(MSenCoreServiceManager& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::EMSenCoreServiceManager, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(MSenRemoteServiceConsumer& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(MSenRemoteServiceConsumer& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::EMSenRemoteServiceConsumer, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(MSenRemoteHostlet& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(MSenRemoteHostlet& aValue)
    {
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::EMSenRemoteHostlet, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenWSDescription& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenWSDescription& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::ECSenWSDescription, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenWSDescription* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenWSDescription* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenWSDescription, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }


EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenChunk& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenChunk& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::ECSenChunk, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenChunk* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenChunk* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenChunk, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }




EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenElement& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenElement& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::ECSenElement, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenElement* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenElement* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenElement, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }



EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenSoapMessage& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenSoapMessage& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::ECSenSoapMessage, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(MSenMessage& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(MSenMessage& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::EMSenMessage, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }


EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenSoapMessage* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenSoapMessage* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenSoapMessage, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }
    
EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenSoapEnvelope2* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenSoapEnvelope2* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenSoapEnvelope2, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }    
EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenAtomEntry* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenAtomMessage, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }    
EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenAtomEntry* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }    

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenMessageBase* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenMessageBase* apOwned)
    { 
    CSenContextItem* pNew = new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenMessageBase, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }  

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenXmlReader& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenXmlReader& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::ECSenXmlReader, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenXmlReader* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenXmlReader* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::ECSenXmlReader, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(MSenProperties& aValue)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(aValue);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(MSenProperties& aValue)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(&aValue, MSenContextItem::EMSenProperties, new (ELeave) TInt(1));
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenXmlProperties* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenXmlProperties* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::EMSenProperties, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewL(CSenLayeredXmlProperties* apOwned)
    { 
    CSenContextItem* pNew = CSenContextItem::NewLC(apOwned);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenContextItem* CSenContextItem::NewLC(CSenLayeredXmlProperties* apOwned)
    { 
    CSenContextItem* pNew = 
        new (ELeave) CSenContextItem(apOwned, MSenContextItem::EMSenProperties, NULL);
    CleanupStack::PushL(pNew);
    return pNew;
    }

// New methods:
TAny* CSenContextItem::ToAnyL() const
    { 
    if( iType != MSenContextItem::ETAny )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return ipData; 
    }

TPtrC8 CSenContextItem::ToPtrC8L() const
    { 
    if( iType != MSenContextItem::ETDesC8 )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    
    return *((TDesC8*)ipData); 
    }

const TDesC8* CSenContextItem::ToDesC8L() const
    { 
    if( iType != MSenContextItem::ETDesC8 )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    
    return (TDesC8*)ipData;
    }


const TInt* CSenContextItem::ToIntL() const
    { 
    if( iType != MSenContextItem::ETInt )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return (TInt*)ipData; 
    }

const TUint* CSenContextItem::ToUintL() const
    { 
    if( iType != MSenContextItem::ETInt )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return (TUint*)ipData; 
    }

MSenCoreServiceManager& CSenContextItem::ToSenCoreL() const
    { 
    if( iType != MSenContextItem::EMSenCoreServiceManager )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((MSenCoreServiceManager*)ipData); 
    }
    
MSenRemoteServiceConsumer& CSenContextItem::ToSenRemoteConsumerL() const
    { 
    if( iType != MSenContextItem::EMSenRemoteServiceConsumer )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((MSenRemoteServiceConsumer*)ipData); 
    }

MSenRemoteHostlet& CSenContextItem::ToSenRemoteHostletL() const
    { 
    if( iType != MSenContextItem::EMSenRemoteHostlet )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((MSenRemoteHostlet*)ipData); 
    }

CSenWSDescription& CSenContextItem::ToSenServiceDescriptionL() const
    { 
    if(ipData)
        {
        if( iType == MSenContextItem::ECSenWSDescription )
            {
            CSenWSDescription* pDesc = (CSenWSDescription*) ipData;
            if( pDesc->HasSuperClass( MSenServiceDescription::EWSDescription ) )
                {
                return *((CSenWSDescription*)ipData); 
                }
            else if ( pDesc->DescriptionClassType() == MSenServiceDescription::EWSDescription )
                {
                return *((CSenWSDescription*)ipData); 
                }
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((CSenWSDescription*)ipData); 
    }

CSenServiceSession& CSenContextItem::ToSenServiceSessionL() const
    { 
    if(ipData) 
        {
        if( iType == MSenContextItem::ECSenWSDescription )
            {
            CSenWSDescription* pDesc = (CSenWSDescription*) ipData;
            if( pDesc->HasSuperClass( MSenServiceDescription::EServiceSession ) )
                {
                return *((CSenServiceSession*)ipData); 
                }
            else if ( pDesc->DescriptionClassType() == MSenServiceDescription::EServiceSession )
                {
                return *((CSenServiceSession*)ipData);
                }
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((CSenServiceSession*)ipData); 
    }

CSenXmlProperties& CSenContextItem::ToSenXmlPropertiesL() const
    { 
    if(ipData)
        {
        if( iType == MSenContextItem::EMSenProperties )
            {
            MSenProperties* pProp = (MSenProperties*) ipData;
            MSenProperties::TSenPropertiesClassType t = pProp->PropertiesClassType();
            if( t == MSenProperties::ESenXmlProperties           ||
                t == MSenProperties::ESenTransportProperties     ||
                t == MSenProperties::ESenHttpTransportProperties ||
                t == MSenProperties::ESenVtcpTransportProperties )
                {
                return *((CSenXmlProperties*)ipData); 
                }
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((CSenXmlProperties*)ipData); 
    }

CSenLayeredXmlProperties& CSenContextItem::ToSenLayeredXmlPropertiesL() const
    { 
    if(ipData) 
        {
        if( iType == MSenContextItem::EMSenProperties )
            {
            MSenProperties* pProp = (MSenProperties*) ipData;
            MSenProperties::TSenPropertiesClassType t = pProp->PropertiesClassType();
            if( t == MSenProperties::ESenLayeredXmlProperties           ||
                t == MSenProperties::ESenLayeredTransportProperties     ||
                t == MSenProperties::ESenLayeredHttpTransportProperties ||
                t == MSenProperties::ESenLayeredVtcpTransportProperties )
                {
                return *((CSenLayeredXmlProperties*)ipData); 
                }
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((CSenLayeredXmlProperties*)ipData); 
    }
    
MSenMessage& CSenContextItem::ToSenMessageL() const
    {
    if(ipData)
        {
        if( iType == MSenContextItem::EMSenMessage )
            {
            return *((MSenMessage*)ipData); 
            }
        else if (iType == MSenContextItem::ECSenSoapEnvelope2)
            {
            return *((CSenSoapEnvelope2*)ipData); 
            }
        else if (iType == MSenContextItem::ECSenAtomMessage)
            {
            return *((CSenAtomEntry*)ipData); 
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((MSenMessage*)ipData); 
    }

CSenSoapMessage& CSenContextItem::ToSenSoapMessageL() const
    {
    if(ipData)
        {
        if( iType == MSenContextItem::ECSenSoapMessage )
            {
            return *((CSenSoapMessage*)ipData); 
            }
        }
    User::Leave( KErrSenCtxItemTypeMismatch );
    return *((CSenSoapMessage*)ipData); 
    }
    

CSenChunk& CSenContextItem::ToSenChunkL() const
    { 
    if( iType != MSenContextItem::ECSenChunk )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((CSenChunk*)ipData); 
    }


CSenElement& CSenContextItem::ToSenElementL() const
    { 
    if( iType != MSenContextItem::ECSenElement )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((CSenElement*)ipData); 
    }

CSenXmlReader& CSenContextItem::ToSenXmlReaderL() const
    { 
    if( iType != MSenContextItem::ECSenXmlReader )
        {
        User::Leave( KErrSenCtxItemTypeMismatch );
        }
    return *((CSenXmlReader*)ipData); 
    }
// End of File

