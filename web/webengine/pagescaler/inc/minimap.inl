/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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



// inlined accessors
TBool CMinimap::Visible() const
    {
    return iVisible;
    }

void CMinimap::SetZoomOutLevel(TInt aPercent)
    {
    iZoomOutLevel = aPercent;
    }

TInt CMinimap::ZoomOutLevel() const
    {
    return iZoomOutLevel;
    }

TInt CMinimap::Transparency() const
    {
    return iTransparency;
    }

MPageScalerCallback& CMinimap::Callback() const
    {
    return *iCallback;
    }
