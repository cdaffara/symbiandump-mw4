/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* WLAN Sniffer list view list widget item.
*/

// System includes

#include <HbIcon>

// User includes

#include "wlansnifferlistitem.h"

/*!
    \class WlanSnifferListItem
    \brief This class implements the custom list widget item used in
           WLAN Sniffer list view.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

WlanSnifferListItem::WlanSnifferListItem() : HbListWidgetItem()
{
    // Fill data model with empty icons to work as a skeleton
    QVariantList decoratorList;
    decoratorList << HbIcon() << HbIcon() << HbIcon();
    setData(decoratorList, Qt::DecorationRole);
    
    // Fill data model with empty text to work as a skeleton
    QVariantList displayList;
    displayList << QString();
    setData(displayList, Qt::DisplayRole);
}

/*!
    Destructor.
*/

WlanSnifferListItem::~WlanSnifferListItem()
{
}

/*!
    List item network name setting function.
    
    @param [in] name Network name to set.
 */

void WlanSnifferListItem::setNetworkName(const QString &name)
{
    setText(name);
}

/*!
    List item left icon setting function.
    
    @param [in] icon Icon logical name.
 */

void WlanSnifferListItem::setLeftIcon(const QString &icon)
{
    setIcon(HbIcon(icon));
}

/*!
    List item signal strength icon setting function.
    
    @param [in] icon Icon logical name.
 */

void WlanSnifferListItem::setSignalIcon(const QString &icon)
{
    setSecondaryIcon(HbIcon(icon));
}

/*!
    List item secure icon setting function.
    
    @param [in] icon Icon logical name.
 */

void WlanSnifferListItem::setSecureIcon(const QString &icon)
{
    // Fetch the current decorators (icons)
    QVariant decorators = data(Qt::DecorationRole);
    Q_ASSERT(decorators.canConvert<QVariantList>());
    QVariantList decoratorList = decorators.toList();
    // There must always be three icons
    Q_ASSERT(decoratorList.count() == 3);

    // Replace old icon
    decoratorList[2] = HbIcon(icon);
    setData(decoratorList, Qt::DecorationRole);
}
