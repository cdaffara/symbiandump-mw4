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
 *   EAP Wizard Page: User Certificate Selection.
 *
 */

// System includes
#include <QGraphicsItem>
#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLabel>
#include <eapqtconfiginterface.h>
#include <eapqtcertificateinfo.h>

// User includes
#include "wlanwizardhelper.h"
#include "eapwizardpagecertuser.h"
#include "eapwizard_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardpagecertuserTraces.h"
#endif


/*!
   \class EapWizardPageCertUser
   \brief Implements EAP wizard page: User Certificate selection
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.

   @param [in] parent Pointer to EAP Wizard private implementation.
 */

EapWizardPageCertUser::EapWizardPageCertUser(EapWizardPrivate* parent) :
    EapWizardPage(parent), 
    mWidget(NULL), 
    mCertList(NULL),
    mErrorLabel(NULL),
    mDocumentLoader(NULL),
    mValid(false)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_EAPWIZARDPAGECERTUSER_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_EAPWIZARDPAGECERTUSER_EXIT );
}

/*!
   Destructor.
 */
EapWizardPageCertUser::~EapWizardPageCertUser()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPAGECERTUSER_EAPWIZARDPAGECERTUSER_ENTRY );
    OstTraceFunctionExit0( DUP1_EAPWIZARDPAGECERTUSER_EAPWIZARDPAGECERTUSER_EXIT );
}

/*!
   See WlanWizardPage. 
 */
HbWidget* EapWizardPageCertUser::initializePage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_INITIALIZEPAGE_ENTRY );
    if (!mWidget) {
        bool ok;
        mDocumentLoader.reset(new HbDocumentLoader(mWizard->wizardHelper()->mainWindow()));
        mDocumentLoader->load(":/docml/occ_eap_wizard_06.docml", &ok);
        Q_ASSERT(ok);
        loadDocmlSection(mWizard->wizardHelper()->mainWindow()->orientation());

        mWidget = qobject_cast<HbWidget*> (mDocumentLoader->findWidget("occ_eap_wizard_06"));
        Q_ASSERT(mWidget);

        mCertList = qobject_cast<HbRadioButtonList*> (mDocumentLoader->findWidget("list"));
        Q_ASSERT(mCertList);

        mErrorLabel = qobject_cast<HbLabel*> (mDocumentLoader->findWidget("errorLabel"));
        Q_ASSERT(mErrorLabel);

        mErrorLabel->setPlainText(hbTrId("txt_occ_dialog_no_certificates_installed_wizard_c"));

        ok = connect(
            mWizard->wizardHelper()->mainWindow(),
            SIGNAL(orientationChanged(Qt::Orientation)), 
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);

        ok = connect(
            mCertList, SIGNAL(itemSelected(int)), 
            this, SLOT(itemSelected(int)));
        Q_ASSERT(ok);
    }

    EapQtConfigInterface* confIf = mWizard->eapConfigurationInterface();
    // ignore return value
    confIf->updateCertificates();
    mCerts = confIf->userCertificates();

    QStringList list;
    for (int i = 0; i < mCerts.count(); ++i) {
        list << mCerts.at(i).value(EapQtCertificateInfo::CertificateLabel).toString();
    }

    if (list.isEmpty()) {
        mErrorLabel->setVisible(true);
        mCertList->setVisible(false);
        mValid = false;
    } else if (list != mCertList->items()){
        // in case the user cert list has been changed, update UI
        mCertList->setItems(list);
        mErrorLabel->setVisible(false);
        mCertList->setVisible(true);

        if (list.count() == 1) {
            mValid = true;
            mCertList->setSelected(0);
        } else {
            mValid = false;
        }
    }
    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
   Loads the required orientation of docml.

   @param [in] orientation Orientation to be loaded. 
 */
void EapWizardPageCertUser::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_LOADDOCMLSECTION_ENTRY );
    EapWizardPage::loadDocmlSection(
        mDocumentLoader.data(),
        orientation,
        ":/docml/occ_eap_wizard_06.docml", 
        "portrait_section",
        "landscape_section");
    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_LOADDOCMLSECTION_EXIT );
}

/*!
   See WlanWizardPage. 

   @return next wizard page: EapWizardPage::PageIdentity
 */
int EapWizardPageCertUser::nextId() const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_NEXTID_ENTRY );
    mWizard->setConfigurations(
        EapWizardPrivate::CertificateUser, 
        qVariantFromValue(
            mCerts.at(mCertList->selected())));

    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_NEXTID_EXIT );
    return EapWizardPage::PageIdentity;
}

/*!
   See WlanWizardPage.

   Validates the content of the page.

   @return true if content is valid.
 */
bool EapWizardPageCertUser::showPage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_SHOWPAGE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_SHOWPAGE_EXIT );
    return mValid;
}

/*!
   Handler for itemsSelected() signal from HbRadioButtonList.

   @param [in] index NOT USED
 */
void EapWizardPageCertUser::itemSelected(int index)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGECERTUSER_ITEMSELECTED_ENTRY );
    Q_UNUSED(index);
    mValid = true;
    mWizard->wizardHelper()->enableNextButton(mValid);
    OstTraceFunctionExit0( EAPWIZARDPAGECERTUSER_ITEMSELECTED_EXIT );
}

