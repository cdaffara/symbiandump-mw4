/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CODEGENCONSTS_H
#define CODEGENCONSTS_H

#include <string>
using namespace std;

// includes
const string KIncludeLayoutInstanceHeaderScalableDef("#include <aknlayout2scalabledef.h>");
// directories
const string KDirEpocSysHeader("/epoc32/include/");
const string KDirDomainSysHeader("/epoc32/include/platform/mw/");

// parameter namesy
const string KParamNameBase("aIndex_");
const string KParamNameB(KParamNameBase + "B");
const string KParamNameL(KParamNameBase + "l");
const string KParamNameR(KParamNameBase + "r");
const string KParamNameNumberOfLinesShown("aNumberOfLinesShown");
const string KParamParentRect("aParentRect");
const string KParamLineIndex("aLineIndex");
const string KParamComponentId("aComponentId");
const string KParamOptionIndex("aVariety");
const string KParamRowIndex("aRow");
const string KParamColIndex("aCol");

// types
const string KTypeTextLineLayout("TAknTextLineLayout");
const string KTypeWindowLineLayout("TAknWindowLineLayout");
const string KTypeLayoutTableLimits("TAknLayoutTableLimits");
const string KTypeMultiLineTextLayout("TAknMultiLineTextLayout");
const string KTypeRect("const TRect&");
const string KTypeInt("TInt");

const string KTypeScreenComponentLayout("TAknScreenComponentLayout");
const string KTypeContainerComponentLayout("TAknContainerComponentLayout");
const string KTypePaneComponentLayout("TAknPaneComponentLayout");
const string KTypeGraphicComponentLayout("TAknGraphicComponentLayout");

const string KTypeWindowComponentLayout("TAknWindowComponentLayout");
const string KTypeTextComponentLayout("TAknTextComponentLayout");
const string KTypeComponentTableLimits("TAknComponentTableLimits");

const string KTypeLayoutScalableTableLimits("TAknLayoutScalableTableLimits");
const string KTypeLayoutScalableParameterLimits("TAknLayoutScalableParameterLimits");

const string KTypeLayoutScalableComponentType("TAknLayoutScalableComponentType");

// function names
const string KFuncMultiline("Multiline_");
const string KFuncLimitsSuffix("_Limits");
const string KFuncParamLimitsSuffix("_ParamLimits");

const string KFuncWindowLine("WindowLine");
const string KFuncTextLine("TextLine");
const string KFuncWindowTable("WindowTable");
const string KFuncTextTable("TextTable");
const string KFuncTableLimits("TableLimits");
const string KFuncParamLimits("ParameterLimits");
const string KFuncParamLimitsTable("ParameterLimitsTable");

const string KFuncGetComponentTypeById("GetComponentTypeById");
const string KFuncGetParamLimitsById("GetParamLimitsById");
const string KFuncGetWindowComponentById("GetWindowComponentById");
const string KFuncGetTextComponentById("GetTextComponentById");


// zoom
const string KDefaultZoomInstanceName("Normal");


#endif
