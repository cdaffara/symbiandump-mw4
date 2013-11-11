/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef HTMLBgsoundElement_H
#define HTMLBgsoundElement_H

#include "HTMLEmbedElement.h"

#if USE(JAVASCRIPTCORE_BINDINGS)
#include <runtime.h>
#else
namespace KJS { namespace Bindings { class Instance; } }
#endif

namespace WebCore {

class SVGDocument;

class HTMLBgsoundElement : public HTMLPlugInElement {

public:
    HTMLBgsoundElement(Document*);
    ~HTMLBgsoundElement();

    virtual int tagPriority() const { return 0; }

    virtual bool mapToEntry(const QualifiedName& attrName, MappedAttributeEntry& result) const;
    virtual void parseMappedAttribute(MappedAttribute*);

    virtual void attach();
    virtual void detach();
    virtual bool rendererIsNeeded(RenderStyle*);
    virtual RenderObject* createRenderer(RenderArena*, RenderStyle*);
    virtual HTMLTagStatus endTagRequirement() const { return TagStatusOptional; }
    
    virtual bool isURLAttribute(Attribute*) const;

#if USE(JAVASCRIPTCORE_BINDINGS)
    virtual KJS::Bindings::Instance* getInstance() const;
#endif
    
    String src() const;
    void setSrc(const String&);
        
    DeprecatedString url;    
    DeprecatedString serviceType;
};

} // namespace WebCore

#endif // HTMLBgsoundElement_H
