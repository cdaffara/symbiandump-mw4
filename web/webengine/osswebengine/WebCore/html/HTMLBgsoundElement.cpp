/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
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
#include "config.h"
#include "HTMLBgsoundElement.h"

#include "CSSPropertyNames.h"
#include "Frame.h"
#include "Page.h"
#include "Settings.h"
#include "HTMLDocument.h"
#include "HTMLNames.h"
#include "csshelper.h"
#include "RenderPartObject.h"

namespace WebCore {

using namespace HTMLNames;
 
HTMLBgsoundElement::HTMLBgsoundElement(Document* doc) 
    : HTMLPlugInElement(bgsoundTag, doc)
{
}

HTMLBgsoundElement::~HTMLBgsoundElement()
{
#if USE(JAVASCRIPTCORE_BINDINGS)
    // m_instance should have been cleaned up in detach().
    assert(!m_instance);
#endif
}

#if USE(JAVASCRIPTCORE_BINDINGS)
KJS::Bindings::Instance *HTMLBgsoundElement::getInstance() const
{
    Frame* frame = document()->frame();
    if (!frame)
        return 0;

    if (m_instance)
        return m_instance.get();
    
    RenderObject *r = renderer();
    if (!r) {
        Node *p = parentNode();
        if (p && p->hasTagName(objectTag))
            r = p->renderer();
    }

    if (r && r->isWidget()) {
        if (Widget* widget = static_cast<RenderWidget*>(r)->widget()) 
            m_instance = frame->createScriptInstanceForWidget(widget);
    }
    return m_instance.get();

}
#endif

bool HTMLBgsoundElement::mapToEntry(const QualifiedName& attrName, MappedAttributeEntry& result) const
{
    return HTMLPlugInElement::mapToEntry(attrName, result);
}

void HTMLBgsoundElement::parseMappedAttribute(MappedAttribute *attr)
{

    DeprecatedString val = attr->value().deprecatedString();
            
    if (attr->name() == srcAttr) {    
        url = parseURL(attr->value()).deprecatedString();        
    }    
    else {
        HTMLPlugInElement::parseMappedAttribute(attr);        
    }

}

bool HTMLBgsoundElement::rendererIsNeeded(RenderStyle *style)
{
    Frame *frame = document()->frame();
    if (!frame || !frame->page() || 
        !frame->page()->settings() || 
        !frame->page()->settings()->arePluginsEnabled())
        return false;

    Node *p = parentNode();
    if (p && p->hasTagName(objectTag)) {
        assert(p->renderer());
        return false;
    }

    return true;
}

RenderObject *HTMLBgsoundElement::createRenderer(RenderArena *arena, RenderStyle *style)
{
    return new (arena) RenderPartObject(this);
}

void HTMLBgsoundElement::attach()
{
    HTMLPlugInElement::attach();

    if (renderer())
        static_cast<RenderPartObject*>(renderer())->updateWidget(true);
}

void HTMLBgsoundElement::detach()
{
#if USE(JAVASCRIPTCORE_BINDINGS_RUNTIME_H)
    m_instance = 0;
#endif
    HTMLPlugInElement::detach();
}

bool HTMLBgsoundElement::isURLAttribute(Attribute *attr) const
{
    return attr->name() == srcAttr;
}

String HTMLBgsoundElement::src() const
{
    return getAttribute(srcAttr);
}

void HTMLBgsoundElement::setSrc(const String& value)
{
    setAttribute(srcAttr, value);
}

}
