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
* Description:  Declares virtual MUpnpTimeoutElementParent callback class
*                and the implemented active object class CUpnpTimeoutElement
*
*/


#ifndef C_CUPNPTIMEOUTELEMENT_H
#define C_CUPNPTIMEOUTELEMENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpTimeoutElement;

// CLASS DECLARATION

/**
*  @brief A virtual parent class.
*
*  This interface class is used to receive the timeout callbacks from timeout
*  elements.
*
*  @since Series60 2.0
*/

class MUpnpTimeoutElementParent 
    {
    public:

    /**
    * A virtual function to be implemented in derived classes.
    * For example, the implementation might contain deletion of the timeout element.
    * @since Series60 2.0
    * @param anElement This pointer contains the element whose timeout has ran out. 
    * @return None
    */
	virtual void TimeoutExpiredL( CUpnpTimeoutElement* anElement ) = 0;
    };

/**
*  @brief An element with timeout
*
*  This is a virtual base class containing a timeout functionality. This class
*  is used by following steps:
*
*  1. Inherit your class from CUpnpTimeoutElement; 
*
*  2. In the constructor of the derived class, call also the constructor of 
*  CUpnpTimeoutElement. This means you have to have an element library class which
*  is derived from the class MUpnpTimeoutElementParent. This element library has to
*  be passed to the constructor of CUpnpTimeoutElement.
*
*  3. Call the SetTimeout function to set the timeout of the element
*
*  4. If the timeout should be renewed after a timeout, call SetRenew to set the
*  member variable iRenew. That variable can be used later to decide the destiny
*  of an element.
*
*  @since Series60 2.0
*/
class CUpnpTimeoutElement : public CActive
    {
    private:
        static const TUint32 KMicroInSec = 1000000;
        static const TInt KTime = 1800;
    
    protected: // Constructors and destructors

        /**
        * Constructor function; call from the constructor of a derived class.
        * @since Series60 2.0
        * @param aParent A class implementing the parent interface 
        * MUpnpTimeoutElementParent, in practice the library class
        */
    IMPORT_C CUpnpTimeoutElement( MUpnpTimeoutElementParent& aParent );
      
        /**
        * Base class constructor function
        * @since Series60 2.0
        */
    IMPORT_C void BaseConstructL();
   
    public:
 
    	/**
	    * Destructor function; called automatically from the destructor of a 
        * derived class
	    **/
	IMPORT_C virtual ~CUpnpTimeoutElement();

        /**
    	* This function sets the timeout of the element. The TimeoutExpired()
        * function of the parent of this element is called after the timeout
        * @since Series60 2.0
        * @param aSeconds
        */
	IMPORT_C void SetTimeout( TInt aSeconds );

    public: // From CActive

        /**
    	* Standard callback function of CActive-derived functions
        * @since Series60 2.0
        * @return None
        */
	IMPORT_C void RunL();

        /**
    	* Standard callback function of CActive-derived functions
        * @since Series60 2.0
        * @return None
        */
	IMPORT_C void DoCancel();

        /**
    	* Standard callback function of CActive-derived functions
        * @since Series60 2.0
        * @return None
        */
    IMPORT_C TInt RunError( TInt aError );

    public: // New Functions

	    // Enumeration for different timeouts:
	    enum TState {
		    EAlive,
		    EDead,
		    ELongTime
	    };

	    // Enumeration for different timeouts:
	    enum TRenew { 
		    // This value is for timeouts that should be renewed after the timeout
		    ERenew, 
		    // This value is for timeouts that declare the closing of 
			// an external (device) element; 
		    EOnce,
			// This value is for timeouts that declare the closing of an element;
			// timeout not renewed anymore.
			ERemove
	    };

        /**
    	* Returns the current renew setting
        * @since Series60 2.0
        * @return Returns the current renew setting
        */
	IMPORT_C TRenew& Renew();

        /**
    	* ets the renew setting of this element
        * @since Series60 2.0
        * @param aRenew The new renew setting
        */
	IMPORT_C void SetRenew( TRenew aRenew );

    private: 

	    TRenew iRenew;
	    TState iState;
	    RTimer iTimer;
	    MUpnpTimeoutElementParent& iParent;
	    TInt iSeconds;
    };

#endif	// C_CUPNPTIMEOUTELEMENT_H

// End of File