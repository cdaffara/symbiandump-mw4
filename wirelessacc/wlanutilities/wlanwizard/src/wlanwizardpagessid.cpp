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
* WLAN Wizard Page: SSID Selection.
*/

// System includes

#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbWidget>
#include <HbLineEdit>
#include <HbEditorInterface>
#include <HbLabel>

#include <wlanqtutils.h>

// User includes

#include "wlanwizardpagessid.h"
#include "wlanwizard_p.h"
#include "wlanwizardutils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagessidTraces.h"
#endif

/*!
 * Constructor method for the SSID query view object.
 * @param [in] parent pointer to parent object.
 */
WlanWizardPageSsid::WlanWizardPageSsid(WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent),
    mWidget(NULL),
    mLabel(NULL),
    mSsid(NULL),
    mLoader(NULL)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_WLANWIZARDPAGESSID_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGESSID_WLANWIZARDPAGESSID_EXIT);
}

/*!
 * Destructor. Loader widget is deleted.
 * All document widgets are deleted by wlanwizard_p destructor.
 */
WlanWizardPageSsid::~WlanWizardPageSsid()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGESSID_WLANWIZARDPAGESSID_ENTRY);
    
    delete mLoader;
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGESSID_WLANWIZARDPAGESSID_EXIT);
}

/*!
 * Page initialization. If view is already loaded, does nothing.
 * @return pointer to widget "occ_add_wlan_01".
 */
HbWidget* WlanWizardPageSsid::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_INITIALIZEPAGE_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESSID_INITIALIZEPAGE,
        "WlanWizardPageSsid::initializePage");

    // It is not possible for this method to be called more than once during
    // wizard lifetime.
    Q_ASSERT(mWidget == NULL);

    bool ok;

    mLoader = new HbDocumentLoader(mWizard->mainWindow());

    mLoader->load(":/docml/occ_add_wlan_01_04.docml", &ok);
    Q_ASSERT(ok);

    // Load orientation
    loadDocmlSection(mWizard->mainWindow()->orientation());

    // Load widgets
    mWidget = qobject_cast<HbWidget*> (mLoader->findWidget(
        "occ_add_wlan_01"));
    Q_ASSERT(mWidget != NULL);

    mLabel = qobject_cast<HbLabel*> (mLoader->findWidget("dialog"));
    Q_ASSERT(mLabel != NULL);

    mSsid = qobject_cast<HbLineEdit*> (mLoader->findWidget("lineEditKey"));
    Q_ASSERT(mSsid != NULL);

    mLabel->setPlainText(hbTrId(
        "txt_occ_dialog_insert_the_name_of_the_new_wlan_net"));

    // Connect orientation signal from the main window to orientation
    // loader.
    ok = connect(mWizard->mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(loadDocmlSection(Qt::Orientation)));
    Q_ASSERT(ok);

    // Connect text change-signal from input dialog to handler function
    ok = connect(mSsid, SIGNAL(textChanged(const QString &)), this,
        SLOT(textChanged(const QString &)));
    Q_ASSERT(ok);

    HbEditorInterface editInterface(mSsid);
    
    editInterface.setInputConstraints(
        HbEditorConstraintAutoCompletingField |
        HbEditorConstraintLatinAlphabetOnly);
    
    editInterface.setSmileyTheme(HbSmileyTheme());
    editInterface.setEditorClass(HbInputEditorClassNetworkName);
    mSsid->setInputMethodHints(
        Qt::ImhNoPredictiveText | Qt::ImhPreferLowercase);
    mSsid->setMaxLength(WlanWizardUtils::SsidMaxLength);

	OstTraceFunctionExit0(WLANWIZARDPAGESSID_INITIALIZEPAGE_EXIT);
	return mWidget;
}

/*!
 * This method is overrides the default implementation from WlanWizardPage.
 * It indicates whether the Next-button should be enabled or not.
 * @return true, if mSsid edit field is not empty.
 */
bool WlanWizardPageSsid::showPage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_SHOWPAGE_ENTRY);
    
    // Initiate the scanning of public APs here.
    mWizard->wlanQtUtils()->scanWlanAps();
    
    // Open virtual keyboard by setting focus to line edit
    mSsid->setFocus();
    
    OstTraceFunctionExit0(WLANWIZARDPAGESSID_SHOWPAGE_EXIT);
    return !(mSsid->text().isEmpty());
}

/*!
 * Stores the SSID selection in the configuration.
 * @param [out] removeFromStack returns false.
 * @return WlanWizardPageScanning page id.
 */
int WlanWizardPageSsid::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_NEXTID_ENTRY);
    
    removeFromStack = false;

    // SSID is stored into configuration
    QString ssid(mSsid->text());
    mWizard->setConfiguration(WlanWizardPrivate::ConfName, ssid);
    mWizard->setConfiguration(WlanWizardPrivate::ConfSsid, ssid.toUtf8());
    
    OstTraceFunctionExit0(WLANWIZARDPAGESSID_NEXTID_EXIT);
    return WlanWizardPageInternal::PageScanning;
}

/*!
 * Loads the document orientation information from occ_add_wlan_01_04.docml
 * This is called each time phone orientation changes.
 * @param [in] orientation indicates whether the phone is in portrait or
 * landscape mode.
 */
void WlanWizardPageSsid::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_LOADDOCMLSECTION_ENTRY);
    
    OstTrace1(
        TRACE_FLOW,
        WLANWIZARDPAGESSID_LOADDOCML,
        "WlanWizardPageSsid::loadDocml - orientation ;orientation=%x",
        (TUint)orientation);

    WlanWizardPageInternal::loadDocmlSection(
        mLoader,
        orientation,
        ":/docml/occ_add_wlan_01_04.docml", 
        "portrait_section",
        "landscape_section");
    
    OstTraceFunctionExit0(WLANWIZARDPAGESSID_LOADDOCMLSECTION_EXIT);
}

/*!
 * Executed when changes have been made to the mSsid field (and the field is
 * not empty).
 * @param [in] text is not used at this point.
 */
void WlanWizardPageSsid::textChanged(const QString &text)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESSID_TEXTCHANGED_ENTRY);

    Q_UNUSED(text);
    OstTrace0(
        TRACE_FLOW,
        WLANWIZARDPAGESSID_TEXTCHANGED,
        "WlanWizardPageSsid::textChanged in text edit widget");

    mWizard->enableNextButton(!(mSsid->text().isEmpty()));

    OstTraceFunctionExit0(WLANWIZARDPAGESSID_TEXTCHANGED_EXIT);
}
