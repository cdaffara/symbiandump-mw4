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
* WLAN Wizard: Private implementation.
*/

#ifndef WLANWIZARD_P_H
#define WLANWIZARD_P_H

// System includes
#include <QObject>
#include <QHash>

// User includes
#include "wlanwizardhelper.h"

// Forward declarations
class QTimer;
class HbTranslator;
class HbStackedWidget;
class HbWidget;
class HbLabel;
class HbDialog;
class HbAction;
class HbDocumentLoader;
class HbMainWindow;
class WlanQtUtils;
class WlanWizardPlugin;
class WlanWizardPage;
class WlanWizard;
class EapWizard;
class WpsWizard;

class TestWlanWizardContext;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{   
 */
class WlanWizardPrivate: public QObject, public WlanWizardHelper
{
	Q_OBJECT

public:
    // API implementation
    WlanWizardPrivate(WlanWizard* wizard, HbMainWindow *mainWindow);
    virtual ~WlanWizardPrivate();

    void setParameters(
        const QString &name,
        const QByteArray &ssid,
        int networkMode, 
        int securityMode,
        bool usePsk, 
        bool wps);

    void show();

public:
    // For WLAN Wizard pages
    int getNextPageId(
        const QString &name,
        const QByteArray &ssid,
        int networkMode, 
        int securityMode,
        bool usePsk, 
        bool hidden, 
        bool wps);
    
    WlanQtUtils* wlanQtUtils() const;
    WlanWizardPlugin* wlanWizardPlugin() const;
    bool isEapEnabled() const;
    bool handleIap();

public:
    // from WlanWizardHelper, for all wizards
    virtual QVariant configuration(ConfigurationId confId) const;
    virtual void setConfiguration(ConfigurationId confId, const QVariant &value);
    virtual void clearConfiguration(ConfigurationId confId);
    virtual bool configurationExists(ConfigurationId confId);
    virtual void enableNextButton(bool enable);
    virtual void addPage(int pageId, WlanWizardPage *page);
    virtual void nextPage();
    virtual HbMainWindow* mainWindow() const;
    virtual bool isCurrentPage(const HbWidget *page) const;
    virtual int nextPageId(bool useWps);

signals:
    
public slots:

    void cancelTriggered();
    
protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPrivate)
    static const int PageTimeout = 1500;

private slots:
    void previousTriggered();
    void nextTriggered();
    void finishTriggered();
    void onTimeOut();
    void startPageOperation();
    
private:
    void toNextPage();
    void showPage(int pageId, bool removeFromStack);
    void createPages();
    void closeViews();
    void updateFrame(int pageId);
    void loadDocml();
    void connectActions();
    void disconnectActions();

private:
    // Not owned pointers
    //! Pointer to Wizard API class, needed to send signals
    WlanWizard *q_ptr;
    //! Pointer to mainwindow from where the wizard was launched
    HbMainWindow *mMainWindow;
    //! used for timer protected pages (progressbar)
    QTimer *mPageTimer;
    //! Wizard heading aka HbDialog::heading
    HbLabel *mTitle;
    //! Toolbar action: Next
    HbAction *mActionNext;
    //! Toolbar action: Previous
    HbAction *mActionPrevious;
    //! Toolbar action: Finish
    HbAction *mActionFinish;
    //! Toolbar action: Cancel
    HbAction *mActionCancel;
    /*! acts as HbDialog::contentWidget. When next is clicked a wizard page
       (HbWidget) is added to the stack. When prev is pressed the current
        item is removed from the stack. The previous pages are always in the
        stacked widget
     */
    HbStackedWidget *mStackedWidget;
    
    // Owned pointers
    
    //! Dialog, from docml
    QScopedPointer<HbDialog> mDialog;
    //! Translator
    QScopedPointer<HbTranslator> mTranslator;
    //! Provides services like: create/update iap, scan, connect, disconnect
    QScopedPointer<WlanQtUtils> mWlanQtUtils;
    //! Document loader for docml
    QScopedPointer<HbDocumentLoader> mDocLoader;
    //! Extensible Authentication Protocol (EAP) Wizard plugin, instantiated
    //! when required
    QScopedPointer<EapWizard> mEapWizard;
    //! Wifi Protected Setup (WPS) Wizard plugin, instantiated when required
    QScopedPointer<WpsWizard> mWpsWizard;
    //! Maps UI object of page into a control object
    QHash<HbWidget* , WlanWizardPage *> mPageMapper;
    //! Maps page id to control object of page.
    QHash<int, WlanWizardPage *> mPages;

    //! Holds wlan wizard configurations. 
    QHash<ConfigurationId, QVariant> mConfigurations;
    //! holds the page id which is displayed when WlanWizard::show() is called
    int mFirstPageId;
    //! Flag to keep track if movement to next page is allowed when timer is used
    bool mPageFinished;
    //! flag to indicate that wizard has been closed via either cancel or finish
    bool mClosed;
    
    // Friend classes
    friend class WlanWizard;
    friend class TestWlanWizardContext;
};

/*! @} */

#endif // WLANWIZARD_P_H
