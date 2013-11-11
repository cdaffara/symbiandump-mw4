/*!	\file 
	\brief File containing class description for SMFUserProfile class.
	
	Copyright (c) 2010 Sasken Communication Technologies Ltd.
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of the "{License}"
	which accompanies  this distribution, and is available
	at the URL "{LicenseUrl}".

	\author Jaspinder Singh, Sasken Communication Technologies Ltd - Initial contribution

	\version 0.1

*/

#ifndef SMFUSERPROFILE_H
#define SMFUSERPROFILE_H

#include <QObject>
#include <QString>

class DataStoreManager; //Forward Declaration

//!	\class 	SMFUserProfile
/*!
	\brief 	This class represents a user's profile.
	\brief 	A user is a contact stored in the handset. This profile is created for contacts in the handset and is related to the social profiles.
			A single user profile can be associated with multiple social profiles in the database.
	
	\sa		SMFSocialProfile
*/
class SMFUserProfile : public QObject
{
	Q_OBJECT
	
    friend class DataStoreManager;

	public:
        SMFUserProfile(const QString& name = "", const QString& contact_id = "", QObject* parent = 0);
		~SMFUserProfile();
		int userID() const;
		const QString& name() const;
		const QString& contactID() const;
		
	public slots:
		void setName(const QString& name);
		void setContactID(const QString& contact_id);
	
	private:
		int m_user_id;
        QString m_name;
        QString m_contact_id;
		
	private slots:
		void setUserID(int id);
};

#endif
