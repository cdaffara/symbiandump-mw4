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
 * Description: 
 *   WLAN Wizard Page: Generic error page.
 *
 */

// System includes
#include <HbMainWindow>
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbLabel>

// User includes
#include "wlanwizard_p.h"
#include "wlanwizardpagegenericerror.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpageprocessingsettingsTraces.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagegenericerrorTraces.h"
#endif

#endif

/*!
   \class WlanWizardPageGenericError
   \brief Implements generic error page for wizard.
   
   Error text is read from the configurations 
   (WlanWizardHelper::ConfGenericErrorString) of the wizard. 
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.
   
   @param [in] parent pointer to private implementation of wizard.
 */
WlanWizardPageGenericError::WlanWizardPageGenericError(
    WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent), 
    mWidget(NULL), 
    mLabel(NULL)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEGENERICERROR_WLANWIZARDPAGEGENERICERROR_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGEGENERICERROR_WLANWIZARDPAGEGENERICERROR_EXIT );
}

/*!
   Destructor.
 */
WlanWizardPageGenericError::~WlanWizardPageGenericError()
{
    OstTraceFunctionEntry0( DUP1_WLANWIZARDPAGEGENERICERROR_WLANWIZARDPAGEGENERICERROR_ENTRY );
    // signals are automatically disconnected
    OstTraceFunctionExit0( DUP1_WLANWIZARDPAGEGENERICERROR_WLANWIZARDPAGEGENERICERROR_EXIT );
}

/*!
   See WlanWizardPage::initializePage()
 */
HbWidget* WlanWizardPageGenericError::initializePage()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEGENERICERROR_INITIALIZEPAGE_ENTRY );
    // Create the visualization at the first time
    if (!mWidget) {
        HbDocumentLoader docLoader(mWizard->mainWindow());
        
        bool ok;
        docLoader.load(":/docml/occ_add_wlan_error.docml", &ok);
        Q_ASSERT(ok);
        
        mWidget = qobject_cast<HbWidget*> (docLoader.findWidget("occ_add_wlan_error"));
        Q_ASSERT(mWidget != NULL);

        mLabel = qobject_cast<HbLabel*> (docLoader.findWidget("dialog"));
        Q_ASSERT(mLabel != NULL);
    }

    // Get the error string from the wizards configurations
    mLabel->setPlainText(
        mWizard->configuration(
            WlanWizardHelper::ConfGenericErrorString).toString());

    OstTraceFunctionExit0( WLANWIZARDPAGEGENERICERROR_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
   See WlanWizardPage::showPage()
   
   @return false. Next button is dimmed when the page is displayed.
 */
bool WlanWizardPageGenericError::showPage()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEGENERICERROR_SHOWPAGE_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGEGENERICERROR_SHOWPAGE_EXIT );
    return false;
}

/*!
    See WlanWizardPage::previousTriggered()
   
   @return steps backwards
 */
int WlanWizardPageGenericError::previousTriggered()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEGENERICERROR_PREVIOUSTRIGGERED_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGEGENERICERROR_PREVIOUSTRIGGERED_EXIT );
    return mWizard->configuration(
        WlanWizardHelper::ConfGenericErrorPageStepsBackwards).toInt();
}
