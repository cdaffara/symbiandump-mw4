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
* Description: WPS wizard page step 4
*
*/

#ifndef WPSWIZARDSTEPFOUR_H_
#define WPSWIZARDSTEPFOUR_H_


// System includes
#include <QObject>
#include <HbDocumentLoader>

// User includes
#include "wpswizardpage.h"
#include "mwpsactiverunnercallback.h"
#include "cwpsactiverunner.h"

// Forward declarations
class HbWidget;
class WpsWizardPrivate;
class WpsPageStepFourPrivate;

// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsPageStepFour : public WpsWizardPage
    {
Q_OBJECT

public:

    explicit WpsPageStepFour(WpsWizardPrivate* parent);
    ~WpsPageStepFour();

public:

    HbWidget* initializePage();

    int nextId(bool &removeFromStack) const;

    int previousTriggered();

    void cancelTriggered();

    void startOperation();

    bool requiresStartOperation();

    void setCredentials(
            QList<TWlanProtectedSetupCredentialAttribute>& credentials,
            int credentialscount);

    void handleError( int aErrorCode );

    bool showPage();

signals:

public slots:

    void loadDocmlSection(Qt::Orientation orientation);

protected:

protected slots:

private:

    void startWpsRunner();

    Q_DISABLE_COPY(WpsPageStepFour)

private slots:

private:

    //data

    //! HbWidget object displayed on the page
    HbWidget *mWidget;
    //! Member variable to hold the result of of the WPS operation
    int mWpsErrorCode;
    //! Member variable containing the count of received settings.
    int mCountCredentials;
    //! variable to determine whether to enable the next button or not.
    bool mValid;
    //! Document loader object.
    HbDocumentLoader *mLoader;
    //! Pointer to Private Implementation
    QScopedPointer<WpsPageStepFourPrivate> d_ptr;

    friend class TestWlanWizardContext;
    };

#endif /* WPSWIZARDSTEPFOUR_H_ */
