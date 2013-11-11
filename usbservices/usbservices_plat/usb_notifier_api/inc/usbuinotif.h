/*
 * Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#ifndef USBUINOTIF_H
#define USBUINOTIF_H

#include <eiknotapi.h>

/**
 * Different note types
 */
enum TUSBUINotes
    {
   };

/**
 * Used with information notes
 */
struct TUSBNotesNotiferParams
    {
    TUSBUINotes iNote;
    };

/**
 * Used with connection notifier
 */
struct TUSBConnectionNotiferParams
    {
    TInt iPersonalityId;
    };

/**
 * Package for information notes
 */
typedef TPckgBuf<TUSBNotesNotiferParams> TUSBNotesNotifierParamsPckg;

/**
 * Package for connection note
 */
typedef TPckgBuf<TUSBConnectionNotiferParams> TUSBConnectionNotifierParamsPckg;

/**
 * Different query types
 */
enum TUSBUIQueries
    {
    EUSBMemoryCardUnlocked,
    EUSBStorageMediaFailure,    
    EUSBPossibleDataLossCable,
    EUSBPossibleDataLossMMC,
    EUSBChangeFromMassStorage,
    EUSBNoMemoryCard,
    EUSBNotEnoughRam,
    EUSBDiskFull
   };

/**
 * Used with queries
 */
struct TUSBQueriesNotiferParams
    {
    TUSBUIQueries iQuery;
    TInt          iDrive;
    };

/**
 * Package for queries
 */
typedef TPckgBuf<TUSBQueriesNotiferParams> TUSBQueriesNotifierParamsPckg;

/**
 * Notifier id for USB Cable Connection
 */
const TUid KCableConnectedNotifierUid = {0x102068DE}; 

/**
 * Notifier id for USB Notes
 */
const TUid KNotesNotifier             = {0x102068E0}; 

/**
 * Notifier id for USB Queries
 */
const TUid KQueriesNotifier           = {0x102068E1}; 

/**
 * Notifier id for USB connection
 */
const TUid KUsbConnectionNotifier     = {0x10282544};


/**
 * Notifier id for USB OTG Warning
 */
const TUid KUsbUiNotifOtgWarning      = {0x2000B001};

/**
 * Notifier id for USB OTG Error
 */
const TUid KUsbUiNotifOtgError     = {0x2000B002};

/**
 * Notifier id for USB OTG indicator
 */
const TUid KUsbUiNotifOtgIndicator      = {0x2000B003};

/**
 * Notifier id for mass storage mount manager error
 * THostMsErrData is used as parameter  for delivering error 
 * information
 */
const TUid KUsbUiNotifMsmmError     = {0x2001FE3A};

/**
 * Parameter for both KUsbUiNotifOtgWarning and KUsbUiNotifOtgError
 * Possible values listed in following two enums
 */
typedef TPckgBuf<TInt> TUsbUiNotifOtgParam;  

/**
 * Possible parameter values for KUsbUiNotifOtgNote
 */
enum TUsbUiNotifOtgWarning
    {
    EUsbOtgPartiallySupportedDevice
    };

/**
 * Possible parameter values for KUsbUiNotifOtgQuery
 */
enum TUsbUiNotifOtgError
    {
    EUsbOtgTooMuchPower,		// Error during operating
    EUsbOtgTooMuchPowerRequired,// Error during enumerating
    EUsbOtgUnsupportedDevice,
    EUsbOtgHubUnsupported,
    EUsbOtgErrorInConnection,
    EUsbOtgErrorAttachTimedOut,
    EUsbOtgErrorNoMemory
    };

#endif // USBUINOTIF_H

// End of File
