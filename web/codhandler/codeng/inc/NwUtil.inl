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
*      Wrapper functions and classes for cXmlLibrary (NW_...) functionality.  
*      
*
*/


#ifndef NW_UTIL_INL
#define NW_UTIL_INL

// ================= GLOBAL FUNCTIONS =======================

// ---------------------------------------------------------
// LeaveIfNwError()
// ---------------------------------------------------------
//
TInt LeaveIfNwErrorL( NW_Status_t aNwStatus )
    {
    return User::LeaveIfError( NwStatus2Error( aNwStatus ) );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNwDomDocumentNode::CNwDomDocumentNode()
// ---------------------------------------------------------
//
CNwDomDocumentNode::CNwDomDocumentNode()
    {
    }

#endif /* def NW_UTIL_INL */
