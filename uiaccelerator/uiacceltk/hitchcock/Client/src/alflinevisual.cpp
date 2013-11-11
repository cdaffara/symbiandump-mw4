/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "alf/alflinevisual.h"
#include "alf/alfcontrol.h"
#include "alf/alfimage.h"
#include "alf/alftexture.h"
#include "alf/alfcurvepath.h"
#include "alf/alfgencomponent.h"
#include "alf/alfmappingfunctions.h"
#include "alflogger.h"
#include "alf/alfconstants.h"
#include "alfskinnableimage.h"
#include "alf/alfenv.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfLineVisual::TLineVisualPrivateData
    {
    TAlfTimedValue iThickness;
    TAlfTimedValue iShadowThickness;
    TAlfTimedValue iStartPos;
    TAlfTimedValue iEndPos;
    CAlfCurvePath* iPath;
    TAlfOwnership iOwnership;
    CAlfSkinnableImage* iImage;
    };

// ======== MEMBER FUNCTIONS ========

EXPORT_C CAlfLineVisual* CAlfLineVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfLineVisual* text = STATIC_CAST(CAlfLineVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeLine, aParentLayout));
    return text;
    }

EXPORT_C CAlfLineVisual::CAlfLineVisual()
    {
    }

EXPORT_C void CAlfLineVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfVisual::ConstructL(aOwner);
    
    iLineVisualData = new (ELeave) TLineVisualPrivateData;
    
    iLineVisualData->iPath = NULL;
    iLineVisualData->iImage = NULL;
    
    iLineVisualData->iImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());
    }

EXPORT_C CAlfLineVisual::~CAlfLineVisual()
    {
    if ( iLineVisualData )
        {
        delete iLineVisualData->iImage;
        iLineVisualData->iImage = NULL;
        
        if ( iLineVisualData->iOwnership == EAlfHasOwnership )
            {
            delete iLineVisualData->iPath;
            }
        iLineVisualData->iPath = NULL;
        }
    delete iLineVisualData;
    iLineVisualData = NULL;
    }

EXPORT_C void CAlfLineVisual::SetPath(CAlfCurvePath* aPath, 
                                      TAlfOwnership aOwnership)
    {
    TInt curvePathHandle = 0;
    if ( aPath )
        {
        MAlfMappingFunction* mappingFunction = aPath;
        curvePathHandle = mappingFunction->MappingFunctionIdentifier();
        } 


    TInt2 params( curvePathHandle, aOwnership );
    TPckgC<TInt2> inBuf( params );

    TInt err = Comms()->DoCmdNoReply(EAlfLineVisualSetPath, inBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetPath panic error %d", err )
        USER_INVARIANT();
        }
    
    // Destroy the old one if owned.
    if ( iLineVisualData->iOwnership == EAlfHasOwnership )
        {
        delete iLineVisualData->iPath;
        }
    
    iLineVisualData->iPath = aPath;
    iLineVisualData->iOwnership = aOwnership;
    }

EXPORT_C CAlfCurvePath* CAlfLineVisual::Path()
    {
    return iLineVisualData->iPath;
    }

EXPORT_C void CAlfLineVisual::SetImage(const TAlfImage& aImage)
    {
    iLineVisualData->iImage->SetImage(aImage);// This texturizes skin graphics if needed
    
    TAlfImageParams params(iLineVisualData->iImage->Image());

    TPckgC<TAlfImageParams> buf(params);

    TInt err = Comms()->DoCmdNoReply(EAlfLineVisualSetImage, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetImage ignore error %d", err )
        }
    }
    
EXPORT_C const TAlfImage& CAlfLineVisual::Image() const
    {
    return iLineVisualData->iImage->Image();
    }

EXPORT_C void CAlfLineVisual::SetAlphaFunction(MAlfMappingFunction* aFunction)
    {
    TInt mappingFunctionHandle = 0;
    if ( aFunction )
        {
        mappingFunctionHandle = aFunction->MappingFunctionIdentifier();
        }
        
    TPckgC<TInt> inBuf( mappingFunctionHandle );
    
    TInt err = Comms()->DoCmdNoReply(EAlfLineVisualSetAlphaFunction, inBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetAlphaFunction panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C void CAlfLineVisual::SetWidthFunction(MAlfMappingFunction* aFunction)
    {
    TInt mappingFunctionHandle = 0;
    if ( aFunction )
        {
        mappingFunctionHandle = aFunction->MappingFunctionIdentifier();
        }
    TPckgC<TInt> inBuf( mappingFunctionHandle );
    
    TInt err =Comms()->DoCmdNoReply(EAlfLineVisualSetWidthFunction, inBuf );
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetWidthFunction panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C const TAlfTimedValue& CAlfLineVisual::Thickness() const
    {
    TPckg<TAlfTimedValue> buf(iLineVisualData->iThickness);
    TInt err = Comms()->DoSynchronousCmd( EAlfLineVisualGetThickness, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::Thickness panic error %d", err )
        USER_INVARIANT();
        }
    
    return iLineVisualData->iThickness;
    }

EXPORT_C void CAlfLineVisual::SetThickness( const TAlfTimedValue& aThickness )
    {
    TPckgC<TAlfTimedValue> buf(aThickness);

    TInt err = Comms()->DoCmdNoReply( EAlfLineVisualSetThickness, buf );

    if ( err == KErrNone )
        {
        iLineVisualData->iThickness = aThickness;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetThickness panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C const TAlfTimedValue& CAlfLineVisual::ShadowThickness() const
    {
    TPckg<TAlfTimedValue> buf(iLineVisualData->iShadowThickness);
    TInt err = Comms()->DoSynchronousCmd( EAlfLineVisualGetShadowThickness, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::ShadowThickness panic error %d", err )
        USER_INVARIANT();
        }
    
    return iLineVisualData->iShadowThickness;
    }

EXPORT_C void CAlfLineVisual::SetShadowThickness( const TAlfTimedValue& aShadowThickness )
    {
    TPckgC<TAlfTimedValue> buf(aShadowThickness);
    
    TInt err = Comms()->DoCmdNoReply( EAlfLineVisualSetShadowThickness, buf );

    if ( err == KErrNone )
        {
        iLineVisualData->iShadowThickness = aShadowThickness;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetShadowThickness panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C const TAlfTimedValue& CAlfLineVisual::StartPos() const
    {
    TPckg<TAlfTimedValue> buf(iLineVisualData->iStartPos);
    TInt err = Comms()->DoSynchronousCmd( EAlfLineVisualGetStartPos, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::StartPos panic error %d", err )
        USER_INVARIANT();
        }
    
    return iLineVisualData->iStartPos;
    }

EXPORT_C void CAlfLineVisual::SetStartPos( const TAlfTimedValue& aStartPos )
    {
    TPckgC<TAlfTimedValue> buf(aStartPos);

    TInt err = Comms()->DoCmdNoReply( EAlfLineVisualSetStartPos, buf );

    if ( err == KErrNone )
        {
        iLineVisualData->iStartPos = aStartPos;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetStartPos panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C const TAlfTimedValue& CAlfLineVisual::EndPos() const
    {
    TPckg<TAlfTimedValue> buf(iLineVisualData->iEndPos);
    TInt err = Comms()->DoSynchronousCmd( EAlfLineVisualGetEndPos, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::EndPos panic error %d", err )
        USER_INVARIANT();
        }
    
    return iLineVisualData->iEndPos;
    }

EXPORT_C void CAlfLineVisual::SetEndPos( const TAlfTimedValue& aEndPos )
    {
    TPckgC<TAlfTimedValue> buf(aEndPos);

    TInt err = Comms()->DoCmdNoReply( EAlfLineVisualSetEndPos, buf );

    if ( err == KErrNone )
        {
        iLineVisualData->iEndPos = aEndPos;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetEndPos panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C void CAlfLineVisual::SetColor(const TRgb& aColor)
    {
    TPckgC<TRgb> buf(aColor);

    TInt err = Comms()->DoCmdNoReply(EAlfLineVisualSetColor, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLineVisual::SetColor ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfLineVisual::RemoveAndDestroyAllD()
    {
    CAlfVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLineVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfLineVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfVisual::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLineVisual::DoRemoveAndDestroyAllD()
    {
    CAlfVisual::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLineVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    


