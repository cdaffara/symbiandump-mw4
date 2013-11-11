/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      API provider.
*  
*
*/


#ifndef MAPIPROVIDER_H
#define MAPIPROVIDER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface to query several API implementations.
*  
*  @lib WidgetUI.app
*  @since Series 60 1.2
*/
class MApiProvider
    {
    public: // New functions

        /**
        * Exit the widget, if the widget is the only widget running, exit the application
        * @since 3.1
        */
        virtual void ExitWidget( ) = 0;
};

#endif

// End of File
