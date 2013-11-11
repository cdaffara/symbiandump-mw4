/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for WlanLoginApplication class
* 
*/

#ifndef WLANLOGINAPPLICATION_H
#define WLANLOGINAPPLICATION_H

// System includes
#include <HbApplication>

//User includes

// forward declarations
class WlanLoginService;
class WlanLoginEngine;
class WlanLoginMainWindow;
class HbTranslator;

// External data types

// Constants

// Class declaration
class WlanLoginApplication : public HbApplication
    {
public:
    WlanLoginApplication(int argc, char* argv[]);
    ~WlanLoginApplication();
    
    WlanLoginService* service() const;
    WlanLoginEngine* engine() const;
    QSharedPointer <WlanLoginMainWindow> mainWindow();
    
    
private: // data
    
    // Not owned data

    // Owned data
    WlanLoginService* mService; //!< Pointer to Qt Highway service provider
    QSharedPointer <HbTranslator> mTranslator; //!< Pointer to HB translator
    WlanLoginEngine* mEngine; //!< Pointer to WLAN login engine
    QSharedPointer <WlanLoginMainWindow> mMainWindow; //!< Pointer to WLAN login application main window      
    };

#endif // WLANLOGINAPPLICATION_H
