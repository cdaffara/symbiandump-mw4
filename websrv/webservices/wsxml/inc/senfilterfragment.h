/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Header declaration
*
*/








#ifndef C_SEN_FILTER_FRAGMENT_H
#define C_SEN_FILTER_FRAGMENT_H

// INCLUDES

#include <e32base.h>
#include <flogger.h>

#include "SenElement.h"
#include "SenBaseFragment.h"
#include <SenXmlElement.h> // check if this include is needed(?)

#include "SenFilterStateMachine.h"
#include "MSenFilterCollector.h"
#include "MSenFilterAction.h"

// CONSTANTS

// FORWARD DECLARATIONS

class CSenFilterStateMachine;
class CSenFilterStateNode;
class CSenFilterTableEntry;
class CSenFilterNewProperty;
class CSenFilterPutProperty;

// CLASS DEFINITIONS

/**
 * @author lewontin
 *
 */


class CSenFilterFragment : public CSenBaseFragment
{

public: // Constructors and Destructors

    IMPORT_C CSenFilterFragment();


    IMPORT_C ~CSenFilterFragment();


public:

    /**
     * 
     * Save any text property
     */

    
    
    IMPORT_C void SetCollector(MSenFilterCollector& aCollector);

    /**
     *  Inherited methods from CSenBaseFragment
     */
    IMPORT_C void StartElementL(const TDesC8& aNamespaceURI, 
                                const TDesC8& aLocalName, 
                                const TDesC8& aQName,
                                const RAttributeArray& aAttributes);


    IMPORT_C void CharactersL(const TDesC8& aCh, 
                              TInt aStart, 
                              TInt aLength);


    IMPORT_C void EndElementL(const TDesC8& aNamespaceURI, 
                              const TDesC8& aLocalName, 
                              const TDesC8& aQName);

    IMPORT_C TInt BuildFrom(const TDesC8& aString);


// Public filter API

    IMPORT_C CSenFilterStateNode& AddFilterLC(CSenFilterStateNode& aNode, 
                                              const TDesC8& aLocalName);


    IMPORT_C CSenFilterStateNode& AddRootFilterLC(const TDesC8& aLocalName);


    IMPORT_C void CompileL();

    /*public void andFilter(const TDesC8& localName) throws FilterException {
        ipFsm.andFilter(localName, NULL, ipNewP, ipPutP);
    }
    public void orFilter(const TDesC8& localName) throws FilterException {
        ipFsm.orFilter(localName, NULL, ipNewP, ipPutP);
    }
    
    public void compile(){
        ipFsm.compile();
    }*/

    /* // Override base parse to make sure the current fsm is compiled first
     public void parse(org.xml.sax.InputSource source)
     throws java.io.IOException, org.xml.sax.SAXException
     {
         ipFsm.compile();
         super.parse(source);
     }*/


public:

    CSenFilterNewProperty* ipNewP;
    CSenFilterPutProperty* ipPutP;


protected:

    //To hold the current string property value
    HBufC8* ipPropertyValue; 

    MSenFilterCollector* ipCollector;


private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    CSenFilterStateMachine* ipFsm;

    //To hold the current property name
    HBufC8* ipPropertyName;

    CSenFilterStateNode* iRoot;

    RFileLogger iLog;

}; // end class CSenFilterFragment


// ------------------------------------------
//
// Declaration of class CSenFilterNewProperty
//
// ------------------------------------------

class CSenFilterNewProperty : public CBase, MSenFilterAction
{

public: // Constructors and Destructors

    IMPORT_C CSenFilterNewProperty(MSenFilterCollector& aCollector);


    IMPORT_C ~CSenFilterNewProperty();


public:

    IMPORT_C void PerformL(const TDesC8& aEvent, 
                           const RAttributeArray& aAttributes);


private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    MSenFilterCollector* ipCol;

    RFileLogger iLog;

}; // end class CSenFilterNewProperty


// ------------------------------------------
//
// Declaration of class CSenFilterPutProperty
//
// ------------------------------------------

class CSenFilterPutProperty : public CBase, MSenFilterAction
{

public: // Constructors and Destructors

    CSenFilterPutProperty(MSenFilterCollector& aCollector);


    ~CSenFilterPutProperty();


public:

    void SetPropertyValueL(const TDesC8& aPropertyValue);


    void PerformL(const TDesC8& aEvent, 
                  const RAttributeArray& aAttributes);


private:

    /**
     * File logger.
     *
     */
    RFileLogger* Log() const;


private:

    HBufC8* ipPropertyValue; 
    MSenFilterCollector* ipCol;

    RFileLogger iLog;

}; // end class CSenFilterPutProperty


#endif // C_SEN_FILTER_FRAGMENT_H

