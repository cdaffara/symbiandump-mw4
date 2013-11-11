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
 *   WPS Wizard: API for wizard pages.
 *
 */

// System includes
#include <HbWidget>

// User includes
#include "wpswizard_p.h"
#include "wpswizardpage.h"

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardpageTraces.h"
#endif

// External function prototypes

// Local constants

/*!
   \class WpsWizardPage
   \brief Implementation of the basic wps wizard page functionality 
 */

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for the WPS page baseclass
   
   @param [in] parent WpsWizardPrivate* pointer to the implementation class
 */

WpsWizardPage::WpsWizardPage(WpsWizardPrivate* parent) :
    QObject(parent),
    mWizard(parent)
{
    OstTraceFunctionEntry1(WPSWIZARDPAGE_WPSWIZARDPAGE_ENTRY, this); 
    OstTraceFunctionExit1(WPSWIZARDPAGE_WPSWIZARDPAGE_EXIT, this);
}

/*!
   Destructor
 */
WpsWizardPage::~WpsWizardPage()
{
    OstTraceFunctionEntry1(WPSWIZARDPAGE_WPSWIZARDPAGE_DESTRUCTOR_ENTRY, this);
    mWizard = NULL;
    OstTraceFunctionExit1(WPSWIZARDPAGE_WPSWIZARDPAGE_DESTRUCTOR_EXIT, this);
}

