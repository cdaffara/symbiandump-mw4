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
* Description:   Panic reasons for Alfred
*
*/



#ifndef C_ALFPANICREASONS_H
#define C_ALFPANICREASONS_H

#include <e32def.h>

/**
 * Enumerates client side panic reasons used in @c Panic defined in alfpanic.h.
 * Values inside this enumeration should not clash with values in TAlfPanicSrv.
 */
enum TAlfPanic
    {
    // Editor panics
    EAlfEditorPanicPredictiveTextDoesNotFit = 100,
    EAlfEditorPanicFormatExtractionFailure,
    EAlfEditorPanicNullPointersPassedIn,
    EAlfEditorPanicBadTextRun,
    EAlfEditorPanicNoVisual,
    EAlfEditorPanicCursorPositionOutOfRange,
    EAlfEditorPanicLineMetricsNotInitialized,
    EAlfEditorPanicParentLayoutAlreadySet,
    EAlfEditorPanicInlineTextProcessingSequenceError,
    EAlfEditorPanicInconsistentLengths,
    EAlfEditorPanicCurrentTextTooLongForNewMaxLength,
    EAlfEditorPanicLineMetricsCorrupted,
    EAlfEditorPanicFailedToObtainAFont,
    // CAlfRoster
    EAlfRosterPanicFocusAnomaly = 200,
    // CAlfEnv
    EAlfEnvPanicMultipleAlfEnvironments = 300
    };

/**
 * Enumerates server side panic reasons used in @c AlfPanicClient
 * defined in alfclientserver.h.
 * Values inside this enumeration should not clash with values in TAlfPanic.
 */
enum TAlfPanicSrv
    {
    /**
     * IPC with @c EAlfTextureUpdateOwnerId command must be issued
     * before using texture management functions. Note that this is
     * automatically sent by CAlfEnv.
     */
    EAlfSrvTexturePanicTextureOwnerIdNotUpdated = 10000
    };

#endif // C_ALFPANICREASONS_H
