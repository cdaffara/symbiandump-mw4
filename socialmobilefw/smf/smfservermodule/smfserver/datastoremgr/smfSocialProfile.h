/*!	\file 
	\brief File containing class description for SMFSocialProfile class.
	
	Copyright (c) 2010 Sasken Communication Technologies Ltd.
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of the "{License}"
	which accompanies  this distribution, and is available
	at the URL "{LicenseUrl}".

	\author Jaspinder Singh, Sasken Communication Technologies Ltd - Initial contribution

	\author
	\version 0.1

*/

#ifndef SMFSOCIALPROFILE_H
#define SMFSOCIALPROFILE_H

#include <QObject>
#include <QString>

#include "smfSns.h"
#include "smfUserProfile.h"

class DataStoreManager; //Forward Declaration

//!	\class 	SMFSocialProfile
/*!
	\brief	This class represents a user's public profile on a social networking site.
	\brief	A social profile represents the information about the user/contact profiles on various social networking sites. 
			Every profile on a social networking site gets a social profile. 
			A single social profile can be tied to one social networking site and one user profile.
			
	\sa 	SMFUserProfile , SMFSocialNetworkingSite
*/
class SMFSocialProfile: public QObject
{
	Q_OBJECT

    friend class DataStoreManager;
	
	public:
        SMFSocialProfile(const SMFSocialNetworkingSite& site, const QString& profile_url = "", const QString& screen_alias = "", QObject* parent = 0);
        SMFSocialProfile(int sns_id = -1, int user_id = -1, const QString& profile_url = "", const QString& screen_alias = "", QObject* parent = 0);
        SMFSocialProfile (const SMFSocialProfile& other);
		~SMFSocialProfile();
		int associatedUserID() const;
		int associatedSnsID() const;
		int profileID() const;
		const QString& profileURL() const;
		const QString& screenAlias() const;
	
	public slots:
		void setAssociatedSns(const SMFSocialNetworkingSite& site);
		void setAssociatedSnsID(int sns_id);
		void setProfileURL(const QString& profile_url);
		void setScreenAlias(const QString& screen_alias);
	
	private:
		int m_social_profile_id;
		int m_user_id;
		int m_sns_id;
		QString m_profile_url;
		QString m_screen_alias;		
		
	private slots:
		void setProfileID(int id);
        void setAssociatedUserID(int user_id);
        void setAssociatedUserProfile(const SMFUserProfile& user_profile);
};

#endif
