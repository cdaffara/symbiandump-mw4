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
* WLAN Sniffer list widget class.
*/

#ifndef WLANSNIFFERLISTWIDGET_H
#define WLANSNIFFERLISTWIDGET_H

// System includes

#include <QSharedPointer>
#include <QList>
#include <QString>

// User includes

// Forward declarations

class HbListWidget;
class HbListWidgetItem;
class WlanSnifferListItem;
class WlanQtUtilsAp;

// External data types

// Constants

// Class declaration

class WlanSnifferListWidget
{
    
public:

    // Data types

    /*!
        Signal strength category boundaries in dB (absolute value).
        Smaller value means better signal.
    */
    enum
    {
        SignalStrengthHigh = 65,        //!< High signal strength 
        SignalStrengthMedium = 79       //!< Medium signal strength
    };

    explicit WlanSnifferListWidget(HbListWidget *listWidget);
    
    virtual ~WlanSnifferListWidget();

    void updateContent(
        QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
        QList< QSharedPointer<WlanQtUtilsAp> > &aps,
        int connectedIapId);
    
    void scrollTo(int row);
    
signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferListWidget)

    void sortIaps(
        QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
        int connectedIapId);

    void sortAps(
        QList< QSharedPointer<WlanQtUtilsAp> > &aps);

    static bool iapLessThan(
        const QSharedPointer<WlanQtUtilsAp> iap1,
        const QSharedPointer<WlanQtUtilsAp> iap2);

    static bool apLessThan(
        const QSharedPointer<WlanQtUtilsAp> ap1,
        const QSharedPointer<WlanQtUtilsAp> ap2);

    WlanSnifferListItem *findFromOldIapList(QSharedPointer<WlanQtUtilsAp> iap);

    WlanSnifferListItem *findFromOldApList(QSharedPointer<WlanQtUtilsAp> ap);

    HbListWidgetItem *listItemCreate(
        QSharedPointer<WlanQtUtilsAp> iap,
        int connectedIapId);
    
    HbListWidgetItem *listItemCreate(QSharedPointer<WlanQtUtilsAp> ap);

    QString signalStrengthIconChoose(int signalStrength);
    
    QString leftIconChoose(int iapId, int connectedIapId);
    
    void removeLostItems(
        const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
        const QList< QSharedPointer<WlanQtUtilsAp> > &aps);
    
    void addDiscoveredItemsAndUpdateRest(
        const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
        const QList< QSharedPointer<WlanQtUtilsAp> > &aps,
        int connectedIapId);
    
    void traceListContent() const;
    
private slots:

private: // data

    // Not owned data
    HbListWidget *mListWidget;          //!< WLAN list view widget

    // Owned data

    // Friend classes
};

#endif // WLANSNIFFERLISTWIDGET_H
