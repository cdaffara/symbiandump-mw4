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
 *   WPS Wizard API
 *
 */

#ifndef WPSWIZARD_H_
#define WPSWIZARD_H_


// System includes
#include <QObject>

// User includes
#include "wlanwizardplugin.h"

// Forward declarations
class WlanWizardHelper;
class WpsWizardPrivate;

// External data types
// Constants


//Export Declaration
#ifdef BUILD_WPS_WIZARD_DLL
#define WPS_WIZARD_EXPORT Q_DECL_EXPORT
#else
#define WPS_WIZARD_EXPORT Q_DECL_IMPORT
#endif

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WPS_WIZARD_EXPORT WpsWizard: public QObject, public WlanWizardPlugin
{
    Q_OBJECT
   
public:
    
   explicit WpsWizard( WlanWizardHelper *wizardHelper);
    ~WpsWizard();

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WpsWizard)

private slots:

private:   
	
    //data 
    
    //! pointer to the private implementation class.
    QScopedPointer<WpsWizardPrivate> d_ptr;
    
    friend class WpsWizardPrivate;
    friend class TestWlanWizardContext;
};

#endif /* WPSWIZARD_H_ */
