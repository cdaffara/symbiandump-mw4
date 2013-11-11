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
* This is the header file for Wlan Qt Utilities test context.
*/
 
#ifndef WLANQTUTILSTESTCONTEXT_H
#define WLANQTUTILSTESTCONTEXT_H

#include <QObject>
#include <QSharedPointer>
#include "wlanqtutilsconnection.h"

class WlanQtUtilsAp;

/**
 * This is context for esock library.
 */
class WlanQtUtilsCtxEsock : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsCtxEsock();
    ~WlanQtUtilsCtxEsock();

    /**
     * Initializes context.
     */
    void initialize();

    /** Return value of RConnection::Start(). */
    int startRetValue_;
};

/**
 * This class is a helper class for testing purposes. It derives all properties of WlanQtUtilsConnection.
 * In addition, this class contains member variables for setting some Connection Monitor specific
 * values which cannot be easily generated from member variables of WlanQtUtilsConnection.
 * E.g., connection status has different enum in Connection Monitor and Wlan Qt Utilities. This class
 * enables setting both of those. Connmon version is set so that the stub can return it when
 * requested, and Wlan Qt Utilities version is used for verifying that tested code returns right
 * value.
 */
class WlanQtUtilsCtxActiveConn : public WlanQtUtilsConnection
{
public:
    WlanQtUtilsCtxActiveConn();
    ~WlanQtUtilsCtxActiveConn();

    /** Bearer type in connmon format. */
    int connMonBearerType_;
    /** Connection status in connmon format. */
    int connMonConnectionStatus_;
    /** WLAN connection mode in connmon format. */
    int connMonWlanConnectionMode_;
    /** WLAN security mode in connmon format. */
    int connMonWlanSecurityMode_;
    /** Application UIDs that map to application strings. */
    QList<int> applicationUids_;
};

/**
 * This class represents active connections.
 */
class WlanQtUtilsCtxActiveConnections : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsCtxActiveConnections();
    ~WlanQtUtilsCtxActiveConnections();

    /**
     * Initializes context.
     */
    void initialize();

    /**
     * Creates a list of active connections for scan results.
     * 
     * @param[in] numberOfActiveConns Number of active connections that are created.
     * @param[in] firstIapId IAP ID of the first connection to be created.
     */
    void createDefaultActiveConnList(int numberOfActiveConns, int firstIapId);

    /**
     * Verifies that the given active connection list returned by the real application matches
     * with the one in the context.
     * 
     * @param[in] activeConnList Active connections that are verified against the context.
     */
    void verifyActiveConnList(QList<WlanQtUtilsConnection *> activeConnList);

    /**
     * Finds an active connection matching with the given connection ID from the context.
     * Asserts if connection with given connection ID is not found.
     * 
     * @param connectionId Connection ID.
     * @return Active connection.
     */
    WlanQtUtilsCtxActiveConn *findActiveConn(uint connectionId) const;

    /**
     * List of active connections that RConnectionMonitor stub will return in response to
     * RConnectionMonitor::Get*Attribute() functions.
     */
    QList<WlanQtUtilsCtxActiveConn *> activeConnList_;

    // TODO: We need return values for all different RConnectionMonitor::Get*Attribute() functions
    //       to test error cases.

private:
    
    /**
     * Destroys and clears the list of WLAN APs.
     */
    void clearActiveConnList();
};

/**
 * This class represents WLAN functionality test context. E.g. APs that
 * represent the results of WLAN scanning are stored here.
 */
class WlanQtUtilsWlanScan : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsWlanScan();
    ~WlanQtUtilsWlanScan();

    /**
     * Initializes context.
     */
    void initialize();

    /**
     * Creates a list of WLAN IAPs for scan results.
     * 
     * @param[in] numberOfWlanIaps Number of WLAN IAPs that are created.
     */
    void createDefaultWlanScanIapResultList(int numberOfWlanIaps);
    
    /**
     * Verifies that the given WLAN IAP list returned by the real application matches
     * with the one in the context.
     * 
     * @param[in] wlanIapList WLAN IAPs that are verified against the context.
     */
    void verifyWlanScanIapResultList(
        QList< QSharedPointer<WlanQtUtilsAp> > wlanIapList);

    /**
     * Creates a list of WLAN APs for scan results.
     * 
     * @param[in] numberOfWlanAps Number of WLAN APs that are created.
     */
    void createDefaultWlanScanApResultList(int numberOfWlanAps);

    /**
     * Verifies that the given WLAN AP list returned by the real application matches
     * with the one in the context.
     * 
     * @param[in] wlanApList WLAN APs that are verified against the context.
     */
    void verifyWlanScanApResultList(
        QList< QSharedPointer<WlanQtUtilsAp> > wlanApList); // TODO: Maybe boolean return value

    // Scan result list iterator. Holds the current index, starting from zero (0).
    int mScanResultIterator;
    
    // List of WLAN IAPs that WLAN Management API stub will return when
    // requesting for wlan IAP scan results.
    QList< QSharedPointer<WlanQtUtilsAp> > mWlanScanIapResultList;
    
    // List of WLAN access points that WLAN Management API stub will return when
    // requesting for wlan scan results.
    QList< QSharedPointer<WlanQtUtilsAp> > mWlanScanApResultList;
    
    /**
     * Should IAP scan be completed immediately or not?
     */
    bool mCompleteWlanIapScan;

    /**
     * Should AP scan be completed immediately or not?
     */
    bool mCompleteWlanApScan;
    
    /** Return value of IAP scan request. */
    int mIapScanRetValue;
    
    /** Return value of AP scan request. */
    int mApScanRetValue;
    
private:
    
    /**
     * Destroys and clears the list of WLAN IAPs.
     */
    void clearWlanScanIapResultList();
    
    /**
     * Destroys and clears the list of WLAN APs.
     */
    void clearWlanScanApResultList();
};

/**
 * Test context for RConnectionMonitor.
 */
class WlanQtUtilsCtxConnMon : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsCtxConnMon();
    ~WlanQtUtilsCtxConnMon();

    /**
     * Initializes context.
     */
    void initialize();

    /** List of active connections. */
    WlanQtUtilsCtxActiveConnections activeConnections_;
};

/**
 * Test context for Internet connectivity test library.
 */
class WlanQtUtilsCtxIct : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsCtxIct();
    ~WlanQtUtilsCtxIct();

    /**
     * Initializes context.
     */
    void initialize();
};

/**
 * Test context. The purpose of this context is to provide values that are used in stubs
 * and in verification of outputs in test cases.
 * 
 * In stubs, context is used to verify the inputs of parameters and to set output
 * parameters into a specific value.
 * 
 * In test cases, context is used to verify that the output parameters match with the once
 * set in the stub.
 */
class WlanQtUtilsTestContext : public QObject
{
    Q_OBJECT

public:
    WlanQtUtilsTestContext();
    ~WlanQtUtilsTestContext();
    
    /**
     * Initializes context.
     */
    void initialize();

    /** Context for esock library. */
    WlanQtUtilsCtxEsock esock_;
    /** Context for connmon library. */
    WlanQtUtilsCtxConnMon connMon_;
    /** Context for ICTS library. */
    WlanQtUtilsCtxIct ict_;
    /** Context for scan results. */
    WlanQtUtilsWlanScan mScan;
};

#endif // WLANQTUTILSTESTCONTEXT_H
