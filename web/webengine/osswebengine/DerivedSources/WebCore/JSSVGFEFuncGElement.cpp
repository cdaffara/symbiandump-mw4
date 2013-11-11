/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"


#if ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)

#include "Document.h"
#include "Frame.h"
#include "SVGDocumentExtensions.h"
#include "SVGElement.h"
#include "SVGAnimatedTemplate.h"
#include "JSSVGFEFuncGElement.h"

#include <wtf/GetPtr.h>

#include "SVGFEFuncGElement.h"

using namespace KJS;

namespace WebCore {

/* Hash table for prototype */

static const HashEntry JSSVGFEFuncGElementPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSSVGFEFuncGElementPrototypeTable = 
{
    2, 1, JSSVGFEFuncGElementPrototypeTableEntries, 1
};

const ClassInfo JSSVGFEFuncGElementPrototype::info = { "SVGFEFuncGElementPrototype", 0, &JSSVGFEFuncGElementPrototypeTable, 0 };

JSObject* JSSVGFEFuncGElementPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSSVGFEFuncGElementPrototype>(exec, "[[JSSVGFEFuncGElement.prototype]]");
}

const ClassInfo JSSVGFEFuncGElement::info = { "SVGFEFuncGElement", &JSSVGComponentTransferFunctionElement::info, 0, 0 };

JSSVGFEFuncGElement::JSSVGFEFuncGElement(ExecState* exec, SVGFEFuncGElement* impl)
    : JSSVGComponentTransferFunctionElement(exec, impl)
{
    setPrototype(JSSVGFEFuncGElementPrototype::self(exec));
}


}

#endif // ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)