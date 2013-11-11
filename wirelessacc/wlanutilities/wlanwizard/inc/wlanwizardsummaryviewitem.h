/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *   Wlan Wizard Page: Summary: Custom list view item
 */

#ifndef WLANWIZARDSUMMARYLISTVIEWITEM_H
#define WLANWIZARDSUMMARYLISTVIEWITEM_H

// System includes
#include <HbListViewItem>

// User includes

// Forward declarations

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardSummaryListViewItem : public HbListViewItem
{
public:
    explicit WlanWizardSummaryListViewItem(QGraphicsItem* parent = 0);
    virtual ~WlanWizardSummaryListViewItem();
        
    virtual HbAbstractViewItem* createItem();
    virtual void updateChildItems();
    
signals:
    
public slots:
    
protected:

protected slots:

private:

private slots:

private: // data    
};

/*! @} */

#endif // WLANWIZARDSUMMARYLISTVIEWITEM_H
