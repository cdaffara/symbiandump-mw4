/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser "base" class.
*
*/


#include <tinternetdate.h>

#include "FeedParser.h"
#include "FeedParserObserver.h"
#include "LeakTracker.h"
#include "XmlUtils.h"


_LIT8(KUrlStr, "url");

// -----------------------------------------------------------------------------
// CFeedParser::CFeedParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedParser::CFeedParser(CXmlUtils& aXmlUtils):
        iFeedMappings(3), iItemMappings(3), iXmlUtils(aXmlUtils)
    {
    }


// -----------------------------------------------------------------------------
// CFeedParser::~CFeedParser
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedParser::~CFeedParser()
    {
    iFeedMappings.Close();
    iItemMappings.Close();
    }
        
        
// -----------------------------------------------------------------------------
// CFeedParser::AddFeedMappingL
//
// Add a feed ElementHandler mapping.  See AddMappingL.
// -----------------------------------------------------------------------------
//
void CFeedParser::AddFeedMappingL(const TDesC8& aNamespace, const TDesC8& aElementName,
        TInt aValueId, ElementHandlerFunctionL aHandler)
    {
    AddMappingL(iFeedMappings, aNamespace, aElementName, aValueId, aHandler);
    }


// -----------------------------------------------------------------------------
// CFeedParser::HandleFeedChildL
//
// Process a child of a feed element.  See HandleChildL
// -----------------------------------------------------------------------------
//
void CFeedParser::HandleFeedChildL(TXmlEngElement aNode, 
        MFeedParserObserver& aObserver) const
    {
    HandleChildL(iFeedMappings, aNode, aObserver);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedParser::AddItemMappingL
//
// Add an item ElementHandler mapping.  See AddMappingL.
// -----------------------------------------------------------------------------
//
void CFeedParser::AddItemMappingL(const TDesC8& aNamespace, const TDesC8& aElementName,
        TInt aValueId, ElementHandlerFunctionL aHandler)
    {
    AddMappingL(iItemMappings, aNamespace, aElementName, aValueId, aHandler);
    }


// -----------------------------------------------------------------------------
// CFeedParser::HandleItemChildL
//
// Process a child of a item element.   See HandleChildL
// -----------------------------------------------------------------------------
//
void CFeedParser::HandleItemChildL(TXmlEngElement aNode, 
        MFeedParserObserver& aObserver) const
    {
    HandleChildL(iItemMappings, aNode, aObserver);
    }
    
    
// -----------------------------------------------------------------------------
// CFeedParser::ElementHandlerCDataL
//
// An ElementHandler function that extracts the value from the 
// child text nodes.
// -----------------------------------------------------------------------------
//
void CFeedParser::ElementHandlerCDataL(const CFeedParser& /*aParser*/, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver)
    {
    TDesC*  ucs2Des = NULL;

    // Get the text.
    ucs2Des = aXmlUtils.ExtractTextL(aNode);
    if ((ucs2Des == NULL) || (ucs2Des->Length() == 0))
        {
        delete ucs2Des;
        return;
        }

    // Set the value
    CleanupStack::PushL(ucs2Des);
    aObserver.AddAttributeL(aValueId, *ucs2Des);
    CleanupStack::PopAndDestroy(ucs2Des);
    }


// -----------------------------------------------------------------------------
// CFeedParser::ElementHandlerTextL
//
// An ElementHandler function that extracts the value from the 
// child text nodes then resolves any html entities and removes any markup.
// -----------------------------------------------------------------------------
//
void CFeedParser::ElementHandlerTextL(const CFeedParser& /*aParser*/, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver)
    {
    HBufC*  ucs2Des = NULL;

    // Get the text.
    ucs2Des = aXmlUtils.ExtractTextL(aNode);
    if ((ucs2Des == NULL) || (ucs2Des->Length() == 0))
        {
        delete ucs2Des;
        return;
        }

    // Clean it up.
    TPtr  ptr(ucs2Des->Des());
    
    CleanupStack::PushL(ucs2Des);
    (void) aXmlUtils.CleanupMarkupL(ptr, 0);

    // Set the value
    if (ucs2Des->Length() > 0)
        {        
        aObserver.AddAttributeL(aValueId, *ucs2Des);
        }
        
    CleanupStack::PopAndDestroy(ucs2Des);
    }


// -----------------------------------------------------------------------------
// CFeedParser::ElementHandlerUrlChildL
//
// An ElementHandler function that extracts the value from a child
// url element.  If the element doesn't contain any elements and it contains
// text it is extracted instead.
// -----------------------------------------------------------------------------
//
void CFeedParser::ElementHandlerUrlChildL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver)
    {
    TXmlEngElement  urlNode = NULL;
    
    // Get the url element.
    urlNode = aXmlUtils.GetFirstNamedChild(aNode, KUrlStr());

    // If it doesn't have a url child and it has no children at all just 
    // extract the url from the node's text.
    if ((urlNode.IsNull()) && (!urlNode.HasChildNodes()))
        {
        urlNode = aNode;
        }

    // Extracts the value url.
    ElementHandlerUrlL(aParser, aXmlUtils, urlNode, aValueId, aObserver);
    }


// -----------------------------------------------------------------------------
// CFeedParser::ElementHandlerUrlL
//
// An ElementHandler function that extracts the value from the 
// child text nodes.  It further performs url related clean up.
// -----------------------------------------------------------------------------
//
void CFeedParser::ElementHandlerUrlL(const CFeedParser& /*aParser*/, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode,  TInt aValueId, MFeedParserObserver& aObserver)
    {
    TDesC*  ucs2Des = NULL;

    // Get the text.
    ucs2Des = aXmlUtils.ExtractTextL(aNode);
    if ((ucs2Des == NULL) || (ucs2Des->Length() == 0))
        {
        delete ucs2Des;
        return;
        }

    CleanupStack::PushL(ucs2Des);

    // Clean up the url.
    TPtr ptr(const_cast<TUint16*>(ucs2Des->Ptr()), ucs2Des->Length());
    aXmlUtils.CleanupUrlL(ptr);

    // TODO: Resolve the href using feed url (the feed's url can be stored in CFeedParser).

    // Set the value
    aObserver.AddAttributeL(aValueId, *ucs2Des);
    CleanupStack::PopAndDestroy(ucs2Des);
    }


// -----------------------------------------------------------------------------
// CFeedParser::ElementHandlerDateL
//
// An ElementHandler function that extracts the date from the 
// child text nodes.  It can handle date formats defined in RFC 3339, RFC 822,
// RFC 1123, RFC 850, and RFC 1036
// -----------------------------------------------------------------------------
//
void CFeedParser::ElementHandlerDateL(const CFeedParser& /*aParser*/, 
        CXmlUtils& /*aXmlUtils*/, TXmlEngElement aNode,  TInt aValueId, MFeedParserObserver& aObserver)
    {
    TTime    date;
    TBool    dateSet = EFalse;
    RBuf8    rbuf;

    // Get the text.
    aNode.WholeTextContentsCopyL( rbuf );
    if ( rbuf.Length() == 0 )
        {
        rbuf.Close();
        return;
        }
    rbuf.CleanupClosePushL();

    // Try to handle the date with TInternetDate first.
    TInternetDate  internetData;
    
    TRAPD(err, internetData.SetDateL( rbuf ));
    if (err == KErrNone)
        {
        date = internetData.DateTime();
        dateSet = ETrue;
        }
    
    // Otherwise if that didn't work then try to interpret it as a RFC 3339 date.
    else if (err == KErrCorrupt)
        {
        TRAP(err, ParseRFC3339L( rbuf, date ));
        if (err == KErrNone)
            {
            dateSet = ETrue;
            }
        }
    
    // Set the value
    if (dateSet)
        {        
        TBuf16<25>  str;
        
        str.Format(_L("%Ld"), date.Int64());
        aObserver.AddAttributeL(aValueId, str);
        }
        
    CleanupStack::PopAndDestroy( /*rbuf*/ );
    }


// -----------------------------------------------------------------------------
// CFeedParser::AddMappingL
//
// Makes a mapping between an element's namespance and name with the normalized
// value-id (i.e. title or description) and a function which extracts out
// relevant data and applies it to a provided ValueHolder using the value-id 
// (see HandleChildL).
// -----------------------------------------------------------------------------
//
void CFeedParser::AddMappingL(RArray<ElementHandlerMapEntry>& aMappings, 
        const TDesC8& aNamespace, const TDesC8& aElementName, TInt aValueId, 
        ElementHandlerFunctionL aHandler)
    {
    ElementHandlerMapEntry  entry;

    // Init the entry.
    entry.iElementNamespace.Set(aNamespace);
    entry.iElementName.Set(aElementName);
    entry.iValueId = aValueId;
    entry.iElementHandlerL = aHandler;
    
    // Append the entry.
    User::LeaveIfError(aMappings.Append(entry));
    }


// -----------------------------------------------------------------------------
// CFeedParser::HandleChildL
//
// Using the mapping defined by calls to AddMappingL it passes aNode, 
// aValueHolder and associated value-id to the associated function.  The method
// does what is needed to extract the relevant values from the node and applies
// them on aValueHolder (using the normalized value-id).
// -----------------------------------------------------------------------------
//
void CFeedParser::HandleChildL(const RArray<ElementHandlerMapEntry>& aMappings, 
        TXmlEngElement aNode, MFeedParserObserver& aObserver) const
    {
    // Find the corresponding entry in the map.
    for (TInt i = 0; i < aMappings.Count(); i++)
        {
        // Call the ElementHandler.
        if (iXmlUtils.IsNamed(aNode, aMappings[i].iElementNamespace, 
                aMappings[i].iElementName))
            {
            (aMappings[i].iElementHandlerL)(*this, iXmlUtils, aNode, 
                    aMappings[i].iValueId, aObserver);            
            break;
            }
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedParser::RFC3339DateL
//
// Converts the given RFC 3339 date into a TTime.
// RFC 3339 format (examples): YYYY-MM-DDTHH:MM:SS.FFZ, 
// YYYY-MM-DDTHH:MM:SS.FF+HH:MM or YYYY-MM-DDTHH:MM:SS.FF-HH:MM
// -----------------------------------------------------------------------------
//
void CFeedParser::ParseRFC3339L(const TDesC8& aDateStr, TTime& aDate)
    {
    enum TPart  {EYear, EMonth, EDay, EHour, EMin, ESec, EMicro, EZoneHour, EZoneMin, ENone};

    TDateTime  dateTime;
    TInt       num;
    TBuf<30>   timeStr;
    TBuf<30>   zoneHour;
    TBuf<30>   zoneMin;
    TPart      part = EYear;
    TInt       zone = 0;
    
    timeStr.Zero();
    
    if (aDateStr.Length() > 30)
        {
        User::Leave(KErrCorrupt);
        }
        
    for (TInt i = 0; i < aDateStr.Length(); i++)
        {
        TChar  c(aDateStr[i]);
        
        // Extract the year.
        if (part == EYear)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if (c == '-')
                {
                // If timeStr.Length() equals 2 then get the current
                // year and use the first two digits to set the four digit year.
                if (timeStr.Length() == 2)
                    {
                    TTime    now;
                    TInt     year;
                    TBuf<4>  yearStr;
                    
                    now.UniversalTime();
                    year = now.DateTime().Year();
                    yearStr.AppendNum(year);
                    
                    timeStr.Insert(0, yearStr.Left(1));
                    timeStr.Insert(1, yearStr.Mid(1, 1));
                    }

                TLex16 lex(timeStr);                
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetYear(num));
                timeStr.Zero();
            
                part = EMonth;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the month.
        else if (part == EMonth)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if (c == '-')
                {
                TLex16  lex(timeStr);
                TMonth  month = EJanuary;
                
                lex.Val(num);
                
                // Convert num to the month enum.
                switch (num)
                    {
                    case 1:
                        month = EJanuary;
                        break;
                    case 2:
                        month = EFebruary;
                        break;
                    case 3:
                        month = EMarch;
                        break;
                    case 4:
                        month = EApril;
                        break;
                    case 5:
                        month = EMay;
                        break;
                    case 6:
                        month = EJune;
                        break;
                    case 7:
                        month = EJuly;
                        break;
                    case 8:
                        month = EAugust;
                        break;
                    case 9:
                        month = ESeptember;
                        break;
                    case 10:
                        month = EOctober;
                        break;
                    case 11:
                        month = ENovember;
                        break;
                    case 12:
                        month = EDecember;
                        break;
                    default:
                        User::Leave(KErrCorrupt);
                        break;
                    }
                    
                User::LeaveIfError(dateTime.SetMonth(month));
                timeStr.Zero();
            
                part = EDay;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the day.
        else if (part == EDay)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if ((c == 'T') || (c == 't'))
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                // Day is zero based for some odd reason...
                User::LeaveIfError(dateTime.SetDay(num - 1));
                timeStr.Zero();
            
                part = EHour;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the hour.
        else if (part == EHour)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if (c == ':')
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetHour(num));
                timeStr.Zero();
            
                part = EMin;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the min.
        else if (part == EMin)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if (c == ':')
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetMinute(num));
                timeStr.Zero();
            
                part = ESec;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
            
        // Extract the sec.
        else if (part == ESec)
            {
            if (c.IsDigit())
                {
                timeStr.Append(c);
                }
            else if (c == '.')
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetSecond(num));
                timeStr.Zero();
            
                part = EMicro;
                }
            else if ((c == 'Z') || (c == 'z'))
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetSecond(num));
                timeStr.Zero();
            
                zone = 0;
                part = ENone;
                }
            else if (c == '+')
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetSecond(num));
                timeStr.Zero();
            
                zone = 1;
                part = EZoneHour;
                }
            else if (c == '-')
                {
                TLex16 lex(timeStr);
                
                lex.Val(num);
                User::LeaveIfError(dateTime.SetSecond(num));
                timeStr.Zero();
            
                zone = -1;
                part = EZoneHour;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Ignore the micro-sec.
        else if (part == EMicro)
            {
            if (c.IsDigit())
                {
                // Ignore microseconds.
                }
            else if ((c == 'Z') || (c == 'z'))
                {
                zone = 0;
                part = ENone;
                }
            else if (c == '+')
                {
                zone = 1;
                part = EZoneHour;
                }
            else if (c == '-')
                {
                zone = -1;
                part = EZoneHour;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the hour.
        else if (part == EZoneHour)
            {
            if (c.IsDigit())
                {
                zoneHour.Append(c);
                }
            else if (c == ':')
                {
                part = EZoneMin;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        // Extract the min.
        else if (part == EZoneMin)
            {
            if (c.IsDigit())
                {
                zoneMin.Append(c);
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
            
        else
            {
            User::Leave(KErrCorrupt);
            }
        }

    // Handle the common date only format (i.e. YYYY-MM-DD).
    if ((part == EDay) && ((aDateStr.Length() == 10) || (aDateStr.Length() == 8)))
        {
        TLex16 lex(timeStr);
        
        lex.Val(num);
        
        // Day is zero based for some odd reason...
        User::LeaveIfError(dateTime.SetDay(num - 1));
        }
    
    // Convert timeStr into a TTime.
    aDate = dateTime;

    // Adjust the aDate to UTC.
    if (zone != 0)
        {
        TLex16 zHoursLex(zoneHour);
        TLex16 zMinLex(zoneMin);
        TInt  zHours;
        TInt  zMins;
        
        // Get the offset.
        zHoursLex.Val(zHours); 
        zMinLex.Val(zMins); 
        
        // Adjust it back to UTC.
        if (zone > 0)
            {
            aDate -= TTimeIntervalHours(zHours);
            aDate -= TTimeIntervalMinutes(zMins);
            }
            
        // Otherwise adjust it forward to UTC.
        else
            {
            aDate += TTimeIntervalHours(zHours);
            aDate += TTimeIntervalMinutes(zMins);
            }
        }
    }

