/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Downloaded content handler interface.
*
*/


#ifndef DOWNLOADEDCONTENTHANDLER_H
#define DOWNLOADEDCONTENTHANDLER_H

#include <e32std.h>
#include <apmstd.h>

#warning The Browser Launcher API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// FORWARD DECLARATIONS

class RFile;
class CAiwGenericParamList;

// CLASS DECLARATION

/**
*  Downloaded content handler interface. Clients should implement 
*  it if they want to handle content that has been downloaded by 
*  the BrowserLauncher.
*/
class MDownloadedContentHandler
    {
    public:

        /**
        * Handle downloaded content that has been saved to a file, 
        * and return ETrue if it was handled successfully.
        * @param aFileName File name with path.
        * @param aParamList Generic parameter list.
        * @param aContinue Output parameter: It must be ETrue if the caller wants 
        *                  Browser not to terminate after the content is handled.
        * @return ETrue if the implementer has processed the content. If it is 
        *                  EFalse, BrowserLauncher opens that content in an 
        *                  appropriate viewer application.
        */
        virtual TBool HandleContentL( const TDesC& aFileName, 
                                      const CAiwGenericParamList& aParamList, 
                                      TBool& aContinue ) = 0;

        // reserved for future use
        virtual TAny* DownloadedContentHandlerReserved1( TAny* /*aAnyParam*/ ) = 0;
    };

#endif // DOWNLOADEDCONTENTHANDLER_H
