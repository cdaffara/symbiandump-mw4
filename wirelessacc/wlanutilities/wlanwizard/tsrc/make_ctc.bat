:: Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
::
:: Description:
:: Batch file to instrument and run test cases with CTC.

@echo off
call ctcwrap -C EXCLUDE=* -C NO_EXCLUDE+..\src\*.cpp -C NO_EXCLUDE+..\..\eapwizard\src\*.cpp -C NO_EXCLUDE+..\..\wpswizard\src\*.cpp -i fd -v -2comp sbs -c winscw_udeb
