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
* WLAN Sniffer QtHighway Service.
*/

#ifndef WLANSNIFFERSERVICE_H
#define WLANSNIFFERSERVICE_H

// System includes

#include <QString>
#include <xqserviceprovider.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class WlanSnifferService: public XQServiceProvider
{
    Q_OBJECT

public:

    // Data types

    explicit WlanSnifferService(QObject *parent = 0);
    
    ~WlanSnifferService();
    
    void complete();

signals:

    /*!
        Signal to inform WLAN Sniffer that List View is requested.
        
        @param [in] title Window title.
     */
    void toListView(const QString &title);

    /*!
        Signal to inform WLAN Sniffer that exit was requested.
     */
    void exitTriggered();

public slots:

    void listView();

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferService)

    QString title() const;

private slots:

private: // data

    // Not owned data

    // Owned data
    int mAsyncRequestIndex;         //!< Service request ID

    // Friend classes
};

#endif // WLANSNIFFERSERVICE_H
