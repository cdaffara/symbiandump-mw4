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
makesis DownloadMgrClientApiTest.pkg
@echo Sign DownloadMgrClientApiTest.sis...
signsis DownloadMgrClientApiTest.sis DownloadMgrClientApiTest.sisx RDTest_02.der RDTest_02.key
@echo Delete intermediate files...
del DownloadMgrClientApiTest.sis
@echo Done, please copy DownloadMgrClientApiTest.sisx to your phone
@echo on