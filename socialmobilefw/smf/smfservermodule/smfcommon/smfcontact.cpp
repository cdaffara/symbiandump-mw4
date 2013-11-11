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

#include <QFile>
#include <QTextStream>

#include "smfcontact.h"
#include "smfcontact_p.h"

/**
 * Constructor with default argument
 * @param aParent The parent object
 */
SmfContact::SmfContact( QObject *aParent)
	{
	Q_UNUSED(aParent);
	d = new SmfContactPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfContact::SmfContact( const SmfContact &aOther ): d (aOther.d)
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfContact& SmfContact::operator=( const SmfContact &aOther )
	{
	d->m_details = aOther.d->m_details;
	return *this;
	}

/**
 * Destructor
 */
SmfContact::~SmfContact( )
	{
	}

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
QStringList SmfContact::subTypes( ) const
	{
	QStringList list = d->m_details.uniqueKeys();
	return list;
	}

/**
 * Method to convert an SmfContact to a QContact
 * Changes made to the returned QContact will not be reflected 
 * in its parent SmfContact object
 * @param aContact QContact corresponding to SmfContact 
 */
void SmfContact::convert( QContact &aContact ) const
	{
// TODO:-
	Q_UNUSED(aContact);
	}

/**
 * Method to get the value of a sub field for this contact
 * @param aSubType The sub field type 
 * @return The value of the sub field subType
 * @see subTypes()
 */
QVariant SmfContact::value( const QString& aSubType ) const
	{
	if(d->m_details.contains(aSubType))
		return d->m_details.value(aSubType);
	else
		return QString();
	}

/**
 * Method to set value for a subtype
 * @param aSubType The subtype string
 * @param value Thhe value to be set for the subtype
 */
void SmfContact::setValue(const QString& aSubType,QVariant& value)
	{
	d->m_details.insert(aSubType,value);
	}

/**
 * Method for Externalization. Writes the SmfContact object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aContact The SmfContact object to be externalized
 * @return reference to the written stream
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfContact& aContact )
	{
	QContactAddress address = aContact.value("Address").value<QContactAddress>();
	aDataStream<<address;
	
	QContactAnniversary anniver = aContact.value("Anniversary").value<QContactAnniversary>();
	aDataStream<<anniver;
	
	QContactAvatar avatar = aContact.value("Avatar").value<QContactAvatar>();
	aDataStream<<avatar;
	
	QContactBirthday bday = aContact.value("Birthday").value<QContactBirthday>();
	aDataStream<<bday;
	
	QContactId contactId = aContact.value("ContactId").value<QContactId>();
	aDataStream<<contactId;
	
	QContactEmailAddress email = aContact.value("EmailAddress").value<QContactEmailAddress>();
	aDataStream<<email;
	
	QContactGender gender = aContact.value("Gender").value<QContactGender>();
	aDataStream<<gender;
	
	QContactGeoLocation geoloc = aContact.value("Geolocation").value<QContactGeoLocation>();
	aDataStream<<geoloc;
	
	QContactGuid guid = aContact.value("Guid").value<QContactGuid>();
	aDataStream<<guid;
	
	QContactName name = aContact.value("Name").value<QContactName>();
	aDataStream<<name;
	
	QContactNickname nickname = aContact.value("Nickname").value<QContactNickname>();
	aDataStream<<nickname;
	
	QContactNote note = aContact.value("Note").value<QContactNote>();
	aDataStream<<note;
	
	QContactOnlineAccount account = aContact.value("OnlineAccount").value<QContactOnlineAccount>();
	aDataStream<<account;
	
	QContactOrganization org = aContact.value("Organization").value<QContactOrganization>();
	aDataStream<<org;
	
	QContactPhoneNumber number = aContact.value("PhoneNumber").value<QContactPhoneNumber>();
	aDataStream<<number;
	
	QContactPresence presence = aContact.value("Presence").value<QContactPresence>();
	aDataStream<<presence;
	
	QContactThumbnail thumbnail = aContact.value("Thumbnail").value<QContactThumbnail>();
	aDataStream<<thumbnail;
	
	QContactTimestamp time = aContact.value("Timestamp").value<QContactTimestamp>();
	aDataStream<<time;
	
	QContactType type = aContact.value("Type").value<QContactType>();
	aDataStream<<type;
	
	QContactUrl url = aContact.value("Url").value<QContactUrl>();
	aDataStream<<url;
	
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfContact object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aContact The SmfContact object to be internalized
 * @return reference to the stream
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		SmfContact& aContact )
	{
	QContactAddress address;
	aDataStream>>address;
	QVariant var1 = QVariant::fromValue(address);
	aContact.setValue("Address",var1);
	
	QContactAnniversary anniver;
	aDataStream>>anniver;
	QVariant var2 = QVariant::fromValue(anniver);
	aContact.setValue("Anniversary",var2);
	
	QContactAvatar avatar;
	aDataStream>>avatar;
	QVariant var3 = QVariant::fromValue(avatar);
	aContact.setValue("Avatar",var3);
	
	QContactBirthday bday;
	aDataStream>>bday;
	QVariant var4 = QVariant::fromValue(bday);
	aContact.setValue("Birthday",var4);
	
	QContactId id;
	aDataStream>>id;
	QVariant var5 = QVariant::fromValue(id);
	aContact.setValue("ContactId",var5);
	
	QContactEmailAddress email;
	aDataStream>>email;
	QVariant var6 = QVariant::fromValue(email);
	aContact.setValue("EmailAddress",var6);
	
	QContactGender gender;
	aDataStream>>gender;
	QVariant var7 = QVariant::fromValue(gender);
	aContact.setValue("Gender",var7);
	
	QContactGeoLocation geo;
	aDataStream>>geo;
	QVariant var8 = QVariant::fromValue(geo);
	aContact.setValue("Geolocation",var8);
	
	QContactGuid guid;
	aDataStream>>guid;
	QVariant var9 = QVariant::fromValue(guid);
	aContact.setValue("Guid",var9);
	
	QContactName name1;
	aDataStream>>name1;
	QVariant var10 = QVariant::fromValue(name1);
	aContact.setValue("Name",var10);
	
	QContactNickname nickname;
	aDataStream>>nickname;
	QVariant var11 = QVariant::fromValue(nickname);
	aContact.setValue("Nickname",var11);
	
	QContactNote note;
	aDataStream>>note;
	QVariant var12 = QVariant::fromValue(note);
	aContact.setValue("Note",var12);
	
	QContactOnlineAccount account;
	aDataStream>>account;
	QVariant var13 = QVariant::fromValue(account);
	aContact.setValue("OnlineAccount",var13);
	
	QContactOrganization org;
	aDataStream>>org;
	QVariant var14 = QVariant::fromValue(org);
	aContact.setValue("Organization",var14);
	
	QContactPhoneNumber number;
	aDataStream>>number;
	QVariant var15 = QVariant::fromValue(number);
	aContact.setValue("PhoneNumber",var15);
	
	QContactPresence presence;
	aDataStream>>presence;
	QVariant var16 = QVariant::fromValue(presence);
	aContact.setValue("Presence",var16);
	
	QContactThumbnail thumbnail;
	aDataStream>>thumbnail;
	QVariant var17 = QVariant::fromValue(thumbnail);
	aContact.setValue("Thumbnail",var17);
	
	QContactTimestamp time;
	aDataStream>>time;
	QVariant var18 = QVariant::fromValue(time);
	aContact.setValue("Timestamp",var18);
	
	QContactType type;
	aDataStream>>type;
	QVariant var19 = QVariant::fromValue(type);
	aContact.setValue("Type",var19);
	
	QContactUrl url;
	aDataStream>>url;
	QVariant var20 = QVariant::fromValue(url);
	aContact.setValue("Url",var20);
	
	return aDataStream;
	}




/**
 * Serialization support for QtMobility::QContactAddress
 */
QDataStream& operator<<( QDataStream &aDataStream, 
		const QContactAddress &aAddress )
	{
	aDataStream<<aAddress.country();
	aDataStream<<aAddress.locality();
	aDataStream<<aAddress.postOfficeBox();
	aDataStream<<aAddress.postcode();
	aDataStream<<aAddress.region();
	aDataStream<<aAddress.street();
	aDataStream<<aAddress.subTypes();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactAddress
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactAddress &aAddress )
	{
	QString str;
	
	// First one to be deserialized is country
	aDataStream>>str;
	aAddress.setCountry(str);

	// Next one to be deserialized is locality
	str.clear();
	aDataStream>>str;
	aAddress.setLocality(str);

	// Next one to be deserialized is postOfficeBox
	str.clear();
	aDataStream>>str;
	aAddress.setPostOfficeBox(str);
	
	// Next one to be deserialized is postCode
	str.clear();
	aDataStream>>str;
	aAddress.setPostcode(str);
	
	// Next one to be deserialized is region
	str.clear();
	aDataStream>>str;
	aAddress.setRegion(str);
	
	// Next one to be deserialized is street
	str.clear();
	aDataStream>>str;
	aAddress.setStreet(str);
	
	// Next one to be deserialized is subtypes
	QStringList list;
	list.clear();
	aDataStream>>list;
	aAddress.setSubTypes(list);
	
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactAnniversary
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const QContactAnniversary &aAnniversary )
	{
	aDataStream<<aAnniversary.calendarId();
	aDataStream<<aAnniversary.event();
	aDataStream<<aAnniversary.originalDate();
	aDataStream<<aAnniversary.subType();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactAnniversary
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactAnniversary &aAnniversary)
	{
	QString str;
	QDate date;
	
	// First one to be deserialized is calendarId
	aDataStream>>str;
	aAnniversary.setCalendarId(str);

	// Next one to be deserialized is event
	str.clear();
	aDataStream>>str;
	aAnniversary.setEvent(str);

	// Next one to be deserialized is originalDate
	aDataStream>>date;
	aAnniversary.setOriginalDate(date);
	
	// Next one to be deserialized is subType
	str.clear();
	aDataStream>>str;
	aAnniversary.setSubType(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactAvatar
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactAvatar &aAvatar )
	{
	//TODO:- QContactAvatar derives from QContactDetail,so internalization/externalization
	//for the same should be provided
	
	//Qt mobility introduces API compatibility break
#ifdef OLDER_QT_MOBILITY
	aDataStream<<aAvatar.avatar();

#else
	aDataStream<<aAvatar.imageUrl();
	aDataStream<<aAvatar.videoUrl();
#endif
	
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactAvatar
 */
QDataStream &operator>>( QDataStream &aDataStream, 
	QContactAvatar &aAvatar)
	{
#ifdef OLDER_QT_MOBILITY
	QString avatarPath;
	aDataStream>>avatarPath;
	aAvatar.setAvatar(avatarPath);
#else
	QUrl url;
	aDataStream>>url;
	aAvatar.setImageUrl(url);
	
	url.clear();
	aDataStream>>url;
	aAvatar.setVideoUrl(url);
#endif
	
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactBirthday
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactBirthday &aBirthday )
	{
	aDataStream<<aBirthday.date();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactBirthday
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactBirthday &aBirthday)
	{
	QDate date;
	aDataStream>>date;
	aBirthday.setDate(date);

	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactId
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactId &aContactId )
	{
	aDataStream<<aContactId.localId();
	aDataStream<<aContactId.managerUri();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactId
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactId &aContactId)
	{
	QContactLocalId localId;
	QString str;
	
	// First one to be deserialized is localId
	aDataStream>>localId;
	aContactId.setLocalId(localId);

	// Next one to be deserialized is managerUri
	aDataStream>>str;
	aContactId.setManagerUri(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactEmailAddress
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactEmailAddress &aEMail )
	{
	aDataStream<<aEMail.emailAddress();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactEmailAddress
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactEmailAddress &aEMail)
	{
	QString str;
	aDataStream>>str;
	aEMail.setEmailAddress(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactGender
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGender &aGender )
	{
	aDataStream<<aGender.gender();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactGender
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGender &aGender)
	{
	QString str;
	aDataStream>>str;
	aGender.setGender(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactGeoLocation
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGeoLocation &aGeolocation )
	{
	aDataStream<<aGeolocation.accuracy();
	aDataStream<<aGeolocation.altitude();
	aDataStream<<aGeolocation.altitudeAccuracy();
	aDataStream<<aGeolocation.heading();
	aDataStream<<aGeolocation.label();
	aDataStream<<aGeolocation.latitude();
	aDataStream<<aGeolocation.longitude();
	aDataStream<<aGeolocation.speed();
	aDataStream<<aGeolocation.timestamp();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactGeoLocation
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGeoLocation &aGeolocation)
	{
	double val;
	QString str;
	QDateTime date;
	
	// First one to be deserialized is accuracy
	val = 0;
	aDataStream>>val;
	aGeolocation.setAccuracy(val);

	// Next one to be deserialized is altitude
	val = 0;
	aDataStream>>val;
	aGeolocation.setAltitude(val);

	// Next one to be deserialized is altitudeAccuracy
	val = 0;
	aDataStream>>val;
	aGeolocation.setAltitudeAccuracy(val);
	
	// Next one to be deserialized is heading
	val = 0;
	aDataStream>>val;
	aGeolocation.setHeading(val);
	
	// First one to be deserialized is label
	aDataStream>>str;
	aGeolocation.setLabel(str);

	// Next one to be deserialized is latitude
	val = 0;
	aDataStream>>val;
	aGeolocation.setLatitude(val);

	// Next one to be deserialized is longitude
	val = 0;
	aDataStream>>val;
	aGeolocation.setLongitude(val);
	
	// Next one to be deserialized is speed
	val = 0;
	aDataStream>>val;
	aGeolocation.setSpeed(val);
	
	// Next one to be deserialized is timestamp
	aDataStream>>date;
	aGeolocation.setTimestamp(date);
		
	return aDataStream;
	}

/**
 * Serialization support for QtMobility::QContactGuid
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactGuid &aGuid )
	{
	aDataStream<<aGuid.guid();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactGuid
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactGuid &aGuid)
	{
	QString str;
	aDataStream>>str;
	aGuid.setGuid(str);
	
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactName
 */
QDataStream &operator<<( QDataStream &aDataStream, 
		const QContactName &aContact )
	{
	//Qt mobility introduces API compatibility break
#ifdef OLDER_QT_MOBILITY
	aDataStream<<aContact.first();
	aDataStream<<aContact.last();
#else
	aDataStream<<aContact.customLabel();
	aDataStream<<aContact.firstName();
	aDataStream<<aContact.lastName();
	aDataStream<<aContact.middleName();
	aDataStream<<aContact.prefix();
	aDataStream<<aContact.suffix();
#endif
	
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactName
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactName &aContact )
	{
	QString str;
	
	// First one to be deserialized is customlabel
	str.clear();
	aDataStream>>str;
	aContact.setCustomLabel(str);
	
	// Next one to be deserialized is firstname
	str.clear();
	aDataStream>>str;
#ifdef OLDER_QT_MOBILITY
	aContact.setFirst(str);
#else
	aContact.setFirstName(str);
#endif

	// Next one to be deserialized is lastName
	str.clear();
	aDataStream>>str;
#ifdef OLDER_QT_MOBILITY
	aContact.setLast(str);
#else
	aContact.setLastName(str);
#endif
	

	
	// Next one to be deserialized is middlename
	str.clear();
	aDataStream>>str;
	aContact.setMiddleName(str);
	
	// Next one to be deserialized is prefix
	str.clear();
	aDataStream>>str;
	aContact.setPrefix(str);
	
	// Next one to be deserialized is suffix
	str.clear();
	aDataStream>>str;
	aContact.setSuffix(str);
	
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactNickname
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactNickname &aNickname )
	{
	aDataStream<<aNickname.nickname();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactNickname
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactNickname &aNickname)
	{
	QString str;
	aDataStream>>str;
	aNickname.setNickname(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactNote
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactNote &aNote )
	{
	aDataStream<<aNote.note();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactNote
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactNote &aNote)
	{
	QString str;
	aDataStream>>str;
	aNote.setNote(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactOnlineAccount
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactOnlineAccount &aOnlineAccount )
	{
	aDataStream<<aOnlineAccount.accountUri();
	aDataStream<<aOnlineAccount.capabilities();
	aDataStream<<aOnlineAccount.serviceProvider();
	aDataStream<<aOnlineAccount.subTypes();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactOnlineAccount
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactOnlineAccount &aOnlineAccount)
	{
	QString str;
	QStringList list;
	
	// First one to be deserialized is accountUri
	aDataStream>>str;
	aOnlineAccount.setAccountUri(str);

	// Next one to be deserialized is capabilities
	aDataStream>>list;
	aOnlineAccount.setCapabilities(list);

	// Next one to be deserialized is serviceProvider
	str.clear();
	aDataStream>>str;
	aOnlineAccount.setServiceProvider(str);
	
	// Next one to be deserialized is subTypes
	list.clear();
	aDataStream>>list;
	aOnlineAccount.setSubTypes(list);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactOrganization
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactOrganization &aOrganization )
	{
	aDataStream<<aOrganization.assistantName();
	aDataStream<<aOrganization.department();
	aDataStream<<aOrganization.location();
	aDataStream<<aOrganization.logoUrl();
	aDataStream<<aOrganization.name();
	aDataStream<<aOrganization.role();
	aDataStream<<aOrganization.title();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactOrganization
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactOrganization &aOrganization )
	{
	QString str;
	QStringList list;
	QUrl url;
	
	// First one to be deserialized is assistantName
	aDataStream>>str;
	aOrganization.setAssistantName(str);

	// Next one to be deserialized is department
	aDataStream>>list;
	aOrganization.setDepartment(list);

	// Next one to be deserialized is location
	str.clear();
	aDataStream>>str;
	aOrganization.setLocation(str);
	
	// Next one to be deserialized is logoUrl
	aDataStream>>url;
	aOrganization.setLogoUrl(url);
	
	// Next one to be deserialized is name
	str.clear();
	aDataStream>>str;
	aOrganization.setName(str);

	// Next one to be deserialized is role
	str.clear();
	aDataStream>>str;
	aOrganization.setRole(str);
	
	// Next one to be deserialized is title
	str.clear();
	aDataStream>>str;
	aOrganization.setTitle(str);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactPhoneNumber
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactPhoneNumber &aPhoneNumber )
	{
	aDataStream<<aPhoneNumber.number();
	aDataStream<<aPhoneNumber.subTypes();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactPhoneNumber
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactPhoneNumber &aPhoneNumber )
	{
	QString str;
	QStringList list;
	
	// First one to be deserialized is number
	aDataStream>>str;
	aPhoneNumber.setNumber(str);
	
	// Next one to be deserialized is subTypes
	aDataStream>>list;
	aPhoneNumber.setSubTypes(list);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactPresence
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactPresence &aPresence )
	{
	aDataStream<<aPresence.customMessage();
	aDataStream<<aPresence.nickname();
	aDataStream<<aPresence.presenceState();
	aDataStream<<aPresence.presenceStateImageUrl();
	aDataStream<<aPresence.presenceStateText();
	aDataStream<<aPresence.timestamp();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactPresence
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactPresence &aPresence )
	{
	QString str;
	QUrl url;
	QDateTime date;
	
	// First one to be deserialized is customMessage
	aDataStream>>str;
	aPresence.setCustomMessage(str);
	
	// Next one to be deserialized is nickname
	str.clear();
	aDataStream>>str;
	aPresence.setNickname(str);
	
	// Next one to be deserialized is presenceState
	int val;
	aDataStream>>val;
	QContactPresence::PresenceState state = (QContactPresence::PresenceState)val;
	aPresence.setPresenceState(state);
	
	// Next one to be deserialized is presenceStateImageUrl
	aDataStream>>url;
	aPresence.setPresenceStateImageUrl(url);
	
	// Next one to be deserialized is presenceStateText
	str.clear();
	aDataStream>>str;
	aPresence.setPresenceStateText(str);
	
	// Next one to be deserialized is timestamp
	aDataStream>>date;
	aPresence.setTimestamp(date);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactThumbnail
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactThumbnail &aThumbnail )
	{
	aDataStream<<aThumbnail.thumbnail();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactThumbnail
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactThumbnail &aThumbnail )
	{
	QImage image;
	aDataStream>>image;
	aThumbnail.setThumbnail(image);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactTimestamp
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactTimestamp &aTimestamp )
	{
	aDataStream<<aTimestamp.created();
	aDataStream<<aTimestamp.lastModified();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactTimestamp
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactTimestamp &aTimestamp )
	{
	QDateTime date1, date2;
	
	// First one to be deserialized is created
	aDataStream>>date1;
	aTimestamp.setCreated(date1);
	
	// Next one to be deserialized is lastModified
	aDataStream>>date2;
	aTimestamp.setLastModified(date2);
		
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactType
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactType &aType )
	{
	aDataStream<<aType.type();
	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactType
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactType &aType )
	{
	QString str;
	aDataStream>>str;
	aType.setType(str);
	
	return aDataStream;
	}


/**
 * Serialization support for QtMobility::QContactUrl
 */
QDataStream &operator<<( QDataStream &aDataStream, 
	const QContactUrl &aUrl )
	{
	aDataStream<<aUrl.subType();
	aDataStream<<aUrl.url();

	return aDataStream;
	}

/**
 * Deserialization support for QtMobility::QContactUrl
 */
QDataStream &operator>>( QDataStream &aDataStream, 
		QContactUrl &aUrl)
	{
	QString str;
	
	// First one to be deserialized is subType
	aDataStream>>str;
	aUrl.setSubType(str);
	
	// Next one to be deserialized is url
	str.clear();
	aDataStream>>str;
	aUrl.setUrl(str);
		
	return aDataStream;
	}

