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
 * Represents activities in terms similar to standard activity 
 * stream http://activitystrea.ms/
 *
 */

#include "smfactions.h"
#include "smfactions_p.h"


/**
 * Constructor
 */
SmfActivityObject::SmfActivityObject( )
	{
	d = new SmfActivityObjectPrivate;
	}

/**
 * Copy Constructor
 * @param aOther The reference object
 */
SmfActivityObject::SmfActivityObject( const SmfActivityObject &aOther )
	: d( aOther.d )
	{
	}

/**
 * Overloaded = operator
 * @param aOther The reference object
 * @return The target reference value
 */
SmfActivityObject& SmfActivityObject::operator=( const SmfActivityObject &aOther )
	{
	d->m_id = aOther.d->m_id;
	d->m_thumbnail = aOther.d->m_thumbnail;
	d->m_caption = aOther.d->m_caption;
	d->m_type = aOther.d->m_type;
	d->m_objData = aOther.d->m_objData;
	d->m_link = aOther.d->m_link;
	d->m_time = aOther.d->m_time;
	d->m_content = aOther.d->m_content;
	return *this;
	}

/**
 * Destructor
 */
SmfActivityObject::~SmfActivityObject( )
	{
	}


/**
 * @return returns service specific Id of this object in the entry 
 */
QString SmfActivityObject::id( ) const
	{
	return d->m_id;
	}

/**
 * @return returns thumbnail if availabel for this object in the entry
 */
QImage SmfActivityObject::thumbnail( ) const
	{
	return d->m_thumbnail;
	}

/**
 * @return Captions for this object
 */
QString SmfActivityObject::caption( ) const
	{
	return d->m_caption;
	}

/**
 * @return retruns the type of this object
 */
SmfActivityObjectType SmfActivityObject::type( ) const
	{
	return d->m_type;
	}

/**
 * @return returns data of the object - see type() for casting
 */
QVariant SmfActivityObject::objData( ) const
	{
	return d->m_objData;
	}

/**
 * @return url of this object, e.g. url of the image on which somebody commented
 */
QString SmfActivityObject::link( ) const
	{
	return d->m_link;
	}

/**
 * @return Time of creation of this object
 */
QDateTime SmfActivityObject::time( ) const
	{
	return d->m_time;
	}

/**
 * @return Description of the content of this object
 */
QString SmfActivityObject::content( ) const
	{
	return d->m_content;
	}

/**
 * Sets service specific id for this object
 * @param aId The id to be set
 */
void SmfActivityObject::setId(const QString& aId)
	{
	d->m_id = aId;
	}

/**
 * Sets thumbnail for this object , not mandatory
 * @param aIcon The thumbnail image
 */
void SmfActivityObject::setThumbnail(const QImage& aIcon)
	{
	d->m_thumbnail = aIcon;
	}

/**
 * Sets caption for this object
 * @param aCaption The caption to be set
 */
void SmfActivityObject::setCaption(const QString& aCap)
	{
	d->m_caption = aCap;
	}

/**
 * Sets type of this object - ObjData should be set next accordingly
 * @param aObjType The activity type to be set
 */
void SmfActivityObject::setType(const SmfActivityObjectType& aObjType)
	{
	d->m_type = aObjType;
	}

/**
 * Sets object data e,g, SmfImage, SmfComment, SmfMusic, SmfEvent etc
 * setType() with suitable type should have been called before
 * @param aData The object data to be set  
 */
void SmfActivityObject::setObjData( const QVariant& aData)
	{
	d->m_objData = aData;
	}

/**
 * Sets the url for the object
 * @param aLink The Link to be set
 */
void SmfActivityObject::setLink( const QString& aLink)
	{
	d->m_link = aLink;
	}

/**
 * Sets the time of creation/last modification of the object
 * @param aTime The time to be set
 */
void SmfActivityObject::setTime( const QDateTime& aTime)
	{
	d->m_time = aTime;
	}

/**
 * Sets the description of the content of this object
 * @param aText The content to be set
 */
void SmfActivityObject::setContent( const QString& aText)
	{
	d->m_content = aText;
	}

/**
 * Method for Externalization. Writes the SmfActivityObject object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aObj The SmfActivityObject object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfActivityObject &aObj )
	{
	aDataStream<<aObj.d->m_id;
	aDataStream<<aObj.d->m_thumbnail;
	aDataStream<<aObj.d->m_caption;
	aDataStream<<aObj.d->m_type;
	aDataStream<<aObj.d->m_objData;
	aDataStream<<aObj.d->m_link;
	aDataStream<<aObj.d->m_time;
	aDataStream<<aObj.d->m_content;
		
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfActivityObject object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aObj The SmfActivityObject object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		SmfActivityObject &aObj)
	{
	aDataStream>>aObj.d->m_id;
	aDataStream>>aObj.d->m_thumbnail;
	aDataStream>>aObj.d->m_caption;
	int i;
	aDataStream>>i;
	aObj.d->m_type = (SmfActivityObjectType)i;
	aDataStream>>aObj.d->m_objData;
	aDataStream>>aObj.d->m_link;
	aDataStream>>aObj.d->m_time;
	aDataStream>>aObj.d->m_content;
	
	return aDataStream;
	}

 
 
SmfActivityEntry::SmfActivityEntry()
 	{
	d = new SmfActivityEntryPrivate;
 	}
SmfActivityEntry::SmfActivityEntry( const SmfActivityEntry &aOther )
	:d( aOther.d )
	{
	}
SmfActivityEntry::~SmfActivityEntry()
	{
	
	}
/**
 * @return Returns service specific id of the of entry 
 */
QString SmfActivityEntry::id() const
	{
	return d->m_id;
	}

/**
 * @return Returns the title of the entry - mostly be a string 
 */
SmfPost SmfActivityEntry::title() const
	{
	return d->m_title;
	}

/**
 * @return Returns detail descriptiopn of this entry in the activity list. might be absent if title is sufficient
 */
SmfPost SmfActivityEntry::details() const
	{
	return d->m_details;
	}

/**
 * @return Returns the author of the activity - the name and the uri field are most commonly used
 * Other information fields might be empty
 */
SmfContact SmfActivityEntry::author() const
	{
	return d->m_author;
	}

/**
 * @return Returns the verb of the activity ,e.g. Robin "marked" Joseph as a friend   
 */
SmfActivityVerb SmfActivityEntry::actionName() const
	{
	return d->m_actionName;
	}

/**
 * There can be multiple objects in a single activity entry, though this may be rare - only for few verbs.
 * @return list of activity objects (mostly one object)
 */
QList<SmfActivityObject> SmfActivityEntry::activities() const
	{
	return d->m_activities;
	}

/**
 * @return Returns information about the target of the activity, for verbs that support a target.
 * For example, a target is a photo album to which photos were added
 */
SmfActivityObject SmfActivityEntry::targetObj() const
	{
	return d->m_targetObj;
	}

/**
 * Sets service specific id of the of entry 
 * @param aId The id to be set
 */
bool SmfActivityEntry::setId( const QString& aId)
	{
	d->m_id = aId;
	return true;
	}

/**
 * Sets the title of the entry - mostly be a string
 * @param aTitle The title to be set 
 */
bool SmfActivityEntry::setTitle(const SmfPost& aTitle)
	{
	d->m_title = aTitle;
	return true;
	}

/**
 * Sets detail description of this entry in the activity list. might be absent if title is sufficient
 * @param aDetails The details to be set
 */
bool SmfActivityEntry::setDetails(const SmfPost& aDetails)
	{
	d->m_details = aDetails;
	return true;
	}

/**
 * Sets the author of the activity - the name and the uri field are most commonly used
 * Oher information fields might be empty
 * @param aContact The author to be set
 */
bool SmfActivityEntry::setAuthor(const SmfContact& aContact)
	{
	d->m_author = aContact;
	return true;
	}

/**
 * Sets the verb of the activity ,e.g. Robin "marked" Joseph as a friend
 * @param aVerb The action name to be set   
 */
void SmfActivityEntry::setActionName(SmfActivityVerb  aVerb)
	{
	d->m_actionName = aVerb;
	}

/**
 * Sets single or multiple objects in a single activity entry, though multiple may be rare - only for few verbs.
 * @param list of activity objects (mostly one object)
 */
bool SmfActivityEntry::setActivities(QList<SmfActivityObject>& aList)
	{
	d->m_activities = aList;
	return true;
	}

/**
 * Returns information about the target of the activity, for verbs that support a target.
 * For example, a target is a photo album to which photos were added
 * @param aTarget The activity object to be set
 */
bool SmfActivityEntry::setTargetObj(const SmfActivityObject& aTarget)
	{
	d->m_targetObj = aTarget;
	return true;
	}


/**
 * Method for Externalization. Writes the SmfActivityEntry object to 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be written
 * @param aObj The SmfActivityEntry object to be externalized
 * @return reference to the written stream
 */
 QDataStream &operator<<( QDataStream &aDataStream, 
		const SmfActivityEntry &aObj )
	{
	aDataStream<<aObj.d->m_id;
	aDataStream<<aObj.d->m_title;
	aDataStream<<aObj.d->m_details;
	aDataStream<<aObj.d->m_author;
	aDataStream<<aObj.d->m_actionName;
	aDataStream<<aObj.d->m_activities;
	aDataStream<<aObj.d->m_targetObj;
		
	return aDataStream;
	}

/**
 * Method for Internalization. Reads a SmfActivityEntry object from 
 * the stream and returns a reference to the stream.
 * @param aDataStream Stream to be read
 * @param aObj The SmfActivityEntry object to be internalized
 * @return reference to the stream
 */
 QDataStream &operator>>( QDataStream &aDataStream, 
		 SmfActivityEntry &aObj)
	{
	aDataStream>>aObj.d->m_id;
	aDataStream>>aObj.d->m_title;
	aDataStream>>aObj.d->m_details;
	aDataStream>>aObj.d->m_author;
	int i;
	aDataStream>>i;
	aObj.d->m_actionName = (SmfActivityVerb)i;
	aDataStream>>aObj.d->m_activities;
	aDataStream>>aObj.d->m_targetObj;
	
	return aDataStream;
	}

 
