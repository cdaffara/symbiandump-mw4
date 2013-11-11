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
 * This is the public implementation Class(Qt-wrapper) for symbian client for Credential Manager server
 *
 */

#ifndef SMFCREDMGRCLIENT_H_
#define SMFCREDMGRCLIENT_H_

#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <QStringList>

#ifdef Q_OS_SYMBIAN
#include <smfcredmgrcommon.h>
#include <smfcredmgrclientglobal.h>
#endif
//Private implementation for different platforms
#ifdef Q_OS_SYMBIAN
class CSmfCredMgrClientSymbian;
class RSmfCredMgrClientSession;
#else
class SmfCredMgrClientStub;
class SmfCredMgrClientStubSession;
#endif

/**
 * @ingroup smf_credmgr_group
 * Qt wrapper class for for exporting APIs of Credential Manager server
 */
class SmfCredMgrClient_EXPORT SmfCredMgrClient : public QObject
	{
	//Q_OBJECT -- todo used when signal and slots implemented
public:

	/**
	 * Constructor
	 */
	SmfCredMgrClient(QObject* parent = 0);

	/**
	 * Destructor
	 */
	~SmfCredMgrClient();

public:

	/**
	 * API to check plugin authentication
	 * @param PluginID The ID of the Plugin to be checked
	 * @return The boolean value-TRUE if authenticated, FALSE if not
	 */
	bool CheckPluginAuthentication(QString PluginID) const;

	/**
	 * API to get the Auth Data set, eg:OAuth_RequestToken; OAuth_AccessToken
	 * @param RegToken Registration Token obtained from SMF while storing this Auth Data set
	 * @param Validity Time by which the Auth set will expire
	 * @param AuthDataSet The Auth Token Set for the plugin, this Api would fill this parameter
	 *   so the caller of The API must create space for SmfAuthParams
	 * @return boolean Value if the reg token is still valid
	 */
	bool AuthDataSet(QString RegToken,QDateTime Validity, SmfAuthParams& AuthDataSet) const;

	/**
	 * API to get list of URLs which plugin can access
	 * @param PluginID The ID of the Plugin whose corresponding list of URLs to be retrieved
	 * @return The URL list
	 */
	QList<QUrl> URLList(QString PluginID) const;

	/**
	 * API to get the list of authenticated plugins
	 * @param RegistrationToken The token that validates a list of such plugins.
	 * @return The list of authenticated plugins
	 */
	QStringList AuthenticatedPluginList(QString RegistrationToken) const;

	/**
	 * API to generate NONCE token
	 * @param Length Lenth of the NONCE tobe generated
	 * @return The generated NONCE. This is never zero, the minimum length is 12.
	 */
	QString GenerateNONCE(const qint64 Length);

	/**
	 * API to update the list of authenticated plugins-add, remove, update to plugins
	 * @param NewPluginID ID of the new plugin
	 * @param Flag flag to enable or disable the plugin
	 * @param OldPluginID ID of the old plugin to be replaced
	 * @todo - should return an error code to indicate error while changing the list.
	 */
	void ChangePluginIDList(QString NewPluginID,bool Flag, QString OldPluginID);
	/**
	 * API to store all data related to an auth app i.e URL list for Plugins, List of Plugins.etc.
	 * @param Set The auth Key set
	 * @param Validity time by which the Auth set will expire
	 * @param URLList The URL list to be set
	 * @param PluginList The plugin list to be set
	 * @param AuthAppId The Authentication Application Id
	 * @param Flag flag to enable or disable the plugins
	 * @return The registration token that completes authentication, its a NULL string if the
	 *   API fails due to any wrong argument
	 */
	QString StoreAuthData(SmfAuthParams Set,
			QDateTime Validity, QList<QUrl> URLList, QStringList PluginList,
			QString AuthAppId, bool Flag);

	/**
	 * API to store RSA Keys
	 * @param KeyLabel First key of the key pair
	 * @param Keydata  Other key of the key pair
	 * @param Validity time by which the key set will expire
	 * @return The Label of key pair
	 */
	QString StoreRSAKeys(const QString KeyLabel,
			const QString Keydata, const QDateTime Validity);

	/**
	 * API to Sign the message
	 * @param Message The message to be signed
	 * @param Key If Signature method is RSA-SHA1 then it must be equal to the value
	 * 				returned while storeRSAKeys(), if signature method is HMAC-SHA1 key
	 * 				must the value to sign the message with
	 * @param Signature The signed message is an output argument
	 * @param AlgorithmUsed The algorithm used for signing
	 * return Smf defined errorcode
	 * @todo - RSA Signing is falgged-put for time being.
	 */
	SMFCredMgrErrorCode SignMessage(QString Message,
			QString Key, QString& Signature, SmfSignatureMethod AlgorithmUsed);
	/**
	 * API To delete the RSA Key from the Key Store
	 * @param KeyLabel The Label of key pair. This is the value returned while StoreRSAKeys().
	 */
	void DeleteRSAKey(QString KeyLabel);

private:
	/**
	 * Handle to private implementation class.
	 */
#ifdef Q_OS_SYMBIAN
	CSmfCredMgrClientSymbian* m_SmfClientPrivate;
	//friend class CSmfCredMgrClientSymbian;
#endif

	};

#endif /* SMFCREDMGRCLIENT_H_ */

