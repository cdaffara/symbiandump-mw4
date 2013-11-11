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
 *   EAP Wizard Page: New PAC store password
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbLineEdit>
#include <HbLabel>
#include <eapqtexpandedeaptype.h>
#include <eapqtconfiginterface.h>
#include <eapqtconfig.h>
#include <eapqtvalidator.h>

// User includes
#include "wlanwizardhelper.h"
#include "eapwizardpagenewpacstore.h"
#include "eapwizard_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardpagenewpacstoreTraces.h"
#endif


/*!
   \class EapWizardPageNewPacStore
   \brief Implements EAP wizard page: New PAC store password
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.
   
   @param [in] parent Pointer to EAP Wizard private implementation.
 */
EapWizardPageNewPacStore::EapWizardPageNewPacStore(
    EapWizardPrivate* parent) :
    EapWizardPage(parent), 
    mDocumentLoader(NULL), 
    mValidator(NULL),
    mWidget(NULL), 
    mEditPasswordNew(NULL),
    mEditPasswordConfirm(NULL), 
    mTitlePasswordNew(NULL), 
    mTitlePasswordConfirm(NULL)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_EAPWIZARDPAGENEWPACSTORE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_EAPWIZARDPAGENEWPACSTORE_EXIT );
}

/*!
   Destructor.
 */
EapWizardPageNewPacStore::~EapWizardPageNewPacStore()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPAGENEWPACSTORE_EAPWIZARDPAGENEWPACSTORE_ENTRY );
    OstTraceFunctionExit0( DUP1_EAPWIZARDPAGENEWPACSTORE_EAPWIZARDPAGENEWPACSTORE_EXIT );
}

/*!
   See WlanWizardPage. 
 */
HbWidget* EapWizardPageNewPacStore::initializePage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_INITIALIZEPAGE_ENTRY );
    if (!mWidget) {
        bool ok;
        mDocumentLoader.reset( 
            new HbDocumentLoader(mWizard->wizardHelper()->mainWindow()));
        mDocumentLoader->load(":/docml/occ_eap_wizard_05_07.docml", &ok);
        Q_ASSERT(ok);
        
        loadDocmlSection(mWizard->wizardHelper()->mainWindow()->orientation());
        
        mWidget = qobject_cast<HbWidget*> (mDocumentLoader->findWidget("view"));
        Q_ASSERT(mWidget);

        mTitlePasswordNew = qobject_cast<HbLabel*> (mDocumentLoader->findWidget("setlabel_55"));
        Q_ASSERT(mTitlePasswordNew);

        mTitlePasswordConfirm = qobject_cast<HbLabel*> (mDocumentLoader->findWidget("setlabel_56"));
        Q_ASSERT(mTitlePasswordConfirm);

        mEditPasswordNew = qobject_cast<HbLineEdit*> (mDocumentLoader->findWidget("lineEditUsername"));
        Q_ASSERT(mEditPasswordNew);

        mEditPasswordConfirm = qobject_cast<HbLineEdit*> (mDocumentLoader->findWidget("lineEditPassword"));
        Q_ASSERT(mEditPasswordConfirm);
        
        mValidator.reset( 
            mWizard->eapConfigurationInterface()->validatorPacStore(
                EapQtPacStoreConfig::PacStorePassword));
        Q_ASSERT(mValidator.data());
        
        mTitlePasswordNew->setPlainText(hbTrId("txt_occ_setlabel_new_pac_store_password"));
        mTitlePasswordConfirm->setPlainText(hbTrId("txt_occ_setlabel_confirm_password"));

        // Configure editors properties
        mValidator->updateEditor(mEditPasswordNew);
        mValidator->updateEditor(mEditPasswordConfirm);

        ok = connect(
            mWizard->wizardHelper()->mainWindow(),
            SIGNAL(orientationChanged(Qt::Orientation)), 
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
        
        ok = connect(
            mEditPasswordNew, SIGNAL(textChanged(const QString &)), 
            this, SLOT(textChanged(const QString &)));
        Q_ASSERT(ok);
        
        ok = connect(
            mEditPasswordConfirm, SIGNAL(textChanged(const QString &)), 
            this, SLOT(textChanged(const QString &)));
        Q_ASSERT(ok);
    }
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
   Loads the required orientation of docml.
   
   @param [in] orientation Orientation to be loaded. 
 */
void EapWizardPageNewPacStore::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_LOADDOCMLSECTION_ENTRY );
    EapWizardPage::loadDocmlSection(
        mDocumentLoader.data(),
        orientation,
        ":/docml/occ_eap_wizard_05_07.docml",
        "portrait_section",
        "landscape_section");
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_LOADDOCMLSECTION_EXIT );
}

/*!
   See WlanWizardPage.
   
   Validates the content of the page.
   
   @return true if content is valid.
 */
bool EapWizardPageNewPacStore::showPage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_SHOWPAGE_ENTRY );
    bool valid = false;
    if (mValidator->validate(mEditPasswordNew->text()) == EapQtValidator::StatusOk &&
        mEditPasswordNew->text() == mEditPasswordConfirm->text()) {
        valid = true;
    }
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_SHOWPAGE_EXIT );
    return valid;
}

/*!
   See WlanWizardPage.
   
   @return next wizard page: EapWizardPage::PageUsernamePassword
 */
int EapWizardPageNewPacStore::nextId() const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_NEXTID_ENTRY );
    mWizard->setConfigurations(EapWizardPrivate::PacStorePassword, mEditPasswordNew->text());
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_NEXTID_EXIT );
    return EapWizardPage::PageUsernamePassword;
}

/*!
   Slot for textChanged() signal from HbLineEdit.
   
   @param [in] text NOT USED.
 */
void EapWizardPageNewPacStore::textChanged(const QString &text)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGENEWPACSTORE_TEXTCHANGED_ENTRY );
    Q_UNUSED(text);
    mWizard->wizardHelper()->enableNextButton(showPage());
    OstTraceFunctionExit0( EAPWIZARDPAGENEWPACSTORE_TEXTCHANGED_EXIT );
}
