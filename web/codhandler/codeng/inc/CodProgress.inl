/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Inline method definitions of class TCodProgress.   
*      
*
*/


#ifndef COD_PROGRESS_INL
#define COD_PROGRESS_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TCodProgress::TCodProgress()
// ---------------------------------------------------------
//
TCodProgress::TCodProgress( MCodLoadObserver* aObserver )
: iPhase( ELoad ), iObserver( aObserver )
    {
    iMax[ELoad] = 0;
    iMax[ENotify] = 0;
    iCur[ELoad] = 0;
    iCur[ENotify] = 0;
    }

// ---------------------------------------------------------
// TCodProgress::FinalValue()
// ---------------------------------------------------------
//
TInt TCodProgress::FinalValue()
    {
    // +1 is for 'DoneL': progress is not completed until explicitly said so.
    return iMax[ELoad] + iMax[ENotify] + 1;
    }

// ---------------------------------------------------------
// TCodProgress::CurrentValue()
// ---------------------------------------------------------
//
TInt TCodProgress::CurrentValue()
    {
    return iCur[ELoad] + iCur[ENotify];
    }

#endif /* def DD_PARSER_INL */
