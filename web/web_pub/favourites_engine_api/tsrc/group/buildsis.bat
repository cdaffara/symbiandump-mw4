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
makesis FavouritesEngineTest.pkg
@echo Sign FavouritesEngineTest.sis...
signsis FavouritesEngineTest.sis FavouritesEngineTest.sisx rd.cer rd-key.pem
@echo Delete intermediate files...
del FavouritesEngineTest.sis
@echo Done, please copy FavouritesEngineTest.sisx to your phone
@echo on