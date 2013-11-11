/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Entry plugin item data implementation.
*/

#ifndef CPWLANENTRYITEMDATA_H
#define CPWLANENTRYITEMDATA_H

// System includes
#include <xqappmgr.h>

// User includes
#include "cpsettingformentryitemdata.h"

// Forward declarations

class WlanStatusInfo;
class XQAiwRequest;

// External data types

// Constants

// Class declaration

class CpWlanEntryItemData : public CpSettingFormEntryItemData
{
    Q_OBJECT

public:

    // Data types

    explicit CpWlanEntryItemData(CpItemDataHelper &itemDataHelper);
    
    virtual ~CpWlanEntryItemData();

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CpWlanEntryItemData)

    virtual CpBaseSettingView *createSettingView() const;
    
private slots:

    void statusUpdate();
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);

private: // data

    WlanStatusInfo *mWlanStatusInfo;        //!< WLAN status info handler
    mutable bool mProcessing; //used to mark if there is a request being processed
    mutable XQAiwRequest *mRequest;
    mutable XQApplicationManager mAiwMgr;

    // Friend classes

};

#endif // CPWLANENTRYITEMDATA_H
