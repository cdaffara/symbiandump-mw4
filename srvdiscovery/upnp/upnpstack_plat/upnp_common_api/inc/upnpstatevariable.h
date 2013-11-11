/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares CUpnpStateVariable class
*
*/


#ifndef C_CUPNPSTATEVARIABLE_H
#define C_CUPNPSTATEVARIABLE_H

// INCLUDES

#include <badesca.h>
#include "upnpcons.h"

// FORWARD DECLARATIONS

class CUpnpServiceImplementation;

// CLASS DECLARATION

/**
*  State variable handler class.
*  Provides easy access to the state variables of the device.
*
*  @since Series60 2.6
*/
class CUpnpStateVariable: public CBase
    {
public: // Constructors and destructor

    static CUpnpStateVariable* NewL();
   
    /**
    * Destructor. Deletes the element from memory. 
    */
    virtual ~CUpnpStateVariable();
    
public: // New functions

    /**
    * Set parent implementation
    * @since Series60 2.6
    * @param aParent the parent service
    */
    void SetParentImplementation( CUpnpServiceImplementation& aParent );

    /**
    * Returns argument type
    * @since Series60 2.6
    * @return argument type
    */
    IMPORT_C TArgumentType Type();   
    
    /**
    * Set argument type
    * @since Series60
    * @aParam aType Type of the state variable
    */
    IMPORT_C void SetTypeL( const TDesC8& aType );
    
    /**
    * Get state variable name
    * @since Series60 2.6
    * @return Name of the state variable as descriptor.
    */
    IMPORT_C const TPtrC8 Name();

    /**
    * Set state variable name
    * @since Series60
    * @param aName  Name of the state variable
    */
    IMPORT_C void SetNameL( const TDesC8& aName );
    
    /**
    * Get value of state variable
    * @since Series60 2.6
    * @return Value of the state variable as descriptor.
    */
    IMPORT_C const TPtrC8 Value();
    
    /**
    * Sets the Value of the state variable (given as a descriptor)
    * @since Series60 2.6
    * @param aValue containing the wanted value of the variable
    */
    IMPORT_C void SetValueL( const TDesC8& aValue,
                             TBool aIsModerated = ETrue );

    /**
    * Sets the Value of the state variable (given as an integer)
    * @since Series60 2.6
    * @param aValue containing the wanted value of the variable
    */
    IMPORT_C void SetValueL( TInt aValue, TBool aIsModerated = ETrue );
    
    /**
    * Get event 
    * @since Series60 2.6
    * @return Eventability, "yes" if eventable.
    */
    IMPORT_C const TPtrC8 Eventable();
    
    /**
    * Set event 
    * @since Series60 
    *  
    */ 
    IMPORT_C void SetEventableL( const TDesC8& aEventable );
    
    /**
    * Get default value
    * @since Series60 2.6
    * @param aValue the reference to the value to be set
    * @return Default value of the variable as a integer.
    */
    IMPORT_C void GetDefaultValue( TInt& aValue );

    /**
    * Get default value
    * @since Series60 2.6
    * @return Default value of the variable.
    */
    IMPORT_C const TPtrC8 DefaultValue();
    
    /**
    * Get list of allowed values
    * @since Series60 2.6
    * @return List of allowed values. 
    * If variable has not allowedValueList a empty array is returned.
    */
    IMPORT_C CDesC8Array* AllowedValuesLC();
     /**
    * Add a allowed value
    * @since Series60
    * @aParam aAllowedValue a new allowed value
    */
    IMPORT_C void AddAllowedValueL( const TDesC8& aAllowedValue );
    
    /**
    * Get maximum value
    * @since Series60 2.6
    * @return Maximum value of an integer variable.
    */
    IMPORT_C TInt MaxValue();
    
    /**
    * Get minimum value
    * @since Series60 2.6
    * @return Minimum value of an integer variable.
    */
    IMPORT_C TInt MinValue();
    
    /**
    * Get step
    * @since Series60 2.6
    * @return Size of an increment for the value.
    */
    IMPORT_C TInt Step();

    /**
    * Is the statevariable ready for eventing
    */
    TBool ReadyForEventing();

    /**
    * Returns the minimum time between eventing
    * this statevariable
    * @return Minimum time between events
    */
    TInt MaxEventRate();

    /**
    * Keeps track of how many timeouts have beend reached after
    * previous event
    */
    void EventTick();

    /**
    * Sets the minimum time between events
    * for this statevariable
    * @param aMaxRate Minimum time between events in microseconds
    * @param aNormalRate Rate that is used to event this state variable.
    */
    void SetMaxEventRate(TInt aMaxRate, TInt aNormalRate);
    
    void SetDefaultValueL(const TDesC8& aDefaultValue );
    void SetRangeMinL(const TDesC8& aRangeMin );
    void SetRangeMaxL(const TDesC8& aRangeMax );
    void SetRangeStepL(const TDesC8& aRangeStep );
    TBool IsModerated();
private: // Constructors

    /**
    * C++ default constructor.
    * @param aElement This pointer contains the state variable element
    */
    CUpnpStateVariable();
    
    /** 
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
private: // Data

    // Parent service implementation, owned
    CUpnpServiceImplementation* iParent;
    // normal event rate
    TInt            iNormalEventRate;
    // max event rate
    TInt            iMaxEventRate;
    // time
    TTime           iTime;
    // event time
    TTime           iEventTime;
    // current event time
    TInt            iCurrentEventTime;
    // eventing ready flag
    TBool           iReadyForEventing;
    // name
    HBufC8*         iName;
    // data type
    HBufC8*         iDataType;
    // default value
    HBufC8*         iDefaultValue;
    // minimal range
    HBufC8*         iRangeMin;
    // maximal range
    HBufC8*         iRangeMax;
    // range step
    HBufC8*         iRangeStep;
        
    // allowed value list
    RPointerArray<HBufC8>   iAllowedValueList;
    // value
    HBufC8*         iValue;
    // event table
    HBufC8*         iEventable;
    // is moderated flag
    TBool           iModerated;
    };

#endif  // C_CUPNPSTATEVARIABLE_H

//  End of File
