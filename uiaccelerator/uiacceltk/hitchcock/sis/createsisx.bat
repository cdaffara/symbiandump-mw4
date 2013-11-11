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

@echo off
echo DoTo: update the location for certificate and private key files

set certificate_file=..\internal\certificates\rd.cer
set private_key_file=..\internal\certificates\rd-key.pem

rem create base sis package
makesis uiaccelerator_base.pkg
signsis uiaccelerator_base.sis uiaccelerator_base.sisx %certificate_file% %private_key_file%

rem create alfred sis package
makesis hitchcock.pkg
signsis hitchcock.sis hitchcock.sisx %certificate_file% %private_key_file%

rem create update sis package
call createupdatesis.bat

rem create combined sis package that has base and update package embedded
makesis uiaccelerator.pkg
signsis uiaccelerator.sis uiaccelerator.sisx %certificate_file% %private_key_file%
