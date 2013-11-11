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
 *   EAP Wizard: API for wizard pages.
 *
 */

// System includes
#include <HbWidget>
#include <HbDocumentLoader>
#include <HbMainWindow>

// User includes
#include "eapwizardpage.h"
#include "eapwizard_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardpageTraces.h"
#endif


/*!
   \class EapWizardPage
   \brief Defines the interface for EAP specific wizard pages.
 */

/*!
   \fn HbWidget* EapWizardPage::initializePage()
   See WlanWizardPage.
 */

/*!
   \fn int EapWizardPage::nextId() const
   See WlanWizardPage.
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.

   @param [in] parent Pointer to EAP Wizard private implementation.
 */
EapWizardPage::EapWizardPage(EapWizardPrivate* parent) :
    QObject(parent), 
    mWizard(parent)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGE_EAPWIZARDPAGE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGE_EAPWIZARDPAGE_EXIT );
}

/*!
   Destructor.
 */
EapWizardPage::~EapWizardPage()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPAGE_EAPWIZARDPAGE_ENTRY );
    mWizard = NULL;
    OstTraceFunctionExit0( DUP1_EAPWIZARDPAGE_EAPWIZARDPAGE_EXIT );
}

/*!
   See WlanWizardPage. 
 */
int EapWizardPage::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGE_NEXTID_ENTRY );
    removeFromStack = false;
    OstTraceFunctionExit0( EAPWIZARDPAGE_NEXTID_EXIT );
    return nextId();
}

/*!
   Loads requested section from given docml file.

   @param [in] loader Document loader for docml
   @param [in] orientation Orientation to be loaded
   @param [in] filename the name of docml filename
   @param [in] portraitSection the name of section to be loaded in portrait mode
   @param [in] landscapeSection the name of section to be loaded in landscape mode
 */
void EapWizardPage::loadDocmlSection(
    HbDocumentLoader *loader,
    Qt::Orientation orientation,
    const QString &filename,
    const QString &portraitSection,
    const QString &landscapeSection) const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGE_LOADDOCMLSECTION_ENTRY );
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
    OstTraceFunctionExit0( EAPWIZARDPAGE_LOADDOCMLSECTION_EXIT );
}
