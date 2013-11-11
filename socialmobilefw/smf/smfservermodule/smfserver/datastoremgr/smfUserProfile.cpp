/*!	\file
        \brief File containing implementation for SMFUserProfile class.

        Copyright (c) 2010 Sasken Communication Technologies Ltd.
        All rights reserved.
        This component and the accompanying materials are made available
        under the terms of the "{License}"
        which accompanies  this distribution, and is available
        at the URL "{LicenseUrl}".

        \author Jaspinder Singh, Sasken Communication Technologies Ltd - Initial contribution

        \version 0.1

*/

#include "smfUserProfile.h"

//! \fn SMFUserProfile(const QString&, const QString& , QObject*)
/*!
    \brief  Constructor for the class SMFUserProfile
    \param  name Reference to the name of the user/person whom this profile identifies.
    \param  contact_id Reference to the contact ID of the user/person whom this profile identifies.
    \param  parent Pointer to a parent object.
*/
SMFUserProfile::SMFUserProfile(const QString &name, const QString &contact_id, QObject *parent):QObject(parent){
    setName(name);
    setContactID(contact_id);
    setUserID(-1);  // Set as negative to indicate that this is not yet stored in the database.
}


//! \fn ~SMFUserProfile()
/*!
    \brief Destructor for the class SMFUserProfile
*/
SMFUserProfile::~SMFUserProfile(){
    // EMpty
}


//! \fn int userID() const
/*!
    \brief  Gets the user ID of this profile.
    \return Integer User ID of this profile. -1 if not assigned a user id yet.
*/
int SMFUserProfile::userID() const{
    return m_user_id;
}


//! \fn const QString& name() const
/*!
    \brief Gets the name of user/person whom this profile identifies.
    \return A const reference to the name of the user/person whom this profile identifies.
    \sa setName()
*/
const QString& SMFUserProfile::name() const {
    return m_name;
}


//! \fn const QString& contactID() const
/*!
    \brief Gets the contact ID of the user/person whom this profile identifies.
    \return A const reference to the contact ID of the user/person whom this profile identifies.
    \sa setContactID()
*/
const QString& SMFUserProfile::contactID() const{
    return m_contact_id;
}


//! \fn void setName(const QString&)
/*!
    \brief Sets the name of user/person whom this profile identifies.
    \param name A const reference to the name of the social networking site
    \sa name()
*/
void SMFUserProfile::setName(const QString& name){
    m_name = name;
}


//! \fn void setContactID(const QString&)
/*!
    \brief  Sets the contact ID of the user/person whom this profile identifies.
    \param  contact_id A const reference to the contact ID of the user/person whom this profile identifies.
    \sa contactID()
*/
void SMFUserProfile::setContactID(const QString& contact_id){
    m_contact_id = contact_id;
}


//! \fn void setUserID(int)
/*!
    \brief  Sets the user ID of this profile.
    \param  id An integer user ID of this profile.
    \sa userID()
*/
void SMFUserProfile::setUserID(int id){
    m_user_id = id;
}
