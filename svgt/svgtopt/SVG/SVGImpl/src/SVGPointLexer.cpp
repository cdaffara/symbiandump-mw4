/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#include "SVGPointLexer.h"

#include "stdlib.h"
#include "string.h"

// Below define is used as a mask for getting LSB of 16bit number
// for endian-agnostic progressing.
#define KLSBMASK ( 0xFF )
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgPointLexer::TSvgPointLexer( const TDesC& aDes ) :
                iLex( aDes ), iNegativeNumber( EFalse )
{
    iDataCopy = NULL;
    iDataCopy = aDes.Alloc();
    if ( iDataCopy )
    {
        iData = (TUint16 *)iDataCopy->Ptr();
        iDataEnd = iData + iDataCopy->Length();
    }
}

void TSvgPointLexer::Cleanup()
{
    if ( iDataCopy )
    {
        delete iDataCopy;
        iDataCopy = NULL;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32 TSvgPointLexer::GetNext( TChar& aCommand, TFloatFixPt& aVal )
    {
    TReal32 realnum = 0.0;

    // Parse a command and minus symbol
    iNegativeNumber = EFalse;
    aCommand = SkipUntilNum();
    if ( iLex.Eos() && aCommand != 'z' )
        {
        return -1;
        }
    else if ( aCommand == 'z' )
        {
        // if 'z' was found return immediately
        // never parse for numbers
        return KErrNone;
        }

    // Parse a number with fraction
    iLex.Mark();
    if ( SkipToNumEnd() )
        {
         TInt result = TLex( iLex.MarkedToken() ).Val( realnum, '.' ) ;
	     if (result != KErrNone)
			{
			realnum= 0.0;
			return result;
			}
        }
    else
        {
        realnum = 0.0;
        }
    if ( iNegativeNumber )
        realnum = -realnum;
    aVal = ( TFloatFixPt ) realnum;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool TSvgPointLexer::GetNextWithNumOfPoints( TFloatFixPt *aVal, TInt NoOfPoints)
    {
    TReal32 realnum = 0.0;
	TUint8 tmpchar;
    TInt i=0;

	for( i=0;i < NoOfPoints;i++ )
		{
        // Since iData is a TUint16 pointer, and we need to 
        // operate with 8bit ASCII, mask LSB to obtain the same
		tmpchar = *iData & KLSBMASK;
        // Traverse the string while character encountered is
        // ' ', LineFeed('\0xa'), ',', Carriage Return ('\0xd')
		while( !(tmpchar^32) || !(tmpchar^10) || !(tmpchar^44) || !(tmpchar^13) )
			{
			iData++;
            if ( iData < iDataEnd )    
                {
		        tmpchar = *iData & KLSBMASK;
                }
            else
                {
                // Exceeding buffer limits, return error
                return EFalse;
                }
			}

        // Check if character encountered is '-' 
		if( !(tmpchar^45) )
			{
			iData++;

            if ( iData < iDataEnd )
                {
    			if(SkipToNumEndWithoutLex(realnum) )
    				{
    				realnum = -realnum;
    				}
    			else
    				{
    				return EFalse;
    				}
                }
            else // if ( iData < iDataEnd )
                {
                // Exceeding buffer limits, return error
                return EFalse;
                }
			}
		else // Not a '-' char
			{
			    if( !(tmpchar^43) ) // to increment the data if '+' is given
			        {
			        iData++;    
			        }
			
			if( !SkipToNumEndWithoutLex(realnum) )
				{
				return EFalse;
				}
			}
        // Save the converted number into float array 
        // provided by caller.
		aVal[i] = ( TFloatFixPt )(realnum);
		}

	return ETrue;
    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32 TSvgPointLexer::GetNextPoint( TChar& aCommand, TFloatFixPt& aX, TFloatFixPt& aY )
    {
    TReal32 realnum = 0.0;

    // Parse X value
    iNegativeNumber = EFalse;
    aCommand = SkipUntilNum();
    if ( iLex.Eos() )
        {
        return -1;
        }
    iLex.Mark();
    if ( SkipToNumEnd() )
        {
        TInt result = TLex( iLex.MarkedToken() ).Val( realnum, '.' ) ;
		if (result != KErrNone)
			{
			realnum= 0.0;
			return result;
			}
        }
    else
        {
        realnum = 0.0;
        }
    if ( iNegativeNumber )
        realnum = -realnum;
    aX = realnum;

    // Parse Y value
    iNegativeNumber = EFalse;
    iLex.Mark();
    SkipUntilNum();
    if ( iLex.Eos() )
        {
        return -1;
        }
    iLex.Mark();
    if ( SkipToNumEnd() )
        {
       TInt result = TLex( iLex.MarkedToken() ).Val( realnum, '.') ;
	   if (result != KErrNone)
			{
			realnum= 0.0;
			return result;
			}
        }
    else
        {
        realnum = 0.0;      // exponential description
        }
    if ( iNegativeNumber )
        realnum = -realnum;
    aY = realnum;

    return KErrNone;
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool TSvgPointLexer::SkipToNumEnd()
    {
    TChar tmpchar = iLex.Peek();
    TBool tmpbool = ETrue;
    TBool acceptMinus = EFalse;

	while ( tmpchar.IsDigit() ||
            !(tmpchar^46) ||
            ( !(tmpchar^45) && acceptMinus ) )
        {
        iLex.Inc();
        tmpchar = iLex.Peek();
        acceptMinus = EFalse;

		if ( !(tmpchar^101) || !(tmpchar^69) )
            {
            // exponential value
            iLex.Inc();
            tmpchar = iLex.Peek();
            tmpbool = EFalse;
            acceptMinus = ETrue;
            }
        }
    return tmpbool;
    }

//
// ---------------------------------------------------------------------------
// TBool TSvgPointLexer::SkipToNumEndWithoutLex(TReal32 &aNum)
//   This function parses the number at the current position into aNum
// ---------------------------------------------------------------------------
TBool TSvgPointLexer::SkipToNumEndWithoutLex(TReal32 &aNum)
    {
    TBool numberFound = EFalse;
    TReal32 lNumber=0;
    // Since iData is a TUint16 pointer, and we need to 
    // operate with 8bit ASCII, mask LSB to obtain the same
    TUint8 tmpchar = *iData & KLSBMASK;
    TInt fractionPart=0;

    // Scan through the string while the character is 
    // between '0' and '9' 
    // or is '.'
    // and there are more characters in the string
    while ( (((tmpchar > 47) && (tmpchar < 58) )||
            !(tmpchar^46)) && iData < iDataEnd )
        {
        // If a '.' is not found
        if(tmpchar^46)
            {
            // This means that the char is between '0' and '9'
            numberFound = ETrue;
            // Append it to lNumber
            lNumber = 10*lNumber + (tmpchar -'0');

            if(fractionPart)
                {
                fractionPart++;
                }
            }
        else // '.' is found
            {
            fractionPart++;
            }
        
        iData++;
        if ( iData < iDataEnd )
            {
            tmpchar = *iData & KLSBMASK;
            
            // Check if characted is 'e' or 'E'
            // for eg. -3.486e-007
            // This is for floating point representation and is
            // not supported currently. Such numbers are treated 
            // as 0.0. 
            // The number is skipped.
            if ( (!(tmpchar^101) || !(tmpchar^69)) )
                {
                iData++;
                if ( iData < iDataEnd )
                    {
                    tmpchar = *iData & KLSBMASK;                    
                
                    // Check if character is '-', if so 
                    // skip it 
                    if( !(tmpchar^45) )
                        {
                        iData++;
                        if ( iData < iDataEnd )
                            {
                            tmpchar = *iData & KLSBMASK;
                            }
                        }
                    // Skip the numbers and decimal point after 'e'
                    while ( ( ( ( tmpchar > 47 ) && ( tmpchar < 58 ) ) ||
                        !( tmpchar^46 ) ) && ( iData < iDataEnd ) )
                        {
                        iData++;
                        if ( iData < iDataEnd )
                            {
                            tmpchar = *iData & KLSBMASK;
                            }
                        }
                    }
                aNum = 0.0;
                return numberFound;
               } // if (exponential number)  
            } // if iData < iDataEnd
        } // end while 
    // Account for decimal point by performing following calculation
    // lNumber = lNumber / ( 10^ ( fractionPart - 1 ) )
    if(numberFound)
        {
        TInt l=0;
        TInt k=1;
        while(l<(fractionPart-1))
            {
            k=(k*10);
            l++;
            }
        lNumber = (lNumber/k);
        aNum = lNumber;
        }

    return numberFound;
    }




//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TChar TSvgPointLexer::SkipUntilNum()
    {
    TChar ret = '\0';
    iLex.SkipSpace();
    TChar tmpchar = iLex.Peek();
    // skip until finding a digit or '.'

    while ( !( tmpchar.IsDigit() || tmpchar == '.' ) && !iLex.Eos() )
        {
        if ( tmpchar == '-' )
            {
            iNegativeNumber = ETrue;
            }
        else if ( tmpchar == 'Z' || tmpchar == 'z' )
            {
            // if 'Z' or 'z' was found, return imediately.
            iLex.Inc();
            return 'z';
            }
        else if ( tmpchar != ' ' && tmpchar != ',' && tmpchar != '\n' )
            // a better way may be for only selected characters to be recognized and not ignored.
        {
            ret = tmpchar;
            }
        iLex.Inc();
        tmpchar = iLex.Peek();
        }

    return ret;
    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool TSvgPointLexer::GetCommand(TUint8 &a)
{
    a = '\0';
    TUint8 tmpchar;
    while (iData < iDataEnd )
        {
        tmpchar = *iData & KLSBMASK;
        // If character is a not between '0' and '9' and not '.' and '-'
        if (!((tmpchar > 47) && (tmpchar < 58)) && (tmpchar^46) && (tmpchar^45) )
             {
             // Check if character is an alphabet
             if((tmpchar >=65 && tmpchar <= 90) || (tmpchar >=97 && tmpchar <= 122) )
                {
                a = tmpchar;
                iData++;
                return ETrue;
                }
            else
                {
                iData++;
                }
             }
         else
             {
             return ETrue;
             }

        }
   return EFalse;

}

