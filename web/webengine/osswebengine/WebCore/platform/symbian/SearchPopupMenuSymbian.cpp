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
#include "SearchPopupMenu.h"


namespace WebCore {

SearchPopupMenu::SearchPopupMenu(PopupMenuClient* client)
    : PopupMenu(client)
{
}

bool SearchPopupMenu::enabled()
{
    return false;
}

void SearchPopupMenu::saveRecentSearches(const AtomicString& , const Vector<String>& )
{
}

void SearchPopupMenu::loadRecentSearches(const AtomicString& , Vector<String>& )
{
}

}
