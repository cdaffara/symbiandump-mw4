/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "History.h"

#include "Frame.h"
#include "FrameLoader.h"

#if PLATFORM(SYMBIAN)
#include "WebCoreFrameBridge.h"
#endif

namespace WebCore {

History::History(Frame* frame)
    : m_frame(frame)
{
}

Frame* History::frame() const
{
    return m_frame;
}

void History::disconnectFrame()
{
    m_frame = 0;
}

unsigned History::length() const
{
    if (!m_frame)
        return 0;
#if PLATFORM(SYMBIAN)
    return m_frame->bridge()->getHistoryLength();
#else
    return m_frame->loader()->getHistoryLength();
#endif
}

void History::back()
{
    if (!m_frame)
        return;
#if PLATFORM(SYMBIAN)
    m_frame->bridge()->scheduleHistoryNavigation(-1);
#else
    m_frame->loader()->scheduleHistoryNavigation(-1);
#endif
}

void History::forward()
{
    if (!m_frame)
        return;
#if PLATFORM(SYMBIAN)
    m_frame->bridge()->scheduleHistoryNavigation(1);
#else
    m_frame->loader()->scheduleHistoryNavigation(1);
#endif
}

void History::go(int distance)
{
    if (!m_frame)
        return;
#if PLATFORM(SYMBIAN)
    m_frame->bridge()->scheduleHistoryNavigation(distance);
#else
    m_frame->loader()->scheduleHistoryNavigation(distance);
#endif
}

} // namespace WebCore
