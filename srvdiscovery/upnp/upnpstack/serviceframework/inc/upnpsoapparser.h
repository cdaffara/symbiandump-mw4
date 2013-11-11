/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the TUpnpSoapParser class
 *
*/


#ifndef __UPNPSOAPPARSER_H__
#define __UPNPSOAPPARSER_H__

// CLASS DECLARATION
class CUpnpContentHandlersController;
class CUpnpAction;
class CUpnpDescriptionProperty;
class CUpnpSoapMessage;

#include <e32def.h>
#include "upnpdescriptionproperty.h"

/**
 * Class is responsible for parsing soap messages logic
 * with usage of CUpnpControllersContentHandler
 */
class TUpnpSoapParser
    {
public:

    /**
     * Constructor
     * @param iParsingControler controler that will be used to invoke soap parsing
     */
    TUpnpSoapParser( CUpnpContentHandlersController& aParsingController );

    /**
    * For internal use. Update all necessary information from a SOAP-message
    * that should be OK answer (e.g. received by Http 200 OK message).
    * @param aMessage SOAP response message
    * @param aAction an action to update
    */
    void UpdateActionWithOKResponseL( CUpnpSoapMessage* aMessage,
        CUpnpAction* aAction );

    /**
     * For internal use. Update all necessary information from a SOAP-message
     * that should contain SOAP error (fault) message (eg. received by http
     * 500 InternalServerError message.
     * @param aMessage SOAP response message
     * @param aAction an action to update
     */
    void UpdateActionWithErrorResponseL( CUpnpSoapMessage* aMessage,
            CUpnpAction* aAction );

    /**
    * Update action with all necessary information from
    * a SOAP request message.
    * @since Series60 3.2
    * @param aMessage SOAP request message
    * @param aAction - an action to update
    */
    void UpdateActionWithRequestL( CUpnpSoapMessage* aMessage, CUpnpAction* aAction );

private:
    /**
     * Process parsed description property passed as an argument, and the rest of parsed
     * properties and update action arguments with them.
     * @param aProperties arguments list (first with action name is ignored)
     * @param aAction an action to update
     */
    void ProcessActionArgumentsL(
        RPointerArray<CUpnpDescriptionProperty>& aProperties, CUpnpAction* aAction );

    /**
     * Update all action arguments with previously parsed properties.
     * @param aProperties arguments list (first with action name is ignored)
     * @param aAction an action to update
     */
    void UpdateActionArgumentsL(
        RPointerArray<CUpnpDescriptionProperty>& aProperty, CUpnpAction* aAction );

    /**
     * Update action error value from parsed description property
     * @param aPropert parsed property
     * @param aAction an action to update
     */
    void UpdateActionFaultL( CUpnpDescriptionProperty* aProperty, CUpnpAction* aAction );

    /**
     * Common code used by 3 particular update soap action methods
     *
     */
    void UpdateActionL( CUpnpSoapMessage* aMessage,
        CUpnpAction* aAction, RPointerArray<CUpnpDescriptionProperty>& aParsedValues );

private:
    //not owned
    CUpnpContentHandlersController& iParsingController;

    };

#endif //__UPNPSOAPPARSER_H__
