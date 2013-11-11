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
* Description:   This direct client to alf server, that does not require Alf 
*                environment.
*                
*/


#include <alf/alfdirectclient.h>
#include <alf/alfconstants.h>

// ---------------------------------------------------------------------------
// Send synchronous command
// ---------------------------------------------------------------------------
//
EXPORT_C void RAlfDirectClient::MeasureFPS( TInt32 aId, TBool aStart )
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return;
            }
        }
    TInt command = EAlfDirectClientFPSCounterOn;
    if ( !aStart )
        {
        command = EAlfDirectClientFPSCounterOff;
        }
    Send( command, TIpcArgs(aId));
    }

EXPORT_C void RAlfDirectClient::EnableLowMemoryState( TBool aMode , TBool aUseSwRendering)
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return;
            }
        }
    SendReceive(EAlfEnableLowMemoryState, TIpcArgs(aMode));
    }

EXPORT_C TInt RAlfDirectClient::ForceSwRendering( TBool aEnabled )
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return err;
            }
        }
    return SendReceive(EAlfForceSwRendering, TIpcArgs(aEnabled));
    }

TInt RAlfDirectClient::GetSizeAndRotation(TSize& aSize, TInt& aRotation)
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return err;
            }
        }

    TPckg<TSize> sizePckg(aSize);
    TPckg<TInt> rotationPckg(aRotation);
    return SendReceive(EAlfGetSizeAndRotation, TIpcArgs(&sizePckg, &rotationPckg));
    }
    
TInt RAlfDirectClient::ReadPixels(TInt aBitmapHandle)
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return err;
            }
        }

    return SendReceive(EAlfReadPixels, TIpcArgs(aBitmapHandle));
    }

EXPORT_C TInt RAlfDirectClient::BlankScreen( TBool aEnabled )
    {
    if (!Handle())
        {
        TRAPD(err, OpenL());
        if (err)
            {
            return err;
            }
        }
    return SendReceive(EAlfBlankScreen, TIpcArgs(aEnabled));
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C RAlfDirectClient::RAlfDirectClient()
    : RAlfClientBase(KAlfServiceUid.iUid) 
    {
    }

// ---------------------------------------------------------------------------
// "destructor"
// ---------------------------------------------------------------------------
//    
EXPORT_C void RAlfDirectClient::Disconnect()
    {
    if (Handle())	
    	     {
    	     Close();
    		   }
    
    }
