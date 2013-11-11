/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility methods for handling connection settings
*
*/

#ifndef CmManagerUtils_H
#define CmManagerUtils_H

#include <e32base.h>

#include "vpnapidefs.h"

class RCmConnectionMethodExt;
class RCmManagerExt;
class CEventLogger;


/**
 *  Utility methods for handling connection settings
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.2.3
 */
class CmManagerUtils
    {
public:

    static void CreateVPNConnectionMethodToIntranetL(const TVpnPolicyInfo& aVpnPolicyInfo,
                                                     CEventLogger& aEventLogger);
    
private:

    static void SetVpnConnectionMethodAttributesL(RCmConnectionMethodExt& aConnectionMethod,
                                                  const TDesC& aConnectionMethodName,
                                                  const TVpnPolicyId aPolicyId,
                                                  const TUint32 aInternetDestinationId);
    
    
    static HBufC* CreateConnectionMethodNameLC(const TDesC& aPolicyName);

    static HBufC* CreateIntranetDestinationNameLC(RCmManagerExt& aCmManagerExt);
    
    static RCmConnectionMethodExt CreateNewConnectionMethodToIntranetL(RCmManagerExt& aCmManagerExt,
                                                                       const TDesC& aConnectionMethodName,
                                                                       const TVpnPolicyId aPolicyId,
                                                                       const TUint32 aInternetDestinationId);

    
    };


#endif // CmManagerUtils_H
