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
* Description:   Sub-session for control group
*
*/



#include "alfcontrolgroupsubsession.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Opens subsession
// ---------------------------------------------------------------------------
//
TInt RAlfControlGroupSubSession::Open( RAlfClient& aSession, TInt aId )
    {
    TAlfImplementationInformation info(EAlfControlGroupSubSessionOpen);
    TPckgC<TAlfImplementationInformation> infoBuf(info);

    return CreateSubSession( 
        aSession,
        EAlfCreateSubSession,
        TIpcArgs(&infoBuf, aId) );
    }
 
// ---------------------------------------------------------------------------
// Closes subsession
// ---------------------------------------------------------------------------
//   
void RAlfControlGroupSubSession::Close()
    {
    CloseSubSession( EAlfCloseSubSession );
    }

// ---------------------------------------------------------------------------
// Appends control
// ---------------------------------------------------------------------------
// 
TInt RAlfControlGroupSubSession::Append( TInt aControlHandle )
    {
    TInt err =  SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfControlGroupAppend, aControlHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfControlGroupSubSession::Append return error %d", err )
        }
    return err;
    }
    
// ---------------------------------------------------------------------------
// Removes a control
// ---------------------------------------------------------------------------
// 
TInt RAlfControlGroupSubSession::Remove( TInt aControlHandle )
    {
    TInt err =  SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfControlGroupRemove, aControlHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfControlGroupSubSession::Remove return error %d", err )
        }
    return err;
    }
    

// ---------------------------------------------------------------------------
// Enables transformation.
// ---------------------------------------------------------------------------
// 
TInt RAlfControlGroupSubSession::EnableTransformation(
        TInt aTransformationHandle,
        TBool aIsTransformed )
    {
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfControlGroupEnableTransformation, aIsTransformed, aTransformationHandle) );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "RAlfControlGroupSubSession::EnableTransformation return error %d", err )
        }
    return err;
    }

