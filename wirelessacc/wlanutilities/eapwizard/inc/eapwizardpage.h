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
 *   EAP Wizard: API for EAP specific wizard pages.
 *
 */

#ifndef EAPWIZARD_PAGE_H
#define EAPWIZARD_PAGE_H

// System includes
#include <QObject>

// User includes
#include "wlanwizardpage.h"

// Forward declarations
class EapWizardPrivate;
class HbDocumentLoader;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
   
   Interface for EAP specific wizard pages.
 */

// Class declaration

class EapWizardPage : public QObject, public WlanWizardPage
{
    Q_OBJECT
    
public:
    /*!
       Page IDs for EAP Wizard.
     */
    enum EapPageIds {
        //! Outer Type selection
        PageOuterType = WlanWizardPage::PageEapStart,
        /*! Certificate Authority certificate selection for EAP-TTLS, EAP-TLS
            and PEAP.
         */
        PageCertificateCa,
        //! User Authority certificate selection for EAP-TLS
        PageCertificateUser,
        //! EAP Identity for TLS methods, EAP-TTLS, EAP-TLS, PEAP
        PageIdentity,
        //! EAP Inner type selection for PEAP
        PageInnerTypePeap,
        //! EAP Inner type selection for EAP-TTLS
        PageInnerTypeEapTtls,
        //! New PAC Store password for EAP-FAST
        PageNewPacStorePassword,
        //! PAC store confirmation for EAP-FAST
        PagePromptPacStorePassword,
        //! Username and password page
        PageUsernamePassword,
    };
public:
    explicit EapWizardPage(EapWizardPrivate* parent);
    virtual ~EapWizardPage();
    
public: // From WlanWizardPage
    virtual HbWidget* initializePage() = 0;
    virtual int nextId(bool &removeFromStack) const;
    virtual int nextId() const = 0;

signals:
        
public slots:

protected:
    void loadDocmlSection(
        HbDocumentLoader *loader,
        Qt::Orientation orientation,
        const QString &filename,
        const QString &portraitSection,
        const QString &landscapeSection) const;
    
protected slots:

protected: // data
    //! Pointer to Eap Wizard private implementation
    EapWizardPrivate* mWizard;

private:
    Q_DISABLE_COPY(EapWizardPage)

private slots:
    
private: // data
    
};

/*! @} */

#endif
