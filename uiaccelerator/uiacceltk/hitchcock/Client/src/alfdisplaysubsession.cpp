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
* Description:   Sub-session for display
*
*/



#include "alfdisplaysubsession.h"
#include "alf/alfconstants.h"
#include "alf/alfdisplaybackgrounditem.h"
#include "alflogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Opens subsession
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::Open( RAlfClient& aSession, const TRect& aRect, TInt aDisplayType, 
	TUid aBufferUid )
    {
    if (aDisplayType >= CAlfDisplay::EDisplayNormal)       
    	{   	
    	TPckg<TRect> rectPckg(aRect);
    
    	TAlfDisplayCreateParams2 params;
    	params.iWindowGroupId = 0;
    	CCoeEnv* coeStatic = CCoeEnv::Static();
    	if ( coeStatic )
        	{
        	params.iWindowGroupId = coeStatic->RootWin().Identifier();
        	}
    	params.iDisplayType = aDisplayType;
    	params.iBufferUid = aBufferUid;
    
    	TPckg<TAlfDisplayCreateParams2> paramPckg(params);    
    	TAlfImplementationInformation info(EAlfDisplaySubSessionOpen2);
    	TPckgC<TAlfImplementationInformation> infoBuf(info);
    	return CreateSubSession( 
        	aSession,
        	EAlfCreateSubSession,
        	TIpcArgs( &infoBuf, &rectPckg, &paramPckg ));
    	}    
    else // deprecated API
    	{
    	TPckg<TRect> rectPckg(aRect);
    
    	TAlfDisplayCreateParams params;
    	params.iWindowGroupId = 0;
    	CCoeEnv* coeStatic = CCoeEnv::Static();
    	if ( coeStatic )
        	{
        	params.iWindowGroupId = coeStatic->RootWin().Identifier();
        	}
    	params.iDisplayType = aDisplayType;
    
    	TPckg<TAlfDisplayCreateParams> paramPckg(params);    
    	TAlfImplementationInformation info(EAlfDisplaySubSessionOpen);
    	TPckgC<TAlfImplementationInformation> infoBuf(info);
    	return CreateSubSession( 
        	aSession,
        	EAlfCreateSubSession,
        	TIpcArgs( &infoBuf, &rectPckg, &paramPckg ));
    	}
    }
 
// ---------------------------------------------------------------------------
// Closes subsession
// ---------------------------------------------------------------------------
//   
void RAlfDisplaySubSession::Close()
    {
    CloseSubSession( EAlfCloseSubSession );
    }
    
// ---------------------------------------------------------------------------
// Display background
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetClearBackground( TInt aClearBackground )
    {
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfDisplaySetClearBackground, aClearBackground ) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetClearBackground return error %d", err )
        }
    return err;
    }
 
// ---------------------------------------------------------------------------
// Sets visible area
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetVisibleArea( const TRect& aRect, TBool aForce)
    {
    TPckg<TRect> rectPckg(aRect);
    TInt err;
    if(aForce)
        {
        err = SendReceive(
            EAlfDoSubSessionCmd, 
            TIpcArgs(EAlfDisplayForceSetVisibleArea, &rectPckg) );
        }
    else
        {
        err = SendReceive(
            EAlfDoSubSessionCmd, 
            TIpcArgs(EAlfDisplaySetVisibleArea, &rectPckg) );        
        }

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetVisibleArea return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Returns visible area
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::VisibleArea( TRect& aRect ) const
    {
    TPckg<TRect> rectPckg(aRect);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplayVisibleArea,0,&rectPckg) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::VisibleArea return error %d", err )
        }
    return err;
    }


// ---------------------------------------------------------------------------
// Sets display dirty.
// ---------------------------------------------------------------------------
//
void RAlfDisplaySubSession::SetDirty()
    {
    Send/*Receive*/(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfDisplaySetDirty ) );
    __ALFLOGSTRING( "RAlfDisplaySubSession::SetDirty sent" );        
    }
    
// ---------------------------------------------------------------------------
// Sets rendering quality
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetQuality(TAlfQuality aRenderingQuality)
    {
    TPckg<TAlfQuality> inPckg(aRenderingQuality);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplaySetQuality, &inPckg) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetQuality return error %d", err )
        }
    return err;        
    }

// ---------------------------------------------------------------------------
// Returns rendering quality
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::Quality(TAlfQuality& aRenderingQuality) const
    {
    TPckg<TAlfQuality> retPckg(aRenderingQuality);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplayQuality,0,&retPckg) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::Quality return error %d", err )
        }
    return err;        
    }

// ---------------------------------------------------------------------------
// Sets flag to specify if depth test is to be used
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetUseDepth(TBool aUseDepth)
    {
    TPckg<TBool> inPckg(aUseDepth);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplaySetUseDepth, &inPckg) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetUseDepth return error %d", err )
        }
    return err;                
    }

// ---------------------------------------------------------------------------
// Sets usage hint for the display.
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetUsage(TUint aUsageHint)
    {
    TPckg<TUint> inPckg(aUsageHint);

    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplaySetUsage,&inPckg) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetUsageHint return error %d", err )
        }
    return err;        
        
    }

// ---------------------------------------------------------------------------
// Sets background items
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SetBackgroundItemsL(const RArray<TAlfDisplayBackgroundItem>& aItems)
    {
    HBufC8* buffer = ExternalizeLC(aItems);

    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplaySetBackgroundItems, buffer) );
        
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetBackgroundItemsL error %d", err )
        }
        
    CleanupStack::PopAndDestroy( buffer );        
    return err;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TPckg<TAdvancedPointerEvent> inPckg(*aPointerEvent.AdvancedPointerEvent());

    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplayHandlePointerEvent,&inPckg) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::HandlePointerEventL return error %d", err )
        }
    return err;        
    }

// ---------------------------------------------------------------------------
// Suppresses automatic fading
// ---------------------------------------------------------------------------
//
TInt RAlfDisplaySubSession::SuppressAutomaticFading( TBool aSuppress )
    {
    TPckg<TBool> inPckg(aSuppress);

    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfDisplaySuppressAutomaticFading, &inPckg ) );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SuppressAutomaticFading return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RAlfDisplaySubSession::SetClientWindowForDrawingL(TInt aWindowGroupId, TInt aClientWindowHandle, TInt aVisualHandle)
	{
	TInt3 data(aWindowGroupId, aClientWindowHandle, aVisualHandle);
    TPckg<TInt3> inPckg(data);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfDisplaySetClientWindowForDrawing, &inPckg) );
        
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfDisplaySubSession::SetClientWindowForDrawing return error %d", err )
        }
  User::LeaveIfError(err);
	}
