/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// This file was generated by:
// CdlCompiler dll -n -p..\generated\Nhd_uiaccel -laknlayout2scalable.lib -lcdlengine.lib -s..\..\decodelib\Aknlayout2scalabledecode.cpp -esysteminclude .. UiAccel_LayoutInst_Nhd 0x2001CB5B @..\generated\Nhd_uiaccel\uiaccel_instances.txt
// This is the main source file for a customisation DLL.
// It may be modified manually.

#include "uiaccel_elaf_nhd_normal_pack.h"
#include "aknlayoutscalable_elaf_hnp_c_nhd_prt_normal.h"
#include "uiaccel_elaf_nhd_small_pack.h"
#include "aknlayoutscalable_elaf_hnp_c_nhd_prt_small.h"
#include "uiaccel_elaf_nhd_large_pack.h"
#include "aknlayoutscalable_elaf_hnp_c_nhd_prt_large.h"
#include "uiaccel_abrw_nhd_normal_pack.h"
#include "aknlayoutscalable_abrw_hnp_c_nhd_prt_normal.h"
#include "uiaccel_abrw_nhd_small_pack.h"
#include "aknlayoutscalable_abrw_hnp_c_nhd_prt_small.h"
#include "uiaccel_abrw_nhd_large_pack.h"
#include "aknlayoutscalable_abrw_hnp_c_nhd_prt_large.h"
#include "uiaccel_apac_nhd_normal_pack.h"
#include "uiaccel_apac_nhd_small_pack.h"
#include "uiaccel_apac_nhd_large_pack.h"
#include "uiaccel_elaf_nhd_land_normal_pack.h"
#include "aknlayoutscalable_elaf_hnl_c_nhd_lsc_normal.h"
#include "uiaccel_elaf_nhd_land_small_pack.h"
#include "aknlayoutscalable_elaf_hnl_c_nhd_lsc_small.h"
#include "uiaccel_elaf_nhd_land_large_pack.h"
#include "aknlayoutscalable_elaf_hnl_c_nhd_lsc_large.h"
#include "uiaccel_abrw_nhd_land_normal_pack.h"
#include "aknlayoutscalable_abrw_hnl_c_nhd_lsc_normal.h"
#include "uiaccel_abrw_nhd_land_small_pack.h"
#include "aknlayoutscalable_abrw_hnl_c_nhd_lsc_small.h"
#include "uiaccel_abrw_nhd_land_large_pack.h"
#include "aknlayoutscalable_abrw_hnl_c_nhd_lsc_large.h"
#include "uiaccel_apac_nhd_land_normal_pack.h"
#include "uiaccel_apac_nhd_land_small_pack.h"
#include "uiaccel_apac_nhd_land_large_pack.h"

CDL_ARRAY_START(SCdlCustomisation, KCdlData)
	{
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_Normal_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnp_c_nhd_prt_Normal),
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_Small_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnp_c_nhd_prt_Small),
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_Large_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnp_c_nhd_prt_Large),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_Normal_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnp_c_nhd_prt_Normal),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_Small_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnp_c_nhd_prt_Small),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_Large_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnp_c_nhd_prt_Large),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_Normal_Pack),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_Small_Pack),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_Large_Pack),
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_land_Normal_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnl_c_nhd_lsc_Normal),
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_land_Small_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnl_c_nhd_lsc_Small),
		CDL_CUSTOMISATION(UiAccel_Elaf_Nhd_land_Large_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Elaf_hnl_c_nhd_lsc_Large),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_land_Normal_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnl_c_nhd_lsc_Normal),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_land_Small_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnl_c_nhd_lsc_Small),
		CDL_CUSTOMISATION(UiAccel_Abrw_Nhd_land_Large_Pack),
		CDL_CUSTOMISATION(AknLayoutScalable_Abrw_hnl_c_nhd_lsc_Large),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_land_Normal_Pack),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_land_Small_Pack),
		CDL_CUSTOMISATION(UiAccel_Apac_Nhd_land_Large_Pack),
	}
CDL_ARRAY_END(SCdlCustomisation, KCdlData);

GLREF_D const SCdlMain KCdlMainExport =
	{
	1,
	0,
	&KCdlData
	};

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

static TAny* MainExport()
	{
	return (TAny*)&KCdlMainExport;
	}

const TImplementationProxy ImplementationTable[] =
	{
		{{0x2001cb5b}, ::MainExport}
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

