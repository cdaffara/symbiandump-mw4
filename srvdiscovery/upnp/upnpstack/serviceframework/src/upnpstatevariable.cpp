/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpStateVariable
*
*/



// INCLUDE FILES
#include "upnpstatevariable.h"
#include "upnpstring.h"
#include "upnpargument.h"
#include "upnpserviceimplementation.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"


// logs
#include "f32file.h" 
#include "s32file.h"

// CONSTANTS
_LIT8(KUi1, "ui1");
_LIT8(KUi2, "ui2");
_LIT8(KUi4, "ui4");
_LIT8(KI1, "i1");
_LIT8(KI2, "i2");
_LIT8(KI4, "i4");
_LIT8(KInt, "int");
_LIT8(KR4, "r4");
_LIT8(KR8, "r8");
_LIT8(KNumber, "number");
_LIT8(KFixed144, "fixed.14.4");
_LIT8(KFloat, "float");
_LIT8(KChar, "char");
_LIT8(KString, "string");
_LIT8(KDate, "date");
_LIT8(KDateTime, "dateTime");
_LIT8(KDateTimeTz, "dateTime.tz");
_LIT8(KTime, "time");
_LIT8(KTimeTz, "time.tz");
_LIT8(KBoolean, "boolean");
_LIT8(KBinBase64, "bin.base64");
_LIT8(KBinHex, "bin.hex");
_LIT8(KUri, "uri");
_LIT8(KUuid, "uuid");
//_LIT8(KDataType, "dataType");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpStateVariable::CUpnpStateVariable
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpStateVariable::CUpnpStateVariable()
{

}



void CUpnpStateVariable::ConstructL()
    {
    iName = HBufC8::NewL(0);
    iDataType = HBufC8::NewL(0);
  //  iDefaultValue = HBufC8::NewL(0);
    iRangeMin = HBufC8::NewL(0);
    iRangeMax = HBufC8::NewL(0);
    iRangeStep = HBufC8::NewL(0);
    iValue = HBufC8::NewL(0);
    iEventable = HBufC8::NewL(0);
    
    iParent = NULL;
    iMaxEventRate = 0;
    iCurrentEventTime = 0; 
    iReadyForEventing = ETrue;

    }

// -----------------------------------------------------------------------------
// CUpnpStateVariable::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpStateVariable* CUpnpStateVariable::NewL()
{
    CUpnpStateVariable* self = new (ELeave) CUpnpStateVariable();
    self->ConstructL();
    return self;
}


// Destructor
CUpnpStateVariable::~CUpnpStateVariable()
{
    delete iName;
    delete iDataType;
    delete iDefaultValue;
    delete iRangeMin;
    delete iRangeMax;
    delete iRangeStep;
    delete iEventable;

    iAllowedValueList.ResetAndDestroy();
    iAllowedValueList.Close();
    delete iValue;
    
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::Name
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpStateVariable::Name()
{
    if ( iName )
    {
        return *iName;
    }
    else
    {
        return KNullDesC8();
    }
}

// -----------------------------------------------------------------------------
EXPORT_C void CUpnpStateVariable::SetNameL( const TDesC8& aName)
    {
    HBufC8* tmp = aName.AllocL();
    delete iName;
    iName = tmp;
    }
// CUpnpStateVariable::Value
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpStateVariable::Value()
{
    LOGS("CUpnpStateVariable::Value()");
    
    if ( iValue )
    {
        return *iValue;
    }
    else
    {
        return KNullDesC8();
    }
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::SetValueL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpStateVariable::SetValueL(const TDesC8& aValue, TBool aIsModerated)
{
    LOGS("CUpnpStateVariable::SetValueL(const TDesC8&)");
    
    if( iValue )
    {
        delete iValue; 
        iValue = NULL;
    }
                
    iValue = aValue.AllocL();
    iModerated = aIsModerated;
    if (iParent)
    {
        iParent->AddEventVariable(*this, aIsModerated);
    }
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::SetValueL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpStateVariable::SetValueL(TInt aValue, TBool aIsModerated)
{
 //   LOGS("CUpnpStateVariable::SetValueL(TInt)");
    TBuf8<10> num;
    num.Num(aValue);
   
    SetValueL( num, aIsModerated );
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::Eventable
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpStateVariable::Eventable()
{
    if ( iEventable )
    {
        return *iEventable;
    }
    else
    {
        return KNullDesC8();
    }
}

EXPORT_C  void CUpnpStateVariable::SetEventableL(const TDesC8& aEventable)
    {
    HBufC8* tmp = aEventable.AllocL();
    delete iEventable;
    iEventable = tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpStateVariable::GetDefaultValue
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpStateVariable::GetDefaultValue(TInt& aValue)
{
    if ( iDefaultValue )
    {
        TLex8 lex( *iDefaultValue );
        TInt err = lex.Val( aValue );
        
        // Check conversion result
        if( err != KErrNone )
                aValue = err;
    }
    else
    {
        aValue = KErrNotFound;
    }
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::DefaultValue
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpStateVariable::DefaultValue()
{
    if ( iDefaultValue )
    {
        return *iDefaultValue;
    }
    else
    {
        return KNullDesC8();
    }
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::AllowedValuesLC
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array* CUpnpStateVariable::AllowedValuesLC()
{
    CDesC8ArrayFlat* values = new (ELeave) CDesC8ArrayFlat(1);
    CleanupStack::PushL(values);
    
    for ( TInt index = 0; index < iAllowedValueList.Count(); index++ )
    {
        HBufC8* tmp = iAllowedValueList[index];
        values->AppendL( *tmp );
    }
      
    return values;
}
EXPORT_C void CUpnpStateVariable::AddAllowedValueL( const TDesC8& aAllowedValue )
    {
    iAllowedValueList.Append( aAllowedValue.AllocL() );
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariable::MaxValue
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpStateVariable::MaxValue()
{
    TInt retValue = KErrNotFound;
    if ( iRangeMax )
    {
        TLex8 lex( *iRangeMax );
        lex.Val( retValue );
    }
    return retValue;    
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::MinValue
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpStateVariable::MinValue()
{
    TInt retValue = KErrNotFound;
    if ( iRangeMin )
    {
        TLex8 lex( *iRangeMin );
        lex.Val( retValue );
    }
    return retValue; 
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::Step
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpStateVariable::Step()
{
    TInt retValue = KErrNotFound;
    if ( iRangeStep )
    {
        TLex8 lex( *iRangeStep );
        lex.Val( retValue );
    }
    return retValue; 
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::Type
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TArgumentType CUpnpStateVariable::Type()
{
    if ( iDataType )
    {
        const TPtrC8 value = *iDataType;
        if (value == KUi1())
        {
            return EUi1;
        }
        else if (value ==KUi2())
        {
            return EUi2;
        }
        else if (value ==KUi4())
        {
            return EUi4;
        }  
        else if (value ==KI1())
        {
            return EI1;
        }
        else if (value ==KI2())
        {
            return EI2;
        }
        else if (value ==KI4())
        {
            return EI4;
        }
        else if (value ==KInt())
        {
            return EInt;        
        }
        else if (value ==KR4())
        {
            return ER4;
        }
        else if (value ==KR8())
        {
            return ER8;
        }
        else if (value ==KNumber())
        {
            return ENumber;
        }
        else if (value ==KFixed144())
        {
            return EFixed144;
        }
        else if (value ==KFloat())
        {
            return EFloat;
        }
        else if (value ==KChar())
        {
            return EChar;
        }
        else if (value ==KString())
        {
            return EString;
        }   
        else if (value ==KDate())
        {
            return EDate;
        }
        else if (value ==KDateTime())
        {
            return EDateTime;
        }
        else if (value ==KDateTimeTz())
        {
            return EDateTimeTz;
        }
        else if (value ==KTime())
        {
            return ETime;
        }
        else if (value ==KTimeTz())
        {
            return ETimeTz;
        }
        else if (value ==KBoolean())
        {
            return EBoolean;
        }
        else if (value ==KBinBase64())
        {
            return EBinBase64;
        }
        else if (value ==KBinHex())
        {
            return EBinHex;
        }
        else if (value ==KUri())
        {
            return EUri;
        }
        else if (value ==KUuid())
        {
            return EUuid;
        }
        else
        {
        
        }
    }

    return EUnknown;
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::SetParentImplementation
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpStateVariable::SetParentImplementation(CUpnpServiceImplementation &aParent)
{
    iParent = &aParent;
}

// -----------------------------------------------------------------------------
// CUpnpStateVariable::SetParentImplementation
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpStateVariable::IsModerated()
{
    return iModerated;
}

EXPORT_C void CUpnpStateVariable::SetTypeL(const TDesC8& aType)
    {
    HBufC8* tmp = aType.AllocL();
    delete iDataType;
    iDataType = tmp;
    }

TBool CUpnpStateVariable::ReadyForEventing()
{
    return iReadyForEventing;
}       

void CUpnpStateVariable::SetMaxEventRate(TInt aMaxRate, TInt aNormalRate)
{
    iMaxEventRate = aMaxRate;
    iReadyForEventing = ETrue;
    iCurrentEventTime = aMaxRate;
    iNormalEventRate = aNormalRate;
}

TInt CUpnpStateVariable::MaxEventRate()
{
    return iMaxEventRate;
}

void CUpnpStateVariable::EventTick()
{
    if(iMaxEventRate > 0)
    {
        iCurrentEventTime = iCurrentEventTime + iNormalEventRate;

        if(iCurrentEventTime >= iMaxEventRate)
        {
                iReadyForEventing = ETrue;
                iCurrentEventTime = 0;

                iEventTime.UniversalTime();
        }
        else
        {
                iTime.UniversalTime();
        
                TInt64 int64(iMaxEventRate);
                TInt64 diffTime((iTime.MicroSecondsFrom(iEventTime)).Int64());

                if(diffTime >= int64)
                {
                        iReadyForEventing = ETrue;
                        iCurrentEventTime = 0;

                        iEventTime.UniversalTime();                             
                }
                else
                {
                        iCurrentEventTime = diffTime;
                                
                        iReadyForEventing = EFalse;                             
                }
        }
    }
}   
   
void CUpnpStateVariable::SetDefaultValueL(const TDesC8& aDefaultValue )
    {
    HBufC8* tmp = aDefaultValue .AllocL();
    delete iDefaultValue; 
    iDefaultValue = tmp;
    }
    
void CUpnpStateVariable::SetRangeMinL(const TDesC8& aRangeMin )
    {
    if( &aRangeMin )
       {
        HBufC8* tmp = aRangeMin.AllocL();
        delete iRangeMin; 
        iRangeMin = tmp;
       }
    }
    
void CUpnpStateVariable::SetRangeMaxL(const TDesC8& aRangeMax )
    {
    if( &aRangeMax )
       {
       HBufC8* tmp = aRangeMax.AllocL();
       delete iRangeMax; 
       iRangeMax = tmp;
       }
    }
    
void CUpnpStateVariable::SetRangeStepL(const TDesC8& aRangeStep )
    {
    HBufC8* tmp = aRangeStep.AllocL();
    delete iRangeStep; 
    iRangeStep = tmp;
    }
    
//  End of File
