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


#ifndef __WebCorePluginHandler_h__
#define __WebCorePluginHandler_h__

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*  @lib webcore.lib
*  @since 3.0
*/
class TWebCorePluginInfo
{
    public:
        TPtrC iName;
        TPtrC iFilename;
        TPtrC iPluginDescription;
        // no need to call close on this
        RPointerArray<HBufC> iMimeTypeArray;
};

/**
*
*  @lib webcore.lib
*  @since 3.0
*/
class MWebCorePluginHandler
{
public:
    virtual TInt getPluginCount() = 0;
    virtual TBool pluginSupportsMIMEType(const TDesC& aMimeType) = 0;
    virtual TBool objectAtIndex(TInt aIndex, TWebCorePluginInfo& aPluginInfo) = 0;
    virtual void refreshPlugins(TBool aReloadPages) = 0;
    virtual HBufC8* extensionsForMIMEType(TInt aIndex, TDesC& aMimeType) = 0;
    virtual HBufC* descriptionForMIMEType(TInt aIndex, TDesC& aMimeType) = 0;
};

/**
*
*  @lib webcore.lib
*  @since 3.0
*/
class WebCorePluginHandler
{
public:
    static void SetWebCorePluginHandler(MWebCorePluginHandler* aPluginHandler);
    static MWebCorePluginHandler* PluginHandler();

private:

    static MWebCorePluginHandler* iPluginHandler;
};


#endif

// End of File
