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
 *   WPS Wizard Page: Step 2
 *
 */

// System includes
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLineEdit>
#include <HbLabel>
#include <HbMainWindow>

// User includes
#include "wpswizardsteptwo.h"
#include "wpswizard_p.h"

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardsteptwoTraces.h"
#endif

// External function prototypes

// Local constants

/*!
   \class WpsPageStepTwo
   \brief Implementation of wps wizard page for step two. 
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for WPS page two
   
   @param [in] parent Pointer to the WPS wizard private implementation 
 */
WpsPageStepTwo::WpsPageStepTwo(WpsWizardPrivate* parent) :
    WpsWizardPage(parent), 
    mWidget(NULL), 
    mRadio(NULL), 
    mHeading(NULL),
    mValid(false), 
    mItemSelected(0),
    mLoader(NULL)
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_WPSPAGESTEPTWO_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPTWO_WPSPAGESTEPTWO_EXIT, this);

}

/*!
   Destructor
 */
WpsPageStepTwo::~WpsPageStepTwo()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_WPSPAGESTEPTWO_DESTRUCTOR_ENTRY, this);
    delete mLoader;
    OstTraceFunctionExit1(WPSPAGESTEPTWO_WPSPAGESTEPTWO_DESTRUCTOR_EXIT, this);
}

/*!
   Loads the page with all the widgets
   
   @return HbWidget* Returns the view widget
 */
HbWidget* WpsPageStepTwo::initializePage()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_INITIALIZEPAGE_ENTRY, this);

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
        mHeading->setPlainText(hbTrId("txt_occ_dialog_selected_network_supports_wifi_pro"));

        mRadio = qobject_cast<HbRadioButtonList*> (mLoader->findWidget("radioButtonList"));
        Q_ASSERT(mRadio); 
        
        //Set Options
        QStringList list;
        list << hbTrId("txt_occ_list_use_pushbutton")
             << hbTrId("txt_occ_list_use_pin_code")
             << hbTrId("txt_occ_list_configure_manually");
        mRadio->setItems(list);

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
    
    OstTraceFunctionExit1(WPSPAGESTEPTWO_INITIALIZEPAGE_EXIT, this);
    return mWidget;
}

/*!
   Funtion to determine the next page to be displayed in the wizard process
   
   @param [out] removeFromStack bool indicating whether the current page should be 
   removed from the stack
   
   @return int Page Id of the next page to be displayed.
 */
int WpsPageStepTwo::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_NEXTID_ENTRY, this);
    int id = WpsWizardPage::PageWpsWizardStep3_Button;
    removeFromStack = false;
    
    if (mItemSelected == WpsPushButtonMode) {
        // Reset Pin code, pin 0 means push-button mode
        mWizard->setPin(0);
        id = WpsWizardPage::PageWpsWizardStep3_Button;
        
    } else if (mItemSelected == WpsPinCodeMode) {
        id = WpsWizardPage::PageWpsWizardStep3_Number;
        
    } else {
        id = mWizard->nextPageId(false);
    }
    
    OstTraceFunctionExit1(WPSPAGESTEPTWO_NEXTID_EXIT, this);
    return id;
}

/*!
   Determines the Number of steps to move backwards when 'Prev' Button
   is clicked
   
   @return int Number of pages to move backwards
 */
int WpsPageStepTwo::previousTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_PREVIOUSTRIGGERED_ENTRY, this);
    OstTraceFunctionExit1(WPSPAGESTEPTWO_PREVIOUSTRIGGERED_EXIT, this);
    return 1;
}

/*!
   CallBack when the cancel button is clicked
 */
void WpsPageStepTwo::cancelTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_CANCELTRIGGERED_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPTWO_CANCELTRIGGERED_EXIT, this);
}

/*!
   Determines whether the Next action button should be enabled or not
   
   @return bool Indicating whether next button is enabled or not.
 */
bool WpsPageStepTwo::showPage()
{
    return mValid;
}

/*!
   Call back when an item is selected from the list on the UI
   
   @param [in] index Indcating the relative position in the list
 */
void WpsPageStepTwo::itemSelected(int index)
{
    OstTraceFunctionEntry1(WPSPAGESTEPTWO_ITEMSELECTED_ENTRY, this);
    mValid = true;
    mWizard->enableNextButton(mValid);
    mItemSelected = index;   
    OstTraceFunctionExit1(WPSPAGESTEPTWO_ITEMSELECTED_EXIT, this);
}

/*!
   Loads docml at initialization phase and when HbMainWindow sends 
   orientation() signal.
   
   @param [in] orientation orientation to be loaded.
 */
void WpsPageStepTwo::loadDocmlSection(Qt::Orientation orientation)
{
    bool ok = false;
    
    // Load the orientation specific section
    if (orientation == Qt::Horizontal) {
        mLoader->load(":/docml/occ_wps_01_05.docml", "landscape", &ok);
        Q_ASSERT(ok);
    }  else {
        Q_ASSERT(orientation == Qt::Vertical);
        mLoader->load(":/docml/occ_wps_01_05.docml", "portrait", &ok);
        Q_ASSERT(ok);
    }
}

