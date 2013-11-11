/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class represents the Widget Preferences object
*
*/

#ifndef __WIDGETENGINEPREFERENCES
#define __WIDGETENGINEPREFERENCES

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATION

// CLASS DECLARATION
template <class K, class V> class RPtrHashMap;

/**
*  PrefElement
*
*  @lib widgetengine.dll
*  @since 3.2
*/
class PrefElement : public CBase
{
public :
    /**
    * Constructor.
    */
    PrefElement();
    
    /**
    * Destructor.
    */
    ~PrefElement();
    
    /**
    * setValueL
    * Set value for a preference
    * @param const TDesC& aValue - The value to be set
    * @return none
    */
    void setValueL( const TDesC& aValue );
    
    /**
    * setValueSize
    * Set length of value 
    * @param const TInt aSize - The length to be set
    * @return none
    */
    void setValueSize( const TInt aSize ) {
        m_valuesize = aSize;
    }
    
    /**
    * setCleanFileFlag
    * @param TBool aCleanFileFlag
    * @return none
    */
    void setCleanFileFlag( const TBool aCleanFileFlag ) {
        m_cleanFileFlag = aCleanFileFlag;
    }

    /**
    * GetValue
    * Get value for a preference
    * @param none
    * @return const TDesC& - value for the preference
    */      
    const TDesC& value() const {
        if ( m_value ) 
            return *m_value;
        else 
            return KNullDesC();
    }
        
    /**
    * GetValueSize
    * Get length of value
    * @param none
    * @return TInt - length of value for the preference
    */      
    TInt valueSize() const { return m_valuesize; }
        
private :
    HBufC*  m_value;     // value of the preference: if size > MAX, it's the file name which stores the value
    TInt    m_valuesize; // length of the value
    TBool   m_cleanFileFlag;  // flag for deleteing individual preference dat file
};


/**
*  WidgetPreferences
*
*  @lib widgetengine.dll
*  @since 3.1
*/
class WidgetPreferences
{

public:        
    WidgetPreferences();
    ~WidgetPreferences();

public:
    /**
    * 
    */
    void setBasePathL(const TDesC& aValue);

    /**
    * 
    */
    void setWidgetId(TInt aValue);

    /**
    * 
    */
    void setWidgetBundleId(const TDesC& aValue);
    
    /**
    * 
    */
    TDesC& getWidgetBundleId();

    /**
    * Load preferences from persistent storage
    */
    void loadL();
    
    /**
    * Save preferences to persistent storage
    */
    void saveL();
    
    /**
    * Get preference for a particular key
    */
    TInt preferenceL( const TDesC& akey, HBufC*& avalue);

    
    /**
    * Set preference for a particular key
    */
    void setPreferenceL( const TDesC& akey, const TDesC& avalue);   

    /**
    * Remove preference for a particular key
    */
    void removePreferenceL( const TDesC& akey, const TDesC& avalue);   
    

private:
    RPtrHashMap<TDesC,PrefElement>* m_preferences;                
    
    TInt   m_widgetid;
    HBufC* m_widgetbundleid;
    HBufC* m_basepath;
    HBufC* m_filepath;

};
    
    
#endif
