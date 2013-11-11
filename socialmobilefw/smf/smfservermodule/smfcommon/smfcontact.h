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
 * The contact class represents a social contact
 * Note: This class has dependencies on QtMobility project
 *
 */

#ifndef SMFCONTACT_H_
#define SMFCONTACT_H_

#include <qtcontacts.h>
#include <qdatastream.h>
#include <QSharedData>
#include <QVariant>

#include "smfclientglobal.h"


using namespace QtMobility;

/* forward declaration*/
class SmfContactPrivate;

/**
 * Max size for one SmfContact
 */
const int MaxSmfContactSize = 1000;

/**
 * @ingroup smf_common_group
 * The contact class represents a social contact
 * 
 * Note: This class has dependencies on QtMobility project
 */
class  SMFCOMMON_EXPORT SmfContact
	{
public:
	/**
	 * Constructor with default argument
	 * @param aParent The parent object
	 */
	SmfContact( QObject *aParent = 0 );
	
	/**
	 * Copy Constructor
	 * @param aOther The reference object
	 */
	SmfContact( const SmfContact &aOther );
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The target reference value
	 */
	SmfContact& operator=( const SmfContact &aOther );
	
	/**
	 * Destructor
	 */
	~SmfContact( );
	
public slots:
	/**
	 * Method to get the available sub fields for contacts.
	 * The following are the available sub types of a contact
	 *   QtMobility::QContactAddress		Address;
	 *   QtMobility::QContactAnniversary	Anniversary;
	 *   QtMobility::QContactAvatar			Avatar;
	 *   QtMobility::QContactBirthday		Birthday;
	 *   QtMobility::QContactId				ContactId;
	 *   QtMobility::QContactEmailAddress	EmailAddress;
	 *   QtMobility::QContactGender			Gender;
	 *   QtMobility::QContactGeoLocation	Geolocation;
	 *   QtMobility::QContactGuid			Guid;
	 *   QtMobility::QContactName			Name;
	 *   QtMobility::QContactNickname		Nickname;
	 *   QtMobility::QContactNote			Note;
	 *   QtMobility::QContactOnlineAccount	OnlineAccount;
	 *   QtMobility::QContactOrganization	Organization;
	 *   QtMobility::QContactPhoneNumber	PhoneNumber;
	 *   QtMobility::QContactPresence		Presence;
	 *   QtMobility::QContactThumbnail		Thumbnail;
	 *   QtMobility::QContactTimestamp		Timestamp;
	 *   QtMobility::QContactType			Type;
	 *   QtMobility::QContactUrl			Url;
	 * @return The Available sub fields for this contact
	 */
	QStringList subTypes( ) const;
	
	/**
	 * Method to convert an SmfContact to a QContact
	 * Changes made to the returned QContact will not be reflected 
	 * in its parent SmfContact object
	 * @param aContact QContact corresponding to SmfContact 
	 */
	void convert( QContact &aContact ) const;
	
	/**
	 * Method to get the value of a sub field for this contact
	 * @param aSubType The sub field type 
	 * @return The value of the sub field subType
	 * @see subTypes()
	 */
	QVariant value( const QString& aSubType ) const;
	
	/**
	 * Method to set value for a subtype
	 * @param aSubType The subtype string
	 * @param value Thhe value to be set for the subtype
	 */
	void setValue(const QString& aSubType,QVariant& value);

private:
	QSharedDataPointer<SmfContactPrivate> d;
	
	friend QDataStream &operator<<( QDataStream &aDataStream, 
			const SmfContact &aContact );

	friend QDataStream &operator>>( QDataStream &aDataStream, 
			SmfContact &aContact );
	
	};


/**
 * Method for Externalization. Writes the SmfContact object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aContact The SmfContact object to be externalized
 * @return reference to the written stream
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfContact &aContact );

/**
 * Method for Internalization. Reads a SmfContact object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aContact The SmfContact object to be internalized
 * @return reference to the stream
 */
SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		SmfContact &aContact );


typedef QList<SmfContact> SmfContactList;


/**
 * Serialization support for QtMobility QContact classes
 */
SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactAddress &aAddress );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactAddress &aAddress );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactAnniversary &aAnniversary );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactAnniversary &aAnniversary );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactAvatar &aAvatar );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactAvatar &aAvatar );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactBirthday &aBirthday );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactBirthday &aBirthday );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactEmailAddress &aEMail );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactEmailAddress &aEMail );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGender &aGender );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGender &aGender );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGeoLocation &aGeolocation );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGeoLocation &aGeolocation );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGuid &aGuid );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGuid &aGuid );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactId &aContactId );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactId &aContactId );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactName &aName );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactName &aName );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactNickname &aNickname );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactNickname &aNickname );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactNote &aNote );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactNote &aNote );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactOnlineAccount &aOnlineAccount );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactOnlineAccount &aOnlineAccount );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactOrganization &aOrganization );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactOrganization &aOrganization );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactPhoneNumber &aPhoneNumber );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactPhoneNumber &aPhoneNumber );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactPresence &aPresence );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactPresence &aPresence );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactThumbnail &aThumbnail );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactThumbnail &aThumbnail );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactTimestamp &aTimestamp );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactTimestamp &aTimestamp );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactType &aType );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactType &aType );

SMFCOMMON_EXPORT QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactUrl &aUrl );

SMFCOMMON_EXPORT QDataStream &operator>>( QDataStream &aDataStream, 
		QContactUrl &aUrl );


// Make the class SmfContact known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfContact)
Q_DECLARE_METATYPE(QList<SmfContact>)

//make qt mobility related classes known to moc
Q_DECLARE_METATYPE(QContactAddress)
Q_DECLARE_METATYPE(QContactAnniversary)
Q_DECLARE_METATYPE(QContactAvatar)
Q_DECLARE_METATYPE(QContactBirthday)
Q_DECLARE_METATYPE(QContactId)
Q_DECLARE_METATYPE(QContactEmailAddress)
Q_DECLARE_METATYPE(QContactGender)
Q_DECLARE_METATYPE(QContactGeoLocation)
Q_DECLARE_METATYPE(QContactGuid)
Q_DECLARE_METATYPE(QContactName)
Q_DECLARE_METATYPE(QContactNickname)
Q_DECLARE_METATYPE(QContactNote)
Q_DECLARE_METATYPE(QContactOnlineAccount)
Q_DECLARE_METATYPE(QContactOrganization)
Q_DECLARE_METATYPE(QContactPhoneNumber)
Q_DECLARE_METATYPE(QContactPresence)
Q_DECLARE_METATYPE(QContactThumbnail)
Q_DECLARE_METATYPE(QContactTimestamp)
Q_DECLARE_METATYPE(QContactType)
Q_DECLARE_METATYPE(QContactUrl)

#endif /* SMFCONTACT_H_ */
