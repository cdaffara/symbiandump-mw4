/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
/*
 * eglext.h
 *
 *  Created on: 1.7.2009
 *      Author: hennailvonen
 *      For stub purposes until the real thing is available (wk32 release)
 *      eglexthackedforsymbian.h was made available in wk32 as an example eglext.h
 */

#ifndef EGLEXT_H_
#define EGLEXT_H_

// Temp, use local extension definitions instead global
#define EGLEXT_LOCAL

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include <EGL/egl.h>
#include <EGL/egltypes.h>


/* Header file version number */
/* Current version at http://www.khronos.org/registry/egl/ */
/* $Revision: 7244 $ on $Date: 2009-01-20 17:06:59 -0800 (Tue, 20 Jan 2009) $ */
#ifndef EGL_EGLEXT_VERSION
#define EGL_EGLEXT_VERSION 3

#ifndef EGLAPIENTRYP
#define EGLAPIENTRYP EGLAPIENTRY *
#endif

/* We want this */
#ifndef EGL_EGLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif

/*************************************************************/


#ifndef EGL_NOK_resource_profiling
#define EGL_NOK_resource_profiling 1
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglQueryProfilingDataNOK(EGLDisplay dpy, EGLint query_bits, EGLint *data, EGLint data_size, EGLint *data_count);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYPROFILINGDATANOKPROC) (EGLDisplay dpy, EGLint query_bits, EGLint *data, EGLint data_size, EGLint *data_count);
typedef void *EGLNativeProcessIdTypeNOK;
#define EGL_PROF_QUERY_GLOBAL_BIT_NOK          0x0001
#define EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK    0x0002
//Returned as attribute identifiers by eglQueryProfilingDataNOK:
#define EGL_PROF_TOTAL_MEMORY_NOK              0x3070
#define EGL_PROF_USED_MEMORY_NOK               0x3071
#define EGL_PROF_PROCESS_ID_NOK                      0x3072
#define EGL_PROF_PROCESS_USED_PRIVATE_MEMORY_NOK     0x3073
#define EGL_PROF_PROCESS_USED_SHARED_MEMORY_NOK      0x3074
#endif /*EGL_NOK_resource_profiling*/

#endif /*EGL_EGLEXT_VERSION*/
#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif /* EGLEXT_H_ */
