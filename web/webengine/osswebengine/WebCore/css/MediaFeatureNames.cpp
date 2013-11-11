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
#define CSS_MEDIAQUERY_NAMES_HIDE_GLOBALS 1
#endif

#include "MediaFeatureNames.h"
#include "StaticConstructors.h"

namespace WebCore {
namespace MediaFeatureNames {

#define DEFINE_MEDIAFEATURE_GLOBAL(name, str) \
    DEFINE_GLOBAL(AtomicString, name##MediaFeature, str)
CSS_MEDIAQUERY_NAMES_FOR_EACH_MEDIAFEATURE(DEFINE_MEDIAFEATURE_GLOBAL)
#undef DEFINE_MEDIAFEATURE_GLOBAL

static bool initialized;

void init()
{
    if (!initialized) {
       // Use placement new to initialize the globals.

        AtomicString::init();
        #define INITIALIZE_GLOBAL(name, str) new ((void*)&name##MediaFeature) AtomicString(str);
        CSS_MEDIAQUERY_NAMES_FOR_EACH_MEDIAFEATURE(INITIALIZE_GLOBAL)
        #undef INITIALIZE_GLOBAL
        initialized = true;
    }
}

void remove()
{
    if( initialized ) {
        #define DESTROY_GLOBAL(name, str) delete ((AtomicString*)&name##MediaFeature)->impl();
        CSS_MEDIAQUERY_NAMES_FOR_EACH_MEDIAFEATURE(DESTROY_GLOBAL)
        #undef DESTROY_GLOBAL
        AtomicString::remove();

        initialized = false;
    }

} //remove()

} // namespace MediaFeatureNames
} // namespace WebCore