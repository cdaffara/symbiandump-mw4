/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface class for the middleware callback
*
*/
#ifndef MWPSACTIVERUNNERCALLBACK_H_
#define MWPSACTIVERUNNERCALLBACK_H_

// System includes
#include <QList>
#include <wlanmgmtcommon.h>

// User includes
// Forward declarations
// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class MWpsActiveRunnerCallback
    {
public:
    /*!
     * Callback to notify the completion of middleware API call
     */
    virtual void WpsActiveRunnerStopped(
        QList<TWlanProtectedSetupCredentialAttribute>& aCredentials,
        TInt aError) = 0;
    };

#endif /* MWPSACTIVERUNNERCALLBACK_H_ */
