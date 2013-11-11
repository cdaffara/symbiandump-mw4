/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Supports mixin class.
*
*/



#ifndef MDOWNLOADHANDLEROBSERVER_H
#define MDOWNLOADHANDLEROBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RHttpDownload;

// CLASS DECLARATION

/**
* Similar class to MApaEmbeddedDocObserver.
*/
NONSHARABLE_CLASS( MDownloadHandlerObserver )
    {
    public:

        /**
        * Notify client that the content handler has finished handling 
        * the content in the given download.
        * @param aDownload The download that was handled or NULL.
        * @param aReason Exit reason.
        * @return -
        */
        virtual void NotifyHandlerExit( RHttpDownload* aDownload, TInt aReason ) = 0;
    };


#endif /* MDOWNLOADHANDLEROBSERVER_H */
