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
* WLAN Wizard test context.
*/

// System includes

#include <HbApplication>
#include <HbDocumentLoader>
#include <HbStackedWidget>
#include <HbRadioButtonList>
#include <HbAction>
#include <HbLineEdit>
#include <HbLabel>
#include <HbCheckBox>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbParameterLengthLimiter>

#include <QGraphicsWidget>
#include <QObjectList>
#include <QtCore>
#include <QTest>
#include <QDebug>
#include <QList>

// User includes

#include "testwlanwizardcontext.h"
#include "hbautotest.h"
#include "wlanwizard.h"
#include "wlanwizard_p.h"
#include "wlanwizardpageinternal.h"
#include "wlanwizardhelper.h"
#include "wlanqtutils_context.h"
#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "eapqtconfiginterface_context.h"
#include "eapwizard.h"
#include "eapwizard_p.h"
#include "eapwizardpage.h"
#include "wpswizardpage.h"
#include "wlanmgmtclient_context.h"
#include "wpswizardstepfour.h"
#include "wpswizardstepfour_p.h"
#include "cwpsactiverunner.h"

// External function prototypes

// Local constants
class TestRadioButtonList: public HbRadioButtonList
{
public:
    void emitActivated(const QModelIndex &modelIndex)
    { HbRadioButtonList::emitActivated(modelIndex); }
};


// ---------------------------------------------------------
// FRAMEWORK FUNCTIONS
// ---------------------------------------------------------

ContextWlanApList::ContextWlanApList()
{

}

ContextWlanApList::~ContextWlanApList()
{
    clear();
}

void ContextWlanApList::Add(
    QString name,
    int netMode,
    int secMode,
    bool wpaPskInUse,
    bool wpsSupported,
    int signalStrength)
{
    QSharedPointer<WlanQtUtilsAp> temp = QSharedPointer<WlanQtUtilsAp>(new WlanQtUtilsAp());
    temp->setValue(WlanQtUtilsAp::ConfIdName, name);
    temp->setValue(WlanQtUtilsAp::ConfIdSsid, name.toUtf8());
    temp->setValue(WlanQtUtilsAp::ConfIdConnectionMode, netMode);
    temp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, secMode);
    temp->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, wpaPskInUse);
    temp->setValue(WlanQtUtilsAp::ConfIdWpsSupported, wpsSupported);
    temp->setValue(WlanQtUtilsAp::ConfIdSignalStrength, signalStrength);

    mList.append(temp);
}

void ContextWlanApList::clear()
{
    mList.clear();
}


TestWlanWizardContext::TestWlanWizardContext() :
    mView(NULL),
    mMainWindow(NULL),
    mWlanQtUtilsContext(NULL),
    mEapQtUtilsContext(NULL),
    mWlanMgmtClientContext(NULL),
    mApList(NULL),
    mApOpenList(NULL)   
{
    
}
TestWlanWizardContext::~TestWlanWizardContext()
{
    
}

/**
 * This function will be called before the first test function is executed.
 */
void TestWlanWizardContext::initTestCase()
{
    qDebug("TestWlanWizardContext::initTestCase");

    mMainWindow = new HbAutoTestMainWindow;

    mView = new TestView();

    mMainWindow->addView(mView);
    mMainWindow->setCurrentView(mView);
    mMainWindow->installEventFilter(this);
    mMainWindow->show();

    QTest::qWait(1);
    while (!mEvent) {
        QTest::qWait(WaitTimeForUi);
    }
    mEvent = false;
    mApList = new ContextWlanApList();
    mApOpenList = new ContextWlanApList();

}

/**
 * This function will be called after the last test function was executed.
 */
void TestWlanWizardContext::cleanupTestCase()
{
    qDebug("TestWlanWizardContext::cleanupTestCase");

    qDebug("delete mApList");
    delete mApList;
    mApList = NULL;
    delete mApOpenList;
    mApOpenList = NULL;

    qDebug("delete mMainWindow");
    mMainWindow->setAttribute( Qt::WA_DeleteOnClose, true );
    mMainWindow->close();
    mMainWindow = NULL;
    QTest::qWait(WaitTimeForUi);
    qDebug("TestWlanWizardContext::cleanupTestCase exit");
}

/**
 * This function will be called before each test function is executed.
 */
void TestWlanWizardContext::init()
{
    qDebug("TestWlanWizardContext::init()");

    mView->createWizard();
    mWlanQtUtilsContext = new WlanQtUtilsContext(mView->mWizard->d_ptr->mWlanQtUtils.data());

    mEapQtUtilsContext = new EapQtConfigInterfaceContext();
    mEapQtUtilsContext->setObject(getEapQtConfig());
    
    mWlanMgmtClientContext = new WlanMgmtClientContext();
    //mWlanMgmtClientContext->setObject(mWlanMgmtClient);
    
    WpsPageStepFour* wps = (WpsPageStepFour*)(mView->mWizard->d_ptr->mPages[WpsWizardPage::PageWpsWizardStep4]);
    Q_ASSERT(wps);
    mWlanMgmtClientContext->setObject(wps->d_ptr->mWpsActiveRunner->iWLANMgmtClient);
}

/**
 * This function will be called after each test function is executed.
 */
void TestWlanWizardContext::cleanup()
{
    qDebug("TestWlanWizardContext::cleanup()");

    delete mWlanQtUtilsContext;
    delete mEapQtUtilsContext;
    delete mWlanMgmtClientContext;
    
    mView->deleteWizard();
    mApList->clear();
    mApOpenList->clear();
    QTest::qWait(1);
}


/*!
 * Filter to catch focus event to the text editor widget.
 */
bool TestWlanWizardContext::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mMainWindow && event->type() == QEvent::Show) {
        mMainWindow->removeEventFilter(this);
        mEvent = true;
    }
    return false;
}

QGraphicsWidget* TestWlanWizardContext::findChildItem(const QString &itemName, QGraphicsItem *widget)
{
    QList<QGraphicsItem*> list = widget->childItems();
    for (int i = 0; i < list.size(); i++) {
        QGraphicsItem* item = list[i];
        
        QGraphicsWidget* widget = NULL;
        if (item->isWidget()) {
            widget = (QGraphicsWidget*)item;
        }
        if (widget && widget->objectName() == itemName) {
            return widget;
        }
        else if ((item = findChildItem(itemName, item))) {
            return (QGraphicsWidget*)item;
        }
    }
    return 0;
}

bool TestWlanWizardContext::verifyStatus(WizardStatusSignal status, int iapId)
{
    return mView->verifyStatus(status, iapId);
}

bool TestWlanWizardContext::verifyCurrentPage(int pageId, int retries, int wait_ms)
{
    bool ret = true;
    for (int i = 0; i < retries; i++) {
        QTest::qWait(wait_ms);
        WlanWizardPrivate *pPrivate = mView->mWizard->d_ptr;
        HbWidget* current = qobject_cast<HbWidget*> (pPrivate->mStackedWidget->currentWidget());
        WlanWizardPage *page = pPrivate->mPageMapper[current];
        
        // TODO: verify title in this method
        
        if (page == pPrivate->mPages[pageId]) {
            switch(pageId) {
            case WlanWizardPageInternal::PageNetworkMode:
                return verifyDialogText("dialog_6", hbTrId("txt_occ_dialog_select_network_mode_and_status"));
            
            case WlanWizardPageInternal::PageNetworkSecurity:
                return verifyDialogText("dialog_6", hbTrId("txt_occ_dialog_select_network_security_mode"));
            
            case WlanWizardPageInternal::PageSsid:
                return verifyDialogText("dialog", hbTrId("txt_occ_dialog_insert_the_name_of_the_new_wlan_net"));
            
            case WlanWizardPageInternal::PageSummary:
                if (pPrivate->mTitle->plainText() != hbTrId("txt_occ_title_wlan_setup_wizard_summary")){
                    qWarning("TestWlanWizardContext::verifyCurrentPage: Invalid title");
                    return false;
                }
                return true;
               
            case EapWizardPage::PageOuterType:
                return verifyDialogText("title", hbTrId("txt_occ_title_select_eap_type"));

            case EapWizardPage::PageCertificateCa:
                return verifyDialogText("title", hbTrId("txt_occ_title_select_authority_certificate"));
                
            case EapWizardPage::PageCertificateUser:
                return verifyDialogText("title", hbTrId("txt_occ_title_select_user_certificate"));
                
            case EapWizardPage::PageNewPacStorePassword:
                ret &= verifyDialogText("setlabel_55", hbTrId("txt_occ_setlabel_new_pac_store_password"));
                ret &= verifyDialogText("setlabel_56", hbTrId("txt_occ_setlabel_confirm_password"));
                return ret;

            case EapWizardPage::PagePromptPacStorePassword:
                return verifyDialogText("setlabel_59", hbTrId("txt_occ_setlabel_eap_pac_store_password"));

            case WpsWizardPage::PageWpsWizardStep2:
                return verifyDialogText("label", hbTrId("txt_occ_dialog_selected_network_supports_wifi_pro"));
                break;

            case WpsWizardPage::PageWpsWizardStep3_Button:
                return verifyDialogText("label_heading", hbTrId("txt_occ_dialog_first_press_button_on_the_wireless"));
                break;

            case WpsWizardPage::PageWpsWizardStep3_Number:
                // TODO: something better here??
                return true;
                
            case WpsWizardPage::PageWpsWizardStep4:
                return verifyDialogText("label_heading", hbTrId("txt_occ_dialog_negotiating"));
                
            case WpsWizardPage::PageWpsWizardStep5:
                return verifyDialogText("label", hbTrId("txt_occ_dialog_settings_received_for_multiple_wlan"));
                
            default:
                return false;
            }
        }
    }
    qWarning("verifyCurrentPage: expected: %d", pageId);
    return false;
}

bool TestWlanWizardContext::verifyCurrentPageWithInfo(
    int pageId, const QString &info, int retries, int wait_ms)
{
    bool ret = true;
    for (int i = 0; i < retries; i++) {
        QTest::qWait(wait_ms);

        WlanWizardPrivate *pPrivate = mView->mWizard->d_ptr;
        HbWidget* current = qobject_cast<HbWidget*> (pPrivate->mStackedWidget->currentWidget());
        WlanWizardPage *page = pPrivate->mPageMapper[current];

        // TODO: verify title in this method
        
        if (page == pPrivate->mPages[pageId]) {
            switch(pageId) {
            case WlanWizardPageInternal::PageGenericError:
                return verifyDialogText("dialog", info);

            case WlanWizardPageInternal::PageKeyQuery:
                return verifyDialogText("dialog", HbParameterLengthLimiter("txt_occ_dialog_enter_key_for_1").arg(info));

            case WlanWizardPageInternal::PageProcessSettings:
                return verifyDialogText("dialog", HbParameterLengthLimiter("txt_occ_dialog_checking_connection_to_1").arg(info));

            case WlanWizardPageInternal::PageScanning:
                return verifyDialogText("dialog", HbParameterLengthLimiter("txt_occ_dialog_searching").arg(info));
               
            case EapWizardPage::PageUsernamePassword:
                ret &= verifyDialogText(
                    "setlabel_55",
                    HbParameterLengthLimiter("txt_occ_setlabel_user_name_for_1").arg(info));
                ret &= verifyDialogText("setlabel_56", hbTrId("txt_occ_setlabel_eap_password"));
                return ret;
                
            case EapWizardPage::PageIdentity:
                ret &= verifyDialogText("setlabel_53", HbParameterLengthLimiter("txt_occ_setlabel_user_name_for_1").arg(info));
                ret &= verifyDialogText("setlabel_53_val", hbTrId("txt_occ_setlabel_user_name_val_generate_automatic"));
                ret &= verifyDialogText("setlabel_54", hbTrId("txt_occ_setlabel_eap_realm"));
                ret &= verifyDialogText("setlabel_54_val", hbTrId("txt_occ_setlabel_eap_realm_val_generate_automatically"));
                return ret;
                
            case EapWizardPage::PageInnerTypePeap:
            case EapWizardPage::PageInnerTypeEapTtls:
                return verifyDialogText("title", HbParameterLengthLimiter("txt_occ_title_select_innear_eap_type_for_1").arg(info));
            
            default:
                return verifyCurrentPage(pageId, retries, wait_ms);
            }
        }
    }
    qWarning("verifyCurrentPage: expected: %d", pageId);
    return false;
}


bool TestWlanWizardContext::verifyDialogText(const QString objName, const QString text)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    HbWidget* widget = qobject_cast<HbWidget*> (
        findChildItem(objName, current));
    
    if (widget) {
        HbLabel *label = qobject_cast<HbLabel*>(widget);
        if (label) {
            if(label->plainText() == text) {
                if (!label->isVisible()){
                    qWarning("verifyDialogText: not visible");
                    return false;
                }
                return true;
                
            } else {
                qWarning("verifyDialogText: not match");
                qDebug() << "expect: " << text;
                qDebug() << "actual: " << label->plainText();
                return false;
            }
        }
        HbCheckBox *checkBox = qobject_cast<HbCheckBox*>(widget);
        
        if (checkBox) {
            if(checkBox->text() == text) {
                if (!checkBox->isVisible()){
                    qWarning("verifyDialogText: not visible");
                    return false;
                }
                return true;
                
            } else {
                qWarning("verifyDialogText: not match");
                qDebug() << "expect: " << text;
                qDebug() << "actual: " << checkBox->text();
                return false;
            }
        }
    }
    
    qWarning("verifyDialogText: object not found");
    
    return false;
}

bool TestWlanWizardContext::verifyActionButtons(ButtonStatus prevStatus,
    ButtonStatus cancelStatus,
    ButtonStatus nextStatus,
    ButtonStatus finishStatus)
{
    WlanWizardPrivate *pPrivate = mView->mWizard->d_ptr;
    ButtonStatus prevReally = ButtonHidden;
    ButtonStatus cancelReally = ButtonHidden;
    ButtonStatus nextReally = ButtonHidden;
    ButtonStatus finishReally = ButtonHidden;

    if(pPrivate->mActionPrevious->isVisible()) {
        if(pPrivate->mActionPrevious->isEnabled()) {
            prevReally = ButtonEnabled;
        } else {
            prevReally = ButtonDisabled;
        }
    }

    if(pPrivate->mActionCancel->isVisible()) {
        if(pPrivate->mActionCancel->isEnabled()) {
            cancelReally = ButtonEnabled;
        } else {
            cancelReally = ButtonDisabled;
        }
    }

    if(pPrivate->mActionNext->isVisible()) {
        if(pPrivate->mActionNext->isEnabled()) {
            nextReally = ButtonEnabled;
        } else {
            nextReally = ButtonDisabled;
        }
    }

    if(pPrivate->mActionFinish->isVisible()) {
        if(pPrivate->mActionFinish->isEnabled()) {
            finishReally = ButtonEnabled;
        } else {
            finishReally = ButtonDisabled;
        }
    }
    bool ret = true;
    if (prevReally != prevStatus){
        qWarning("Previous Button: really: %d, status: %d", prevReally, prevStatus);
        ret = false;
    }
    if (cancelReally != cancelStatus){
        qWarning("Cancel Button: really: %d, status: %d", cancelReally, cancelStatus);
        ret = false;
    }
    if (nextReally != nextStatus){
        qWarning("Next Button: really: %d, status: %d", nextReally, nextStatus);
        ret = false;
    }
    if (finishReally != finishStatus){
        qWarning("Finish Button: really: %d, status: %d", finishReally, finishStatus);
        ret = false;
    }
    
    return ret;
}

bool TestWlanWizardContext::verifySummaryPage(
    const QString &ssid, 
    int netMode, 
    bool hidden,
    int secMode, 
    bool useWpaPsk,
    const QString &destination, 
    int outerType,
    int innerType,
    bool eapFastProvMode)
{
    bool ret = true;
    int i = 0;
    HbListWidget* listWidget = qobject_cast<HbListWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    
    if (!listWidget){
        qWarning("verifySummaryPage: ListWidget not found");
        return false;
    }
    
    // VERIFY: NETWORK NAME
    HbListWidgetItem *item = listWidget->item(i++);
    if (item->text() != hbTrId("txt_occ_dblist_network_name")) {
        qWarning("verifySummaryPage: no match network name");
        ret = false;
    }
    
    if (item->secondaryText() != ssid){
        qWarning("verifySummaryPage: no match ssid");
        ret = false;
    }
    
    // VERIFY: NETWORK MODE
    item = listWidget->item(i++);
    if (item->text() != hbTrId("txt_occ_dblist_network_mode")) {
        qWarning("verifySummaryPage: no match network mode");
        ret = false;
    }
    QString netModeString(toNetworkModeString(netMode, hidden));
    if (item->secondaryText() != netModeString){
        qWarning("verifySummaryPage: no match network mode value");
        qDebug() << "Actual: " << item->secondaryText();
        qDebug() << "Expected: " << netModeString;
        ret = false;
    }
    
    // VERIFY: SECURITY MODE
    item = listWidget->item(i++);
    if (item->text() != hbTrId("txt_occ_dblist_security_mode")) {
        qWarning("verifySummaryPage: no match security mode");
        ret = false;
    }
    
    if (item->secondaryText() != toSecurityModeString(secMode, useWpaPsk)){
        qWarning("verifySummaryPage: no match security mode value");
        ret = false;
    }
    
    // VERIFY: EAP OUTER TYPE
    if (outerType != -1) {
        item = listWidget->item(i++);
        if (item->text() != hbTrId("txt_occ_dblist_outer_eap")) {
            qWarning("verifySummaryPage: no match eap outer type");
            ret = false;
        }
        if (item->secondaryText() != eapTypeToString(outerType)){
            qWarning("verifySummaryPage: no match eap outer type value");
            ret = false;
        }
    }
    
    // VERIFY: EAP INNER TYPE
    if (innerType != -1) {
        item = listWidget->item(i++);
        if (item->text() != hbTrId("txt_occ_dblist_inner_eap")) {
            qWarning("verifySummaryPage: no match eap inner type");
            ret = false;
        }
        if (item->secondaryText() != eapTypeToString(innerType)){
            qWarning("verifySummaryPage: no match eap inner type value");
            ret = false;
        }
    }
    
    // VERIFY: EAP FAST PROVISIONING MODE
    if (eapFastProvMode) {
        item = listWidget->item(i++);
        if (item->text() != hbTrId("txt_occ_dblist_provisioning_mode_for_eapfast")) {
            qWarning("verifySummaryPage: no match eap prov mode");
            ret = false;
        }
        if (item->secondaryText() != hbTrId("txt_occ_dblist_provisioning_mode_for_val_unauthent")){
            qWarning("verifySummaryPage: no match eap prov mode value");
            ret = false;
        }
    }

    // VERIFY: DESTINATION
    if (destination.length() > 0) {
        item = listWidget->item(i++);
        if (item->text() != hbTrId("txt_occ_dblist_destination")) {
            qWarning("verifySummaryPage: no match destination");
            ret = false;
        }
        if (item->secondaryText() != destination){
            qWarning("verifySummaryPage: no match destination value");
            ret = false;
        }
    }

    
    return ret;
}

bool TestWlanWizardContext::mouseClickObject(const QString objName)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    HbWidget* widget = (HbWidget*) findChildItem(objName, current);
    
    if (widget) {
        HbAutoTest::mouseClick(mMainWindow, widget);
        widget->clearFocus();
        widget->setFocus();
        return true;
    }
    qWarning("mouseClickObject: object not found");
    
    return false;
}

bool TestWlanWizardContext::insertTextToObject(const QString objName, const QString text)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    HbWidget* widget = (HbWidget*) findChildItem(objName, current);

    if (widget) {
        for (int i = 0; i < text.size(); i++) {
            HbAutoTest::keyPress(mMainWindow, text.at(i).toAscii(), 0, 10);
            QTest::qWait(20);
        }
        return true;
    }
    qWarning("insertTextToObject: object not found");
    return false;
}

bool TestWlanWizardContext::selectRadioButton(const QString objName, int index)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    TestRadioButtonList* widget = (TestRadioButtonList*) findChildItem(objName, current);

    if (widget) {
        if (widget->items().length() <= index) {
            qWarning("selectRadioButton: index out of range");
            return false;
        }
        widget->setSelected(index);
        widget->emitActivated(widget->currentIndex());
        return true;
    }
    qWarning("selectRadioButton: object not found");
    
    return false;
}

bool TestWlanWizardContext::verifyRadioButtonState(
    const QString objName, 
    int index, 
    const QString &text)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    
    HbRadioButtonList* widget = qobject_cast<HbRadioButtonList*>( 
        findChildItem(objName, current));

    if (widget) {
        if (widget->selected() != index) {
            qWarning("verifyRadioButtonState: invalid index");
            return false;
        }
        
        if (widget->items().at(index) != text) {
            qWarning("verifyRadioButtonState: text does not match");
            return false;
        }
        
        return true;
    }
    qWarning("verifyRadioButtonState: object not found");
    
    return false;
}

QStringList TestWlanWizardContext::verifyRadioButtons(const QString objName)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());

    HbRadioButtonList* widget = qobject_cast<HbRadioButtonList*>( 
        findChildItem(objName, current));
    
    QStringList ret;

    if (widget) {
        ret = widget->items();
    }
    else {
        qWarning("verifyRadioButtons: object not found");
    }

    return ret;
}

int TestWlanWizardContext::verifySelectedRadioButton(const QString objName)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());

    HbRadioButtonList* widget = qobject_cast<HbRadioButtonList*>( 
        findChildItem(objName, current));

    int ret = ListNotFound;

    if (widget) {
        ret = widget->selected();
    }
    else {
        qWarning("verifySelectedRadioButton: object not found");
    }

    return ret;
}

bool TestWlanWizardContext::verifyCheckBoxState(const QString &objName, bool checked)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    HbCheckBox* widget = qobject_cast<HbCheckBox*>(
        findChildItem(objName, current));

    if (widget) {
        if (widget->isChecked() != checked) {
            qWarning("Checkbox in wrong state");
            return false;
        }
        return true;
    } 
    qWarning("verifyCheckBoxState: object not found");
    return false;
}

bool TestWlanWizardContext::selectCheckBoxState(const QString &objName, bool check)
{
    HbWidget* current = qobject_cast<HbWidget*> (
        mView->mWizard->d_ptr->mStackedWidget->currentWidget());
    HbCheckBox* widget = qobject_cast<HbCheckBox*>(
        findChildItem(objName, current));

    if (widget) {
        if (check) {
            widget->setCheckState(Qt::Checked);
        } else {
            widget->setCheckState(Qt::Unchecked);
        }
        return true;
        
    } 
    qWarning("selectCheckBoxState: object not found");
    return false; 
}

bool TestWlanWizardContext::mouseClickNext()
{
    if (mView->mWizard->d_ptr->mActionNext->isEnabled()) {
        mView->mWizard->d_ptr->mActionNext->activate(QAction::Trigger);
        return true;
    }
    qWarning("Next button not enabled");
    return false;
}

bool TestWlanWizardContext::mouseClickPrevious()
{
    if (mView->mWizard->d_ptr->mActionPrevious->isEnabled()) {
        mView->mWizard->d_ptr->mActionPrevious->activate(QAction::Trigger);
        return true;
    }
    qWarning("mouseClickPrevious: Previous button not enabled");
    
    return false;
}

bool TestWlanWizardContext::mouseClickCancel()
{
    if (mView->mWizard->d_ptr->mActionCancel->isEnabled()) {
        mView->mWizard->d_ptr->mActionCancel->activate(QAction::Trigger);
        return true;
    }
    qWarning("mouseClickCancel: Cancel button not enabled");
    
    return false;
}

bool TestWlanWizardContext::mouseClickFinish()
{
    if (mView->mWizard->d_ptr->mActionFinish->isEnabled()) {
        mView->mWizard->d_ptr->mActionFinish->activate(QAction::Trigger);
        return true;
    }
    qWarning("mouseClickFinish: Finish button not enabled");
    
    return false;
}

QString TestWlanWizardContext::toNetworkModeString(int netMode, bool isHidden)
{
    QString ret;
    switch (netMode) {
    case CMManagerShim::Adhoc:
        ret = hbTrId("txt_occ_dblist_network_mode_val_adhoc");
        break;

     default:
         if (isHidden) {
             ret = hbTrId("txt_occ_dblist_network_mode_val_infrastructure_hi");
         } else {
             ret = hbTrId("txt_occ_dblist_network_mode_val_infrastructure_pu");
         }
         break;
     }
    return ret;
} 

QString TestWlanWizardContext::toSecurityModeString(int secMode, int useWpaPsk)
{
    QString ret;
    switch (secMode) {
    case CMManagerShim::WlanSecMode802_1x:
        ret = hbTrId("txt_occ_dblist_security_mode_val_8021x");
        break;
        
    case CMManagerShim::WlanSecModeWep:
        ret = hbTrId("txt_occ_dblist_security_mode_val_wep");
        break;
        
    case CMManagerShim::WlanSecModeWpa:
        if (useWpaPsk) {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpawpa2psk");
        } else {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpawpa2_with_eap");
        }
        break;
        
    case CMManagerShim::WlanSecModeWpa2:
        if (useWpaPsk) {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpa2_with_passwor");
        } else {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpa2_with_eap");
        }
        break;
        
    default:
        Q_ASSERT(secMode == CMManagerShim::WlanSecModeOpen);
        ret = hbTrId("txt_occ_dblist_security_mode_val_open");
        break;
    }
    return ret;
}


EapQtConfigInterface *TestWlanWizardContext::getEapQtConfig()
{
    return mView->mWizard->d_ptr->mEapWizard->d_ptr->mEapConfIf.data();
}

QString TestWlanWizardContext::eapTypeToString(int type)
{
    QCOMPARE(mView->mWizard->d_ptr->mEapWizard != NULL, true);
    return mView->mWizard->d_ptr->mEapWizard->d_ptr->eapTypeToString(type);
}

void TestWlanWizardContext::callWlanWizard_startPageOperation()
{
    mView->mWizard->d_ptr->startPageOperation();
}


void TestWlanWizardContext::setWlanMgmtClientObject(CWlanMgmtClient* object)
{
    // TODO: mWlanMgmtClient = object;
}

TestView::TestView() : mWizard(NULL)
{
    qDebug("TestView::TestView()");
}

TestView::~TestView()
{
    qDebug("TestView::~TestView()");
}

void TestView::createWizard()
{
    qDebug("TestView::createWizard");
    Q_ASSERT(mWizard == NULL);
    mWizard = new WlanWizard(mainWindow());
    bool ok;
    ok = connect(
        mWizard, SIGNAL(finished(int, bool)), 
        this, SLOT(finished(int, bool)), 
        Qt::QueuedConnection);
    Q_ASSERT(ok);

    ok = connect(
        mWizard, SIGNAL(cancelled()), 
        this, SLOT(cancelled()),
        Qt::QueuedConnection);
    Q_ASSERT(ok);
    
    mWizardStatus = WizardStatusSignalNone;
    mConnectedIapId = -100;
}

void TestView::showWizard()
{
    qDebug("TestView::showWizard()");
    Q_ASSERT(mWizard);
    mWizard->show();
}

void TestView::deleteWizard()
{
    qDebug("TestView::deleteWizard");

    Q_ASSERT(mWizard != NULL);
    QMetaObject::invokeMethod(mWizard, "deleteLater", Qt::QueuedConnection);
    QTest::qWait(100); 
    mWizard = NULL; 
}

void TestView::finished(int iapId, bool connected)
{
    qDebug("TestView::complete(), iap id: %d, connected: %d", iapId, connected);
    
    if (mWizardStatus != WizardStatusSignalNone) {
        qWarning("TestView::finished: multiple signals received");
        mWizardStatus = WizardStatusSignalUndefined;
    } else {
        mWizardStatus = WizardStatusSignalFinished;
        mConnectedIapId = iapId;
    }
}

void TestView::cancelled()
{
    qDebug("TestView::cancelled()");
    if (mWizardStatus != WizardStatusSignalNone) {
        qWarning("TestView::cancelled: multiple signals received");
        mWizardStatus = WizardStatusSignalUndefined;
    } else {
        mWizardStatus = WizardStatusSignalCancelled;
    }
}

bool TestView::verifyStatus(WizardStatusSignal status, int iapId )
{
    // Since connections to cancelled and finished signals are queued
    // we need to use qWait() here.
    QTest::qWait(100);
    bool ret = true;
    if (status != mWizardStatus){
        qWarning("TestView::verifyStatus, status: expected: %d, actual: %d", status, mWizardStatus);
        ret = false;
    }
    if (status == WizardStatusSignalFinished) {
        if (iapId != mConnectedIapId) {
            qWarning("TestView::verifyStatus, iapid: expected: %d, actual: %d", iapId, mConnectedIapId);
            ret = false;
        }
    }
    return ret;
}
