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
 *
 * Description: Session implementation for Qt desktop builds
 *
 */

#include <qdebug.h>

#include "smfserverqtsession.h"
#include "smfserverqt_p.h"
#include "smfserver.h"

SmfServerQtSession::SmfServerQtSession(QLocalSocket *clientConnection, SmfServerQt *server)
    : m_opCode(-1), m_clientConnection(clientConnection), m_server(server)
	{
    connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(readDataFromClient()));
    connect(m_clientConnection, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(socketError(QLocalSocket::LocalSocketError)));
	}

SmfServerQtSession::~SmfServerQtSession()
	{
    // The socket has the QLocalServer as a parent, but it should be deleted to
    // save unnecessary accumulation of these objects in memory. It won't be double deleted.
    delete m_clientConnection;
	}

void SmfServerQtSession::readDataFromClient()
	{
    // TODO: This needs to be much safer.
    if(m_clientConnection->bytesAvailable() < sizeof(typeof(SmfRequestTypeID)))
    	{
        // Don't read yet, haven't received opcode
        return;
    	}

    if (m_opCode == -1)
    	{
        QDataStream in(m_clientConnection);
        in >> m_opCode;
    	}

    // m_opCode set, so handle request
    handleRequest();
	}

/**
 * Call the appropriate handler function
 */
void SmfServerQtSession::handleRequest()
	{
    switch (m_opCode)
		{
		case SmfGetService:
			handleGetService();
			break;
		default:
			qDebug()<<"Bad Request received =  "<<m_opCode);
		}
	}

/**
 * Find available services for specified interface and return list to client.
 */
void SmfServerQtSession::handleGetService()
	{
    qDebug()<<"SmfServerQtSession::handleGetService()";

    QDataStream in(m_clientConnection);

    // Get interface type requested
    SmfInterfaceID ifName;
    in >> ifName;
    qDebug()<<"Requested = "<<ifName;

    // Get the available services for this interface
    QMap<SmfPluginID, SmfProvider> services;
    m_server->wrapper()->getPlugins(ifName, services);

    // write back the results
    QDataStream out(m_clientConnection);
    out << services;
	}

void SmfServerQtSession::clientAuthorizationFinished(bool success)
	{
    Q_UNUSED(success);
	}

void SmfServerQtSession::socketError(QLocalSocket::LocalSocketError error)
	{
    switch (error)
		{
		case QLocalSocket::PeerClosedError:
			qDebug()<<"Peer closed connection.";
			break;
		case QLocalSocket::SocketTimeoutError:
			qDebug()<<"error: connection timed out.";
			break;
		default:
			qDebug()<<"error: unkown socket error: "<<error;
			break;
		}
	}
