/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares macros for heap checks
*
*/



#ifndef UPNPUHEAPMARK_H
#define UPNPUHEAPMARK_H
// INCLUDES

#include <e32base.h>

//variables
const TBool KUpnpHeapChecksToHeapChecks = ETrue; //checks heap only if ETrue
_LIT( KUpnpHeapChecksAllocInfo, "%D;%D" ); //panic category if heap checks fails
const TInt KUpnpHeapChecksMaxAllocInfoLength = 21; //10 + 1 + 10
const TInt KUpnpHeapChecksAllocPanicReason = 0; //panic reason if heap checks fails

//functions
static void UPnpHeapChecks( TInt aCells, TInt aBytes )
    {
    if ( aCells )
        {
        TBuf<KUpnpHeapChecksMaxAllocInfoLength> allocInfo;
        allocInfo.AppendFormat(  KUpnpHeapChecksAllocInfo(), aCells, aBytes );
        User::Panic( allocInfo, KUpnpHeapChecksAllocPanicReason );
        }
    }


#define __UPNP_UHEAP_MARK     TInt cells_begin;                                     \
                              TInt bytes_begin;                                     \
                              RHeap& heap = User::Heap();                           \
                              if ( KUpnpHeapChecksToHeapChecks )                    \
                              {                                                     \
                              cells_begin = heap.AllocSize( bytes_begin );          \
                              }    

#define __UPNP_UHEAP_MARKEND if ( KUpnpHeapChecksToHeapChecks )                     \
                              {                                                     \
                              TInt cells_end;                                       \
                              TInt bytes_end;                                       \
                              cells_end = heap.AllocSize( bytes_end );              \
                              UPnpHeapChecks( cells_end-cells_begin,                \
                                                          bytes_end-bytes_begin );  \
                              }    
    
#endif // UPNPUHEAPMARK_H

//end of file
