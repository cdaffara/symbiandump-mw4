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
* Description: WPS wizard page step 5
*
*/

#ifndef WPSWIZARDSTEPFIVE_H_
#define WPSWIZARDSTEPFIVE_H_


// System includes
#include <QObject>
#include <HbDocumentLoader>

// User includes
#include "wpswizardpage.h"

// Forward declarations
class HbRadioButtonList;
class HbWidget;
class HbLabel;
class HbProgressBar;
class WpsWizardPrivate;


// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsPageStepFive : public WpsWizardPage
    {
Q_OBJECT

public:
    
    explicit WpsPageStepFive(WpsWizardPrivate* parent);
    ~WpsPageStepFive();

public:

    HbWidget* initializePage();

    int nextId(bool &removeFromStack) const;

    int previousTriggered();

    void cancelTriggered();

    bool showPage();
	
signals:

public slots:

    void itemSelected(int index);
    
    void loadDocmlSection(Qt::Orientation orientation);

protected:

protected slots:

private:

    Q_DISABLE_COPY(WpsPageStepFive)
	
private slots:

private:    
    
    //data

    //! Pointer to the HbWidget object
    HbWidget *mWidget;
    //! Radio button list object
    HbRadioButtonList *mRadio;
    //! Heading label object
    HbLabel *mHeading;
    //! variable used to determine if next button has to be enabled.
    bool mValid;
    //! Documentloader object
    HbDocumentLoader *mLoader;
    };

#endif /* WPSWIZARDSTEPFIVE_H_ */
