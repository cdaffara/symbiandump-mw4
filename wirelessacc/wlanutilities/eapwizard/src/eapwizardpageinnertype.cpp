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
 *   EAP Wizard Page: Inner Type selection
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLabel>
#include <HbParameterLengthLimiter>
#include <eapqtpluginhandle.h>
#include <eapqtplugininfo.h>
#include <eapqtconfiginterface.h>

// User includes
#include "wlanwizardhelper.h"
#include "eapwizardpageinnertype.h"
#include "eapwizard_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardpageinnertypeTraces.h"
#endif



/*!
   \class EapWizardPageInnerType
   \brief Implements EAP wizard page: Inner EAP type selection.
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.

   @param [in] parent Pointer to EAP Wizard private implementation.
   @param [in] outerType Identifies the outer type: Peap or EapTtls
 */
EapWizardPageInnerType::EapWizardPageInnerType(
    EapWizardPrivate* parent, int outerType) :
    EapWizardPage(parent), 
    mWidget(NULL), 
    mRadio(NULL), 
    mTitle(NULL),
    mDocumentLoader(NULL),    
    mOuterType(outerType),
    mValid(false)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_EAPWIZARDPAGEINNERTYPE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_EAPWIZARDPAGEINNERTYPE_EXIT );
}

/*!
   Destructor.
 */
EapWizardPageInnerType::~EapWizardPageInnerType()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPAGEINNERTYPE_EAPWIZARDPAGEINNERTYPE_ENTRY );
    OstTraceFunctionExit0( DUP1_EAPWIZARDPAGEINNERTYPE_EAPWIZARDPAGEINNERTYPE_EXIT );
}

/*!
   See WlanWizardPage. 
 */
HbWidget* EapWizardPageInnerType::initializePage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_INITIALIZEPAGE_ENTRY );
    if (!mWidget) {
        bool ok;
        mDocumentLoader.reset(
            new HbDocumentLoader(mWizard->wizardHelper()->mainWindow()));
        mDocumentLoader->load(":/docml/occ_eap_wizard_01_02_04.docml", &ok);
        Q_ASSERT(ok);

        loadDocmlSection(mWizard->wizardHelper()->mainWindow()->orientation());

        mWidget = qobject_cast<HbWidget*> (mDocumentLoader->findWidget("occ_eap_wizard_01"));
        Q_ASSERT(mWidget);

        mTitle = qobject_cast<HbLabel*> (mDocumentLoader->findWidget("title"));
        Q_ASSERT(mTitle);

        mRadio = qobject_cast<HbRadioButtonList*> (mDocumentLoader->findWidget("list"));
        Q_ASSERT(mRadio);

        ok = connect(
            mWizard->wizardHelper()->mainWindow(),
            SIGNAL(orientationChanged(Qt::Orientation)), 
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);

        QStringList list;
        switch (mOuterType){
        case EapQtPluginHandle::PluginPeap:
            appendToList(list, EapQtPluginHandle::PluginEapMschapv2);    
            break;

        default:
            Q_ASSERT(mOuterType == EapQtPluginHandle::PluginEapTtls);
            appendToList(list, EapQtPluginHandle::PluginPlainMschapv2);
            appendToList(list, EapQtPluginHandle::PluginPap);
            break;
        }
        appendToList(list, EapQtPluginHandle::PluginEapGtc);
        mRadio->setItems(list);

        ok = connect(mRadio, SIGNAL(itemSelected(int)), this, SLOT(itemSelected(int)));
        Q_ASSERT(ok);
    }

    mTitle->setPlainText(
        HbParameterLengthLimiter("txt_occ_title_select_innear_eap_type_for_1").arg(
            mWizard->eapTypeToString(
                mWizard->configurations(
                    EapWizardPrivate::OuterType).toInt())));

    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
   Loads the required orientation of docml.

   @param [in] orientation Orientation to be loaded. 
 */
void EapWizardPageInnerType::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_LOADDOCMLSECTION_ENTRY );
    EapWizardPage::loadDocmlSection(
        mDocumentLoader.data(),
        orientation,
        ":/docml/occ_eap_wizard_01_02_04.docml", 
        "portrait_section",
        "landscape_section");
    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_LOADDOCMLSECTION_EXIT );
}

/*!
   See WlanWizardPage.

   @return next wizard page: EapWizardPage::PageUsernamePassword
 */
int EapWizardPageInnerType::nextId() const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_NEXTID_ENTRY );
    mWizard->setConfigurations(
        EapWizardPrivate::InnerType, 
        mEapTypes.at(mRadio->selected()));

    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_NEXTID_EXIT );
    return EapWizardPage::PageUsernamePassword;
}

/*!
   See WlanWizardPage.

   Validates the content of the page.

   @return true if content is valid.
 */
bool EapWizardPageInnerType::showPage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_SHOWPAGE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_SHOWPAGE_EXIT );
    return mValid;
}

/*!
   Handler for itemsSelected() signal from HbRadioButtonList.

   @param [in] index NOT USED
 */
void EapWizardPageInnerType::itemSelected(int index )
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_ITEMSELECTED_ENTRY );
    Q_UNUSED(index);
    mValid = true;
    mWizard->wizardHelper()->enableNextButton(mValid);
    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_ITEMSELECTED_EXIT );
}

/*!
   Appends given method to radiobutton list if plugin (method) is supported

   @param [in,out] items the content of radiobutton list
   @param [in] pluginHandle EAP Type plugin handle to be added.
 */
void EapWizardPageInnerType::appendToList(
    QStringList &items, 
    EapQtPluginHandle::Plugin innerType)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEINNERTYPE_APPENDTOLIST_ENTRY );
    if (mWizard->eapConfigurationInterface()->isSupportedInnerType(
        static_cast<EapQtPluginHandle::Plugin>(mOuterType), innerType)) {
        items << mWizard->eapTypeToString(innerType);
        mEapTypes.append(innerType);
    }
    OstTraceFunctionExit0( EAPWIZARDPAGEINNERTYPE_APPENDTOLIST_EXIT );
}
