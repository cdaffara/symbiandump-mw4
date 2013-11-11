/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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







#include "senattribute.h"

#include "senlogger.h"


//#ifdef _SENDEBUG
// // CONSTATS for file logging
//namespace
//    {
//    _LIT(KLogFileDir, "SenUtils");
//    _LIT(KLogFileName, "SenAttribute.log"); 
//    }
//#endif // _SENDEBUG

EXPORT_C CSenAttribute* CSenAttribute::NewL(const TDesC8& aName,
                                            const TDesC8& aType,
                                            const TDesC8& aValue,
                                            const TDesC8& aFriendlyName)
    {
    CSenAttribute* pNew = new (ELeave) CSenAttribute;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aName, aType, aValue, aFriendlyName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenAttribute* CSenAttribute::NewL(const TDesC8& aName,
                                            const TDesC8& aType,
                                            const TDesC8& aValue)
    {
    CSenAttribute* pNew = new (ELeave) CSenAttribute;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aName, aType, aValue, KNullDesC8());
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenAttribute::~CSenAttribute()
    {
    delete ipName;
    delete ipType;
    delete ipFriendlyName;
    iValues.ResetAndDestroy();

//    // Close the log file and the connection to the server.
//    LOG_WRITE((_L("Log file closed.")));
//    LOG_CLOSELOG;
//    LOG_CLOSE;
    }

EXPORT_C CSenAttribute::CSenAttribute()
    {
    }


EXPORT_C void CSenAttribute::ConstructL(const TDesC8& aName,
                                        const TDesC8& aType,
                                        const TDesC8& aValue,
                                        const TDesC8& aFriendlyName)
    {
//    LOG_CONNECT;
//    LOG_CREATELOG((KLogFileDir, KLogFileName, EFileLoggingModeOverwrite));
//    LOG_WRITE(( _L8("CSenAttribute::ConstructL(): Log opened") ));
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenAttribute::ConstructL(): Start");
    ipName = aName.AllocL() ;
    ipType = aType.AllocL() ;
    iValues.Append(aValue.AllocL());
    // If the friendly nameparam isn't set, then don't set the owned value
    ipFriendlyName = aFriendlyName.AllocL() ;
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenAttribute::ConstructL(): End");
    }


EXPORT_C TPtrC8 CSenAttribute::Name() const
    {
    if ( ipName == NULL )
        {
        return KNullDesC8();
        } else
        {
        return *ipName;
        }
    }


EXPORT_C TPtrC8 CSenAttribute::Value() const
    {
    if ( 0 == iValues.Count() )
        {
        return KNullDesC8();
        } else
        {
        return *iValues[0];
        }
    }


EXPORT_C TPtrC8 CSenAttribute::Type() const
    {
    if ( ipType == NULL )
        {
        return KNullDesC8();
        } else
        {
        return *ipType;
        }
    }


EXPORT_C TPtrC8 CSenAttribute::FriendlyName() const
    {
    if ( ipFriendlyName == NULL )
        {
        return KNullDesC8();
        } else
        {
        return *ipFriendlyName;
        }
    }


EXPORT_C void CSenAttribute::SetValueL(const TDesC8& aValue)	//codescannerwarnings
    {
    iValues.ResetAndDestroy();
    iValues.Append(aValue.AllocL());
    }


EXPORT_C void CSenAttribute::AddValueL(const TDesC8& aValue)	//codescannerwarnings
    {
    iValues.Append(aValue.AllocL());
    }


EXPORT_C const RAttrValueArray CSenAttribute::GetValues() const
    {
    return iValues;
    }


EXPORT_C TInt CSenAttribute::NumValues() const
    {
    return iValues.Count();
    }


EXPORT_C TBool CSenAttribute::Equals(const CSenAttribute& aAttr) const
    {
    if ( 0 != aAttr.Name().Compare(*ipName) ) return EFalse;
    if ( 0 != aAttr.Type().Compare(*ipType) ) return EFalse;
    if ( 0 != aAttr.FriendlyName().Compare(*ipFriendlyName) ) return EFalse;

    TInt count = iValues.Count();
    if ( count != aAttr.NumValues() ) return EFalse;

    RAttrValueArray thatValues = aAttr.GetValues();
    TBool ret = ETrue;
    for ( TInt i = 0; i < count; i++ )
        {
        if ( EFalse == ret ) break;
        ret = EFalse;
        for ( TInt ii = 0; ii < count; ii++ )
            {
            if ( 0 == iValues[i]->Compare(*thatValues[ii]) )
                {
                ret = ETrue;
                break;
                }
            }
        }
    return ret;
    }


//RFileLogger* CSenAttribute::Log() const
//    {
//    return(RFileLogger*) &iLog;
//    }


