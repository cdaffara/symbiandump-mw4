/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of scan frame offsets.
*
*/



#ifndef HSSSCANOFFSETS_H
#define HSSSCANOFFSETS_H


/** Offset for RCPI value. */
const TUint32 RX_LEVEL_OFFSET         = 0;   

/** Offset for SNR value. Deprecated. */
const TUint32 RX_SNR_OFFSET           = 4;                

/** Offset for beacon/probe response frame data length. */
const TUint32 LENGTH_OFFSET           = 8;                  

/** Length of the control information header. */
const TUint32 CNTRL_HEADER_LEN        = 12;

/** Start of beacon/probe response frame data. */
const TUint32 DOT11_BASE_OFFSET       = CNTRL_HEADER_LEN;

/** Start of BSSID field (6 octets ). */
const TUint32 BSSID_OFFSET            = DOT11_BASE_OFFSET + 16;           

/** Start of Timestamp field (8 octets). */
const TUint32 TIMESTAMP_OFFSET        = DOT11_BASE_OFFSET + 24;

/** Start of Beacon Interval field (2 octets). */
const TUint32 BEACON_INTERVAL_OFFSET  = DOT11_BASE_OFFSET + 32;

/** Start of Capability field (2 octets). */
const TUint32 CAPABILITY_OFFSET       = DOT11_BASE_OFFSET + 34;

/** Start of first IE in beacon/probe response frame. */
const TUint32 BODY_OFFSET             = DOT11_BASE_OFFSET + 36;


#endif // HSSSCANOFFSETS_H
