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


#include <e32svr.h>

#include "SVGAnimTimingParser.h"
#include "SVGStringTokenizer.h"
#include "SVGAnimationBase.h"

#include "SVGTokenizer.h"
_LIT( KTextRepeat, "repeat" );

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgAnimTimingParser* CSvgAnimTimingParser::NewL( const TDesC& aTimingDes, CSvgElementImpl* aElement )
    {
    CSvgAnimTimingParser* self = NewLC( aTimingDes, aElement );
    CleanupStack::Pop();
    return self;
    }
//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimTimingParser* CSvgAnimTimingParser::NewLC( const TDesC& aTimingDes, CSvgElementImpl* aElement )
    {
    CSvgAnimTimingParser* self = new ( ELeave ) CSvgAnimTimingParser();
    CleanupStack::PushL( self );
    self->ConstructL( aTimingDes, aElement );
    return self;
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimTimingParser::CSvgAnimTimingParser() : iTimingDes( NULL, 0 )
    {

    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimingParser::ConstructL( const TDesC& aTimingDes, CSvgElementImpl* aElement )
    {
    // Copy Timing descriptor and trim all spaces
    iBuf = HBufC::NewL( aTimingDes.Length() );
    *iBuf = aTimingDes; // copy data
    iTimingDes.Set( iBuf->Des() );
    iTimingDes.TrimAll();
    iElement = aElement;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimTimingParser::~CSvgAnimTimingParser()
    {
    delete iBuf;
    iBuf = NULL;
    }


//***************************************************************
// 'begin' and 'end' attribute parser
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimingParser::Parse( TDes& aIdValue,
                                  TSvgEvent& aEvent,
                                  TInt32& aClockValue,
                                  TReal32& aRepeatValue,
                                  TBool aBeginAttribute )
    {
    // Init value
    aEvent = ESvgEventNone;
    aClockValue = 0;
    aRepeatValue = 0;

    TLex lex( iTimingDes );
    
    lex.SkipSpace();
    
    // Check if Offset-value only
    if ( lex.Peek() == '+' || lex.Peek() == '-' || lex.Peek().IsDigit() || lex.Peek() == '.' )
        {
        lex.SkipSpace();
        ParseClockValue( lex, aClockValue );
        aEvent = ESvgEventNone;
        return;
        }

    // Parse first token
    lex.Mark();
    SkipAlphaNumeric( lex );
    TPtrC firstToken = lex.MarkedToken();
    // Checks if the first part of the begin attribute is an event
    TSvgEvent event = DesToEventId( firstToken );

    if ( event == ESvgEventNone )
        {
        if ( firstToken == _L( "accessKey" ) )
            {
            // accessKey(x)
            aEvent = ESvgEventKey;
            //Skip any spaces between "Access Key" and opening Brace "("
            lex.SkipSpace();
            //Get automatically moves the pointer to the next character
            TChar openBraces = lex.Get();
            //If the next character is opening brace then continue
            if(openBraces == '(')
                {
                    TChar curValue = 0;
                    TInt inc=0;
                    
                    //inc checks how many chars are there between opening and closing braces
                    // it is needed to ensure only one char is there between "(" and ")"
                    do
                    {
                        inc++;
                        //If there are more than one char between "(" and ")" exit
                        if(inc >1)
                            break;
                        //Move to the next char
                        curValue = lex.Peek();
                        lex.Inc();
                    }while(lex.Peek() != ')');
                    
                    //assign cur value to access key if no of chars between "(" and ")" is 1 
                    if(inc == 1)
                    {
                        iAccessKeyValue = curValue;    
                    }
                    
                   if ( lex.Peek() == ')' ) 
                    {
                    lex.Inc();
                    }
                   // if some offset value is given with accesskey then parse it. 
                   ParseClockValue( lex, aClockValue ); 
                    
                }
            }
        else if ( firstToken == KTextRepeat )
            {
            // repeat(x) without Id-value
            // Not yet supported
            aEvent = ESvgEventRepeatEvent;
            }
        else if ( firstToken == _L( "wallclock" ) )
            {
            // wallclock(....)
            // Not yet supported
            aEvent = ESvgEventWallClock;
            }
        else //
            {
                // The first token was 'id'. Parse next token as event
            if ( lex.Peek() != '.')
				{
				// This is not a valid begin value
				aEvent = ESvgEventNone;
				aIdValue = _L("");
				aClockValue= KTimeIndefinite; // same as KTimeIndefinite
				return;
				}
            lex.Inc();      // skip '.': no space allowed between id, '.', and event
			lex.Mark();
            aIdValue = firstToken;      // copy token string

            SkipAlphaNumeric( lex );
            TPtrC secondToken = lex.MarkedToken();

            // What if the event is not clearly mentioned
            aEvent = DesToEventId( secondToken );
            if(aEvent != ESvgEventNone)
            	{
			if ( secondToken == KTextRepeat )
                {
                // repeat(x) with Id-value

             	((CSvgAnimationBase*)iElement)->StoreRepeatId(firstToken, aBeginAttribute);

                aEvent = ESvgEventRepeatEvent;
				// TChar tmpchar = lex.Peek();
                if ( lex.Peek() == '(' )
                    {
                    lex.Inc();
                    if (lex.Val(aRepeatValue, '.' )!= KErrNone)
						{
						    if(lex.Val(aRepeatValue) != KErrNone )
						aRepeatValue=1;
						}
                        lex.Inc();
                    }

                }
                ParseClockValue( lex, aClockValue );
            }
	         else
	         	{
	         	// this is not a valid begin value;
	         	aEvent = ESvgEventNone;
				aIdValue = _L("");
				aClockValue= KTimeIndefinite; // same as KTimeIndefinite
				return;
	         	}

            }
        }
    else
        {
        // Event without id
        aEvent = event;
//        TPtrC tempToken = lex.MarkedToken();
        ParseClockValue( lex, aClockValue );
//        aClockValue = 0;
        }
    }

//***************************************************************
// Private methods

// ---------------------------------------------------------------------------
// Parse clock value
// ---------------------------------------------------------------------------
void CSvgAnimTimingParser::ParseClockValue( TLex& aLex, TInt32& aClockValue )
    {
    TReal32 value = 0;

	aLex.SkipSpaceAndMark();
	
	TBool wasAddition = ETrue;
	
	if (aLex.Peek() == '+')
	{
		aLex.Inc();
		aLex.Mark();
		aLex.SkipSpaceAndMark();	
	}
	else if (aLex.Peek() == '-')
	{
		wasAddition = EFalse;
		aLex.Inc();
		aLex.Mark();
		aLex.SkipSpaceAndMark();
	}
	
	TTokenizer tokenizer( aLex.Remainder() );
	
    // blank, setting to zero
    if ( tokenizer.IsAtEnd() )
        {
        aClockValue = 0;
        }
    else if ( tokenizer.SkipDecimal() )
        {
        // Decimal number , extract it
        TPtrC decimalString = tokenizer.SkippedString();
        TLex lex( decimalString );
        // Specify the decimal seperator, instead of using
        // locale specific seperator.        
        lex.Val( value, '.' );
        
        tokenizer.SkipWhiteSpace();
        // Get the units
        TPtrC remainder = tokenizer.Remainder();
        // millseconds
        if ( remainder == _L( "ms" ) )
            {
            aClockValue = value;
            
            if (!wasAddition)
            aClockValue = 0 - aClockValue;
            }
        // seconds: implied or 's'
        else if ( remainder.Length() == 0 || remainder == _L( "s" ) )
            {
            aClockValue = value * 1000;
            
            if (!wasAddition)
            aClockValue = 0 - aClockValue;
            }
        // anything else is invalid
        else
            {
            aClockValue = KTimeIndefinite;
            }
        }
    // invalid
    else
        {
        aClockValue = KTimeIndefinite;
        }
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimingParser::SkipUntilNumEnd( TLex& aLex )
    {
    TChar tmpchar = aLex.Peek();
    while ( tmpchar.IsDigit() || tmpchar == '.' )
		{
        tmpchar = aLex.Get();
		}

    if ( !aLex.Eos() && (aLex.Offset() > 0) )
		{
        aLex.UnGet();
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimingParser::SkipAlphaNumeric( TLex& aLex )
    {
    TChar tmpchar = aLex.Peek();
    while ( tmpchar.IsAlphaDigit() || tmpchar == '_' || tmpchar == '-' )
		{
        tmpchar = aLex.Get();
		}
    if ( !aLex.Eos() && (aLex.Offset() > 0) )
		{
        aLex.UnGet();
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgEvent CSvgAnimTimingParser::DesToEventId( const TDesC& aEventDes )
    {
    // 'begin' matches with 'begineEvent' and 'end' matches with 'endEvent'

    if ( aEventDes == _L("begin") )
		{
		return ESvgEventBeginEvent;
		}
	else if ( aEventDes == _L("end") )
		{
		return ESvgEventEndEvent;
		}
	else if ( aEventDes == _L("repeat") )
		{
		return ESvgEventRepeatEvent;
		}
	else if ( aEventDes == _L("focusin") )
		{
		return ESvgEventFocusin;
		}
	else if ( aEventDes == _L("focusout") )
		{
		return ESvgEventFocusout;
		}
	else if ( aEventDes == _L("activate") )
		{
		return ESvgEventActivate;
		}
	else if ( aEventDes == _L("click") )
		{
		return ESvgEventClick;
		}
	else if ( aEventDes == _L("mousedown") )
		{
		return ESvgEventMousedown;
		}
	else if ( aEventDes == _L("mouseup") )
		{
		return ESvgEventMouseup;
		}
	else if ( aEventDes == _L("mouseover") )
		{
		return ESvgEventMouseover;
		}
	else if ( aEventDes == _L("mousemove") )
		{
		return ESvgEventMousemove;
		}
	else if ( aEventDes == _L("mouseout") )
		{
		return ESvgEventMouseout;
		}
	else if ( aEventDes == _L("DOMSubtreeModified") )
		{
		return ESvgEventDOMSubtreeModified;
		}
	else if ( aEventDes == _L("DOMNodeInserted") )
		{
		return ESvgEventDOMNodeInserted;
		}
	else if ( aEventDes == _L("DOMNodeRemoved") )
		{
		return ESvgEventDOMNodeRemoved;
		}
	else if ( aEventDes == _L("DOMNodeRemovedFromDocument") )
		{
		return ESvgEventDOMNodeRemovedFromDocument;
		}
	else if ( aEventDes == _L("DOMNodeInsertedIntoDocument") )
		{
		return ESvgEventDOMNodeInsertedIntoDocument;
		}
	else if ( aEventDes == _L("DOMAttrModified") )
		{
		return ESvgEventDOMAttrModified;
		}
	else if ( aEventDes == _L("DOMCharacterDataModified") )
		{
		return ESvgEventDOMCharacterDataModified;
		}
	else if ( aEventDes == _L("SVGLoad") )
		{
		return ESvgEventSVGLoad;
		}
	else if ( aEventDes == _L("SVGUnload") )
		{
		return ESvgEventSVGUnload;
		}
	else if ( aEventDes == _L("SVGAbort") )
		{
		return ESvgEventSVGAbort;
		}
	else if ( aEventDes == _L("SVGError") )
		{
		return ESvgEventSVGError;
		}
	else if ( aEventDes == _L("SVGResize") )
		{
		return ESvgEventSVGResize;
		}
	else if ( aEventDes == _L("SVGScroll") )
		{
		return ESvgEventSVGScroll;
		}
	else if ( aEventDes == _L("SVGZoom") )
		{
		return ESvgEventSVGZoom;
		}

	else
		{
        return ESvgEventNone;
    }

    }
