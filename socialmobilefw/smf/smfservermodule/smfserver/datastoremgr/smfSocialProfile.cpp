/*!	\file
    \brief File containing implementation for SMFSocialProfile class.

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

#include "smfSocialProfile.h"

//! \fn SMFSocialProfile(const SMFSocialNetworkingSite&, const SMFUserProfile&, QObject*)
/*!
    \brief  Constructor for the class SMFSocialProfile.
    \param  site A constant reference to the SMFSocialNetworkingSite profile associated with this profile.
    \param  user_profile A constant reference to the SMFUserProfile object associated with this profile.
    \param  parent Reference to the parent object.
*/
SMFSocialProfile::SMFSocialProfile(const SMFSocialNetworkingSite &site, const QString& profile_url, const QString& screen_alias, QObject *parent):QObject(parent){
    setAssociatedSns(site);
    m_social_profile_id = -1;    // Set as negative to indicate that this is not yet stored in the database.
    setAssociatedUserID(-1);
    m_screen_alias = screen_alias;
    m_profile_url = profile_url;
}


SMFSocialProfile::SMFSocialProfile (const SMFSocialProfile& other){
    this->setAssociatedSnsID(other.associatedSnsID());
    this->setAssociatedUserID(other.associatedUserID());
    this->setScreenAlias(other.screenAlias());
    this->setProfileURL(other.profileURL());
    this->setParent(other.parent());
    m_social_profile_id = other.profileID();
}


//! \fn SMFSocialProfile(int , int , QObject* )
/*!
    \brief  Constructor for the class SMFSocialProfile.
    \param  sns_id Integer id of the SMFSocialNetworkingSite profile associated with this profile.
    \param  user_id Integer id of the SMFUserProfile associated with this profile.
    \param  parent Reference to the parent object.
*/
SMFSocialProfile::SMFSocialProfile(int sns_id, int user_id, const QString& profile_url, const QString& screen_alias, QObject* parent):QObject(parent){
    setAssociatedUserID(user_id);
    setAssociatedSnsID(sns_id);
    m_social_profile_id = -1;    // Set as negative to indicate that this is not yet stored in the database.
    m_screen_alias = screen_alias;
    m_profile_url = profile_url;
}

//! \fn ~SMFSocialProfile()
/*!
    \brief  Destructor for the class SMFSocialProfile.
*/
SMFSocialProfile::~SMFSocialProfile(){
    // Empty
}


//! \fn associatedSnsID()
/*!
    \brief  Gets the ID of the Social Networking Site associated with this profile.
    \return Integer ID of the Social Networking Site associated with this profile.

    \sa setAssociatedSnsID()
    \sa setAssociatedSns()
*/
int SMFSocialProfile::associatedSnsID() const{
    return m_sns_id;
}


//! \fn associatedUserID()
/*!
    \brief  Gets the ID of the User Profile associated with this profile.
    \return Integer ID identifying the User Profile associated with this profile.

    \sa setAssociatedUserID()
    \sa setAssociatedUserProfile()
*/
int SMFSocialProfile::associatedUserID() const{
    return m_user_id;
}


//! \fn profileURL()
/*!
    \brief  Gets profile URL of the social profile.
    \return Returns a QString containing the profile URL of this social profile.
            An empty string is returned if the value is not present.

    \sa setProfileURL()
*/
const QString& SMFSocialProfile::profileURL() const{
    return m_profile_url;
}


//! \fn screenAlias()
/*!
    \brief  Gets the screen alias of the social profile.
    \return Returns a QString containing the screen alias of this social profile.
            An empty string is returned if the value is not present.

    \sa setScreenAlias()
*/
const QString& SMFSocialProfile::screenAlias() const{
    return m_screen_alias;
}


//! \fn profileID()
/*!
    \brief  Gets the profile ID of the social profile.
    \return Returns an Integer ID of this profile.
*/
int SMFSocialProfile::profileID() const{
    return m_social_profile_id;
}

//! \fn setProfileID()
/*!
    \brief  Private function to set the profile ID.
    \param  id Integer ID to be set.
*/
void SMFSocialProfile::setProfileID(int id){
    m_social_profile_id = id;
}

//! \fn setAssociatedSns(const SMFSocialNetworkingSite& )
/*!
    \brief  Sets the social networking site associated to this ID. This function and the related
            function setAssociatedSnsID() do the same job of associating a social networking site to
            this profile. Either one of these can be called to create an association. There is no
            requirement to call both separately.
    \param  site Constant reference to the SMFSocialNetworkingSite to be associated.

    \sa     setAssociatedSnsID()
    \sa     associatedSnsID()
*/
void SMFSocialProfile::setAssociatedSns(const SMFSocialNetworkingSite &site){
    setAssociatedSnsID(site.snsID());
}


//! \fn setAssociatedSnsID( int )
/*!
    \brief  Sets the social networking site associated to this ID. This function and the related
            function setAssociatedSns() do the same job of associating a social networking site to
            this profile. Either one of these can be called to create an association. There is no
            requirement to call both separately.
    \param  id Integer ID of the Social Networking site to be associated.

    \sa     setAssociatedSns()
    \sa     associatedSnsID()
*/
void SMFSocialProfile::setAssociatedSnsID(int sns_id){
    m_sns_id = sns_id;
}


//! \fn setAssociatedUserProfile( const SMFUserProfile& )
/*!
    \brief  Sets the social user profile associated to this ID. This function and the related
            function setAssociatedUserID() do the same job of associating a user profile to
            this profile. Either one of these can be called to create an association. There is no
            requirement to call both separately.
    \param  user_profile Constant reference to the SMFUserProfile to be associated with this profile.

    \sa     setAssociatedUserID()
    \sa     associatedUserID()
*/
void SMFSocialProfile::setAssociatedUserProfile(const SMFUserProfile &user_profile){
    setAssociatedUserID(user_profile.userID());
}


//! \fn setAssociatedUserProfile( const SMFUserProfile& )
/*!
    \brief  Sets the social user profile associated to this ID. This function and the related
            function setAssociatedUserProfile() do the same job of associating a user profile to
            this profile. Either one of these can be called to create an association. There is no
            requirement to call both separately.
    \param  id Integer ID identifying the SMFUserProfile to be associated with this profile.

    \sa     setAssociatedUserProfile()
    \sa     associatedUserID()
*/
void SMFSocialProfile::setAssociatedUserID(int user_id){
    m_user_id = user_id;
}


//! \fn setProfileURL( const QString& )
/*!
    \brief  Sets the profile URL for the social profile
    \param  profile_url Constant reference to the profile URL string.

    \sa     profileURL()
*/
void SMFSocialProfile::setProfileURL(const QString &profile_url){
    m_profile_url = profile_url;
}


//! \fn setScreenAlias( const QString& )
/*!
    \brief  Sets the screen alias for the social profile
    \param  screen_alias Constant reference to the screen alias string.

    \sa     screenAlias()
*/
void SMFSocialProfile::setScreenAlias(const QString &screen_alias){
    m_screen_alias = screen_alias;
}
