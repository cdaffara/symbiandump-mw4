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
 *   WPS Wizard Page: Step 3 Push button number
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLineEdit>
#include <HbLabel>
#include <HbMainwindow>

// User includes
#include "wpswizardstepthreebutton.h"
#include "wpswizard_p.h"

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardstepthreebuttonTraces.h"
#endif

// External function prototypes

// Local constants

/*!
   \class WpsPageStepThreeButton
   \brief Implementation of wps wizard page for step three button press mode.
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for WPS page three button
   
   @param [in] parent Pointer to the WPS wizard private implementation 
 */
WpsPageStepThreeButton::WpsPageStepThreeButton(WpsWizardPrivate* parent) :
    WpsWizardPage(parent), 
    mWidget(NULL), 
    mHeading(NULL),
    mLoader(NULL)
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREEBUTTON_WPSPAGESTEPTHREEBUTTON_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPTHREEBUTTON_WPSPAGESTEPTHREEBUTTON_EXIT, this);

}

/*!
   Destructor
 */
WpsPageStepThreeButton::~WpsPageStepThreeButton()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREEBUTTON_WPSPAGESTEPTHREEBUTTON_DESTRUCTOR_ENTRY, this);
    delete mLoader;
    OstTraceFunctionExit1(WPSPAGESTEPTHREEBUTTON_WPSPAGESTEPTHREEBUTTON_DESTRUCTOR_EXIT, this);
}

/*!
   Loads the page with all the widgets
   
   @return HbWidget* Returns the view widget
 */
HbWidget* WpsPageStepThreeButton::initializePage()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREEBUTTON_INITIALIZEPAGE_ENTRY, this);

    if (!mWidget) {
        bool ok;
        mLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        mLoader->load(":/docml/occ_wps_02_03.docml", &ok);
        Q_ASSERT(ok);
        
        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget("occ_wps_P2"));
        Q_ASSERT(mWidget);

        mHeading = qobject_cast<HbLabel*> (mLoader->findWidget("label_heading"));
        Q_ASSERT(mHeading);
        
        mHeading->setPlainText(hbTrId("txt_occ_dialog_first_press_button_on_the_wireless"));
    } 
    
    OstTraceFunctionExit1(WPSPAGESTEPTHREEBUTTON_INITIALIZEPAGE_EXIT, this);
    return mWidget;
}

/*!
   Funtion to determine the next page to be displayed in the wizard process
   
   @param [out] removeFromStack bool indicating whether the current page should be 
   removed from the stack
   
   @return int Page Id of the next page to be displayed.
 */
int WpsPageStepThreeButton::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREEBUTTON_NEXTID_ENTRY, this);
    removeFromStack = false;
    OstTraceFunctionExit1(WPSPAGESTEPTHREEBUTTON_NEXTID_EXIT, this);

    return WpsWizardPage::PageWpsWizardStep4;
}

/*!
   Determines the Number of steps to move backwards when 'Prev' Button
   is clicked
   
   @return int Number of pages to move backwards
 */
int WpsPageStepThreeButton::previousTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREEBUTTON_PREVIOUSTRIGGERED_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPTHREEBUTTON_PREVIOUSTRIGGERED_EXIT, this);
    return (PageWpsWizardStep3_Button - PageWpsWizardStep2);
}



