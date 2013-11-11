/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "config.h"
#include "WebCoreCache.h"

#include "Cache.h"

TWebCoreCacheStatistics WebCoreCache::Statistics()
    {
    /*WebCore::Cache::Statistics s = WebCore::Cache::getStatistics();

    return [NSArray arrayWithObjects:
        [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithInt:s.images.count], @"images",
            [NSNumber numberWithInt:s.movies.count], @"movies",
            [NSNumber numberWithInt:s.styleSheets.count], @"style sheets",
            [NSNumber numberWithInt:s.scripts.count], @"scripts",
            [NSNumber numberWithInt:s.other.count], @"other",
            nil],
        [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithInt:s.images.size], @"images",
            [NSNumber numberWithInt:s.movies.size], @"movies",
            [NSNumber numberWithInt:s.styleSheets.size] ,@"style sheets",
            [NSNumber numberWithInt:s.scripts.size], @"scripts",
            [NSNumber numberWithInt:s.other.size], @"other",
            nil],
        nil];*/
    return TWebCoreCacheStatistics();
    }

void WebCoreCache::Empty()
    {
    WebCore::cache()->prune();
    }

void WebCoreCache::SetDisabled( TBool aDisabled )
    {
    WebCore::cache()->setDisabled( aDisabled );
    }
