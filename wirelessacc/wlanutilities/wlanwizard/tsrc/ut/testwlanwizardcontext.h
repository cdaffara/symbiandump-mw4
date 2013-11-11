/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 */

#ifndef TESTWLANWIZARDRNDUICONTEXT_H_
#define TESTWLANWIZARDRNDUICONTEXT_H_

#include <QObject>
#include <HbView>
#include <QSharedPointer>

class QGraphicsItem;
class HbAutoTestMainWindow;
class WlanWizard;
class WlanQtUtilsContext;
class WlanQtUtils;
class WlanQtUtilsAp;
class QString;
class EapWizard;
class EapQtConfigInterfaceContext;
class EapQtConfigInterface;
class WlanMgmtClientContext;
class CWlanMgmtClient;

//! 1sec 
#define WaitTimeForUi 500

enum WizardStatusSignal{
    WizardStatusSignalNone,
    WizardStatusSignalFinished,
    WizardStatusSignalCancelled,
    WizardStatusSignalUndefined,
};

class ContextWlanApList
{
public:
    ContextWlanApList();
    ~ContextWlanApList();
    
    void Add(QString name,
        int netMode,
        int secMode,
        bool wpaPskInUse,
        bool wpsSupported,
        int signalStrength = 0);
    
    void clear();
    
    const QList<QSharedPointer<WlanQtUtilsAp> > &List() { return mList; }
private:
    QList<QSharedPointer<WlanQtUtilsAp> > mList;
};

class TestView: public HbView
{
Q_OBJECT

public: 


public:
    TestView();
    virtual ~TestView();
       
    void createWizard();
    void showWizard();
    bool verifyStatus(WizardStatusSignal status, int iapId = -100);
    
private slots:
    void finished(int iapId, bool connected);
    void cancelled();

public:
    void deleteWizard();
    
public:
    WlanWizard *mWizard;

private:
    WizardStatusSignal mWizardStatus;
    int mConnectedIapId;
};


class TestWlanWizardContext : public QObject
{
Q_OBJECT
public:
    TestWlanWizardContext();
    virtual ~TestWlanWizardContext();

public slots:
    // Test framework functions
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
protected:
    static const int ListNotFound = -2;
    static const int ListNoneSelected = -1;
    
    enum ButtonStatus {
        ButtonHidden,
        ButtonDisabled,
        ButtonEnabled
    };
    /*!
     * Reimplements QObject function that is invoked on any event. This
     * function handles only a focus event to mSsid edit field.
     * @param obj contains the object, where the event originates from.
     * @param event contains the event id.
     */
    bool eventFilter(QObject *obj, QEvent *event);
    
    QGraphicsWidget* findChildItem(const QString &itemName, QGraphicsItem *widget);
    bool verifyStatus(WizardStatusSignal status, int iapId = -100);
    bool verifyCurrentPage(int pageId, int retries = 10, int wait_ms = 200);
    bool verifyCurrentPageWithInfo(int pageId, const QString &info, int retries = 10, int wait_ms = 200);
    bool verifyDialogText(const QString objName, const QString text);
    bool verifyActionButtonVisibility(bool prevVisible, bool cancelVisible,
        bool nextVisible, bool finishVisible);
    bool verifyActionButtons(ButtonStatus prevStatus, ButtonStatus cancelStatus,
        ButtonStatus nextStatus, ButtonStatus finishStatus);
    bool verifySummaryPage(
        const QString &ssid, 
        int netMode,
        bool hidden,
        int secMode, 
        bool useWpaPsk,
        const QString &destination = QString(), 
        int outerType = -1,
        int innerType = -1,
        bool eapFastProvMode = false);
    
    bool mouseClickObject(const QString objName);
    bool insertTextToObject(const QString objName, const QString text);
    bool selectRadioButton(const QString objName, int index);
    bool verifyRadioButtonState(const QString objName, int index, const QString &text);
    bool verifyCheckBoxState(const QString &objName, bool checked);
    QStringList verifyRadioButtons(const QString objName);
    int verifySelectedRadioButton(const QString objName);
    bool selectCheckBoxState(const QString &objName, bool check);
    
    bool mouseClickNext();
    bool mouseClickPrevious();
    bool mouseClickCancel();
    bool mouseClickFinish();
    
    QString toNetworkModeString(int netMode, bool isHidden);
    QString toSecurityModeString(int secMode, int useWpaPsk);
    
    EapQtConfigInterface *getEapQtConfig();
    QString eapTypeToString(int type);
    
    void callWlanWizard_startPageOperation();
    
public:
    static void setWlanMgmtClientObject(CWlanMgmtClient* object);
    
public:
    
    static CWlanMgmtClient *mWlanMgmtClient;
    
protected:
    bool mEvent;

    // Test data
    TestView *mView;
    HbAutoTestMainWindow *mMainWindow;
    WlanQtUtilsContext *mWlanQtUtilsContext;
    EapQtConfigInterfaceContext *mEapQtUtilsContext;
    WlanMgmtClientContext *mWlanMgmtClientContext;
    ContextWlanApList *mApList;
    ContextWlanApList *mApOpenList;
};

#endif /* TESTWLANWIZARDRNDUICONTEXT_H_ */
