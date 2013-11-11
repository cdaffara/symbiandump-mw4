/*!	\file
        \brief File containing implementation for SMFSocialNetworkingSite class.

        Copyright (c) 2010 Sasken Communication Technologies Ltd.
        All rights reserved.
        This component and the accompanying materials are made available
        under the terms of the "{License}"
        which accompanies  this distribution, and is available
        at the URL "{LicenseUrl}".

        \author Jaspinder Singh, Sasken Communication Technologies Ltd - Initial contribution

        \version 0.1

*/

#include "smfSns.h"

//! \fn SMFSocialNetworkingSite(const QString& name, const QString& url, QObject* parent = 0)
/*!
    \brief Constructor for SMFSocialNetworkingSite class
    \param name Name of the Social Networking site
    \param url URL of the Social Networking Site
    \param parent Parent of the class. Default 0.
*/
SMFSocialNetworkingSite::SMFSocialNetworkingSite(const QString& name, const QString& url, QObject* parent): QObject( parent ){
    setSnsName(name);
    setSnsURL(url);
    setSnsID(-1); // Set a negative ID till we store the object in the database or retrieve it from the database.
}


//! \fn ~SMFSocialNetworkingSite()
/*!
    \brief Destructor for SMFSocialNetworkingSite class
*/
SMFSocialNetworkingSite::~SMFSocialNetworkingSite(){
    //Empty
}


//! \fn int snsID() const
/*!
    \brief Returns the ID for the social networking site the object corresponds to.
    \return Integer ID for the social networking site the object represents.
*/
int SMFSocialNetworkingSite::snsID() const{
    return m_sns_id;
}


//! \fn const QString& snsName() const
/*!
    \brief Gets the name of the social networking site.
    \return A const reference to the name of the social networking site
    \sa setSnsName()
*/
const QString& SMFSocialNetworkingSite::snsName() const{
    return m_sns_name;
}


//! \fn const QString& snsURL() const
/*!
    \brief Gets the URL of the social networking site
    \return A const reference to the URL of the social networking site
    \sa setSnsURL()
*/
const QString& SMFSocialNetworkingSite::snsURL() const{
    return m_sns_url;
}


//! \fn void setSnsName(const QString& name)
/*!
    \brief 	Setter method for name of the social networking site
    \param 	name 	reference to the name object
    \sa 	snsName()
*/
void SMFSocialNetworkingSite::setSnsName(const QString &name){
    m_sns_name = name;
}


//! \fn void setSnsURL(const QString& url)
/*!
    \brief Setter method for the URL of the social networking site
    \param 	url 	const reference to the URL object
    \sa 	snsURL()
*/
void SMFSocialNetworkingSite::setSnsURL(const QString &url){
    m_sns_url = url;
}


//! \fn void setSnsID(int id)
/*!
    \brief Setter method for the id of the social networking site
    \param 	id 	Integer ID
*/
void SMFSocialNetworkingSite::setSnsID(int id){
    m_sns_id = id;
}
