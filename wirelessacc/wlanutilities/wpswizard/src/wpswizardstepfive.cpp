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
 *   WPS Wizard Page: Step 5
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLineEdit>
#include <HbLabel>
#include <HbMainWindow>

// User includes
#include "wpswizardstepfive.h"
#include "wpswizard_p.h"

// Trace Includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardstepfiveTraces.h"
#endif

// External function prototypes

// Local constants


/*!
   \class WpsPageStepFive
   \brief Implementation of wps wizard page for step five. 
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor for WPS page five
   
   @param [in] parent WpsWizardPrivate* Pointer to the WPS wizard private 
          implementation 
 */
WpsPageStepFive::WpsPageStepFive(WpsWizardPrivate* parent) :
    WpsWizardPage(parent), 
    mWidget(NULL), 
    mRadio(NULL), 
    mHeading(NULL),
    mValid(false),
    mLoader(NULL)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_WPSPAGESTEPFIVE_ENTRY, this);
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_WPSPAGESTEPFIVE_EXIT, this);
}

/*!
   Destructor
 */
WpsPageStepFive::~WpsPageStepFive()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_WPSPAGESTEPFIVE_DESTRUCTOR_ENTRY, this); 
    delete mLoader;
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_WPSPAGESTEPFIVE_DESTRUCTOR_EXIT, this);
}

/*!
   Loads the page with all the widgets
   
   @return HbWidget* Returns the view widget
 */
HbWidget* WpsPageStepFive::initializePage()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_INITIALIZEPAGE_ENTRY, this);

    if (!mWidget) {
        bool ok;
        
        mLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        mLoader->load(":/docml/occ_wps_01_05.docml", &ok);
        Q_ASSERT(ok);
        
        // Initialize orientation
        loadDocmlSection(mWizard->mainWindow()->orientation());

        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget("occ_wps_P1"));
        Q_ASSERT(mWidget);

        mHeading = qobject_cast<HbLabel*> (mLoader->findWidget("label"));
        Q_ASSERT(mHeading);
        
        mHeading->setPlainText(hbTrId("txt_occ_dialog_settings_received_for_multiple_wlan"));

        mRadio = qobject_cast<HbRadioButtonList*> (mLoader->findWidget(
                "radioButtonList"));
        Q_ASSERT(mRadio);

        QList<TWlanProtectedSetupCredentialAttribute> arr = mWizard->getSettingsArray();

        QStringList ssidList;
        for (int count = 0; count < arr.count(); count++)
            {
            TWlanProtectedSetupCredentialAttribute attr = arr[count];
            QString ssid = QString::fromUtf8((const char*) attr.iSsid.Ptr(),
                    attr.iSsid.Length());
            ssidList.append(ssid);
            }
        mRadio->setItems(ssidList);

        bool connectOk = connect(
            mRadio, 
            SIGNAL(itemSelected(int)), 
            this,
            SLOT(itemSelected(int)));
        Q_ASSERT(connectOk);
        
        connectOk = connect(
            mWizard->mainWindow(), 
            SIGNAL(orientationChanged(Qt::Orientation)),
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
                   
        Q_ASSERT(connectOk);
    }
    
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_INITIALIZEPAGE_EXIT, this);
    return mWidget;
}

/*!
   Determines the Number of steps to move backwards when 'Prev' Button
   is clicked
   
   @return int Number of pages to move backwards
 */
int WpsPageStepFive::previousTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_PREVIOUSTRIGGERED_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_PREVIOUSTRIGGERED_EXIT, this);
    
    return (PageWpsWizardStep5 - PageWpsWizardStep2) - 1;
}
/*!
   CallBack when the cancel button is clicked
 */
void WpsPageStepFive::cancelTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_CANCELTRIGGERED_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_CANCELTRIGGERED_EXIT, this);
}

/*!
   Funtion to determine the next page to be displayed in the wizard process
   
   @param [out] removeFromStack bool RemoveFromStack indicating whether the current 
          page should be removed from the stack
   
   @return int Page Id of the next page to be displayed.
 */
int WpsPageStepFive::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_NEXTID_ENTRY, this);
    removeFromStack = false;
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_NEXTID_EXIT, this);
    return WlanWizardPage::PageProcessSettings;
}

/*!
   Determines whether the Next action button should be enabled or not
   
   @return bool Indicating whether next button is enabled or not.
 */
bool WpsPageStepFive::showPage()
{
    return mValid;
}


/*!
   Call back when an item is selected from the list on the UI
   
   @param [in] index Indicating the relative position in the list
 */
void WpsPageStepFive::itemSelected(int index)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFIVE_ITEMSELECTED_ENTRY, this);
    mValid = true;
    mWizard->storeSettings(index);
    mWizard->enableNextButton(mValid);
    OstTraceFunctionExit1(WPSPAGESTEPFIVE_ITEMSELECTED_EXIT, this);
}


/*!
   Loads docml at initialization phase and when HbMainWindow sends orientation()
   signal.
   
   @param [in] orientation orientation to be loaded.
 */
void WpsPageStepFive::loadDocmlSection(Qt::Orientation orientation)
{
    bool ok = false;
    
    // Load the orientation specific section
    if (orientation == Qt::Horizontal) {
    mLoader->load(":/docml/occ_wps_01_05.docml", "landscape", &ok);
        Q_ASSERT(ok);
    } 
    else {
        Q_ASSERT(orientation == Qt::Vertical);
        mLoader->load(":/docml/occ_wps_01_05.docml", "portrait", &ok);
        Q_ASSERT(ok);
    }
}

