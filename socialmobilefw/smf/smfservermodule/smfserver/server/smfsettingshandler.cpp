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
 * Description:
 * Reads/Writes settings key in a platform independent way
 */
#include "smfsettingshandler.h"

#ifdef __FOR_SYMBIAN_CR_USAGE__
	#include "smfsettingshandler_symbian.h"
#else
	#include "smfsettingshandler_qt.h"
#endif

SmfSettingsHandler::SmfSettingsHandler(QObject *parent)
{
	Q_UNUSED(parent)
#ifdef __FOR_SYMBIAN_CR_USAGE__
	m_repository = CSettingsRepository::NewL();
#else
	m_repository = new SmfSettingsRepositoryQt(this);
#endif
}
SmfSettingsHandler::~SmfSettingsHandler()
{
	if(m_repository)
		{
		delete m_repository;
		m_repository = NULL;
		}
}
void SmfSettingsHandler::SetAuthExpirationValue(QString & aVal)
{
	
	m_repository->SetAuthExpirationValue(aVal);
}

void SmfSettingsHandler::SetRoamingStatusforDataTransfer(QString & aStatus)
{
	m_repository->SetAuthExpirationValue(aStatus);
}

QString SmfSettingsHandler::GetMaxDataTransferLimit() const
{
	return m_repository->GetMaxDataTransferLimit();
}

void SmfSettingsHandler::SetUploadFileType(QString & aFileType)
{
	m_repository->SetUploadFileType(aFileType);
}

QString SmfSettingsHandler::GetUploadFileType() const
{
	return m_repository->GetUploadFileType();
}

void SmfSettingsHandler::SetMaxDataTransferLimit(QString & aVal)
{
	m_repository->SetMaxDataTransferLimit(aVal);
}

void SmfSettingsHandler::GetPluginDetails(QMap<QString,QString>& aDetails) const
{
	m_repository->GetPluginDetails(aDetails);
}

QString SmfSettingsHandler::GetAuthExpirationValue() const
{
	return m_repository->GetAuthExpirationValue();
}

void SmfSettingsHandler::SetPluginDetails(QString & aPluginName, QString & aPluginStatus)
{
	m_repository->SetPluginDetails(aPluginName,aPluginStatus);
}

QString SmfSettingsHandler::GetRoamingStatusforDataTransfer() const
{
	return m_repository->GetRoamingStatusforDataTransfer();
}
