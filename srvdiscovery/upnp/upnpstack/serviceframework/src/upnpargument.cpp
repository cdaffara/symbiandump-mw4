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
* Description:  Defines the CUpnpargument class
*
*/


// INCLUDE FILES

#include <e32base.h>
#include "upnpargument.h"
#include "upnpservice.h"
#include "upnpstring.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"

// logs
#include "f32file.h" 
#include "s32file.h"





// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpArgument::CUpnpArgument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpArgument::CUpnpArgument( CUpnpService& aParentService )
: iParentService( aParentService )
{
    
}

void CUpnpArgument::ConstructL()
    {
    iName = HBufC8::NewL(0);
    iValue = HBufC8::NewL(0);
    iRelatedStateVariable = HBufC8::NewL(0);
    }


// -----------------------------------------------------------------------------
// CUpnpArgument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//.
void CUpnpArgument::ConstructL( CUpnpArgument& aArgument )
{
    
    
    iName = aArgument.Name().AllocL();
    
    iValue = HBufC8::NewL(0);
    
    iDirection = aArgument.Direction();
    
    iRelatedStateVariable = aArgument.RelatedStateVariable().AllocL();
    
    
    CUpnpStateVariable* var = iParentService.StateVariable( *iRelatedStateVariable );
    
    if ( var )
    {
        iType = var->Type();
    }
    else 
    {
        iType = EUndefined;
    }
}

// -----------------------------------------------------------------------------
// CUpnpArgument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpArgument* CUpnpArgument::NewL( CUpnpService& aParentService )
        {
        CUpnpArgument* self = new(ELeave) CUpnpArgument(aParentService);
        self->ConstructL();
        return self;
        }

// -----------------------------------------------------------------------------
// CUpnpArgument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpArgument* CUpnpArgument::NewL( CUpnpArgument& aArgument, 
                                             CUpnpService& aParentService )
{
    CUpnpArgument* self = new (ELeave) CUpnpArgument( aParentService );
    CleanupStack::PushL( self );
    self->ConstructL( aArgument );
    CleanupStack::Pop( self );
    
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpArgument::~CUpnpArgument
// Destructor
// -----------------------------------------------------------------------------
// 
CUpnpArgument::~CUpnpArgument()
{
    delete iName;
    delete iValue;
    delete iRelatedStateVariable;
}

// -----------------------------------------------------------------------------
// CUpnpArgument::Name
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpArgument::Name()
{
    return *iName;
}

void CUpnpArgument::SetNameL(const TDesC8& aName)
    {
    HBufC8* tmp = aName.AllocL();
    delete iName;
    iName = tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpArgument::SetValueL
// If the value is inproper, the method leaves with EInvalidArgs,
// which is the internal upnp error code used by UPnP Stack
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpArgument::SetValueL( const TDesC8& aValue )
{
    LOGS("CUpnpArgument::SetValueL(TDesC8&) [Name/Value]");


    // Dates, times, URIs and UUIDs are not validated to save process time.
    // These must be validated if used in actual service,
    // which uses these arguments.
    TArgumentType type = Type();
    LOGS1( "type: %i", type );
    TLex8 lex(aValue);
    switch ( type )
    {
        
        case EUi1:
        {
            TUint8 tryVal;
            CheckErrorL( ( lex.Val(tryVal, EDecimal) ), aValue );
            break;
        }
        case EUi2:
        {
            TUint16 tryVal;
            CheckErrorL ( lex.Val(tryVal, EDecimal), aValue );
            break;
        }
        case EUi4:
        {
            TUint32 tryVal;
            CheckErrorL( lex.Val(tryVal, EDecimal), aValue );
            break;
        }
        case EI1:
        {
            TInt8 tryVal;
            CheckErrorL( lex.Val(tryVal), aValue );
            break;
        }
        case EI2:
        {
            TInt16 tryVal;
            CheckErrorL( lex.Val(tryVal), aValue );
            break;
        }
        case EI4:
        {
            TInt32 tryVal;
            CheckErrorL( lex.Val(tryVal), aValue );
            break;
        }
        case EInt:
        {
            TInt64 tryVal;
            CheckErrorL( lex.Val(tryVal), aValue );
            break;
        }
        case ER4:
        {
            TReal32 tryVal;
            CheckErrorL( lex.Val(tryVal), aValue );
            break;
        }
        case ER8:       
        case EFloat:
        case ENumber:
        {
            TReal64 tryVal;
            CheckErrorL( ( lex.Val(tryVal) ), aValue );
            break;
        }
        case EFixed144:
        {
            TReal64 tryVal;            
            switch ( lex.Val(tryVal) )
            {
                case KErrOverflow:
                {
                    // sizeof argument
                    User::Leave(EInvalidArgs);
                    break;
                }
                case KErrGeneral:
                {
                    // bad argument
                    User::Leave(EInvalidArgs);
                    break;
                }
                default:
                {

                    TInt left = aValue.Find(UpnpString::KDot8);                    
                    TInt right;
                    //only decimal part exists
                    if ( left == KErrNotFound )
                    {
                        left = aValue.Length();
                        right = left;
                        //occurances of minus are not counted as a seprate digit positions
                        TInt minus = aValue.Find(UpnpString::KMinus);
                        
                        if (minus > KErrNone)
                        {
                        left --;
                        }
                        else if(minus == KErrNone)
                        {
                        	left--;                        	
                        	if(aValue.Find(UpnpString::KMinus)>=KErrNone)
                        	{
                        		left--;
                        	}                        	
                        }
                    }                  
                    else //fractional part exists
                    {
                    	right = left+1;                    	
                    	if(tryVal<0)
						{
							left--;
						}	
                    	
                    	if (aValue.Mid(right).Find(UpnpString::KMinus) >= KErrNone)
                    	{
                    		right++;	
                    	}                    
                    }
                                        	
					//checking decimal digits
                    if ( left > KMaxFixed144Left )
                    {
                        User::Leave(EInvalidArgs);
                    }
					//checking fractal digits
                    right = (aValue.Length() - right );
                    if ( right > KMaxFixed144Right )
                    {
                        User::Leave(EInvalidArgs);
                    }

                    // NULL value, if AllocL leaves then the pointer is invalid
                    DeleteAndNullValue();
                    iValue = aValue.AllocL();
                }
            }
            break;
        }
        case EBinBase64:
        {
            // NULL value, if AllocL leaves then the pointer is invalid
            DeleteAndNullValue();
            iValue = aValue.AllocL();

            break;
        }
        case EBinHex:
        {
            TInt64 tryVal;
            CheckErrorL( lex.Val(tryVal, EHex), aValue );
            break;
        }
        case EChar:
        {
            // just one character allowed
            if (aValue.Length() > 1)
            {
                User::Leave(EInvalidArgs);
            }
            // NULL value, if AllocL leaves then the pointer is invalid
            DeleteAndNullValue();
            iValue = aValue.AllocL();
            
            break;
        }
        case EDate:         
        case EDateTime:     
        case EDateTimeTz:   
        case ETime:         
        case ETimeTz:       
        case EUri:          
        case EUuid:         
        case EString:
        {
            // NULL value, if AllocL leaves then the pointer is invalid
            DeleteAndNullValue();
            iValue = aValue.AllocL();
            
            break;
        }
        default:
        {
            // unknown
            // must be tolerant
            // NULL value, if AllocL leaves then the pointer is invalid
            DeleteAndNullValue();
            iValue = aValue.AllocL();
            
            break;
        }
    }


}

// -----------------------------------------------------------------------------
// CUpnpArgument::Value
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpArgument::Value()
{
    LOGS("CUpnpArgument::Value() [Name/Value]");
    return *iValue;
}

// -----------------------------------------------------------------------------
// CUpnpArgument::RelatedStateVariable
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpArgument::RelatedStateVariable()
{
    return *iRelatedStateVariable;
}

void CUpnpArgument::SetRelatedStateVarL(const TDesC8& aRelatedStateVar)
    {
    HBufC8* tmp = aRelatedStateVar.AllocL();
    delete iRelatedStateVariable;
    iRelatedStateVariable = tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpArgument::Direction
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpArgument::Direction() const
{
    return iDirection;
}

void CUpnpArgument::SetDirectionL( TInt aDirection )
    {
    iDirection = aDirection;
    }

// -----------------------------------------------------------------------------
// CUpnpArgument::Type
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TArgumentType CUpnpArgument::Type()
{
    CUpnpStateVariable* var = iParentService.StateVariable( *iRelatedStateVariable );
    if( var )
    {
        return var->Type();
    }
    else
    {
        return EUnknown;
    }
}

// -----------------------------------------------------------------------------
// CUpnpArgument::DeleteAndNullValue
// Delete and NULL iValue.
// -----------------------------------------------------------------------------
//
void CUpnpArgument::DeleteAndNullValue()
{
    delete iValue;
    iValue = NULL;
}

// -----------------------------------------------------------------------------
// CUpnpArgument::CheckErrorL
// Checks the error code for the SetValue method and leaves if needed.
// -----------------------------------------------------------------------------
//
void CUpnpArgument::CheckErrorL(TInt aError, const TDesC8& aValue)
{
    switch ( aError )
    {
        case KErrOverflow:
        {
        // sizeof argument
            User::Leave(EInvalidArgs);
            break;
        }
        case KErrGeneral:
        {
            // bad argument
            User::Leave(EInvalidArgs);
            break;
        }
        default:
        {
        // NULL value, if AllocL leaves then the pointer is invalid
            DeleteAndNullValue();
            iValue = aValue.AllocL();
        }
    }                       
}
    
void CUpnpArgument::SetType(TInt aType)
    {
    iType = aType;
    }
    
// End of File
