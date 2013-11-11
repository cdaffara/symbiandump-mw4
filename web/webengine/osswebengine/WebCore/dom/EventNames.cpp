/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 2005 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#ifdef AVOID_STATIC_CONSTRUCTORS
#define DOM_EVENT_NAMES_HIDE_GLOBALS 1
#endif

#include "EventNames.h"
#include "StaticConstructors.h"

namespace WebCore { namespace EventNames {

#define DEFINE_EVENT_GLOBAL(name) \
    DEFINE_GLOBAL(AtomicString, name##Event, #name)
DOM_EVENT_NAMES_FOR_EACH(DEFINE_EVENT_GLOBAL)

static bool initialized = false;

void init()
{
    if (!initialized) {
        // Use placement new to initialize the globals.
        
        AtomicString::init();
        #define INITIALIZE_GLOBAL(name) new ((void*)&name##Event) AtomicString(#name);
        DOM_EVENT_NAMES_FOR_EACH(INITIALIZE_GLOBAL)
        initialized = true;
    }
}

void remove()
{
    if( initialized ) {
        #define DESTROY_GLOBAL(name) delete ((AtomicString*)&name##Event)->impl();
        DOM_EVENT_NAMES_FOR_EACH(DESTROY_GLOBAL)
        AtomicString::remove();

        initialized = false;
    }

} //remove()

} }
