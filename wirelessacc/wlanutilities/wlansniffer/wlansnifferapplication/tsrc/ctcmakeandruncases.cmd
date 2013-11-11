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
del MON.*
del profile.txt
rmdir /S /Q CTCHTML
call qmake "CONFIG -= debug"
call sbs clean
call sbs reallyclean
call ctcwrap -C EXCLUDE=* -C NO_EXCLUDE+..\src\wlansnifferlistwidget.cpp -i d -2comp sbs -c winscw_udeb
call \epoc32\release\winscw\udeb\t_wlansniffer.exe