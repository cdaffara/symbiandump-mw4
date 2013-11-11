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

#ifndef JSSVGAnimatedPoints_H
#define JSSVGAnimatedPoints_H


#if ENABLE(SVG)

#include "kjs_binding.h"

namespace WebCore {

class SVGAnimatedPoints;

class JSSVGAnimatedPoints : public KJS::DOMObject {
public:
    JSSVGAnimatedPoints(KJS::ExecState*, SVGAnimatedPoints*);
    virtual ~JSSVGAnimatedPoints();
    virtual bool getOwnPropertySlot(KJS::ExecState*, const KJS::Identifier&, KJS::PropertySlot&);
    KJS::JSValue* getValueProperty(KJS::ExecState*, int token) const;
    virtual const KJS::ClassInfo* classInfo() const { return &info; }
    static const KJS::ClassInfo info;

    enum {
        // Attributes
        PointsAttrNum, AnimatedPointsAttrNum
    };
    SVGAnimatedPoints* impl() const { return m_impl.get(); }
private:
    RefPtr<SVGAnimatedPoints> m_impl;
};

KJS::JSValue* toJS(KJS::ExecState*, SVGAnimatedPoints*);
SVGAnimatedPoints* toSVGAnimatedPoints(KJS::JSValue*);

class JSSVGAnimatedPointsPrototype : public KJS::JSObject {
public:
    static KJS::JSObject* self(KJS::ExecState* exec);
    virtual const KJS::ClassInfo* classInfo() const { return &info; }
    static const KJS::ClassInfo info;
    JSSVGAnimatedPointsPrototype(KJS::ExecState* exec)
        : KJS::JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()) { }
};

} // namespace WebCore

#endif // ENABLE(SVG)

#endif