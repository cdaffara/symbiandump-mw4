REM /*
REM * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
REM * All rights reserved.
REM * This component and the accompanying materials are made available
REM * under the terms of the License "Eclipse Public License v1.0"
REM * which accompanies this distribution, and is available
REM * at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM *
REM * Initial Contributors:
REM * Nokia Corporation - initial contribution.
REM *
REM * Contributors:
REM *
REM * Description:  
REM *
REM */


call cd group
call bldmake bldfiles
call abld reallyclean
call abld export
call abld build winscw udeb memman
call abld build armv5 urel memman
call cd ..\non-opensourced
call bldmake bldfiles
call abld reallyclean
call abld build winscw udeb
call abld build armv5 urel
call cd ..\group
call abld reallyclean
call abld build winscw udeb
call abld build armv5 urel
call cd ..\non-opensourced
abld target winscw udeb
abld target armv5 urel
call cd ..\group
abld target winscw udeb
abld target armv5 urel
call cd ..\non-opensourced
abld target winscw udeb
abld target armv5 urel
call cd ..\group
abld target winscw udeb
abld target armv5 urel