/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Definitions and declarations that affect XML Engine Core
*
*/


#ifndef XMLSCHEMAS_GLOBALS_H
#define XMLSCHEMAS_GLOBALS_H

typedef struct _xmlSchemaTypesGlobalData xmlSchemaTypesGlobalData;
typedef xmlSchemaTypesGlobalData* xmlSchemaTypesGlobalDataPtr;

#include "Libxml2_schemasInternals.h"

struct _xmlSchemaTypesGlobalData{
    //int xmlSchemaTypesInitialized; //defined as separate global variable
    xmlHashTablePtr xmlSchemaTypesBank;

    /*
     * Basic types
     */
    xmlSchemaTypePtr xmlSchemaTypeStringDef;
    xmlSchemaTypePtr xmlSchemaTypeAnyTypeDef;
    xmlSchemaTypePtr xmlSchemaTypeAnySimpleTypeDef;
    xmlSchemaTypePtr xmlSchemaTypeDecimalDef;
    xmlSchemaTypePtr xmlSchemaTypeDatetimeDef;
    xmlSchemaTypePtr xmlSchemaTypeDateDef;
    xmlSchemaTypePtr xmlSchemaTypeTimeDef;
    xmlSchemaTypePtr xmlSchemaTypeGYearDef;
    xmlSchemaTypePtr xmlSchemaTypeGYearMonthDef;
    xmlSchemaTypePtr xmlSchemaTypeGDayDef;
    xmlSchemaTypePtr xmlSchemaTypeGMonthDayDef;
    xmlSchemaTypePtr xmlSchemaTypeGMonthDef;
    xmlSchemaTypePtr xmlSchemaTypeDurationDef;
    xmlSchemaTypePtr xmlSchemaTypeFloatDef;
    xmlSchemaTypePtr xmlSchemaTypeBooleanDef;
    xmlSchemaTypePtr xmlSchemaTypeDoubleDef;
    xmlSchemaTypePtr xmlSchemaTypeHexBinaryDef;
    xmlSchemaTypePtr xmlSchemaTypeBase64BinaryDef;
    xmlSchemaTypePtr xmlSchemaTypeAnyURIDef;

    /*
     * Derived types
     */
    xmlSchemaTypePtr xmlSchemaTypePositiveIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNonPositiveIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNegativeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeNonNegativeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeIntegerDef;
    xmlSchemaTypePtr xmlSchemaTypeLongDef;
    xmlSchemaTypePtr xmlSchemaTypeIntDef;
    xmlSchemaTypePtr xmlSchemaTypeShortDef;
    xmlSchemaTypePtr xmlSchemaTypeByteDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedLongDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedIntDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedShortDef;
    xmlSchemaTypePtr xmlSchemaTypeUnsignedByteDef;
    xmlSchemaTypePtr xmlSchemaTypeNormStringDef;
    xmlSchemaTypePtr xmlSchemaTypeTokenDef;
    xmlSchemaTypePtr xmlSchemaTypeLanguageDef;
    xmlSchemaTypePtr xmlSchemaTypeNameDef;
    xmlSchemaTypePtr xmlSchemaTypeQNameDef;
    xmlSchemaTypePtr xmlSchemaTypeNCNameDef;
    xmlSchemaTypePtr xmlSchemaTypeIdDef;
    xmlSchemaTypePtr xmlSchemaTypeIdrefDef;
    xmlSchemaTypePtr xmlSchemaTypeIdrefsDef;
    xmlSchemaTypePtr xmlSchemaTypeEntityDef;
    xmlSchemaTypePtr xmlSchemaTypeEntitiesDef;
    xmlSchemaTypePtr xmlSchemaTypeNotationDef;
    xmlSchemaTypePtr xmlSchemaTypeNmtokenDef;
    xmlSchemaTypePtr xmlSchemaTypeNmtokensDef;
}; // struct _xmlSchemaGlobalData

#endif /* XMLSCHEMAS_GLOBALS_H */

