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









#ifndef M_SEN_FILTER_COLLECTOR_H
#define M_SEN_FILTER_COLLECTOR_H


// INCLUDES

#include <e32base.h>

#include "SenBaseFragment.h"
#include <SenXmlElement.h> // check if this include is needed(?)

//#include "SenAttributes.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DEFINITIONS


/**
 * @author lewontin
 *
 * The interface to be implemented by objects that want to collect filter results.
 */


class MSenFilterCollector 
{
public:
    /**PutL
     * Initialize the collector:
     * This is called before the collector starts collecting. Use this
     * to initialize the data structure into which elements are collected.
     */ 
    virtual void Initialize() = 0;

    /**
     * Collect element tag:
     * Called when an element is first encountered (i.e. before any of the
     * element's content has been parsed.) Use this to allocate a data type
     * that represents an element with other element content. 
     * 
     */
    virtual void PutElementL(const TDesC8& aName, 
                             const RAttributeArray& aAttributes) = 0;

    /**
     * Collect element text content:
     * Called when an element's content has been fully parsed. This provides the
     * text content as a "property", i.e. an elementname/value pair.
     */
    virtual void PutL(const TDesC8& aName, 
                      const TDesC8& aValue) = 0;


};

#endif // M_SEN_FILTER_COLLECTOR_H

