/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * This is the public implementation Class(Qt wrapper) for symbian client for Credential Manager server
 *
 */
#include "smfcredmgrclient.h"
#include "smfcredmgrclient_p.h"

SmfCredMgrClient::SmfCredMgrClient(QObject* Parent) :
	QObject(Parent)
	{
	//Symbian specific compilation
#ifdef Q_OS_SYMBIAN 
	QT_TRAP_THROWING(m_SmfClientPrivate = CSmfCredMgrClientSymbian::NewL(this));
#else    
	m_SmfClientPrivate = new SmfCredMgrClientStub(this);
#endif
	}
SmfCredMgrClient::~SmfCredMgrClient()
	{
	delete m_SmfClientPrivate;
	}

QString SmfCredMgrClient::StoreAuthData(SmfAuthParams Set, QDateTime Validity,
		QList<QUrl> URLList, QStringList PluginList, QString AuthAppId,
		bool Flag)
	{
	QString qs = NULL;
	if (!(Set.isEmpty() || URLList.isEmpty() || PluginList.isEmpty()
			|| AuthAppId.isEmpty() || Validity.isNull()))
		{
		QT_TRAP_THROWING(qs  = m_SmfClientPrivate->storeAuthDataL(Set, Validity, URLList,
					PluginList, AuthAppId, Flag));
		}
	return qs;
	}

QStringList SmfCredMgrClient::AuthenticatedPluginList(QString RegistrationToken) const
	{
	QStringList List;
	if (RegistrationToken.isEmpty())
		{
		//return the empty list
		return List;
		}
	else
		{
		QT_TRAP_THROWING( m_SmfClientPrivate->authenticatedPluginListL(RegistrationToken, List));
		return List;
		}
	
	}

QList<QUrl> SmfCredMgrClient::URLList(QString PluginID) const
	{
	QList<QUrl> List;
	if (!(PluginID.isEmpty()))
		{
		QT_TRAP_THROWING ( m_SmfClientPrivate->URLListL(PluginID, List) );
		}
	return List;
	}

QString SmfCredMgrClient::GenerateNONCE(const qint64 Length)
	{
	return m_SmfClientPrivate->generateNONCE(Length);
	}

void SmfCredMgrClient::ChangePluginIDList(QString NewPluginID, bool Flag,
		QString OldPluginID)
	{
	if (!(OldPluginID.isEmpty() || NewPluginID.isEmpty()))
		{
		QT_TRAP_THROWING (m_SmfClientPrivate->changePluginIDListL(NewPluginID, Flag, OldPluginID));
		}
	}

bool SmfCredMgrClient::CheckPluginAuthentication(QString PluginID) const
	{
	bool tb = false;
	if (!(PluginID.isEmpty()))
		{
		QT_TRAP_THROWING (tb = m_SmfClientPrivate->isPluginAuthenticatedL(PluginID));
		}
	return tb;
	}

bool SmfCredMgrClient::AuthDataSet(QString RegToken, QDateTime Validity,
		SmfAuthParams& AuthTokenSet) const
	{
	if (!(RegToken.isEmpty() || Validity.isNull()))
		{
		TBool Flag = EFalse;
		QT_TRAP_THROWING ( Flag = m_SmfClientPrivate->AuthDataSetL(RegToken, Validity, AuthTokenSet));
		if (Flag)
			return true;
		else
			return false;
		}
	else
		{
		AuthTokenSet.clear();
		return false;
		}
	}

QString SmfCredMgrClient::StoreRSAKeys(const QString KeyLabel,
		const QString keydata, const QDateTime Validity)
	{
	QString qs = NULL;
	if (!(KeyLabel.isEmpty() || keydata.isEmpty()) && Validity.isValid())
		{
		QT_TRAP_THROWING (qs = m_SmfClientPrivate->storeRSAKeysL(KeyLabel, keydata, Validity));
		}
	return qs;
	}

SMFCredMgrErrorCode SmfCredMgrClient::SignMessage(QString Message, QString Key,
		QString& Signature, SmfSignatureMethod AlgorithmUsed)
	{
	SMFCredMgrErrorCode ec = SmfErrBadParameter;
	if (!(Message.isEmpty() || Key.isEmpty()))
		{
		QT_TRAP_THROWING ( ec = m_SmfClientPrivate->signMessageL(Message, Key, 
				Signature,AlgorithmUsed));
		}
	return ec;
	}

void SmfCredMgrClient::DeleteRSAKey(QString KeyLabel)
	{
	if (!(KeyLabel.isEmpty()))
		{
		m_SmfClientPrivate->deleteRSAKey(KeyLabel);
		}
	}
