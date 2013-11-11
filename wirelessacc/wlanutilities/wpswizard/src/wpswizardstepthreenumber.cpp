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
 *   WPS Wizard Page: Step 3 Number enter mode.
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbRadiobuttonList>
#include <HbLineEdit>
#include <HbLabel>
#include <QTextStream>
#include <QTime>
#include <HbMainWindow>
#include <HbParameterLengthLimiter>

// User includes
#include "wpswizardstepthreenumber.h"
#include "wpswizard_p.h"

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardstepthreenumberTraces.h"
#endif

// External function prototypes

// Local constants
const static int KMaxPINLength = 8;

/*!
   \class WpsPageStepThreeNumber
   \brief Implementation of wps wizard page for step three PIN Entry mode. 
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for WPS page three number entry mode
   
   @param [in] parent Pointer to the WPS wizard private implementation 
 */
WpsPageStepThreeNumber::WpsPageStepThreeNumber(WpsWizardPrivate* parent) :
    WpsWizardPage(parent), 
    mWidget(NULL), 
    mHeading(NULL),
    mLoader(NULL)
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_WPSPAGESTEPTHREENUMBER_ENTRY, this); 
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_WPSPAGESTEPTHREENUMBER_EXIT, this);

}

/*!
   Destructor
 */
WpsPageStepThreeNumber::~WpsPageStepThreeNumber()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_WPSPAGESTEPTHREENUMBER_DESTRUCTOR_ENTRY, this);
    delete mLoader;
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_WPSPAGESTEPTHREENUMBER_DESTRUCTOR_EXIT, this);
}

/*!
   Loads the page with all the widgets
   
   @return HbWidget* Returns the view widget
 */
HbWidget* WpsPageStepThreeNumber::initializePage()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_INITIALIZEPAGE_ENTRY, this);

    if (!mWidget) {
        bool ok;
        mLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        mLoader->load(":/docml/occ_wps_02_03.docml", &ok);
        Q_ASSERT(ok);
        
        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget("occ_wps_P2"));
        Q_ASSERT(mWidget);

        mHeading= qobject_cast<HbLabel*> (mLoader->findWidget("label_heading"));
        Q_ASSERT(mHeading);
    } 
    
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_INITIALIZEPAGE_EXIT, this);

    return mWidget;
}

/*!
   Funtion to determine the next page to be displayed in the wizard process
   
   @param [out] removeFromStack bool indicating whether the current page should be 
   removed from the stack
   
   @return int Page Id of the next page to be displayed.
 */
int WpsPageStepThreeNumber::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_NEXTID_ENTRY, this);
    removeFromStack = false;
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_NEXTID_EXIT, this);

    return WpsWizardPage::PageWpsWizardStep4;
}

/*!
   Determines the Number of steps to move backwards when 'Prev' Button
   is clicked
   
   @return int Number of pages to move backwards
 */
int WpsPageStepThreeNumber::previousTriggered()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_PREVIOUSTRIGGERED_ENTRY, this);
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_PREVIOUSTRIGGERED_EXIT, this);
    return (PageWpsWizardStep3_Number - PageWpsWizardStep2) - 1;
}

/*!
   Determines whether the Next action button should be enabled or not
   
   @return bool Indicating whether next button is enabled or not.
 */
bool WpsPageStepThreeNumber::showPage()
{
    int randomNumber = computeRandNumber();
    mHeading->setPlainText(
        HbParameterLengthLimiter(
            "txt_occ_dialog_enter_1_on_the_wireless_station_t").arg(
                QString::number(randomNumber)));
    mWizard->setPin(randomNumber);
    return true;
}

/*!
   Computes the Random number based on current system time
   
   @return int Returns the generated random number
 */
int WpsPageStepThreeNumber::computeRandNumber()
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_COMPUTERANDNUMBER_ENTRY, this);

    int pin = 0;
    QTime time(0, 0);
    int seed = time.secsTo(QTime::currentTime());
    
    do {
        qsrand(seed);
        pin = qrand();
    } while (pin < (10 ^ (KMaxPINLength - 2)) || (((pin / 1000000) % 10)) == 0);
    
    //last digit is checksum, so we need 7 digits
    //and the first shouldn't be 0
    pin = pin % 10000000;
    int checkSum = computeCheckSum(pin);
    pin *= 10;
    pin += checkSum;
    
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_COMPUTERANDNUMBER_EXIT, this);
    return pin;
}

/*!
   Computes the checksum for a given pin
   
   @return Returns the generated checksum
 */
int WpsPageStepThreeNumber::computeCheckSum(int aPin)
{
    OstTraceFunctionEntry1(WPSPAGESTEPTHREENUMBER_COMPUTECHECKSUM_ENTRY, this);

    int accum = 0;
    aPin *= 10;
    accum += 3 * ((aPin / 10000000) % 10);
    accum += 1 * ((aPin / 1000000) % 10);
    accum += 3 * ((aPin / 100000) % 10);
    accum += 1 * ((aPin / 10000) % 10);
    accum += 3 * ((aPin / 1000) % 10);
    accum += 1 * ((aPin / 100) % 10);
    accum += 3 * ((aPin / 10) % 10);
    int digit = (accum % 10);
    OstTraceFunctionExit1(WPSPAGESTEPTHREENUMBER_COMPUTECHECKSUM_EXIT, this);
    return (10 - digit) % 10;
}
