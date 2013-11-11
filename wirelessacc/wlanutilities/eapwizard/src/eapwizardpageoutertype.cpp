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
 *   EAP Wizard Page: Outer type selection
 *
 */

// System includes
#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbLabel>
#include <eapqtpluginhandle.h>
#include <eapqtplugininfo.h>
#include <eapqtconfiginterface.h>
#include <eapqtconfig.h>
#include <eapqtpacstoreconfig.h>

// User includes
#include "wlanwizardhelper.h"
#include "eapwizard_p.h"
#include "eapwizardpageoutertype.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizardpageoutertypeTraces.h"
#endif


/*!
   \class EapWizardPageOuterType
   \brief Implements EAP wizard page: Outer EAP type selection
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.
   
   @param [in] parent Pointer to EAP Wizard private implementation.
 */
EapWizardPageOuterType::EapWizardPageOuterType(EapWizardPrivate* parent) :
    EapWizardPage(parent),
    mWidget(NULL),
    mRadio(NULL),
    mTitle(NULL),
    mDocumentLoader(NULL),    
    mValid(false)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_EAPWIZARDPAGEOUTERTYPE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_EAPWIZARDPAGEOUTERTYPE_EXIT );
}

/*!
   Destructor.
 */
EapWizardPageOuterType::~EapWizardPageOuterType()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPAGEOUTERTYPE_EAPWIZARDPAGEOUTERTYPE_ENTRY );
    OstTraceFunctionExit0( DUP1_EAPWIZARDPAGEOUTERTYPE_EAPWIZARDPAGEOUTERTYPE_EXIT );
}

/*!
   See WlanWizardPage. 
 */
HbWidget* EapWizardPageOuterType::initializePage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_INITIALIZEPAGE_ENTRY );
    if (!mWidget) {
        bool ok;
        mDocumentLoader.reset(
            new HbDocumentLoader(mWizard->wizardHelper()->mainWindow()));
        mDocumentLoader->load(":/docml/occ_eap_wizard_01_02_04.docml", &ok);
        Q_ASSERT(ok);
        
        loadDocmlSection(mWizard->wizardHelper()->mainWindow()->orientation());
        
        mWidget = qobject_cast<HbWidget*> (mDocumentLoader->findWidget("occ_eap_wizard_01"));
        Q_ASSERT(mWidget);

        mRadio = qobject_cast<HbRadioButtonList*> (mDocumentLoader->findWidget("list"));
        Q_ASSERT(mRadio);

        mTitle = qobject_cast<HbLabel*> (mDocumentLoader->findWidget("title"));
        Q_ASSERT(mTitle);
        
        mTitle->setPlainText(hbTrId("txt_occ_title_select_eap_type"));

        QStringList items;
        appendToList(items, EapQtPluginHandle::PluginPeap);
        appendToList(items, EapQtPluginHandle::PluginEapTtls);
        appendToList(items, EapQtPluginHandle::PluginEapTls);
        appendToList(items, EapQtPluginHandle::PluginEapFast);
        appendToList(items, EapQtPluginHandle::PluginLeap);
        appendToList(items, EapQtPluginHandle::PluginEapAka);
        appendToList(items, EapQtPluginHandle::PluginEapSim);
        mRadio->setItems(items);

        ok = connect(
            mRadio, SIGNAL(itemSelected(int)), 
            this, SLOT(itemSelected(int)));
        Q_ASSERT(ok);
        
        ok = connect(
            mWizard->wizardHelper()->mainWindow(), 
            SIGNAL(orientationChanged(Qt::Orientation)), 
            this, 
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
    }
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
   Loads the required orientation of docml.
   
   @param [in] orientation Orientation to be loaded. 
 */
void EapWizardPageOuterType::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_LOADDOCMLSECTION_ENTRY );
    EapWizardPage::loadDocmlSection(
        mDocumentLoader.data(),
        orientation,
        ":/docml/occ_eap_wizard_01_02_04.docml",
        "portrait_section",
        "landscape_section");
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_LOADDOCMLSECTION_EXIT );
}

/*!
   See WlanWizardPage.
   
   @return next wizard page:
   - WlanWizardPage::PageProcessSettings: For EAP-AKA and EAP-SIM
   - EapWizardPage::PageCertificateCa: For PEAP, EAP-TLS and EAP-TTLS 
   - EapWizardPage::PageUsernamePassword: For LEAP and EAP-FAST (pac confirmed)
   - EapWizardPage::PageNewPacStorePassword: For EAP-FAST (no pac store)
   - EapWizardPage::PagePromptPacStorePassword: For EAP-FAST (confirm pac store)
 */
int EapWizardPageOuterType::nextId() const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_NEXTID_ENTRY );
    int id = WlanWizardPage::PageNone;
    EapQtPluginHandle::Plugin handle = 
        static_cast<EapQtPluginHandle::Plugin>(mEapTypes.at(mRadio->selected()));
    switch (handle) {
    case EapQtPluginHandle::PluginPeap:
    case EapQtPluginHandle::PluginEapTtls:
    case EapQtPluginHandle::PluginEapTls:
        id = EapWizardPage::PageCertificateCa;
        break;

    case EapQtPluginHandle::PluginEapFast:
        id = nextIdForEapFast();
        break;
        
    case EapQtPluginHandle::PluginLeap:
        id = EapWizardPage::PageUsernamePassword;
        break;

    default:
        Q_ASSERT(EapQtPluginHandle::PluginEapAka == handle ||
            EapQtPluginHandle::PluginEapSim == handle);   
        id = WlanWizardPage::PageProcessSettings;
        break;
    }
    
    mWizard->setConfigurations(EapWizardPrivate::OuterType, handle);

    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_NEXTID_EXIT );
    return id;
}

/*!
   See WlanWizardPage.
   
   Validates the content of the page.
   
   @return true if content is valid.
 */
bool EapWizardPageOuterType::showPage()
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_SHOWPAGE_ENTRY );
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_SHOWPAGE_EXIT );
    return mValid;
}

/*!
   Slot for itemSelected() signal from HbRadioButtonList.
   
   @param [in] index NOT USED.
 */
void EapWizardPageOuterType::itemSelected(int index)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_ITEMSELECTED_ENTRY );
    Q_UNUSED(index);
    mValid = true;
    mWizard->wizardHelper()->enableNextButton(mValid);
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_ITEMSELECTED_EXIT );
}

/*!
   Appends given method to radiobutton list if plugin (method) is supported
   
   @param [in,out] items the content of radiobutton list
   @param [in] pluginHandle EAP Type plugin handle to be added.
 */
void EapWizardPageOuterType::appendToList(
    QStringList &items, int pluginHandle)
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_APPENDTOLIST_ENTRY );
    if (mWizard->eapConfigurationInterface()->isSupportedOuterType(
        (EapQtPluginHandle::Plugin)pluginHandle)) {
        items << mWizard->eapTypeToString(pluginHandle);
        mEapTypes.append(pluginHandle);
    }
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_APPENDTOLIST_EXIT );
}

/*
   Getter for next page id when outer eap type is EAP-FAST.
   
   @return Page ID.
 */
int EapWizardPageOuterType::nextIdForEapFast() const
{
    OstTraceFunctionEntry0( EAPWIZARDPAGEOUTERTYPE_NEXTIDFOREAPFAST_ENTRY );
    int pageId;
    
    mWizard->setConfigurations(EapWizardPrivate::InnerType,
        EapQtPluginHandle::PluginEapMschapv2);
    
    EapQtPacStoreConfig pacStoreConf;
    int pacStoreState = EapQtPacStoreConfig::PacStoreStateStoreNotExists;
    
    if (mWizard->eapConfigurationInterface()->readPacStoreConfiguration(pacStoreConf)) {
        pacStoreState = pacStoreConf.value(EapQtPacStoreConfig::PacStoreState).toInt();   
    } else {
        pacStoreConf.clear();
        pacStoreConf.setValue(EapQtPacStoreConfig::PacStoreReset, true);
        mWizard->eapConfigurationInterface()->savePacStoreConfiguration(pacStoreConf);
    }
    
    mWizard->setConfigurations(EapWizardPrivate::PacStoreState, pacStoreState);
    
    switch (pacStoreState) {
    case EapQtPacStoreConfig::PacStoreStateStoreNotExists:
        pageId = EapWizardPage::PageNewPacStorePassword;
        break;
    
    case EapQtPacStoreConfig::PacStoreStatePasswordRequired:
        pageId = EapWizardPage::PagePromptPacStorePassword;
        break;
    
    default:
        Q_ASSERT(pacStoreState == EapQtPacStoreConfig::PacStoreStatePasswordStored);
        pageId = EapWizardPage::PageUsernamePassword;
        break;
    }
    
    OstTraceFunctionExit0( EAPWIZARDPAGEOUTERTYPE_NEXTIDFOREAPFAST_EXIT );
    return pageId;
}
