/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application Sever
*
*/


#include "alf/alftimedvalue.h"
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <f32file.h>
#include <alf/ftokenclient.h>
#include <uiacceltk/huimetric.h>
#include <alf/alfmetric.h>

#include "alf/alfserverutils.h"
#include "alf/alfextensionfactory.h"
#include "alflogger.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedValueUtility::CopyTimedValue(TAlfTimedValue& aSrc, 
    THuiTimedValue& aDst, MAlfInterfaceProvider& aResolver )
    {    
    if ( aSrc.Flags()&EAlfMappingFunctionChanged )
        {
        
        if (aSrc.MappingFunctionIdentifier() != KErrNotFound && 
            aSrc.MappingFunctionIdentifier() != 0)
            {
            TAny* any = NULL;
            TRAP_IGNORE(any = aResolver.GetInterfaceL( 
                    EHuiInterfaceMappingFunction, 
                    aSrc.MappingFunctionIdentifier() ))

            if (any)
                {
                MHuiMappingFunction* mappingFunction = 
                    static_cast<MHuiMappingFunction*> (any);
                aDst.SetFunction(mappingFunction);                    
                }
            }
        else
            {
            aDst.SetFunction(NULL);    
            }          
       
        }
        
    AlfTimedValueUtility::CopyTimedValue(aSrc,aDst);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedValueUtility::CopyTimedValue(TAlfTimedValue& aSrc, 
    THuiTimedValue& aDst)
    {

#ifdef _ALF_LOGGING
    if (aSrc.Flags()==EAlfValueFlagsNone)
        {
        __ALFLOGSTRING("AlfTimedValue: No changes (EAlfValueFlagsNone), assignment does not have effect")
        }
#endif

    if (aSrc.Flags()&EAlfValueNowChanged)    
        {
        aDst.Set(aSrc.ValueNow());        
        }
    
    if (aSrc.Flags()&EAlfValueTargetChanged)
        {
        if (aSrc.Flags()&EAlfSpeedChanged && aSrc.Speed() != 0)
            {
            aDst.SetWithSpeed(aSrc.Target(), aSrc.Speed());    
            }
        else
            {
            aDst.Set(aSrc.Target(), aSrc.TimeToTargetinMilliSeconds());                        
            }
        }
         
    // Currently we assume that interpolationstyle needs no change
    // if default requested, we further assume that is this safe assumption. 
    if (aSrc.Flags()&EAlfInterpolationStyleChanged && aSrc.Style())
        {
        aDst.SetStyle(static_cast<THuiInterpolationStyle>(aSrc.Style()-EAlfTimedValueStyleNone));
        }
    
    aSrc.Flags() = EAlfValueFlagsNone;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedValueUtility::CopyTimedValue(THuiTimedValue& aSrc, 
    TAlfTimedValue& aDst)
    {
    TAlfTimedValue newValue;    
    newValue.SetValueNow(aSrc.Now());
    newValue.SetTarget(aSrc.Target(), aSrc.RemainingTime());
    newValue.SetStyle(TAlfInterpolationStyle(aSrc.Style() + EAlfTimedValueStyleNone));
    aDst = newValue;            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedPointUtility::CopyTimedPoint(TAlfTimedPoint& aSrc, 
    THuiTimedPoint& aDst, MAlfInterfaceProvider& aResolver)
    {
    AlfTimedValueUtility::CopyTimedValue(aSrc.iX, aDst.iX, aResolver);
    AlfTimedValueUtility::CopyTimedValue(aSrc.iY, aDst.iY, aResolver);        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedPointUtility::CopyTimedPoint(TAlfTimedPoint& aSrc, 
    THuiTimedPoint& aDst)
    {
    AlfTimedValueUtility::CopyTimedValue(aSrc.iX, aDst.iX);
    AlfTimedValueUtility::CopyTimedValue(aSrc.iY, aDst.iY);                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfTimedPointUtility::CopyTimedPoint(THuiTimedPoint& aSrc, 
    TAlfTimedPoint& aDst)
    {
    AlfTimedValueUtility::CopyTimedValue(aSrc.iX, aDst.iX);
    AlfTimedValueUtility::CopyTimedValue(aSrc.iY, aDst.iY);        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void AlfFileTokenUtility::OpenFileForTokenL(RFile& aFile, const TDesC8& aToken)
    {
    RFTokenClient client;
    User::LeaveIfError(client.Connect());
    
    CleanupClosePushL(client);
    User::LeaveIfError(client.OpenFileForToken(aFile, aToken));    
    CleanupStack::PopAndDestroy();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfBoxMetricUtility::CopyMetric(TAlfBoxMetric& aSrc, THuiBoxMetric& aDst)
    {
    AlfMetricUtility::CopyMetric(aSrc.iTop, aDst.iTop);        
    AlfMetricUtility::CopyMetric(aSrc.iBottom, aDst.iBottom);        
    AlfMetricUtility::CopyMetric(aSrc.iLeft, aDst.iLeft);        
    AlfMetricUtility::CopyMetric(aSrc.iRight, aDst.iRight);                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfBoxMetricUtility::CopyMetric(THuiBoxMetric& aSrc, TAlfBoxMetric& aDst)
    {
    AlfMetricUtility::CopyMetric(aSrc.iTop, aDst.iTop);        
    AlfMetricUtility::CopyMetric(aSrc.iBottom, aDst.iBottom);        
    AlfMetricUtility::CopyMetric(aSrc.iLeft, aDst.iLeft);        
    AlfMetricUtility::CopyMetric(aSrc.iRight, aDst.iRight);                        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfXYMetricUtility::CopyMetric(TAlfXYMetric& aSrc, THuiXYMetric& aDst)
    {
    AlfMetricUtility::CopyMetric(aSrc.iX, aDst.iX);        
    AlfMetricUtility::CopyMetric(aSrc.iY, aDst.iY);                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfXYMetricUtility::CopyMetric(THuiXYMetric& aSrc, TAlfXYMetric& aDst)
    {
    AlfMetricUtility::CopyMetric(aSrc.iX, aDst.iX);        
    AlfMetricUtility::CopyMetric(aSrc.iY, aDst.iY);                        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfMetricUtility::CopyMetric(TAlfMetric& aSrc, THuiMetric& aDst)
    {
    aDst.iMagnitude = aSrc.iMagnitude;    
    aDst.iUnit = aSrc.iUnit;        
    aDst.iReferenceTextStyleId = aSrc.iReferenceTextStyleId;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void AlfMetricUtility::CopyMetric(THuiMetric& aSrc, TAlfMetric& aDst)
    {
    aDst.iMagnitude = aSrc.iMagnitude;    
    aDst.iUnit = aSrc.iUnit;        
    aDst.iReferenceTextStyleId = aSrc.iReferenceTextStyleId;                
    }

