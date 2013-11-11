/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Inline method definitions of class TDdParser.   
*      
*
*/


#ifndef DD_PARSER_INL
#define DD_PARSER_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TDdParser::TDdParser()
// ---------------------------------------------------------
//
TDdParser::TDdParser()
    {
    // NULL pointers for clarity.
    // Otherwise, semantics ensure hat these are never used
    // uninitialized (the only public method takes the data as arguments).
    iData = NULL;
    }

// ---------------------------------------------------------
// TDdParser::Error()
// ---------------------------------------------------------
//
void TDdParser::Error( TInt aError )
    {
    // Store error, but keep going. We can still get the installNotifyURI and
    // report. Parsing will leave with the stored error code after processing
    // is done.
    if ( !iError )
        {
        iError = aError;
        }
    }

#endif /* def DD_PARSER_INL */
