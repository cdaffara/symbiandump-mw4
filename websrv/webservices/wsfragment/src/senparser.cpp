/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Class implements XML Parser capable of service multiple handlers
*
*/








// INCLUDE FILES
#include <e32std.h>
#include "senparserimpl.h"

CSenParser::CSenParser()
    {
    }

EXPORT_C CSenParser* CSenParser::NewL()
    {
    return CSenParserImpl::NewL();
    }

EXPORT_C CSenParser* CSenParser::NewLC()
    {
    return CSenParserImpl::NewLC();
    }

EXPORT_C CSenParser* CSenParser::NewL(CParser* apParser)
    {
    return CSenParserImpl::NewL(apParser);
    }

EXPORT_C CSenParser* CSenParser::NewLC(CParser* apParser)
    {
    return CSenParserImpl::NewLC(apParser);
    }

EXPORT_C CSenParser* CSenParser::NewL(const TDesC8& aParserMimeType)
    {
    return CSenParserImpl::NewL(aParserMimeType);
    }

EXPORT_C CSenParser* CSenParser::NewLC(const TDesC8& aParserMimeType)
    {
    return CSenParserImpl::NewLC(aParserMimeType);
    }

// End of file




