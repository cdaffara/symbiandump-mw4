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
 * Description: 
 *   Test Application for wizards
 *
 */

#ifndef MYAPPLICATION_H_
#define MYAPPLICATION_H_

#include <QSharedPointer>
#include <HbApplication.h>

// forward declarations
class MyAppEngine;
class MyAppMainWindow;

/*!
 * @addtogroup group_test_eap_wizard
 * @{
 */

class MyApplication : public HbApplication
    {
public:
    MyApplication(int argc, char* argv[]);
    virtual ~MyApplication();
    
private:
    QSharedPointer<MyAppEngine> engine;         // owned
    QSharedPointer<MyAppMainWindow> mainWindow; // owned
    };

/*! @} */

#endif /* MYAPPLICATION_H_ */
