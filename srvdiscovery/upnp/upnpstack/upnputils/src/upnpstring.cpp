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
* Description:  String handling
*
*/


// INCLUDES
#include <e32base.h>
#include <charconv.h>
#include <f32file.h>
#include <s32mem.h>
#include <escapeutils.h>
#include "upnpcons.h"
#include "upnpstring.h"
#include "upnpcommonupnplits.h"

// ================= MEMBER FUNCTIONS =======================


namespace UpnpString
    {
    
    // PR: following for optimized encode/decodexml functions    
    _LIT8( KGtEntity,       "gt;");
    _LIT8( KLtEntity,       "lt;");
    _LIT8( KQuotEntity,     "quot;");
    _LIT8( KAposEntity,     "apos;");
    _LIT8( KAmpEntity,      "amp;");
    
    // for formatting a date string (in UT)
    _LIT8( KDay1,"Mon");
    _LIT8( KDay2,"Tue");
    _LIT8( KDay3,"Wed");
    _LIT8( KDay4,"Thu");
    _LIT8( KDay5,"Fri");
    _LIT8( KDay6,"Sat");
    _LIT8( KDay7,"Sun");
    
    // for formatting a date string (in UT)
    _LIT8( KMonth1,"Jan");
    _LIT8( KMonth2,"Feb");
    _LIT8( KMonth3,"Mar");
    _LIT8( KMonth4,"Apr");
    _LIT8( KMonth5,"May");
    _LIT8( KMonth6,"Jun");
    _LIT8( KMonth7,"Jul");
    _LIT8( KMonth8,"Aug");
    _LIT8( KMonth9,"Sep");
    _LIT8( KMonth10,"Oct");
    _LIT8( KMonth11,"Nov");
    _LIT8( KMonth12,"Dec");
    
    //for formatting UT
    _LIT8( KTimeGMT,"GMT");
    
    //for formatting UT
    const static TInt KWidthFour = 4;
    const static TInt KWidthTwo = 2;
    
    const static TInt KMaxEntityLength = 6; // e.g. &quot;
    const static TInt KEntitiesCount = 5;
    const static TReal KReallocRatio = 1.3;


// -----------------------------------------------------------------------------
// UpnpString::DeleteArray
// 
// -----------------------------------------------------------------------------
//
void DeleteArray(TAny* param);
    
// -----------------------------------------------------------------------------
// UpnpString::IsColon
// Is char colon.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool IsColon(const TUint8& aChar)
    {
    return aChar == ':';
    }
// -----------------------------------------------------------------------------
// UpnpString::FromUnicodeL
// Convert from unicode.
// -----------------------------------------------------------------------------
//    
EXPORT_C HBufC8* FromUnicodeL(const TDesC& aBuffer)
    {
    return EscapeUtils::ConvertFromUnicodeToUtf8L(aBuffer);
    }

// -----------------------------------------------------------------------------
// UpnpString::ToUnicodeL
// Convert to unicode.
// -----------------------------------------------------------------------------
//    
EXPORT_C HBufC16* ToUnicodeL(const TDesC8& aBuffer)
    {
    return EscapeUtils::ConvertToUnicodeFromUtf8L(aBuffer);
    }

// -----------------------------------------------------------------------------
// UpnpString::CurrentDateLC
// Return current date.
// -----------------------------------------------------------------------------
//    
EXPORT_C HBufC8* CurrentDateLC()
    {
    HBufC8* timeBuf = NULL;
        
    TTime time;    
    time.UniversalTime();    
    
    timeBuf = GetDateLC(time);
    
    return timeBuf;
    }

// -----------------------------------------------------------------------------
// UpnpString::GetDateLC
// Return current date.
// -----------------------------------------------------------------------------
//    
HBufC8* GetDateLC(const TTime& aTime)
    {
    TDateTime datetime = aTime.DateTime();    
    
    HBufC8* timeBuf = HBufC8::NewLC(KDateTimeStringLen);
    //weekday
    timeBuf->Des().Copy(GetWeekday(aTime));
    timeBuf->Des().Append(KComma);
    timeBuf->Des().Append(KSpace);
    
    //date
    //days numerated from 0 to 30, so +1
    timeBuf->Des().AppendNumFixedWidth((datetime.Day()+1),EDecimal, KWidthTwo);
    timeBuf->Des().Append(KSpace);
    timeBuf->Des().Append(GetMonth(datetime));
    timeBuf->Des().Append(KSpace);    
    timeBuf->Des().AppendNumFixedWidth(datetime.Year(),EDecimal, KWidthFour);
    timeBuf->Des().Append(KSpace);
    
    //time
    timeBuf->Des().AppendNumFixedWidth((datetime.Hour()),EDecimal, KWidthTwo);
    timeBuf->Des().Append(KColon);
    timeBuf->Des().AppendNumFixedWidth((datetime.Minute()),EDecimal, KWidthTwo);
    timeBuf->Des().Append(KColon);
    timeBuf->Des().AppendNumFixedWidth((datetime.Second()),EDecimal, KWidthTwo);
    timeBuf->Des().Append(KSpace);
    
    //GMT
    timeBuf->Des().Append(KTimeGMT);    
    
    return timeBuf;
    }

// -----------------------------------------------------------------------------
// UpnpString::GetWeekday
// 
// -----------------------------------------------------------------------------
//    
const TDesC8& GetWeekday(const TTime& aDate)
    {
    switch (aDate.DayNoInWeek())
        {
        case EMonday:
            return KDay1();            
        case ETuesday:
            return KDay2();    
        case EWednesday:
            return KDay3();
        case EThursday:
            return KDay4();
        case EFriday:
            return KDay5();   
        case ESaturday:
            return KDay6();
        case ESunday:
            return KDay7();
        default:
            return KNullDesC8;                                                    
            
        }
    }

// -----------------------------------------------------------------------------
// UpnpString::GetMonth
// 
// -----------------------------------------------------------------------------
//    
const TDesC8& GetMonth(const TDateTime& aDate)
    {
    switch (aDate.Month())
        {
        case EJanuary:
            return KMonth1();            
        case EFebruary:
            return KMonth2();    
        case EMarch:
            return KMonth3();
        case EApril:
            return KMonth4();
        case EMay:
            return KMonth5();   
        case EJune:
            return KMonth6();
        case EJuly:
            return KMonth7();
        case EAugust:
            return KMonth8();
        case ESeptember:
            return KMonth9();
        case EOctober:
            return KMonth10();
        case ENovember:
            return KMonth11();
        case EDecember:
            return KMonth12();                    
        default:
            return KNullDesC8;        
        }
    }    

// -----------------------------------------------------------------------------
// UpnpString::CutToPiecesL
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C void CutToPiecesL( TPtrC8 aPtr, 
                            TChar aDelim, 
                            RPointerArray<TPtrC8>& aArray )
    {
    TInt i = 0;
    CleanupStack::PushL(TCleanupItem(&DeleteArray,&aArray));

    while(KErrNotFound != aPtr.Locate(aDelim))
        {    
        TPtrC8* tmp = new (ELeave)TPtrC8();
        CleanupStack::PushL(tmp);
        User::LeaveIfError( aArray.Append( tmp ) );
        CleanupStack::Pop(tmp);
    
        aArray[i]->Set(aPtr.Left((aPtr.Locate(aDelim))));
        aPtr.Set(aPtr.Right(aPtr.Length() - (aPtr.Locate(aDelim) + 1)) ); 
        aArray[i]->Set(Trim(*aArray[i], EFalse));
        i++;
        }
    TPtrC8* tmp = new (ELeave)TPtrC8();
    CleanupStack::PushL(tmp);
    User::LeaveIfError( aArray.Append( tmp ) );
    CleanupStack::Pop(tmp);        
       
    aArray[i]->Set(aPtr);
    aArray[i]->Set(Trim(*aArray[i], EFalse));
        
    CleanupStack::Pop(&aArray);    
    }

// -----------------------------------------------------------------------------
// UpnpString::Trim
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TPtrC8 Trim(TPtrC8 aPtr, TBool aQuotations)
    {
    if( aPtr.Length() == 0 )
        {
        return aPtr;
        }
    while( ( ('"' == aPtr[0]) && aQuotations ) ||
        (' ' == aPtr[0]) )
        {
            aPtr.Set(aPtr.Right(aPtr.Length() - 1));
        if( aPtr.Length() == 0) 
            {
            return aPtr;
            }
        }

    while( ( ('"' == aPtr[aPtr.Length() - 1] ) && aQuotations ) ||
        (' ' == aPtr[aPtr.Length() - 1] ) )
        {
        aPtr.Set(aPtr.Left(aPtr.Length() - 1 ));
        if( aPtr.Length() == 0 ) 
            {
            return aPtr;
            }
        }
    return aPtr;
    }

// -----------------------------------------------------------------------------
// CutToPiecesL
// 
// -----------------------------------------------------------------------------
//
void CutToPiecesL( TPtrC8 aPtr,TChar aDelim, RArray<TPtrC8>& aArray )
    {
    TInt i = 0;
    TInt loc = aPtr.Locate(aDelim);
    while(KErrNotFound != loc)
        {    
        TPtrC8 r(aPtr.Left(loc));
        User::LeaveIfError(aArray.Append(r));
        aPtr.Set(aPtr.Right(aPtr.Length() - (loc + 1)) ); 
        i++;
        loc = aPtr.Locate(aDelim);
        }
    TInt parenthesis;
    parenthesis =  aPtr.Locate(')');
    if( KErrNotFound != parenthesis )
        {
        User::LeaveIfError(aArray.Append(aPtr.Left( parenthesis )));
        }
    else
        {
        User::LeaveIfError(aArray.Append(aPtr));
          }
    }

// -----------------------------------------------------------------------------
// UpnpString::ValueFromCsvL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* ValueFromCsvL(const TDesC8& aCSV, TInt aPlace)
    {
    RArray<TPtrC8> array;
    CleanupClosePushL(array);
    CutToPiecesL(aCSV, ',', array);

    HBufC8* value = HBufC8::NewL(array[aPlace].Length());
    value->Des().Copy(array[aPlace]);
    CleanupStack::PopAndDestroy();
    return value;
    }

// -----------------------------------------------------------------------------
// UpnpString::AddValueToCsvL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* AddValueToCsvL(const TDesC8& aCSV, const TDesC8& aValue)
    {
    HBufC8* buf=NULL;
    if( 0 == KEmptyString().Compare(aCSV) )
        {
        buf = aValue.AllocL();
        }
    else
        {
        buf = HBufC8::NewL(aCSV.Length() + KComma().Length() + aValue.Length());
        buf->Des().Append(aCSV);
        buf->Des().Append(KComma());
        buf->Des().Append(aValue);
        }
    return buf;
    }

// -----------------------------------------------------------------------------
// UpnpString::RemoveFromCsvLC
// 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RemoveFromCsvLC(const TDesC8& aCSV, const TDesC8& aValue)
    {
    RPointerArray<TPtrC8> array;
    CsvToRPointerArrayL(aCSV, array);
    TPtrC8 val(Trim(aValue, EFalse));
    for( TInt i = 0; i < array.Count(); i++ )
        {
        if( 0 == array[i]->CompareF(val) )
            {
            delete array[i];
            array.Remove(i);
            }
        }
    CleanupStack::PushL(TCleanupItem(&DeleteArray, &array));    
    HBufC8* buf = RPointerArrayToCsvLC( array );
    CleanupStack::Pop( buf ); 
    CleanupStack::PopAndDestroy( &array ); 
    CleanupStack::PushL( buf ); 
    array.Close();
    return buf;
    }

// -----------------------------------------------------------------------------
// UpnpString::CsvToRPointerArrayL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CsvToRPointerArrayL( const TDesC8& aCSV, 
                                   RPointerArray<TPtrC8>& aArray )
    {
    aArray.ResetAndDestroy();
    CutToPiecesL(aCSV, ',', aArray);
    }

// -----------------------------------------------------------------------------
// UpnpString::RPointerArrayToCsvLC
// 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RPointerArrayToCsvLC( RPointerArray<TPtrC8>& aArray )
    {
    TInt length (0);
    TInt i(0);
    for( ; i< aArray.Count(); ++i)
        {
        length += aArray[i]->Length() + KComma().Length();
        }

    HBufC8* buf = HBufC8::NewLC(length);
    TPtr8 des = buf->Des();

    for( i = 0; i < aArray.Count(); i++ )
        {
        des.Append(*aArray[i]);
        if( i != aArray.Count() - 1 )
            {
            des.Append(KComma());
            }
        }
    return buf;
    }

// -----------------------------------------------------------------------------
// UpnpString::AppendStringL
// Append to string.
// -----------------------------------------------------------------------------
//
EXPORT_C void AppendStringL( CBufFlat& aBuffer, const TDesC8& aString )
    {
    RBufWriteStream bufWs(aBuffer, aBuffer.Size());
    CleanupClosePushL(bufWs);
    bufWs.WriteL(aString);
    bufWs.CommitL();
    CleanupStack::PopAndDestroy(); 
    }

// -----------------------------------------------------------------------------
// UpnpString::AppendStringL
// Append to string.
// -----------------------------------------------------------------------------
//
EXPORT_C void AppendStringL( HBufC8*& aString, const TDesC8& aString2 )
    {
    if(aString==NULL)
        { 
        if(aString2.Length()==0)
            {
            return;
            }
        aString = HBufC8::NewLC(aString2.Length());
        aString->Des().Append(aString2);
        CleanupStack::Pop();
        }
    else if(aString2.Length()==0)
        {
        return;
        }
    else
        {
        HBufC8* nString = HBufC8::NewLC(aString->Length() + aString2.Length());
        nString->Des().Append(aString->Des());
        nString->Des().Append(aString2);

        delete aString;
        aString = nString;
        CleanupStack::Pop();
        }
    }

// -----------------------------------------------------------------------------
// UpnpString::StringToInt
// String to integer.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt StringToInt(const TDesC8& aStr, TInt* aInt)
    {
    if( 0 == aStr.Length() || aInt == NULL)
        {
        return KErrArgument;
        }
    TLex8 lex( aStr );
    return lex.Val( *aInt );
    }

// -----------------------------------------------------------------------------
// UpnpString::StringReplaceL
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C void StringReplaceL( const TDesC8& aOrginal, 
                              HBufC8*& aResult, 
                              const TDesC8& aTrg, 
                              const TDesC8& aReplacement )
    {
    TInt place = aOrginal.Find( aTrg );
    if( KErrNotFound != place )
        {
        aResult = StringReplaceL(aOrginal, aTrg, aReplacement);
        }
    else
        {    
        delete aResult;
        aResult = NULL;
        aResult = aOrginal.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// UpnpString::StringReplaceL
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C HBufC8* StringReplaceL( const TDesC8& aStr, 
                                 const TDesC8& aTrg, 
                                 const TDesC8& aReplacement )
    {
    __ASSERT_DEBUG(aTrg.Length() && aStr.Length(), User::Panic(_L("String"), 0));
    TInt length = aStr.Length();
    if(aReplacement.Length() > aTrg.Length())
        {
        TInt position(0);
        TPtrC8 loc;
        TInt numOccurrences(0);
        loc.Set(aStr);
        for(;;)
            {
            position = loc.Find(aTrg);
            if(position == KErrNotFound)
                {
                break;
                }
            numOccurrences++;
            position +=aTrg.Length();
            if(position >= loc.Length())
                {
                break;
                }
            loc.Set(loc.Mid(position)); 
            }
        length += (aReplacement.Length() - aTrg.Length())*numOccurrences;
        }
    HBufC8* result = HBufC8::NewLC(length);
    TPtr8 ptr = result->Des();
    ptr.Copy(aStr);
    StringReplaceInPlaceL(ptr, aTrg, aReplacement);
    if(ptr.MaxLength() > ptr.Length())
        {
        result = result->ReAllocL(result->Length());
        }
    CleanupStack::Pop(); 
    return result;
    }

// -----------------------------------------------------------------------------
// UpnpString::StringReplaceInPlaceL
// expected: aTrg.Length() >= aReplacement.Length()
// -----------------------------------------------------------------------------
//    
EXPORT_C void StringReplaceInPlaceL(
    TDes8& aString, 
    const TDesC8& aTrg,
    const TDesC8& aReplacement )
    {
    __ASSERT_DEBUG(aTrg != aReplacement, User::Panic(_L("String"), 1));

    TPtrC8 ptr(aString);
    TInt position(0);
    FOREVER
        {
        TInt found = ptr.Find( aTrg );
        if(found < 0)
            {
            return;
            }
        aString.Replace( position + found, aTrg.Length(), aReplacement );
        if(position + found + aReplacement.Length() < aString.Length())
            {
            position += found + aReplacement.Length();
            ptr.Set(aString.Mid(position));
            }
        else
            return;
        }
    }

// -----------------------------------------------------------------------------
// String::EncodeXmlStringL
// Encode XML string.
// -----------------------------------------------------------------------------
// 

EXPORT_C HBufC8* EncodeXmlStringL( HBufC8*& aBuf )
    {
    return EncodeXmlStringL( *aBuf );
    }

// -----------------------------------------------------------------------------
// String::DecodeXmlStringL
// Decode XML string.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* DecodeXmlStringL( HBufC8*& aBuf )
    {
    return DecodeXmlStringL( *aBuf );
    }

EXPORT_C HBufC8* EncodeXmlStringL( const TDesC8& aXmlString )
    {
    const TPtrC8 KEntities[5] = 
        {
        KGtEntity(), KLtEntity(), KQuotEntity(), KAposEntity(), KAmpEntity()
        };
    const TChar KChars[5] = 
        {
        '>', '<', '"', '\'', '&' 
        };
    
    HBufC8* result = HBufC8::NewLC( aXmlString.Length() * KReallocRatio );
    TPtr8 ptr = result->Des();
    
    TLex8 lexer( aXmlString );
    TChar ch;
    while ( ch = lexer.Get() )
        {
        if ( result->Length() >= ( ptr.MaxLength() - KMaxEntityLength ) )
            {
            TInt newLength( ptr.MaxLength() * KReallocRatio ) ;
            
            result = result->ReAllocL( newLength );
            CleanupStack::Pop(); // "old" result descriptor
            CleanupStack::PushL( result );
            ptr.Set( result->Des() );
            
            }
        TBool found = EFalse;
        for ( TInt i = 0; i < KEntitiesCount; i++ )
            {
            if ( ch == KChars[i] )
                {
                ptr.Append( '&' );
                ptr.Append( KEntities[i] );
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
            ptr.Append( ch );
            }
        }
    CleanupStack::Pop( result );
    return result;
    }

EXPORT_C HBufC8* DecodeXmlStringL( const TDesC8& aXmlString )
    {
    const TPtrC8 KEntities[5] = 
        {
        KGtEntity(), KLtEntity(), KQuotEntity(), KAposEntity(), KAmpEntity()
        };
    const TChar KChars[5] = 
        {
        '>', '<', '"', '\'', '&' 
        };
    
    HBufC8* result = HBufC8::NewLC( aXmlString.Length() );
    TPtr8 ptr = result->Des();
    
    TLex8 lexer( aXmlString );
    TChar ch;
    TBool insideEntity = EFalse;
    
    while ( ch = lexer.Get() )
        {
        if ( insideEntity )
            {
            if ( ch == ';' ) // entity ends
                {
                insideEntity = EFalse;
                TPtrC8 entity = lexer.MarkedToken();
                TBool found = EFalse;
                
                for ( TInt i = 0; i < KEntitiesCount; i++ )
                    {
                    if ( !entity.Compare( KEntities[i] )) // match
                        {
                        ptr.Append( KChars[i] );
                        found = ETrue;
                        break;           
                        }
                    }
                if ( !found )
                    {
                    // no match, appending as it is
                    ptr.Append( '&' );
                    ptr.Append( entity );    
                    }
                }
            }
        else 
            {
            if ( ch == '&' ) // entity starts
                {
                insideEntity = ETrue;
                lexer.Mark();
                }
            else 
                {
                ptr.Append( ch );
                }
            }
        }
    CleanupStack::Pop( result );
    return result;
    }


// -----------------------------------------------------------------------------
// UpnpString::InetToStringL
// Intet address to string.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* InetToStringL(const TInetAddr& aAddress)
    {
    TBuf<20> address;
    aAddress.Output(address);
    HBufC8* host8 = UpnpString::FromUnicodeL(address);
    CleanupStack::PushL(host8);
    TBuf8<10> port;
    port.Num(aAddress.Port());
    
    HBufC8* host = HBufC8::NewL(host8->Length() + port.Length() + 1);
    host->Des().Zero();
    host->Des().Append(*host8);
    host->Des().Append(UpnpString::KColon());
    host->Des().Append(port);
    
    CleanupStack::PopAndDestroy(host8);

    return host;
    }
    
// -----------------------------------------------------------------------------
// UpnpString::TrimLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C HBufC8* TrimLC(const TDesC8& aBuf, TBool aQuotations)
    {
    if( aBuf.Length() == 0 )
        {
        return aBuf.AllocLC();
        }
    TPtrC8 ptr(aBuf);    

    while( ( ('"' == ptr[0]) && aQuotations ) ||
        (' ' == ptr[0]) )
        {
        if(ptr.Length() == 1)
            {
            return HBufC8::NewLC(0);
            }
        ptr.Set(ptr.Mid(1));
        }
    while( ( ('"' == ptr[ptr.Length() - 1] ) && aQuotations ) ||
        (' ' == ptr[ptr.Length() - 1] ) )
        {
        ptr.Set(ptr.Left(ptr.Length() -1));            
        }

    return ptr.AllocLC();
    }

// -----------------------------------------------------------------------------
// UpnpString::DeleteArray
// Empties the array and deletes the referenced objects.
// -----------------------------------------------------------------------------
//
void DeleteArray(TAny* param)
    {
    RPointerArray<TPtrC8>* array;
    
    array = reinterpret_cast<RPointerArray<TPtrC8>*>(param);
    array->ResetAndDestroy();
    }

    // -----------------------------------------------------------------------------
    // UpnpString::ReplaceHttpCharacters
    // -----------------------------------------------------------------------------
    //
EXPORT_C void ReplaceHttpCharacters(TDes8& aString)
    {
    TInt i=-1;
    while(++i < aString.Length())
        {    //check if it is "%" and that the length from this position is at least 
        //3 = length of "%" HEX HEX
            if( (aString[i] == '%') && (aString.Length() >= i+3) )
            {
            TLex8 lexer(aString.Mid(i+1, KUrlCharNoLen-1)); // two characters next to '%'
            TUint8 charNo;
            if(lexer.Val(charNo, EHex) == KErrNone )
                {
                TText8 c(charNo);
                TPtrC8 charDes(&c, KOneCharLen);
                aString.Replace(i, KUrlCharNoLen, charDes);
                }
            }
        } 
    }
    
// -----------------------------------------------------------------------------
// UpnpString::ReplaceHttpCharactersL
// -----------------------------------------------------------------------------
//
EXPORT_C void ReplaceHttpCharactersL(TDes& aString)
    {
    HBufC8* temp8 = FromUnicodeL(aString);
    CleanupStack::PushL(temp8);
    TPtr8 tempPtr(temp8->Des());
    ReplaceHttpCharacters(tempPtr);
    
    HBufC* temp16 = ToUnicodeL(tempPtr);
    aString.Copy(*temp16);
    delete temp16;
    CleanupStack::PopAndDestroy(temp8);
    }

}
//  End of file
