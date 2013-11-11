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
 *   Stub for EAP WIZARD testing: EAP QT configuration validator interface
 *
 */

#ifndef EAPQTVALIDATOR_STUB_H
#define EAPQTVALIDATOR_STUB_H

// System includes
#include <eapqtvalidator.h>

// User includes

// Forward declarations

class HbLineEdit;

// External data types

// Constants

// Class declaration

/*!
 * @addtogroup group_eap_config_api
 * @{
 */

class EapQtValidatorStub : public EapQtValidator 
{
   
public:

    EapQtValidatorStub(EapQtValidator::Status status);
    ~EapQtValidatorStub();
    virtual EapQtValidator::Status validate(const QVariant & value);
    void updateEditor(HbLineEdit* const edit );
    void setReturnValue(EapQtValidator::Status status);
    
private:
    EapQtValidator::Status mStatus;
    Q_DISABLE_COPY(EapQtValidatorStub)
};

/*! @} */

#endif
