/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "senattributes.h"

#include "sendebug.h"
#include "senlogger.h"


EXPORT_C CSenAttributes* CSenAttributes::NewL()
    {
    CSenAttributes* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenAttributes* CSenAttributes::NewLC()
    {
    CSenAttributes* self = new (ELeave) CSenAttributes();
    CleanupStack::PushL(self);
    self->BaseConstructL();
    return self;
    }

EXPORT_C CSenAttributes* CSenAttributes::NewL(const CSenAttributes& aAttributes)
    {
    CSenAttributes* self = NewLC(aAttributes);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenAttributes* CSenAttributes::NewLC(const CSenAttributes& aAttributes)
    {
    CSenAttributes* self = new (ELeave) CSenAttributes();
    CleanupStack::PushL(self);
    self->BaseConstructL(aAttributes);
    return self;
    }

void CSenAttributes::BaseConstructL()
    {
    TLSLOG_OPEN(KSenUtilsLogChannel,KSenUtilsLogLevel,KSenUtils,KSenUtilsLog);
    TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenAttributes::BaseConstructL(): Log opened");
    TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenAttributes::BaseConstructL(): End");
    }

void CSenAttributes::BaseConstructL(const CSenAttributes& aAttributes)
    {
    TLSLOG_OPEN(KSenUtilsLogChannel,KSenUtilsLogLevel,KSenUtils,KSenUtilsLog);
    TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenAttributes::BaseConstructL(): Log opened");
    TLSLOG_L(KSenUtilsLogChannel,KMinLogLevel ,"CSenAttributes::BaseConstructL(): End");

    TInt error(KErrNone);
    for (TInt i = 0; i<aAttributes.Count(); i++)
        {
        error = iAttrList.Append( ((CSenAttributes&)aAttributes).iAttrList.KeyAt(i),
                                  ((CSenAttributes&)aAttributes).iAttrList.ValueAt(i));
        if(error!=KErrNone)
            {
            iAttrList.Reset();
            User::Leave(error);
            }
        }
    }


CSenAttributes::CSenAttributes() : 
    iAttrList(ETrue, EFalse)
    {
    }


EXPORT_C CSenAttributes::~CSenAttributes()
    {
    iAttrList.Reset();

    // Close the log file and the connection to the server.
    TLSLOG(KSenUtilsLogChannel,KMinLogLevel,(_L("Log file closed.")));
    TLSLOG_CLOSE(KSenUtilsLogChannel);
    }


EXPORT_C void CSenAttributes::AddAttributeL(const CSenAttribute& aValue)	//codescannerwarnings
    {
    TPtrC8 name = aValue.Name();
    if ( KErrNotFound != iAttrList.Find(name) )
        {
        // Replace, if attribute exists
        iAttrList.RemoveByKey(name);
        }
    iAttrList.Append(name.AllocL(),&aValue);
    }


EXPORT_C const CSenAttribute& CSenAttributes::GetAttribute(const TDesC8& aName) const
    {
    TInt index = iAttrList.Find(aName);
    return *(iAttrList.ValueAt(index));
    }


EXPORT_C TInt CSenAttributes::Count() const
    {
    return iAttrList.Count();
    }


EXPORT_C TBool CSenAttributes::Equals(const CSenAttribute& aValue) const
    {
    return GetAttribute(aValue.Name()).Equals(aValue);
    }


EXPORT_C TBool CSenAttributes::MatchAny(const CSenAttributes& aAttrs)
    {
    for ( TInt i = 0; i < aAttrs.Count(); i++ )
        {
        for ( TInt ii = 0; ii < iAttrList.Count(); ii++ )
            {
            if ( 0 == aAttrs.AttributeAt(i).Equals(*(iAttrList.ValueAt(ii))) )
                {
                return(ETrue);
                }
            }
        }
    return EFalse;
    }


const CSenAttribute& CSenAttributes::AttributeAt(TInt aIndex) const
    {
    return *(iAttrList.ValueAt(aIndex));
    }

/*
RFileLogger* CSenAttributes::Log() const
    {
    return(RFileLogger*) &iLog;
    }

*/
