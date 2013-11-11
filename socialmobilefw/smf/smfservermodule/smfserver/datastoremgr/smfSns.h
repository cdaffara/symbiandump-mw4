/*!	\file 
	\brief File containing class description for SMFSocialNetworkingSite class.
	
	Copyright (c) 2010 Sasken Communication Technologies Ltd.
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of the "{License}"
	which accompanies  this distribution, and is available
	at the URL "{LicenseUrl}".

	\author Jaspinder Singh, Sasken Communication Technologies Ltd - Initial contribution

	\version 0.1

*/


#ifndef SMFSOCIALNETWORKINGSITE_H
#define SMFSOCIALNETWORKINGSITE_H

#include <QObject>
#include <QString>

class DataStoreManager; //Forward Declaration

//!	\class 	SMFSocialNetworkingSite
/*!
	\brief 	This class represents a social networking site.
	
	\sa		SMFSocialProfile
*/
class SMFSocialNetworkingSite : public QObject
{
	Q_OBJECT

    friend class DataStoreManager;

	public:
        SMFSocialNetworkingSite(const QString& name="", const QString& url="", QObject* parent = 0);
        int snsID() const;
		const QString& snsName() const;
		const QString& snsURL() const;
		~SMFSocialNetworkingSite();
			
	public slots:	
		void setSnsName(const QString& name);
		void setSnsURL(const QString& url);

	private:
                int m_sns_id;                   // m_sns_id gets generated. It is never set through a setter method. Hence, no public setter for ID.
						// It gets generated when the social networking site is stored in the database.
		QString m_sns_name;
		QString m_sns_url;	
		
        private slots:
                void setSnsID(int id);          //private setter method for ID. Might be required.
};

#endif
