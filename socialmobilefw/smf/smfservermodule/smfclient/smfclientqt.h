/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Contributors:
 * Manasij Roy, Nalina Hariharan
 */

#ifndef SMFCLIENTQT_H
#define SMFCLIENTQT_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QLocalSocket>

#include "smfglobal.h"

class SmfClientQt : public QObject
	{
Q_OBJECT

public:
    explicit SmfClientQt(QObject *parent = 0);
    
    ~SmfClientQt();

public:
    /**
     * Send a request to the server.
     * @param aSerializedData serialized by the caller.
     * @param aInterfaceName Interface name
     * @param requestType Opcode
     */
    int sendRequest(QByteArray& aSerializedData,
                QString aInterfaceName,
                SmfRequestTypeID requestType);

    /**
     * This overloaded API is for ESmfGetServices, where data should be
     * fetched synchronously
     */
    QByteArray sendRequest(QString aInterfaceName,
                SmfRequestTypeID requestType);

    /**
     * For testing purpose only
     */
    int sendDummyRequest(QByteArray* provider,QString aInterfaceName,
                SmfRequestTypeID requestType);

    /**
    * CancelRequest.
    * Cancels an outstanding request.
    */
    void CancelRequest();

private slots:
    void connectionEstablished();
    void readIncomingData();
    void handleError(QLocalSocket::LocalSocketError error);

private:
    QLocalSocket *m_serverConnection;
	};

#endif // SMFCLIENTQT_H
