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
 *   WLAN Wizard: Interface for wizard pages (wlan internal).
 *
 */

// System includes
#include <HbWidget>
#include <HbDocumentLoader>
#include <HbMainWindow>

// User includes
#include "wlanwizard_p.h"
#include "wlanwizardpageinternal.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpageinternalTraces.h"
#endif


/*!
   \class WlanWizardPageInternal
   \brief Interface for internal wlan wizard pages. Contains pointer to private
   implementation of wlan wizard.
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor
   
   @param [in] wizard pointer to private implementation of wizard
 */
WlanWizardPageInternal::WlanWizardPageInternal(WlanWizardPrivate *wizard) :
    QObject(wizard), 
    mWizard(wizard)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEINTERNAL_WLANWIZARDPAGEINTERNAL_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGEINTERNAL_WLANWIZARDPAGEINTERNAL_EXIT );
}

/*!
   Destructor.
 */
WlanWizardPageInternal::~WlanWizardPageInternal()
{
    OstTraceFunctionEntry0( DUP1_WLANWIZARDPAGEINTERNAL_WLANWIZARDPAGEINTERNAL_ENTRY );
    OstTraceFunctionExit0( DUP1_WLANWIZARDPAGEINTERNAL_WLANWIZARDPAGEINTERNAL_EXIT );
}


/*!
   Loads requested section from given docml file.
   
   @param [in] loader Document loader for docml
   @param [in] orientation Orientation to be loaded
   @param [in] filename the name of docml filename
   @param [in] portraitSection the name of section to be loaded in portrait mode
   @param [in] landscapeSection the name of section to be loaded in landscape mode
 */
void WlanWizardPageInternal::loadDocmlSection(
    HbDocumentLoader *loader,
    Qt::Orientation orientation,
    const QString &filename,
    const QString &portraitSection,
    const QString &landscapeSection) const
{
    OstTraceFunctionEntry0( WLANWIZARDPAGEINTERNAL_LOADDOCMLSECTION_ENTRY );
    bool ok;
    
    // Then load the orientation specific section
    if (orientation == Qt::Horizontal) {
        loader->load(filename, landscapeSection, &ok);
        Q_ASSERT(ok);
    } else {
        Q_ASSERT(orientation == Qt::Vertical);
        loader->load(filename, portraitSection, &ok);
        Q_ASSERT(ok);
    }
    OstTraceFunctionExit0( WLANWIZARDPAGEINTERNAL_LOADDOCMLSECTION_EXIT );
}
