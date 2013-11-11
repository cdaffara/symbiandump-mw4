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

#include <e32std.h>

extern "C" void external_memset(void* dst, int data, int size)
{
    Mem::Fill(dst, size, data);
}

extern "C" void external_memcpy(void* dst, void* src, int size)
{
    Mem::Copy(dst, src, size);
}

extern "C" void *external_memmove(void* dst, void* src, int size)
{
    return Mem::Move(dst, src, size);
}

extern "C" void *external_malloc(int size)
{
    return User::Alloc(size);
}

extern "C" void external_free(void *buf)
{
    User::Free(buf);
}



