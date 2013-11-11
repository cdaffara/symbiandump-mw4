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
* Description:   Drop shadow handler
*
*/



#include "alfsrvdropshadowhandler.h"
#include "uiacceltk/huidropshadow.h"
#include "alf/alfconstants.h"
#include "alf/alfserverutils.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Checks if the command can be handled by this class
// ---------------------------------------------------------------------------
//
TBool AlfSrvDropShadowHandler::CanHandleCommand( TInt aCommand )
    {
    switch( aCommand )
        {
        case EAlfDropShadowSetOffsetInPolar:
        case EAlfDropShadowSetOffsetInXY:
        case EAlfDropShadowSetColorRgb:
        case EAlfDropShadowSetColorSkin:
        case EAlfDropShadowSetOpacity:
        case EAlfDropShadowSetBlurRadius:
        case EAlfDropShadowSetScale:
            return ETrue;
        default:
            return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Handles the command
// ---------------------------------------------------------------------------
//    
void AlfSrvDropShadowHandler::HandleCommandL( 
        CHuiDropShadow& aDropShadow,
        TInt aCommandId, 
        const TDesC8& aInputBuffer, 
        TDes8& /*aResponse*/ )
    {
    switch( aCommandId )
        {
        case EAlfDropShadowSetOffsetInPolar:
            {
            const TAlfDropShadowOffsetPolarParams* const params = (TAlfDropShadowOffsetPolarParams*)aInputBuffer.Ptr();
            
            THuiMetric huiMetric;
            huiMetric.iMagnitude = params->iDistance.iMagnitude;
            huiMetric.iReferenceTextStyleId = params->iDistance.iReferenceTextStyleId;
            huiMetric.iUnit = params->iDistance.iUnit;
            
            aDropShadow.SetOffset( params->iAngle, huiMetric, params->iTransitionTime );
            }
            break;
            
        case EAlfDropShadowSetOffsetInXY:
            {
            TAlfDropShadowOffsetXYParams* params = (TAlfDropShadowOffsetXYParams*)aInputBuffer.Ptr();
            aDropShadow.iOffsetUnit = params->iOffsetUnit;     
            AlfTimedPointUtility::CopyTimedPoint( params->iOffset, aDropShadow.iOffset );
            }
            break;
            
        case EAlfDropShadowSetColorRgb:
            {
            const TAlfDropShadowColorRgbParams* const params = (TAlfDropShadowColorRgbParams*)aInputBuffer.Ptr();
            aDropShadow.SetColor( params->iColor, params->iTransitionTime );
            }
            break;
            
        case EAlfDropShadowSetColorSkin:
            {
            const TAlfDropShadowColorSkinParams* const params = (TAlfDropShadowColorSkinParams*)aInputBuffer.Ptr();
            aDropShadow.SetColor( params->iID,params->iIndex, params->iTransitionTime );
            }
            break;
            
        case EAlfDropShadowSetOpacity:
            {
            TAlfTimedValue* params = (TAlfTimedValue*)aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue( *params, aDropShadow.iOpacity );
            }
            break;
            
        case EAlfDropShadowSetBlurRadius:
            {
            TAlfTimedValue* params = (TAlfTimedValue*)aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue( *params, aDropShadow.iRadius );
            }
            break;
            
        case EAlfDropShadowSetScale:
            {
            TAlfTimedValue* params = (TAlfTimedValue*)aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue( *params, aDropShadow.iScale );
            }
            break;
            
        default:
            User::Leave( KErrNotSupported );
        }
    
    }

