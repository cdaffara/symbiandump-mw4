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
* Description: Step 3 Button press mode
*
*/

#ifndef WPSWIZARDSTEPTHREEBUTTON_H_
#define WPSWIZARDSTEPTHREEBUTTON_H_

// System includes
#include <QObject>
#include <HbDocumentLoader>

// User includes
#include "wpswizardpage.h"

// Forward declarations
class HbRadioButtonList;
class HbWidget;
class HbLabel;
class WpsWizardPrivate;

// External data types

// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsPageStepThreeButton : public WpsWizardPage
    {
Q_OBJECT

public:
    explicit WpsPageStepThreeButton(WpsWizardPrivate* parent);
    ~WpsPageStepThreeButton();

public:

    HbWidget* initializePage();

    int nextId(bool &removeFromStack) const;

    int previousTriggered();
    
signals:

public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(WpsPageStepThreeButton)

private slots:

private: //data    
    //! Pointer to the widget object
    HbWidget *mWidget;
    //! Pointer to the label displaying the heading
    HbLabel *mHeading;
    //! Document loader object.
    HbDocumentLoader *mLoader;
    };

#endif /* WPSWIZARDSTEPTHREEBUTTON_H_ */
