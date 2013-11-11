/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include <e32std.h>

#include "senserviceconnectionimpl.h"

// ============================= LOCAL FUNCTIONS ===============================

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aObserver,
                                                             const TDesC8& aContract )
    {
    return CSenServiceConnectionImpl::NewL( aObserver, aContract );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aObserver,
                                                              const TDesC8& aContract )
    {
    return CSenServiceConnectionImpl::NewLC( aObserver, aContract );
    }


EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aObserver,
                                                             MSenServiceDescription& aServiceDescription )
    {
    return CSenServiceConnectionImpl::NewL( aObserver, aServiceDescription );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aObserver,
                                                              MSenServiceDescription& aServiceDescription )
    {
    return CSenServiceConnectionImpl::NewLC( aObserver, aServiceDescription );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aObserver,
                                                             CSenServicePattern& aServicePattern )
    {
    return CSenServiceConnectionImpl::NewL( aObserver, aServicePattern );
    }


EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aObserver,
                                                              CSenServicePattern& aServicePattern )
    {
    return CSenServiceConnectionImpl::NewLC( aObserver, aServicePattern );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aConsumer,
                                                             const TDesC8& aContract,
                                                             MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewL( aConsumer, aContract, aExtendedConsumer );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aConsumer,
                                                              const TDesC8& aContract,
                                                              MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewLC( aConsumer, aContract, aExtendedConsumer );
    }


EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aConsumer,
                                                             MSenServiceDescription& aServiceDescription,
                                                             MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewL( aConsumer, aServiceDescription, aExtendedConsumer );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aConsumer,
                                                              MSenServiceDescription& aServiceDescription,
                                                              MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewLC( aConsumer, aServiceDescription, aExtendedConsumer );
    }

EXPORT_C CSenServiceConnection* CSenServiceConnection::NewL( MSenServiceConsumer& aConsumer,
                                                             CSenServicePattern& aServicePattern,
                                                             MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewL( aConsumer, aServicePattern, aExtendedConsumer );
    }


EXPORT_C CSenServiceConnection* CSenServiceConnection::NewLC( MSenServiceConsumer& aConsumer,
                                                              CSenServicePattern& aServicePattern,
                                                              MSenExtendedConsumerInterface& aExtendedConsumer)
    {
    return CSenServiceConnectionImpl::NewLC( aConsumer, aServicePattern, aExtendedConsumer );
    }


CSenServiceConnection::CSenServiceConnection()
    : CActive(EPriorityStandard)
    {
    }

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return(KErrNone);
    }
#endif

// End of file
