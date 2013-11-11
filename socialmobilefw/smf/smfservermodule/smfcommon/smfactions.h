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
 * Represents activities in terms similar to standard activity stream http://activitystrea.ms/
 */

#ifndef SMFACTIONS_H_
#define SMFACTIONS_H_

#include <QList>
#include <QImage>
#include <QDateTime>
#include <QStringList>
#include <QUrl>
#include <qdatastream.h>
#include <QSharedData>
#include <QMetaType>

#include "smfclientglobal.h"
#include "smfpost.h"
#include "smfcontact.h"

/**
 * Implementation const
 */
const int MaxSmfActivityEntrySize = 5000;

/**
 * The actions in the activity based on Atom Activity Base Schema
 * see http://activitystrea.ms/schema/1.0/activity-schema-01.html
 */
enum SmfActivityVerb
	{
	SmfActivityMarkAsFavorite,
	SmfActivityStartFollowing,
	SmfActivityMarkLiked,
	SmfActivityMakeFriend,
	SmfActivityJoin,
	SmfActivityPlay,
	SmfActivityPost,
	SmfActivitySave,
	SmfActivityShare,
	SmfActivityTag,
	SmfActivityUpdate
	};

/**
 * The type of the object on which the activity is performed - as per Atom Activity Base Schema
 * see http://activitystrea.ms/schema/1.0/activity-schema-01.html
 */
enum SmfActivityObjectType
	{
	SmfActivityObjTypeArticle,
	SmfActivityObjTypeAudio,
	SmfActivityObjTypeBookmark,
	SmfActivityObjTypeComment,
	SmfActivityObjTypeFile,
	SmfActivityObjTypeFolder,
	SmfActivityObjTypeGroup,
	SmfActivityObjTypeList,
	SmfActivityObjTypeNote,
	SmfActivityObjTypePerson,
	SmfActivityObjTypePhoto,
	SmfActivityObjTypePhotoAlbum,
	SmfActivityObjTypePlace,
	SmfActivityObjTypePlaylist,
	SmfActivityObjTypeProduct,
	SmfActivityObjTypeReview,
	SmfActivityObjTypeService,
	SmfActivityObjTypeStatus,
	SmfActivityObjTypeVideo,
	SmfActivityObjTypeMusic,
	SmfActivityObjTypeEvent,
	SmfActivityObjTypeAdvanced
	};

enum SmfActivityFor
	{
		SmfActivitySelf,
		SmfActivityFriend,
		SmfActivityOthers
	};

class SmfActivityObjectPrivate;
//class SmfActivityEntryPrivate;

/**
 * @ingroup smf_common_group
 * The object class represents an object in an activity (e.g.a picture or a music track on which somebody commented)
 */
class SMFCOMMON_EXPORT SmfActivityObject 
	{
public:
	/**
	 * Constructor
	 */
	SmfActivityObject();

	/**
	 * Copy Constructor
	 */
	SmfActivityObject(const SmfActivityObject& aOther);
	
	/**
	 * Overloaded = operator
	 * @param aOther The reference object
	 * @return The target reference value
	 */
	SmfActivityObject& operator=( const SmfActivityObject &aOther );
	
	/**
	 * Destructor
	 */
	~SmfActivityObject();

	/**
	 * @return returns service specific Id of this object in the entry 
	 */
	QString id() const;
	
	/**
	 * @return returns thumbnail if availabel for this object in the entry
	 */
	QImage thumbnail() const;
	
	/**
	 * @return Captions for this object
	 */
	QString caption() const;
	
	/**
	 * @return retruns the type of this object
	 */
	SmfActivityObjectType type( ) const;
	
	/**
	 * @return returns data of the object - see type() for casting
	 */
	QVariant objData() const;
	
	/**
	 * @return url of this object, e.g. url of the image on which somebody commented
	 */
	QString link() const;
	
	/**
	 * @return Time of creation of this object
	 */
	QDateTime time() const;
	
	/**
	 * @return Description of the content of this object
	 */
	QString content() const;

	/**
	 * Sets service specific id for this object
	 * @param aId The id to be set
	 */
	void setId(const QString& aId);
	
	/**
	 * Sets thumbnail for this object , not mandatory
	 * @param aIcon The thumbnail image
	 */
	void setThumbnail(const QImage& aIcon);
	
	/**
	 * Sets caption for this object
	 * @param aCaption The caption to be set
	 */
	void setCaption(const QString& aCap);
	
	/**
	 * Sets type of this object - ObjData should be set next accordingly
	 * @param aObjType The activity type to be set
	 */
	void setType(const SmfActivityObjectType& aObjType);
	
	/**
	 * Sets object data e,g, SmfImage, SmfComment, SmfMusic, SmfEvent etc
	 * setType() with suitable type should have been called before  
	 * @param aData The object data to be set  
	 */
	void setObjData( const QVariant& aData);
	
	/**
	 * Sets the url for the object
	 * @param aLink The Link to be set
	 */
	void setLink( const QString& aLink);
	
	/**
	 * Sets the time of creation/last modification of the object
	 * @param aTime The time to be set
	 */
	void setTime( const QDateTime& aTime);
	
	/**
	 * Sets the description of the content of this object
	 * @param aText The content to be set
	 */
	void setContent( const QString& aText) ;
	
private:
	QSharedDataPointer<SmfActivityObjectPrivate> d;
	
	friend QDataStream& operator >> ( QDataStream &aOut, SmfActivityObject& aData);
	friend QDataStream& operator << ( QDataStream &aIn, const SmfActivityObject& aData);	
	};

SMFCOMMON_EXPORT QDataStream& operator >> ( QDataStream &aOut, SmfActivityObject& aData);
SMFCOMMON_EXPORT QDataStream& operator << ( QDataStream &aIn, const SmfActivityObject& aData);	

typedef QList<SmfActivityObject> SmfActivityObjectList;

// Make the class SmfActivityObject known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfActivityObject)
Q_DECLARE_METATYPE(QList<SmfActivityObject>)


class SmfActivityEntryPrivate: public QSharedData
	{
public:
	SmfActivityEntryPrivate( ) {
		m_id.clear();
	}
	
	SmfActivityEntryPrivate( const SmfActivityEntryPrivate& aOther ) :
		QSharedData (aOther),
		m_id (aOther.m_id),
		m_title (aOther.m_title),
		m_details (aOther.m_details),
		m_author (aOther.m_author),
		m_actionName (aOther.m_actionName),
		m_activities (aOther.m_activities),
		m_targetObj (aOther.m_targetObj)  { }

	~SmfActivityEntryPrivate( ) { }
	
	/**
	 * Data members
	 */
	QString m_id;
	SmfPost m_title;
	SmfPost m_details;
	SmfContact m_author;
	SmfActivityVerb m_actionName;
	QList<SmfActivityObject> m_activities;
	SmfActivityObject m_targetObj;
	};

/**
 * @ingroup smf_common_group
 * The object class represents an entry in the list of activities 
 */
class SMFCOMMON_EXPORT SmfActivityEntry
	{

public:
	
	SmfActivityEntry();
	SmfActivityEntry(const SmfActivityEntry& aOther);
	~SmfActivityEntry();
	
	/**
	 * @return Returns service specific id of the of entry 
	 */
	QString id() const;
	/**
	 * @return Returns the title of the entry - mostly be a string 
	 */
	SmfPost title() const;

	/**
	 * @return Returns detail descriptiopn of this entry in the activity list. might be absent if title is sufficient
	 */
	SmfPost details() const;
	
	/**
	 * @return Returns the author of the activity - the name and the uri field are most commonly used
	 * Other information fields might be empty
	 */
	SmfContact author() const;
	
	/**
	 * @return Returns the verb of the activity ,e.g. Robin "marked" Joseph as a friend   
	 */
	SmfActivityVerb actionName() const;

	/**
	 * There can be multiple objects in a single activity entry, though this may be rare - only for few verbs.
	 * @return list of activity objects (mostly one object)
	 */
	QList<SmfActivityObject> activities() const;
	
	/**
	 * @return Returns information about the target of the activity, for verbs that support a target.
	 * For example, a target is a photo album to which photos were added
	 */
	SmfActivityObject targetObj() const;

	/**
	 * Sets service specific id of the of entry 
	 * @param aId The id to be set
	 */
	bool setId( const QString& aId);
	
	/**
	 * Sets the title of the entry - mostly be a string
	 * @param aTitle The title to be set 
	 */
	bool setTitle(const SmfPost& aTitle);

	/**
	 * Sets detail description of this entry in the activity list. might be absent if title is sufficient
	 * @param aDetails The details to be set
	 */
	bool setDetails(const SmfPost& aDetails);
	
	/**
	 * Sets the author of the activity - the name and the uri field are most commonly used
	 * Oher information fields might be empty
	 * @param aContact The author to be set
	 */
	bool setAuthor(const SmfContact& aContact);
	
	/**
	 * Sets the verb of the activity ,e.g. Robin "marked" Joseph as a friend
	 * @param aVerb The action name to be set   
	 */
	void setActionName(SmfActivityVerb  aVerb);

	/**
	 * Sets single or multiple objects in a single activity entry, though multiple may be rare - only for few verbs.
	 * @param list of activity objects (mostly one object)
	 */
	bool setActivities(QList<SmfActivityObject>& aList);
	
	/**
	 * Returns information about the target of the activity, for verbs that support a target.
	 * For example, a target is a photo album to which photos were added
	 * @param aTarget The activity object to be set
	 */
	bool setTargetObj(const SmfActivityObject& aTarget);
	
private:
	QSharedDataPointer<SmfActivityEntryPrivate> d;
	
	friend QDataStream& operator >> ( QDataStream &aOut, SmfActivityEntry& aData);
	friend QDataStream& operator << ( QDataStream &aIn, const SmfActivityEntry& aData);	
	};

SMFCOMMON_EXPORT QDataStream& operator >> ( QDataStream &aOut, SmfActivityEntry& aData);
SMFCOMMON_EXPORT QDataStream& operator << ( QDataStream &aIn, const SmfActivityEntry& aData);	

typedef QList<SmfActivityEntry> SmfActivityEntryList;

// Make the class SmfActivityEntry known to QMetaType, so that as to register it.
Q_DECLARE_METATYPE(SmfActivityEntry)
Q_DECLARE_METATYPE(QList<SmfActivityEntry>)

#endif /* SMFACTIONS_H_ */
