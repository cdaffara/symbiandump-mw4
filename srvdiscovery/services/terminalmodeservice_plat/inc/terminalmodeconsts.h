/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class defines all the error codes related to Terminal Mode Service
*
*/

#ifndef __TERMINALMODECONSTS_H__
#define __TERMINALMODECONSTS_H__

/**
 * Error codes for Terminal Mode extention 
 * These error codes are located in the non-standard error code range 
 * (800 – 899).
 */
enum TTerminalModeErrorCode
    {
    /**
     * Terminal Mode success error code.
     * Represents the no-error condition.
     */
    ETerminalModeSuccess = 200,

    /**
     * Terminal Mode failure error code.
     * Represents the error condition.
     */
    ETerminalModeFailure = 201,

    /**
     *Terminal Mode error range marker.
     */
    ETerminalModeErrorRangeBegin = 700,

    /**
     * Operation Rejected. 
     * Terminal Mode Server rejected the operation.
     */
    ETerminalModeOperationRejected = 701,

    /**
     * Malformed or Non-existent AppID.
     * The AppID is of incorrect format or does not exist.
     */
    ETerminalModeBadAppId = 810,

    /**
     * Unauthorized AppID.
     * The application identified by this AppID cannot be used remotely.
     */
    ETerminalModeUnauthorizedApp = 811,

    /**
     * Cannot determine application status.
     * The status of the application cannot be cur-rently determined. 
     * Client should try again later.
     */
    ETerminalModeAppStatusError = 812,

    /**
     * Failed to launch application.
     * The application is available on the mobile device but failed 
     * to launch.
     */
    ETerminalModeAppLaunchError = 813,
        
    /**
     * The requested application resource is busy.
     * This error can occur when the resource is already busy and 
     * resourceStatus in the AppListing is set equal to “NA”.
     */
    ETerminalModeResourceBusy = 814,
        
    
    /**
     * Failed to process the action as the device hosting the TmApplicationServer 
     * service is locked
     */
    ETerminalModeDeviceLocked = 815,
    
    /**
     * Failed to process the action as the input argument passed is invalid
     */
    ETerminalModeInvalidArgument = 820,
        
    /**
     * The clientProfile argument passed does not conform to A_ARG_TYPE_ClientProfile
     * XML specifications.
     */
    ETerminalModeInvalidProfile = 825,
    
    /**
     * Failed to process the action as the the profile identifier does not exist 
     * or the application cannot use the specified profile identifier.
     */
    ETerminalModeInvalidProfileId = 830,
    
    /**
     * Invalid license.The HU does not have valid license or has not started license 
     * verification process for the service.
     */
    ETerminalModeInvalidLicense = 891,
    /**
     * Terminal Mode error range marker.
     */
    ETerminalModeErrorRangeEnd = 899
    };

#endif // __TERMINALMODECONSTS_H__
