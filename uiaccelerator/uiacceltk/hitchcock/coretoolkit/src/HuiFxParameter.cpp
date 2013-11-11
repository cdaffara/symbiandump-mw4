/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/



#include "HuiFxParameter.h"

MHuiFxParameter::~MHuiFxParameter()
    {
    }

static void DoCleanupFxParameter(TAny* aAny)
    {
    MHuiFxParameter* parameter = static_cast<MHuiFxParameter*>(aAny);
    delete parameter;
    }

void CleanupFxParameterL(MHuiFxParameter* aParameter)
    {
    CleanupStack::PushL(TCleanupItem(DoCleanupFxParameter, aParameter));
    }

void ReRefInternal(TReal32 *aValue, TReal32 **aValue2)
    {
    *aValue2 = aValue;
    }
void ReRefInternal(TRgb *aValue, TRgb **aValue2)
    {
    *aValue2 = aValue;
    }
void ReRefInternal(TReal32 *aValue, TRgb **aValue2)
    {
    // empty because assignment from TReal to TRgb fails
    }
void ReRefInternal(TRgb *aValue, TReal32 **aValue2)
    {
    // empty because assignment from TRgb to TReal32 fails
    }
