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
 * Description:
 * SMF Server implementation for platforms other than Symbian.
 * Uses  QLocalServer-QLocalSocket classes
 *
 */

#include <QLocalServer>
#include <QLocalSocket>

#include "smfserverqt_p.h"
#include "smfserverqtsession.h"
#include "smfserver.h"


SmfServerQt::SmfServerQt(SmfServer *wrapper)
    : m_generic(wrapper)
	{
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newClientConnected()));
	}

SmfServerQt::~SmfServerQt()
	{
    m_server->close();
	}

/**
  * Start the server listening for connections.
  */
bool SmfServerQt::start()
	{
    const QString KServerName("SmfServerQt");
    if (m_server->listen(KServerName))
    	{
        qDebug()<<(QString(m_server->serverName() + ": listening for connections."));
        return true;
    	}
    else
    	{
        qDebug()<<(QString(KServerName + ": failed to start"));
        qDebug()<<(QString(m_server->errorString()));
        return false;
    	}
	}

/**
 * Return the number of open sessions
 */
int SmfServerQt::sessionListCount() const
		{
    return m_sessions.count();
		}

/**
 * Called by the SmfServer when client authorization finishes.
 * @param success success of the authorization
 */
void SmfServerQt::clientAuthorizationFinished(bool success)
	{
    Q_UNUSED(success);
	}

/**
  * Slot to receive incoming connections
  */
void SmfServerQt::newClientConnected()
	{
    QLocalSocket *client(m_server->nextPendingConnection());
    if (!client)
    	{
        qDebug()<<"SmfServerQt::newClientConnected(): no socket - client may have dropped.";
        return;
    	}

    // Create a new session for this client.
    qDebug()<<"Client connected.";

    m_sessions.append(new SmfServerQtSession(client, this));
	}

void  SmfServerQt::removeFromList()
	{
	}

int SmfServerQt::findAndServiceclient(int requestID,QByteArray* parsedData,SmfError error)
	{
    Q_UNUSED(requestID);
    Q_UNUSED(parsedData);
    Q_UNUSED(error);
    return 0;
	}

