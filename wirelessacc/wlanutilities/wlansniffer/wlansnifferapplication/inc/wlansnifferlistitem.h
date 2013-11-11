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

#ifndef WLANSNIFFERLISTITEM_H
#define WLANSNIFFERLISTITEM_H

// System includes

#include <HbListWidgetItem>
#include <QString>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration

class WlanSnifferListItem : public HbListWidgetItem
{
    
public:

    // Data types

    WlanSnifferListItem();
    
    ~WlanSnifferListItem();

    void setNetworkName(const QString &text);

    void setLeftIcon(const QString &icon);

    void setSignalIcon(const QString &icon);

    void setSecureIcon(const QString &icon);

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferListItem)
    
private slots:

private: // data

    // Friend classes
};

#endif // WLANSNIFFERLISTITEM_H
