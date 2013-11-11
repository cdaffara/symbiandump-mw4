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
 *   Test application for wizards.
 *
 */

#include "myappmainwindow.h"
#include "myappengine.h"
#include "firstview.h"

// ViewManager Constructor
MyAppMainWindow::MyAppMainWindow()
{
    
    // Add the views to the main window
    addFirstView();
    // show the main window (which will display the last view that was added)
    toFirstView();
    show();
}

// Destructor
MyAppMainWindow::~MyAppMainWindow()
{
}

// Methods to add views to the main window
void MyAppMainWindow::addFirstView()
{
    mFirstView = new FirstView();
    addView(mFirstView);
}


// Slots to handle view change
void MyAppMainWindow::toFirstView()
{
    setCurrentView(mFirstView);
}

