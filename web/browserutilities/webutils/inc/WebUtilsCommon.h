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


#ifndef  __WEBUTILSCOMMON_H
#define  __WEBUTILSCOMMON_H

// INCLUDES

// FORWARD DECLARATIONS

enum TFragmentSelector
    {
    EAllFragmentsExcludeMMS,
    EFirstFragment,
    EAllExceptFirstExcludeMMS,
    EMMSFragments,
    ENameFragmentMMS
    };
    
 enum TFragmentRemoveSelector
    {
    ERemoveAll,
    ERemoveMainFragment,
    ERemoveSubFragment
    };
    
    
#endif // __WEBUTILSCOMMON_H