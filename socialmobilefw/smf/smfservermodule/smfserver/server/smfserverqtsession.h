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

#ifndef SMFSERVERQTSESSION_H
#define SMFSERVERQTSESSION_H

#include <QObject>
#include <QLocalSocket>

class SmfServerQt;

class SmfServerQtSession : public QObject
	{
    Q_OBJECT

public:
    SmfServerQtSession(QLocalSocket *clientConnection, SmfServerQt *server);
    ~SmfServerQtSession();

    void clientAuthorizationFinished(bool success);

private:
    void handleRequest();
    void handleGetService();

public slots:
    void readDataFromClient();
    void socketError(QLocalSocket::LocalSocketError error);

private:
    int m_opCode;
    QLocalSocket *m_clientConnection;
    SmfServerQt *m_server;
	};

#endif // SMFSERVERQTSESSION_H
