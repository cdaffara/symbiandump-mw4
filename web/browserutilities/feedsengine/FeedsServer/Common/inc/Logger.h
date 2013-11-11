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
* Description:  Feeds logging
*
*/


#ifndef LOGGER_H
#define LOGGER_H

#ifdef _DEBUG
#include <flogger.h>

#define FEED_LOG(a, b, c, d) RFileLogger::WriteFormat((a), (b), (c), (d))
#define FEED_LOG1(a, b, c, d, e) RFileLogger::WriteFormat((a), (b), (c), (d), (e))
#define FEED_LOG2(a, b, c, d, e, f) RFileLogger::WriteFormat((a), (b), (c), (d), (e), (f))
#define FEED_LOG3(a, b, c, d, e, f, g) RFileLogger::WriteFormat((a), (b), (c), (d), (e), (f), (g))
#define FEED_LOG4(a, b, c, d, e, f, g, h) RFileLogger::WriteFormat((a), (b), (c), (d), (e), (f), (g), (h))
#define FEED_LOG5(a, b, c, d, e, f, g, h, i) RFileLogger::WriteFormat((a), (b), (c), (d), (e), (f), (g), (h), (i))
#define FEED_LOG6(a, b, c, d, e, f, g, h, i, j) RFileLogger::WriteFormat((a), (b), (c), (d), (e), (f), (g), (h), (i), (j))

#else

#define FEED_LOG(a, b, c, d)
#define FEED_LOG1(a, b, c, d, e)
#define FEED_LOG2(a, b, c, d, e, f)
#define FEED_LOG3(a, b, c, d, e, f, g)
#define FEED_LOG4(a, b, c, d, e, f, g, h) 
#define FEED_LOG5(a, b, c, d, e, f, g, h, i) 
#define FEED_LOG6(a, b, c, d, e, f, g, h, i, j) 

#endif
#endif

