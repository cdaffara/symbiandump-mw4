rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of the License "Eclipse Public License v1.0"
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
makesis BrCtlApiTest.pkg
@echo Sign BrCtlApiTest.sis...
signsis BrCtlApiTest.sis BrCtlApiTest.sisx rd.cer rd-key.pem
@echo Delete intermediate files...
del BrCtlApiTest.sis
@echo Done, please copy BrCtlApiTest.sisx to your phone
@echo on