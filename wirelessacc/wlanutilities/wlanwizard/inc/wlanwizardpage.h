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
 *   WLAN Wizard Plugin API: Interface for wizard pages.
 *
 */

#ifndef WLANWIZARDPAGE_H
#define WLANWIZARDPAGE_H

// System includes

// User includes

// Forward declarations
class HbWidget;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard_api_internal
   @{
 */

/*!
   Interface for wizard pages.
   
   Princibles of the interface, see description of the method for detailed
   information.
   - initializePage() is called at first to create the UI visualizatio
   - showPage() is called to detect whether next button should be enabled. This 
     method is called every time before the current page is displayed.
   - requiresStartOperation() is called to check if page needs timer protection.
     This is mainly used for pages where are progress bar and some asyncronous 
     operation with network layer, which commands on success to progress to next
     wizard page.
   - startOperation() is called to start page operation, this is called if above 
     method returs true.
   - nextId() is called when user presses next toolbar button or when page has
     called WlanWizardHelper::nextPage() method, which is used with timer
     protected pages.
   - previousTriggered() is called when previous toolbar button is pressed.
   - cancelTriggered() is called when cancel toolbar button is pressed.
   
   Wizard Page must implement all pure virtual methods
   - initializePage()
   - nextId()
   
   For other methods there is default implementation, which can be overwritten.
 */
class WlanWizardPage
{
public:
    //! default value for previousTriggered(), step one step.
    static const int OneStepBackwards = 1;
    static const int SingleResult = 1;
    
    /*!
       Defines common page ids for wizard and page id pools for wizard plugins.
       
       Remember to update trace.properties file upon changes.
     */
    enum PageIds {
        //! No need to change the wizard page.
        PageNone = 0,
        //! Process settings. Start connection and runs ict
        PageProcessSettings,
        //! Generic Error note page
        PageGenericError,
        //! Starting value for WLAN wizard page ids. Defined by WLAN Wizard.
        PageWlanStart = 0x1000,
        //! Starting value for EAP wizard page ids. Defined by EAP Wizard.
        PageEapStart = 0x2000,
        //! Starting value for WPS wizard page ids. Defined by WPS Wizard.
        PageWpsStart = 0x3000,
    };

public:

    /*!
       Creates a visualization of the wizard page and returns ownership of the
       object to the caller. Wizard framework deletes the visualization at the
       desctructor of WlanWizardPrivate implementation. WizardPage control
       object can safely use the same pointer during the life time of the
       control object. This method is called every time when moving to a new
       page, not when user presses "previous" button. showPage() method is
       called in both cases. 
       
       @return pointer to a visualization of the page.
     */
    virtual HbWidget* initializePage() = 0;

    /*!
       This method is called, after a visualization is created with
       initializePage() method, to detect whether next button should be enabled
       or not. It is up to the policy of the page when next button should be
       enabled. This method is called everytime before the current 
       active page is displayed. In other words this method is "aboutToShow()"
       
       Default implementation: wizard page is valid and next button is enabled.
       
       @return validity.       
     */
    virtual bool showPage()
    {
        return true;
    };

    /*!
       If the wizard page requires timer protection and asyncronous time
       lasting operation, return true. This is used in pages e.g. where
       progressbar is shown. The whole point is to initialize the UI before
       starting the operation, so that the progress bar is painted before the
       operation takes place.
       
       In practise this means that if true is returned by this method.
       - WlanWizardPage::startOperation() is called to start the page operation.
       - 1.5sec timer is used to protect the page
       - wizardPage MUST call WlanWizardHelper::nextPage() to trigger entry to
         next wizard page.
       
       @return true if protection is needed.
     */

    virtual bool requiresStartOperation()
    {
        return false;
    };

    /*!
       In case the wizard page needs timer protection to the page use this
       method to start control operation of the page. This method will be called
       after requiresStartOperation() if true is returned with different call
       stack.
       
       See requiresStartOperation(), WlanWizardHelper::nextPage().
     */
    virtual void startOperation() {};

    /*!
       Returns id of next page. This method is called when end user has pressed
       "next" toolbar button or WlanWizardHelper::nextPage() is called.
       
       In case the wizard page does not want to make any movement forwards
       WlanWizardPage::PageNone MUST be returned.
       
       Default implementation: Page ID WlanWizardPage::PageNone is retuned and
       removeFromStack is false.
       
       @param [out] removeFromStack return value to the caller, if this page
       needs to be removed from the stack, which means that this page cannot be
       accessible from the next page using "previous" toolbar button. 
       
       @return page identifier of next wizard page
     */
    virtual int nextId(bool &removeFromStack) const
    {
        removeFromStack = false;
        return WlanWizardPage::PageNone;
    }

    /*!
       This method is called when "previous" button has been pressed.
     
       Default implementation: one page backwards
       
       @return how many steps should be gone backwards. 
     */
    virtual int previousTriggered()
    {
        return OneStepBackwards;
    };

    /*!
       This method is called when "cancel" button has been pressed.
       
       Default implementation: No actions.
     */
    virtual void cancelTriggered() {};
    
signals:
    
public slots:
    
protected:
    
protected slots:

private:

private slots:

private: // data
};

/*! @} */

#endif // WLANWIZARDPAGE_H
