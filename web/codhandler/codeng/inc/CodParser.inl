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
*      Inline method definitions of class CodParser.   
*      
*
*/


#ifndef COD_PARSER_INL
#define COD_PARSER_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TCodParser::TCodParser()
// ---------------------------------------------------------
//
TCodParser::TCodParser()
    {
    // NULL pointers for clarity.
    // Otherwise, semantics ensure hat these are never used
    // uninitialized (the only public method takes the data as arguments).
    iBuf = NULL;
    iData = NULL;
    iCurP = NULL;
    iEndP = NULL;
    }

// ---------------------------------------------------------
// TCodParser::Error()
// ---------------------------------------------------------
//
void TCodParser::Error( TInt aError )
    {
    // Store error, but keep going. We can still get the installNotifyURI and
    // report. Parsing will leave with the stored error code after processing
    // is done.
    if ( !iError )
        {
        iError = aError;
        }
    }

#endif /* def COD_PARSER_INL */
