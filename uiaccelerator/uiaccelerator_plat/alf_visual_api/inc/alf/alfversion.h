/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Version information of the Alfred.
*
*/



#ifndef C_ALFVERSION_H
#define C_ALFVERSION_H

/** 
 * This header contains version information of the Alfred. Version information
 * is divided into three parts: major, minor and build.
 *
 * ============================================================================ 
 *
 * ALF_VERSION_MAJOR defines client API version, it is increased if API 
 * backwards compatibility is broken during API future development. 
 *
 * If application compilation time major-version value is found to be 
 * different than  the runtime value of the Alfred, then client can not use 
 * Alfred at all because the API:s are not compatible.
 *
 * ============================================================================
 *
 * ALF_VERSION_MINOR defines Server & coretoolkit API version, it is increased 
 * if API backwards compatibility is broken during API future development.
 *
 * If application compilation time minor-version value is found to be 
 * different than the runtime value of the Alfred, then no server extensions 
 * are allowed to be loaded because the compatibility of the coretoolkit/server 
 * API is no more what it was when the server extension was compiled.
 *
 * ============================================================================
 *
 * ALF_VERSION_BUILD defines API version is general level, it is increased if 
 * API forward compatibility is broken during API future development. This kind 
 * of break comes for intance when new exported methods are added into API as 
 * a part of normal API future development.
 *
 * If application compilation time build-version value is found to be larger 
 * than the runtime value of the Alfred, then no server extension is allowed 
 * to be loaded for the application because there might potentially not exist 
 * runtime implementation for the methods that the server extension expects 
 * to exist in the API.
 *
 * ============================================================================
 *
 * Version history:
 * 1.1.3 - ??.??.2007 : -
 * 1.1.4 - 24.10.2007 : Increasing version at the end of development sprint.
 *                      First version which has SIS upgrade enablers in place.
 * 1.1.5 - ??.??.???? : Increasing version at the end of development sprint.
 *                      Some functional break in alfclient pointer event handling.
 * 1.1.6 - 22.10.2007 : Increasing version at the end of development sprint.
 * 1.1.7 -  7.12.2007 : Increasing version at the end of development sprint.
 * 1.1.8 - 20.12.2007 : Increasing version at the end of development sprint.
 * 1.1.9 - 17.01.2008 : Increasing version at the end of development sprint.
 * 1.1.10 - 30.01.2008: Increasing version at the end of development sprint.
 * 1.1.11 - 14.03.2008: Increasing version at the end of development sprint.
 * 1.1.12 - 2.4.2008  : Increasing version at the devbranch -> S60 5.0 update.
 * 1.1.13 - 24.4.2008 : Increasing version at the end of development sprint.
 * 1.1.14 - 18.6.2008 : Increasing version at the end of development sprint.
 * 1.1.16 - 21.8.2008 : Increasing version at the end of development sprint.                      
 * 1.1.17 - 18.9.2008 : Increasing version at the end of development sprint.                      
 * 1.1.18 - 14.11.2008 : Increasing version at the end of development sprint.
 * 1.1.19 - 04.02.2009 : Increasing version at the end of development sprint.                      
 */

/**
 * NOTE: Update the version history and also change version numbers in package 
 * files in /uiaccelerator/uiacceltk/hitchcock/sis/ 
 */

/** Client API version, increased only if backwards compatibility is broken */
#define ALF_VERSION_MAJOR 1

/** Server & core API version, increased only if backwards compatibility is broken */
#define ALF_VERSION_MINOR 1

/** Common build version, increased if either of the API:s are no more forward compatible */
#define ALF_VERSION_BUILD 22

#endif