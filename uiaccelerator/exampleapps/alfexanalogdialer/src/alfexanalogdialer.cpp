/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the entry point for the Analog Dialer.*
*/



#include <eikstart.h>

#include "alfexanalogdialerapplication.h"

#ifdef __UI_FRAMEWORKS_V2__

// ---------------------------------------------------------------------------
// NewApplication
// Creates the application class object and returns pointer to it
// ---------------------------------------------------------------------------
//
CApaApplication* NewApplication()
	{
	return new CAlfExAnalogDialerApplication;
	}

// ---------------------------------------------------------------------------
// E32Main
// Entry point for this application (executable) file. Function pointer
// to function creating the CAlfExAnalogDialerApplication objects is given
// ---------------------------------------------------------------------------
//
TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	
	}

#else // __UI_FRAMEWORKS_V2__

// Create an application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication()
  {
  return new CAlfExAnalogDialerApplication;
  }

// DLL entry point, return that everything is ok
GLDEF_C TInt E32Dll(TDllReason)
  {
  return KErrNone;
  }

#endif // __UI_FRAMEWORKS_V2__

