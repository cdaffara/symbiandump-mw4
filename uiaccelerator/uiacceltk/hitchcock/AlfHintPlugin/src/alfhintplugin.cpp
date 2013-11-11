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
* Description:   interface for creating custom extensions
*
*/
 

#include <s32mem.h> //RDesReadStream
#include <graphics/commandbuffer.h>
#include <alfhintpluginuids.h>
#include <alf/alfhintplugin.h>

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfHintGraphic::CAlfHintGraphic()
	{
	}

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfHintGraphic::~CAlfHintGraphic()
	{
	iMsgDataBuf.Close();
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfHintGraphic* CAlfHintGraphic::NewL(  )
	{
	CAlfHintGraphic* self = new(ELeave) CAlfHintGraphic;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfHintGraphic::ConstructL()
    {
    /*iMsgDataBuf.ReAllocL(0x100);
    RDesWriteStream out(iMsgDataBuf);
    TPckgBuf<TInt32> tag(aWindowHandle);
    out.WriteL( tag );
    out.CommitL();*/
    
    BaseConstructL( KAlfHintDrawerInstanceUid,  KAlfHintDrawerTypeUid, iMsgDataBuf );
    //iGc = aGc;
    

    }
EXPORT_C void CAlfHintGraphic::SetGc( CWindowGc* aGc )
    {
    iGc = aGc;
    }
// ---------------------------------------------------------------------------
// HandleMessage
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfHintGraphic::HandleMessage(const TDesC8& /*aData*/)
	{
	}

// ---------------------------------------------------------------------------
// OnReplace
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfHintGraphic::OnReplace()
	{
	}

// ---------------------------------------------------------------------------
// ShareGlobally
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfHintGraphic::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

// ---------------------------------------------------------------------------
// UnShareGlobally
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfHintGraphic::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

// ---------------------------------------------------------------------------
// Share
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfHintGraphic::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

// ---------------------------------------------------------------------------
// UnShare
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfHintGraphic::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}

// ---------------------------------------------------------------------------
// InsertTagL
// ---------------------------------------------------------------------------
//
TInt CAlfHintGraphic::InsertTagL( TInt aTag  )
    {
    TRect aBoundingRectangle(0,0,360,640);
    TInt aLayerId = 0;
    if ( aTag >= 0 && aTag < EAlfContentLastTag )
        {
        iMsgDataBuf.ReAllocL(0x100);
        RDesWriteStream out(iMsgDataBuf);
        TPckgBuf<TUint8> tag(aTag);
        TPckgBuf<TUint8> zero(0);
        TPckgBuf<TRect> rect(aBoundingRectangle); 
        TPckgBuf<TInt32> layerId(aLayerId);
        out.WriteL( tag );
        out.WriteL( zero );
        out.WriteL( rect );
        out.WriteL( layerId );
        out.CommitL();
        const TUid drawerCrpUid = KAlfHintDrawerInstanceUid;
        const TWsGraphicId KDrawerCrpId(drawerCrpUid);

        static_cast<CWindowGc*>(iGc)->DrawWsGraphic(KDrawerCrpId, TRect(0,0,360,640), iMsgDataBuf );
        out.Close();
        }
    else
        {
        return KErrNotSupported;
        }
    return KErrNone;
    }

//
// This version of InsertTagL() is meant for subwindow effects. 
//
TInt CAlfHintGraphic::InsertTagL( TInt aTag, TRect aBoundingRectangle, TInt aLayerId )
    {
    if ( aTag >= 0 && aTag < EAlfContentLastTag )
        {
        iMsgDataBuf.ReAllocL(0x100);
        RDesWriteStream out(iMsgDataBuf);
        TPckgBuf<TUint8> tag(aTag);
        TPckgBuf<TUint8> zero((TInt)1);
        TPckgBuf<TRect> rect(aBoundingRectangle); 
        TPckgBuf<TInt32> layerId(aLayerId);
        out.WriteL( tag );
        out.WriteL( zero );
        out.WriteL( rect );
        out.WriteL( layerId );
        out.CommitL();
        const TUid drawerCrpUid = KAlfHintDrawerInstanceUid;
        const TWsGraphicId KDrawerCrpId(drawerCrpUid);
        static_cast<CWindowGc*>(iGc)->DrawWsGraphic(KDrawerCrpId, TRect(0,0,360,640), iMsgDataBuf );
        out.Close();
        }
    else
        {
        return KErrNotSupported;
        }
    return KErrNone;
    
    }

EXPORT_C TInt CAlfHintGraphic::InsertTag( TInt aTag, TRect aBoundingRectangle, TInt aLayerId )
    {
    TInt result(KErrNone);
    TRAPD( err, result = InsertTagL( aTag, aBoundingRectangle, aLayerId ) );
    if (err )
        {
        return err;
        }
    else
        {
        return result;        
        }
    
    }

// ---------------------------------------------------------------------------
// InsertTag
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfHintGraphic::InsertTag( TInt aTag  )
    {// TODO; trap or change name
    TInt result(KErrNone);
    TRAPD( err, result = InsertTagL( aTag ) );
    if (err )
        {
        return err;
        }
    else
        {
        return result;        
        }
    } 
