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
 * This is the private implementation Class on Symbian OS for Credential Manager Client.
 *  Derives from CAtive to support asynchronous requests.
 *
 */

#ifndef __SMFCREDMGRCLIENT_P_H__
#define __SMFCREDMGRCLIENT_P_H__

//  Include Files
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <S32MEM.H> 
#include <e32des16.h>
#endif
#include <QUrl>
#include <QDateTime>
#include <private/qcore_symbian_p.h>
#include <QtCore/qglobal.h>
#include <qstring.h>
#include <QStringList>
#include <qbytearray.h>
#include <qlist.h>

#ifdef Q_OS_SYMBIAN
#include "smfcredmgrcommon.h"  //client-server common header.
#include "smfcredmgrclientsession.h"
#include "smfcredmgrclientglobal.h"
#else
#endif

const TInt KMinBufSize = 128;
const TInt KMaxBufSize = 512;

//forward declarations
class SmfCredMgrClient;

/**
 * The private implementation class for the APIs exported by Credential Manager.
 *  Its a symbian implementation.
 */
class CSmfCredMgrClientSymbian : public CActive
	{
public:
	/**
	 * new methods
	 * @param aPublicImpl The Qt client object
	 */
	static CSmfCredMgrClientSymbian* NewL(SmfCredMgrClient* aPublicImpl);
	static CSmfCredMgrClientSymbian* NewLC(SmfCredMgrClient* aPublicImpl);

	/**
	 * Destructor
	 */
	~CSmfCredMgrClientSymbian();

	/**
	 * Method to store all data related to an auth app i.e URL list for Plugins, List of Plugins.etc.
	 * Called by storeAuthData() of SmfCredMgrClient.
	 * @param Set The auth Key set
	 * @param Validity time by which the Auth set will expire
	 * @param URLList The URL list to be set
	 * @param PluginList The plugin list to be set
	 * @param AuthAppAID The Authentication Application Id
	 * @param Flag flag to enable or disable the plugin
	 * @return The registration token that completes authentication
	 */
	QString storeAuthDataL(SmfAuthParams Set, QDateTime Validity,
			QList<QUrl> URLList, QStringList PluginList, QString AuthAppAID,
			bool Flag);

	/**
	 * Method to check the authentication of a Plugin
	 * @param PluginID Id of the Plugin to be checked.
	 * @return True if authenticated or False if not
	 */
	TBool isPluginAuthenticatedL(QString PluginID);

	/**
	 * Method to get the list of authenticated plugins
	 * @param RegistrationToken The token that validates a list of such plugins.
	 * @param List The list of authenticated plugins to be retrieved
	 */
	void authenticatedPluginListL(QString RegistrationToken, QStringList& List);

	/**
	 * Method to get list of URLs which plugin can access
	 * @param PluginID The ID of the Plugin whose corresponding list of URLs to be retrieved
	 * @param List The URL list to be retrieved
	 */
	void URLListL(QString PluginID, QList<QUrl>& List);

	/**
	 * Method to generate NONCE token
	 * @param Length Lenth of the NONCE tobe generated
	 * @return The generated NONCE
	 */
	QString generateNONCE(const qint64 Length);

	/**
	 * Method to update the list of authenticated plugins-add, remove, update to plugins
	 * Called by changePluginIDList() of SmfCredMgrClient.
	 * @param NewPluginID ID of the new plugin 
	 * @param Flag boolean to indicate plugin is enabled or disabled
	 * @param OldPluginID ID of the old plugin to be replaced
	 */
	void changePluginIDListL(const QString NewPluginID, const bool Flag,
			const QString OldPluginID);

	/**
	 * Method to get the Auth Token set, eg:OAuth_RequestToken; OAuth_AccessToken
	 * @param RegToken Registration Token previously obtained from SMF
	 * @param Validity time by which the Auth set will expire, its to check if reg token is still valid
	 * @param AuthTokenSet the Auth Token Set for the plugin
	 * @return boolean value if the reg token is still valid
	 */
	TBool AuthDataSetL(QString RegToken, QDateTime Validity,
			SmfAuthParams& AuthTokenSet);

	/**
	 * Method to store RSA Keys
	 * @param KeyLabel First key of the key pair
	 * @param keydata Other key of the key pair
	 * @param Validity time by which the key set will expire
	 * @return The Label of key pair
	 */
	QString storeRSAKeysL(const QString KeyLabel, const QString keydata,
			const QDateTime Validity);
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
	SMFCredMgrErrorCode signMessageL(QString Message, QString Key,
			QString& Signature, SmfSignatureMethod AlgorithmUsed);
	/**
	 * API To delete the RSA Key from the Key Store 
	 * @param KeyLabel The Label of key pair. This is the value returned while StoreRSAKeys().
	 */
	void deleteRSAKey(QString KeyLabel);

protected:
	// Functions from base classes

	/**
	 * From CActive, RunL.
	 * Callback function.
	 * Invoked to handle responses from the server.
	 */
	void RunL();

	/**
	 * From CActive, DoCancel.
	 * Cancels any outstanding operation.
	 */
	void DoCancel();

private:
	/**
	 * Constructor
	 * @param aObserver symbianClientObserver object to notify the Qt-Client
	 */
	CSmfCredMgrClientSymbian(SmfCredMgrClient* aPublicImpl);

	/**
	 * Symbian two phase constructor
	 */
	void ConstructL();

private:
	/**
	 *Handle to the session
	 */
	RSmfCredMgrClientSession iSession;
	/**
	 * Wrapper class object. 
	 */
	SmfCredMgrClient* iPublicImpl;

	};

#endif  // __SMFCREDMGRCLIENT_P_H__
