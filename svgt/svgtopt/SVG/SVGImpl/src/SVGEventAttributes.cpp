/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#include "SVGEventAttributes.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CSvgEventAttributes::CSvgEventAttributes() : iOnClick( NULL ),
                                             iOnLoad( NULL ),
                                             iOnMouseMove( NULL )
    {
    }

CSvgEventAttributes::~CSvgEventAttributes()
    {
    delete iOnClick;
    delete iOnLoad;
    delete iOnMouseMove;
    }

void CSvgEventAttributes::SetOnClickL( const TDesC& aString )
    {
    if ( iOnClick != NULL )
        {
        delete iOnClick;
        iOnClick = NULL;
        }
    iOnClick = aString.AllocL();
    }

const HBufC* CSvgEventAttributes::OnClick()
    {
    return iOnClick;
    }

void CSvgEventAttributes::SetOnLoadL( const TDesC& aString )
    {
    if ( iOnLoad != NULL )
        {
        delete iOnLoad;
        iOnLoad = NULL;
        }
    iOnLoad = aString.AllocL();
    }

const HBufC* CSvgEventAttributes::OnLoad()
    {
    return iOnLoad;
    }

void CSvgEventAttributes::SetOnMouseMoveL( const TDesC& aString )
    {
    if ( iOnMouseMove != NULL )
        {
        delete iOnMouseMove;
        iOnMouseMove = NULL;
        }
    iOnMouseMove = aString.AllocL();
    }

const HBufC* CSvgEventAttributes::OnMouseMove()
    {
    return iOnMouseMove;
    }
