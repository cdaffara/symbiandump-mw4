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
*   WLAN Wizard Page: Scan processing
*/

#ifndef WLANWIZARDPAGESCANNING_H
#define WLANWIZARDPAGESCANNING_H

// System includes
#include <QSharedPointer>
#include <QList>
#include <QHash>

// User includes
#include "wlanwizardpageinternal.h"
#include "wlanwizard.h"
#include "wlanwizardscanlist.h"

// Forward declarations
class WlanWizardPrivate;
class HbLabel;
class HbProgressBar;
class HbDocumentLoader;
class WlanQtUtilsAp;

// External data types

// Constants

/*!
 * @addtogroup group_wlan_wizard
 * @{
 */

class WlanWizardPageScanning: public WlanWizardPageInternal
{
    Q_OBJECT
    
public:
    explicit WlanWizardPageScanning(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageScanning();
    
public:
    HbWidget* initializePage();
    int nextId(bool &removeFromStack) const;
    int previousTriggered();
    bool showPage();
    bool requiresStartOperation();
    void startOperation();

signals:
    
public slots:
    void loadDocmlSection(Qt::Orientation orientation);
    void wlanScanResultPreCheck(int scanStatus);
    void wlanScanDirectReady(int scanStatus);
    
protected:
    
protected slots:
    
private:
    Q_DISABLE_COPY(WlanWizardPageScanning)
    void wlanScanApReady();
    void getSsidMatchList(
        QString ssid,
        const QList<QSharedPointer<WlanQtUtilsAp> > &matchList);
    void getFinalScanResults(
        const QList<QSharedPointer<WlanQtUtilsAp> > &directScanResults,
        const QList<QSharedPointer<WlanQtUtilsAp> > &openScanResults,
        QList<WlanScanResult> &finalResults);
    int processMultipleScanResults(const QList<WlanScanResult> &finalResults);
    void selectNextPageActions(const QList<WlanScanResult> &finalResults);
    
private slots:

private:
    /*!
     * Pointer to the view.
     */
    HbWidget *mWidget;

    /*!
     * Pointer to the label widget.
     */
    HbLabel *mLabel;

    /*!
     * Pointer to document loader object.
     */
    HbDocumentLoader *mLoader;

    /*!
     * The next page identifier. 
     */
    int mNextPageId;
    
    /*!
     * Indicator, whether AP scan results are available or not.
     */
    bool mScanResultsAvailable;
    
    /*!
     * Indicator of the current scan status.
     */
    int mScanStatus;
    
    /*!
     * list containing the results of a normal scan of open networks.
     */
    QList<QSharedPointer<WlanQtUtilsAp> > mWlanApList;
};

/*! @} */

#endif
