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

makesis huitk.pkg
signsis huitk.sis huitk.sisx ..\..\internal\certificates\rd.cer ..\..\internal\certificates\rd-key.pem
makesis huitk_debug.pkg
signsis huitk_debug.sis huitk_debug.sisx ..\..\internal\certificates\rd.cer ..\..\internal\certificates\rd-key.pem
makesis huitk_update.pkg
signsis huitk_update.sis huitk_update.sisx ..\..\internal\certificates\rd.cer ..\..\internal\certificates\rd-key.pem
makesis huitk_update_debug.pkg
signsis huitk_update_debug.sis huitk_update_debug.sisx ..\..\internal\certificates\rd.cer ..\..\internal\certificates\rd-key.pem
makesis -s huitk_stub.pkg