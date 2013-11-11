/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility methods for eventviewer
*
*/



#ifndef __EVENTVIEWER__
#define __EVENTVIEWER__

#include <cdblen.h>

////////////////////////////////////////////////////////
// Typedefs
////////////////////////////////////////////////////////

typedef TBuf8<KCommsDbSvrMaxFieldLength> TIapName;


/////////////////////////////////////////////////////////
// EventViewer  Class
/////////////////////////////////////////////////////////
class EventViewer 
{
public: 
    IMPORT_C static TInt GetIapName(TUint32 aIapId, TIapName& aIapName);
    IMPORT_C static TInt GetSnapName(TUint32 aSnapId, TIapName& aSnapName);

    IMPORT_C static TInt GetIapNames(TUint32 aIapId1, TIapName& aIapName1,
                                  TUint32 aIapId2, TIapName& aIapName2);


private:
    static void DoGetIapNameL(TUint32 aIapId, TIapName& aIapName);

    static void DoGetIapNamesL(TUint32 aIapId1, TIapName& aIapName1,
                               TUint32 aIapId2, TIapName& aIapName2);
                                                                                             
    static void DoGetSnapNameL(TUint32 aSnapId, TIapName& aSnapName);                               
};
#endif