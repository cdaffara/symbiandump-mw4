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

#include "smfclientqt.h"

SmfClientQt::SmfClientQt(QObject *parent)
    : QObject(parent)
{
    m_serverConnection = new QLocalSocket();

    connect(m_serverConnection, SIGNAL(connected()), this, SLOT(connectionEstablished()));
    connect(m_serverConnection, SIGNAL(readyRead()), this, SLOT(readIncomingData()));
    connect(m_serverConnection, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(handleError(QLocalSocket::LocalSocketError)));

    m_serverConnection->connectToServer("SmfServerQt", QIODevice::ReadWrite);
}

SmfClientQt::~SmfClientQt()
{
    m_serverConnection->close();
    delete m_serverConnection;
}

/**
 * Send a request to the server.
 * @param aSerializedData serialized by the caller.
 * @param aInterfaceName Interface name
 * @param requestType Opcode
 */
int SmfClientQt::sendRequest(QByteArray& serializedData, QString interfaceName,
                             SmfRequestTypeID requestType)
{
    QDataStream out(m_serverConnection);
    out << requestType;
    out << interfaceName;
    out << serializedData.size();
    out << serializedData;
}

/**
 * This overloaded API is for ESmfGetServices, where data should be
 * fetched synchronously
 */
QByteArray SmfClientQt::sendRequest(QString interfaceName,
            SmfRequestTypeID requestType)
{
    QDataStream out(m_serverConnection);
    out << requestType;
    out << interfaceName;

    // TODO: This needs to be asynchronous. Remove this wait when user API is updated.
    m_serverConnection->waitForBytesWritten(-1);

    QByteArray in;
    out >> in;
    return in;
}

/**
 * For testing purpose only
 */
int SmfClientQt::sendDummyRequest(QByteArray* provider,QString aInterfaceName,
            SmfRequestTypeID requestType)
{
    Q_UNUSED(provider);
    Q_UNUSED(aInterfaceName);
    Q_UNUSED(requestType);
    // ToDo :-
}

/**
* CancelRequest.
* Cancels an outstanding request.
*/
void SmfClientQt::CancelRequest()
{
// ToDo :-
}

void SmfClientQt::connectionEstablished()
{
    qDebug() << "Connected to server successfully.";
    // ToDo :-
}

void SmfClientQt::readIncomingData()
{
	// ToDo :-
}

void SmfClientQt::handleError(QLocalSocket::LocalSocketError error)
{
    switch(error)
    {
    case QLocalSocket::ServerNotFoundError:
        qDebug() << "Server not found.";
        break;
        
    default:
        qDebug() << "Unhandled socket error";
        break;
    }
}
