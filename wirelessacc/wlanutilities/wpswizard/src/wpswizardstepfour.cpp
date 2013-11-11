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
 *   WPS Wizard Page: Step 4 progress bar display
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLineEdit>
#include <HbLabel>
#include <HbProgressBar>
#include <HbMessageBox>
#include <HbMainWindow>
#include <platform\wlanerrorcodes.h>

// User includes
#include "wpswizardstepfour.h"
#include "wpswizardstepfour_p.h"
#include "wpswizard_p.h"

// Trace includes


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardstepfourTraces.h"
#endif

// External function prototypes

// Local constants


/*!
   \class WpsPageStepFour
   \brief Implementation of wps wizard page for step four.
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor for WPS page four
   
   @param [in] parent Pointer to the WPS wizard private implementation 
 */
WpsPageStepFour::WpsPageStepFour(WpsWizardPrivate* parent) :
    WpsWizardPage(parent), 
    mWidget(NULL), 
    mWpsErrorCode(0), 
    mCountCredentials(0), 
    mLoader(NULL),
    d_ptr(NULL)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_WPSPAGESTEPFOUR_ENTRY, this);
    d_ptr.reset(new WpsPageStepFourPrivate(this));
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_WPSPAGESTEPFOUR_EXIT, this);
}

/*!
   Destructor
 */
WpsPageStepFour::~WpsPageStepFour()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_WPSPAGESTEPFOUR_DESTRUCTOR_ENTRY, this);   
    delete mLoader;
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_WPSPAGESTEPFOUR_DESTRUCTOR_EXIT, this);
}

/*!
   Loads the page with all the widgets
   
   @return HbWidget* Returns the view widget
 */
HbWidget* WpsPageStepFour::initializePage()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_INITIALIZEPAGE_ENTRY, this);

    if (!mWidget) {    
        bool ok;

        mLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        mLoader->load(":/docml/occ_wps_04.docml", &ok);
        Q_ASSERT(ok);
        
        // Initialize orientation
        loadDocmlSection(mWizard->mainWindow()->orientation());

        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget("occ_wps_04"));
        Q_ASSERT(mWidget);

        bool connectOk = connect(
            mWizard->mainWindow(), 
            SIGNAL(orientationChanged(Qt::Orientation)),
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(connectOk);
    }

    OstTraceFunctionExit1(WPSPAGESTEPFOUR_INITIALIZEPAGE_EXIT, this);

    return mWidget;
}

/*!
   In case the wizard page needs timer protection to the page use this
   method to start control operation of the page. This method will be called
   after requiresStartOperation() if true is returned with different call
   stack.
   
   See requiresStartOperation(), WlanWizardHelper::nextPage().
 */

void WpsPageStepFour::startOperation()
{
    startWpsRunner();
}

/*!
   Funtion to determine the next page to be displayed in the wizard process
   
   @param [out] removeFromStack bool variable indicating whether the current 
   page should be removed from the stack
   
   @return int Page Id of the next page to be displayed.
 */
int WpsPageStepFour::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_NEXTID_ENTRY, this);

    int id = WpsWizardPage::PageNone;
    removeFromStack = false;
    
    // Error has occured go to error page
    if(mWpsErrorCode != 0) {
        return WlanWizardPage::PageGenericError;
    }
    
    if(mCountCredentials == 0) {
        id = WlanWizardPage::PageProcessSettings;
    } else if (mCountCredentials >= 2) {
        id = WpsWizardPage::PageWpsWizardStep5;
    } else {
        // only one response received. So go to the summary page
        mWizard->storeSettings(0);
        id = WlanWizardPage::PageProcessSettings;
    } 
    
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_NEXTID_EXIT, this);
    return id;
}

/*!
   If the wizard page requires timer protection and asyncronous time
   lasting operation, protect parameters should return true. This is used
   in pages e.g. where progressbar is shown. The whole point is to 
   initialize the UI before starting the operation, so that the progress bar
   is painted before the operation takes place.
   
   In practise this means that if true is returned in protect.
   - WlanWizardPage::startOperation() is called to start the page operation.
   - 1.5sec timer is used to protect the page
   - wizardPage MUST call WlanWizardHelper::nextPage() to trigger entry to
     next wizard page.
   
   @return true if protection is need. see description of the method.
 */

bool WpsPageStepFour::requiresStartOperation()
{
    return true;
}

/*!
   Determines the Number of steps to move backwards when 'Prev' Button
   is clicked
   
   @return int Number of pages to move backwards
 */
int WpsPageStepFour::previousTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_PREVIOUSTRIGGERED_ENTRY, this);
    d_ptr->Cancel();
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_PREVIOUSTRIGGERED_EXIT, this);
    return (PageWpsWizardStep4 - PageWpsWizardStep2) - 1;
}

/*!
   Determines whether the Next action button should be enabled or not
   
   @return bool Indicating whether next button is enabled or not.
 */
bool WpsPageStepFour::showPage()
{
    mWpsErrorCode = 0;
    
    return false;
}

/*!
   Handles the error case. Based on the error code decides
   whether to retry or stop the wizard
   
   @param [in] aErrorCode integer variable indicating the error codes
 */
void WpsPageStepFour::handleError(int aErrorCode)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_HANDLEERROR_ENTRY, this);
    QString str;
    OstTrace1( TRACE_ERROR, WPSPAGESTEPFOUR_HANDLEERROR, "WpsPageStepFour::handleError;aErrorCode=%d", aErrorCode );
    mWpsErrorCode = aErrorCode;
    switch (aErrorCode) {
        case KErrWlanProtectedSetupOOBInterfaceReadError:
        case KErrWlanProtectedSetupDecryptionCRCFailure:
        case KErrWlanProtectedSetup2_4ChannelNotSupported:
        case KErrWlanProtectedSetup5_0ChannelNotSupported:
        case KErrWlanProtectedSetupNetworkAuthFailure:
        case KErrWlanProtectedSetupNoDHCPResponse:
        case KErrWlanProtectedSetupFailedDHCPConfig:
        case KErrWlanProtectedSetupIPAddressConflict:
        case KErrWlanProtectedSetupCouldNotConnectToRegistrar:
        case KErrWlanProtectedSetupMultiplePBCSessionsDetected:
        case KErrWlanProtectedSetupRogueActivitySuspected:
        case KErrWlanProtectedSetupDeviceBusy:
        case KErrWlanProtectedSetupSetupLocked:
        case KErrWlanProtectedSetupMessageTimeout:
        case KErrWlanProtectedSetupRegistrationSessionTimeout:
            str = QString(hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
            break;

        case KErrWlanProtectedSetupDevicePasswordAuthFailure:
        case KErrWlanProtectedSetupPINMethodNotSupported:
        case KErrWlanProtectedSetupPBMethodNotSupported:
        default:
            str = QString(hbTrId("txt_occ_dialog_configuration_failed_authenticatio"));
            break;

        } 
    
    mValid = false;
    mWizard->setConfiguration(WlanWizardHelper::ConfGenericErrorString, str);
    mWizard->setConfiguration(
            WlanWizardHelper::ConfGenericErrorPageStepsBackwards,
            (PageWpsWizardStep4 - PageWpsWizardStep2));
    mWizard->nextPage();
  
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_HANDLEERROR_EXIT, this) ;
}

/*!
   Provides the functionality to initiate the WPS functionality
 */
void WpsPageStepFour::startWpsRunner()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOUR_STARTWPSRUNNER_ENTRY, this);
    mValid  = false;
    d_ptr->StartSetup(mWizard->getSsid(), mWizard->getPin());
    OstTraceFunctionExit1(WPSPAGESTEPFOUR_STARTWPSRUNNER_EXIT, this);
}

/*!
   Stores the credentials set by the user, through the WPs Wizard
   
   @param [in] credentials List of Credentials entered by user.
   
   @param credentialscount Number Of Credentials in the list 
 */
void WpsPageStepFour::setCredentials(
    QList<TWlanProtectedSetupCredentialAttribute>& credentials,
    int credentialscount)
{
    OstTraceFunctionEntry1( WPSPAGESTEPFOUR_SETCREDENTIALS_ENTRY, this );

    mCountCredentials = credentialscount;
    mWizard->storeSettingsArray(credentials);
    mWizard->nextPage();
    // enable the next button

    OstTraceFunctionExit1( WPSPAGESTEPFOUR_SETCREDENTIALS_EXIT, this );
}

/*!
   CallBack when the cancel button is clicked
 */
void WpsPageStepFour::cancelTriggered()
{
    OstTraceFunctionEntry1( WPSPAGESTEPFOUR_CANCELTRIGGERED_ENTRY, this );
    d_ptr->Cancel();
    OstTraceFunctionExit1( WPSPAGESTEPFOUR_CANCELTRIGGERED_EXIT, this );
}

/*!
   Loads docml at initialization phase and when HbMainWindow sends 
   orientation() signal.
   
   @param orientation[in] orientation to be loaded.
 */
void WpsPageStepFour::loadDocmlSection(Qt::Orientation orientation)
{
    bool ok = false;
    
    // Load the orientation specific section
    if (orientation == Qt::Horizontal) {
    mLoader->load(":/docml/occ_wps_04.docml", "landscape", &ok);
        Q_ASSERT(ok);
    } 
    else {
        Q_ASSERT(orientation == Qt::Vertical);
        mLoader->load(":/docml/occ_wps_04.docml", "portrait", &ok);
        Q_ASSERT(ok);
    }
}

