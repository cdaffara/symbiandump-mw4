/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   extension of WebCore::WebInspectorClient
*
*/

#ifndef WebInspectorClient_h
#define WebInspectorClient_h

#include "InspectorClient.h"

namespace WebCore {
    class Node;
    class Page;
    class String;

    class WebInspectorClient : public InspectorClient {
    public:
        virtual void inspectorDestroyed();

        virtual Page* createPage();

        virtual void showWindow();
        virtual void closeWindow();

        virtual void attachWindow();
        virtual void detachWindow();

        virtual void highlight(Node*);
        virtual void hideHighlight();
        virtual void inspectedURLChanged(const String& newURL);
    };
}

#endif
