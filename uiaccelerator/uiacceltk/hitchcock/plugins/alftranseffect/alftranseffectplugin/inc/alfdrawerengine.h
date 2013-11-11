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
* Description:   Implements functions that are used by the CWsServerDrawerHandler
*
*/



#ifndef M_ALFDRAWERENGINE_H
#define M_ALFDRAWERENGINE_H

#include <e32base.h>
#include <fbs.h>

class MAlfDrawerEngine
    {
// Functions originally taken from MWsServerDrawerEngine
// Unused functions removed, new needed ones may be added.    
public:
//	virtual TInt SaveFB() = 0;
//	virtual TInt SaveRedirected() = 0;
//	virtual TInt RedirectGc() = 0;
//	virtual void UnredirectGc() = 0;
//	virtual void UpdateToScreen() = 0;
	virtual TUid& FromUid() = 0;
	virtual TSecureId& FromSid() = 0;
	virtual TInt FromWg() = 0;
	virtual TUid& ToUid() = 0;
	virtual TSecureId& ToSid() = 0;
	virtual TInt ToWg() = 0;
	virtual TUint& Action() = 0;
	virtual TInt& Flags() = 0;
	virtual void IncreaseControlHandle() = 0;
	virtual TInt CurrentControlHandle() = 0;
	virtual TInt CurrentFullScreenHandle() = 0;
	virtual void StartEndChecker() = 0;
	virtual void CancelEndChecker() = 0;
	virtual void SendBeginFullscreen() = 0;
	virtual TInt SendEndFullscreen(TBool aTimeout = EFalse) = 0;
	virtual TInt SendAbortFullscreen() = 0;
	virtual TInt SendAbortControlTransition() = 0;
    virtual	TInt SendBeginControlTransition() = 0;
	virtual TInt SendFinishControlTransition() = 0;
	virtual TBool EffectFinishedFullScreen() const = 0;

    };

#endif //M_ALFDRAWERENGINE_H
