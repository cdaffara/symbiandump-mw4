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
* Stub for EAP WIZARD testing: EAP QT configuration validator interface.
*/

#ifndef EAPQTVALIDATOR_H
#define EAPQTVALIDATOR_H

// System includes
#include <QVariant>

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

/*
 * Eap Qt Validator interface. This interface provides a method to configure
 * the editor to use required constraints, hints, editor classes and so one
 * depending on the EAP type and the configuration identifier.
 * See updateEditor() method for further details.
 * 
 * validate() method checks that the content and format are valid for
 * given configuration parameter.
 * 
 * An instance of a validator is created with
 * EapQtConfigInterface::validatorEap() method.
 */

class EapQtValidator
{
    
public:

    // Data types

    /*
     * Validation status. Can be extended.
     */
    enum Status {
        /// Ok
        StatusOk = 0,
        /// Content is invalid
        StatusInvalid,
        /// The length is not valid
        StatusInvalidLength,
        /// Invalid characters detected
        StatusInvalidCharacters,
        /// Input is too short
        StatusTooShort, 
        /// Input is too long
        StatusTooLong,  
    };

    EapQtValidator() {};
    virtual ~EapQtValidator() {};

    /*!
     * Validates the given input \a value.
     *
     * @param value Value to be validated
     * 
     * @return Status code.
     */
    virtual Status validate(const QVariant &value) = 0;
   
    /*!
     * Sets required configurations to the line edit.
     * 
     * Configurations depend on given EAP type and the configuration
     * identifire which was used to instantiate the validator.
     * 
     * @param edit LineEdit to be updated.
     */
    virtual void updateEditor(HbLineEdit* const edit) = 0;
    
private:

    Q_DISABLE_COPY(EapQtValidator)
};

/*! @} */

#endif
