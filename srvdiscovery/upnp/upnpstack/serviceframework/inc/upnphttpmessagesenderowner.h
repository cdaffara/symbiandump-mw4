/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the MUpnpHttpMessageSenderOwner interface
 *
*/


#ifndef __UPNPHTTPMESSAGESENDEROWNER_H__
#define __UPNPHTTPMESSAGESENDEROWNER_H__

class CUpnpHttpMessageSender;

/**
* MUpnpHttpMessageSenderOwner
* Interface that let sender owner to receive information that sender
* isn't needed any more.
*/
class MUpnpHttpMessageSenderOwner
    {
    public:
        /*
         * Sender has finished it's task and can be destroyed.
         */
        virtual void SenderFinishedLD( CUpnpHttpMessageSender* aSenderToRelease ) = 0;
    };

#endif /*__UPNPHTTPMESSAGESENDEROWNER_H__*/

//  End of File

