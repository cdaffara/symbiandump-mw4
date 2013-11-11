/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic utility
*
*/



#ifndef DMGRUILIBPANIC_H
#define DMGRUILIBPANIC_H

// INCLUDE FILES

#include <e32std.h>

enum TUiLibPanReason
	{
    EUiLibPanNull = 1, 
    EUiLibPanNotNull, 
    EUiLibPanNotSupported,
    EUiLibPanNoDownloadsListModel,
    EUiLibPanNoDialogWhenVisible,
    EUiLibPanUnexpDialogWhenInvisible,
    EUiLibPanDLNegItemIndex,
    EUiLibPanNullHandledDownload,
    EUiLibPanNullHandlerObserver,
    EUiLibPanDivisionWithZero,
    EUiLibPanSmallDivBuffer,
    EUiLibPanCalledWhenUserExit,
    EUiLibPanBadExitType,
    EUiLibPanOptionsShownWhileNoDownloads,
    EUiLibPanStateIsStillMoved,
    EUiLibPanInProgressDownloadNotFound,
    EUiLibPanDlStateNotCompleted,
    EUiLibPanCreatedNotVisible,
    EUiLibPanProgressNotVisible,
    EUiLibPanPausedNotVisible,
    EUiLibPanCompletedNotVisible,
    EUiLibPanFailedNotVisible,
    EUiLibPanHideMenuWhileNoDialog,
    EUiLibPanAlreadyExistWhenAppend,
    EUiLibPanDocHandlerAlreadyRunning,
    EUiLibPanNullListBoxInSetModel,
    EUiLibPanNullListBoxInHandleModelChange,
    EUiLibPanNullMenuBar
	};

GLREF_C void Panic( TUiLibPanReason aReason );

#endif // DMGRUILIBPANIC_H
