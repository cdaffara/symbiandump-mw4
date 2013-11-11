rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: 
rem

makesis -s uiaccelerator_stub.pkg

set certificate_file=..\internal\certificates\rd.cer
set private_key_file=..\internal\certificates\rd-key.pem

rem add update version information to dll's and exe's
elftran -version 10.1 \epoc32\release\armv5\urel\hitchcock.dll
elftran -version 10.1 \epoc32\release\armv5\urel\alfclient.dll
elftran -version 10.1 \epoc32\release\armv5\urel\alfappservercore.dll
elftran -version 10.1 \epoc32\release\armv5\urel\!ftokenserver.exe
elftran -version 10.1 \epoc32\release\armv5\urel\ftokenclient.dll
elftran -version 10.1 \epoc32\release\armv5\urel\alfredserver.exe

makesis uiaccelerator_update.pkg
signsis uiaccelerator_update.sis uiaccelerator_update.sisx %certificate_file% %private_key_file%

rem return version information of dll's and exe's to same as in rom
elftran -version 10.0 \epoc32\release\armv5\urel\hitchcock.dll
elftran -version 10.0 \epoc32\release\armv5\urel\alfclient.dll
elftran -version 10.0 \epoc32\release\armv5\urel\alfappservercore.dll
elftran -version 10.0 \epoc32\release\armv5\urel\!ftokenserver.exe
elftran -version 10.0 \epoc32\release\armv5\urel\ftokenclient.dll
elftran -version 10.0 \epoc32\release\armv5\urel\alfredserver.exe
