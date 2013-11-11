/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef QWEBPAGEHISTORY_P_H
#define QWEBPAGEHISTORY_P_H

#include "BackForwardList.h"
#include "HistoryItem.h"

class QWebHistoryItemPrivate : public QSharedData
{
public:
    QWebHistoryItemPrivate(WebCore::HistoryItem *i)
    {
        i->ref();
        item = i;
    }
    ~QWebHistoryItemPrivate()
    {
        item->deref();
    }
    
    WebCore::HistoryItem *item;
};

class QWebPageHistoryPrivate : public QSharedData
{
public:
    QWebPageHistoryPrivate(WebCore::BackForwardList *l)
    {
        l->ref();
        lst = l;
    }
    ~QWebPageHistoryPrivate()
    {
        lst->deref();
    }
    WebCore::BackForwardList *lst;
};


#endif
