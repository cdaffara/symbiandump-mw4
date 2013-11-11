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
* EAP method validator: Stub or EapWizard testing
*/

#include "eapqtvalidator_stub.h"

EapQtValidatorStub::EapQtValidatorStub(
    EapQtValidator::Status status) :
    mStatus(status)
{
}

EapQtValidatorStub::~EapQtValidatorStub()
{
}

EapQtValidator::Status EapQtValidatorStub::validate(const QVariant & value)
{
    Q_UNUSED(value);
    return mStatus;
}

void EapQtValidatorStub::updateEditor(HbLineEdit* const edit)
{
    Q_UNUSED(edit);
}

void EapQtValidatorStub::setReturnValue(EapQtValidator::Status status)
{
    mStatus = status;
}
