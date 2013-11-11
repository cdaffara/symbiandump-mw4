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
*/

#ifndef WLANQTUTILS_CONTEXT_H
#define WLANQTUTILS_CONTEXT_H

#include <QObject>
#include <QSharedPointer>

#include "wlanqtutilscommon.h"
#include "wlanqtutils.h"

class WlanQtUtilsWlanIap;
class WlanQtUtilsAp;

class Q_DECL_EXPORT WlanQtUtilsContext: public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsContext(WlanQtUtils *utils );
    ~WlanQtUtilsContext();
  
public:
    void setScanWlanApsResult( QList<QSharedPointer<WlanQtUtilsAp> > result );
    
    void setScanWlanDirectResult( QString ssid, 
        QList<QSharedPointer<WlanQtUtilsAp> > result );
    
    void setCreateWlanIapResult(int iapId);
    
    void setActiveWlanIapResult(int iapId);
    
    void setUpdateWlanIapResult(bool returnValue);
    
    void setConnectionSuccessed(bool value);
    
    void setSignalWlanNetworkOpened(int iapId);
    
    void setSignalWlanNetworkClosed(int iapId, int reason);
    
    void setSignalIctResult(int iapId, WlanQtUtils::IctStatus result);
    
    void setImmediateApScanResult(bool immediate);
    
    void setScanApStatus(int status);
    
    void setScanDirectStatus(int status);
    
    void emitScanApReady();
    
    QStringList calledMethods();
    
    bool verifyWlanIapSettings(WlanQtUtilsAp &ref);
    
    void emitWlanNetworkOpened(int iapId);
    void emitWlanNetworkClosed(int iapId, int reason);
        
private:
    WlanQtUtils* mUtils;
};

#endif /* WLANQTUTILS_CONTEXT_H */

// End of File
