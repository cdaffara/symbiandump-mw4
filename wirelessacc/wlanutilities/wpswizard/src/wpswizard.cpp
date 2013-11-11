/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: 
 *   WPS Wizard: API implementation.
 *
 */

// System includes
#include <QApplication>

// User includes
// WLAN Wizard
#include "wlanwizardhelper.h"

// WPS Wizard 
#include "wpswizard.h"
#include "wpswizard_p.h"

// Wizard Pages
#include "wpswizardsteptwo.h"
#include "wpswizardstepthreebutton.h"
#include "wpswizardstepthreenumber.h"
#include "wpswizardstepfour.h"
#include "wpswizardstepfive.h"

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardTraces.h"
#endif

//Forward Declarations
class WlanWizardHelper;

// External function prototypes

//Local Constants

/*!
    \class WpsWizard
    \brief Provides the WPS wizard implementation 
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Creates the WpsWizard object
   
   @param [in] wizardHelper WlanWizardHelper* Helper class to from the framework
 */

WpsWizard::WpsWizard(WlanWizardHelper *wizardHelper) :
    d_ptr(NULL)
{
    OstTraceFunctionEntry1(WPSWIZARD_WPSWIZARD_ENTRY, this); 
    d_ptr.reset(new WpsWizardPrivate(this, wizardHelper));
    OstTraceFunctionExit1(WPSWIZARD_WPSWIZARD_EXIT, this);
}

/*!
   Destructor
   
 */
WpsWizard::~WpsWizard()
{
    OstTraceFunctionEntry1(WPSWIZARD_WPSWIZARD_DESTRUCTOR_ENTRY, this);
    OstTraceFunctionExit1(WPSWIZARD_WPSWIZARD_DESTRUCTOR_EXIT, this);
}

