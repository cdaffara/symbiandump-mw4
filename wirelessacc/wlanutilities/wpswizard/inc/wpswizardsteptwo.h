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
* Description: WPS wizard page 2
*
*/
#ifndef WPSWIZARDSTEPTWO_H_
#define WPSWIZARDSTEPTWO_H_

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
class WpsPageStepTwo : public WpsWizardPage
    {
Q_OBJECT

public:
    explicit WpsPageStepTwo(WpsWizardPrivate* parent);
    ~WpsPageStepTwo();

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
    Q_DISABLE_COPY(WpsPageStepTwo)

private slots:   

private: //data
    //! Pointer to the widget object
    HbWidget *mWidget;
    //! Radio button list object
    HbRadioButtonList *mRadio;
    //! Label object used to display the heading
    HbLabel *mHeading;
    //! Variable used to determine whether to enable next button or not
    bool mValid;
    //! Variable used to store the item selected.
    int mItemSelected;
    //! Document loader object.
    HbDocumentLoader* mLoader;
    };


#endif /* WPSWIZARDSTEPTWO_H_ */
