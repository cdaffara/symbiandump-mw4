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
 *   EAP Wizard: API implementation.
 */

// System includes

// User includes
#include "eapwizard.h"
#include "eapwizard_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardTraces.h"
#endif

/*!
   \class EapWizard
   \brief EAP plugin for WLAN wizard.
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor of EAP Wizard.
   
   @param [in] wizardHelper pointer to the helpper instance.
 */
EapWizard::EapWizard(WlanWizardHelper *wizardHelper) : d_ptr(NULL)
{
    OstTraceFunctionEntry0(EAPWIZARD_EAPWIZARD_ENTRY);
    
    d_ptr.reset( new EapWizardPrivate(wizardHelper));
    
    OstTraceFunctionExit0(EAPWIZARD_EAPWIZARD_EXIT);
}

/*!
   Destructor.
 */
EapWizard::~EapWizard()
{
    OstTraceFunctionEntry0(EAPWIZARD_DEAPWIZARD_ENTRY);
    OstTraceFunctionExit0(EAPWIZARD_DEAPWIZARD_EXIT);
}

/*!
   See WlanWizardPlugin::summary().
   
   This method is used to query eap plugin specific summary items. 
  
   Both item and value are localized strings.
   
   @param [in]     sum   Summary id to be queried
   @param [in,out] item  The item string is returned here 
   @param [in,out] value The value of item is returned here.
   
   @return true if summary item is found, false otherwise.
 */
bool EapWizard::summary(WlanWizardPlugin::Summary sum, QString &item, QString &value)
{
    OstTraceFunctionEntry0(EAPWIZARD_SUMMARY_ENTRY);
    bool ret = d_ptr->summary(sum, item, value); 
    OstTraceFunctionExit0(EAPWIZARD_SUMMARY_EXIT);
    return ret;
}

/*!
   See WlanWizardPlugin::storeSettings().
   
   Stores EAP specific settings.
 */
bool EapWizard::storeSettings()
{
    OstTraceFunctionEntry0(EAPWIZARD_STORESETTINGS_ENTRY);
    
    bool ret = d_ptr->storeSettings();
    
    OstTraceFunctionExit0(EAPWIZARD_STORESETTINGS_EXIT);
    
    return ret;
}


/*!
   See WlanWizardPlugin::errorString().
   
   Gets EAP Wizard specific error code. In case mapping cannot be done an empty
   string is returned.
 */
QString EapWizard::errorString(int errorCode)
{
    OstTraceFunctionEntry0(EAPWIZARD_ERRORSTRING_ENTRY);
    
    QString string = d_ptr->errorString(errorCode);
    
    OstTraceFunctionExit0(EAPWIZARD_ERRORSTRING_EXIT);
    return string;
}
