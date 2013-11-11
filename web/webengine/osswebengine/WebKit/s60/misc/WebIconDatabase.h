/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implemetation of WebIconDatabase
*
*/

#ifndef __WebIconDatabase_H__
#define __WebIconDatabase_H__

#include <e32std.h>

class CGulIcon;

class WebIconDatabase
{
public:

    WebIconDatabase();
    virtual ~WebIconDatabase();
    void openSharedDatabase();
    
    CGulIcon* iconForPageURL(const TDesC8& url);
    void retainIconForURL(const TDesC8& url);
    void releaseIconForURL(const TDesC8& url);

private:

    WebIconDatabase(const WebIconDatabase&);                 // not implemented
    WebIconDatabase& operator=(const WebIconDatabase&);      // not implemented

};

#endif // __WebIconDatabase_H__

// END OF FILE



