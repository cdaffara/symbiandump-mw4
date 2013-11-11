/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef HUIFXVG10_H_
#define HUIFXVG10_H_

#include <VG/openvg.h>
#include <EGL/egl.h>

#ifdef _DEBUG
// This macro will cause panic if there was programming error, e.g. invalid handle passed to openvg.
#define HUIFX_VG_INVARIANT() {TInt vgError = vgGetError();__ASSERT_ALWAYS((vgError == VG_NO_ERROR || vgError == VG_OUT_OF_MEMORY_ERROR), User::Panic(_L("HuiFx"), vgError));}
#else
#define HUIFX_VG_INVARIANT() {}
#endif

#endif /*HUIFXVG10_H_*/
