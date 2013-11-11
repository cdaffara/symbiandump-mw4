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

#ifndef SMFSERVERQT_P_H
#define SMFSERVERQT_P_H

#include <QObject>
#include <smfglobal.h>

class SmfServer;
class QLocalServer;
class QLocalSocket;

class SmfServerQt : public QObject
	{
    Q_OBJECT

public:
    SmfServerQt(SmfServer* wrapper);
    ~SmfServerQt();

    inline SmfServer *wrapper() const;

    bool start();
    int sessionListCount() const;
    void clientAuthorizationFinished(bool success);
    int findAndServiceclient(int requestID,QByteArray* parsedData,SmfError error);

    ~SmfServerQt() {}
    bool start() {return false;}
    int sessionListCount() const {return 0;}
private slots:
    void newClientConnected();
    void removeFromList();

private:
    SmfServer *m_generic;
    QLocalServer *m_server;
    QList<SmfServerQtSession*> m_sessions;
};

inline SmfServer *SmfServerQt::wrapper() const
{
    return m_generic;
}
    
private:
    SmfServerQt *iServer;
    QLocalSocket *iClientConnection;

	};

#endif // SMFSERVERQT_P_H
