/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Frame in webkit side
*
*/

#include "config.h"
#include <../bidi.h>
#include <e32math.h>
#include <e32base.h>
#include "WebTabbedNavigation.h"
#include "WebUtil.h"
#include "Document.h"
#include "Frame.h"
#include "HitTestRequest.h"
#include "HitTestResult.h"
#include "HTMLNames.h"
#include "FrameTree.h"
#include "BrCtlDefs.h"
#include "WebCursor.h"
#include "StaticObjectsContainer.h"
#include "Page.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include <wtf/RefPtr.h>
#include "EventHandler.h"
#include "WebKitLogger.h"
#include "FocusController.h"
#include "RenderListBox.h"
#include "HTMLSelectElement.h"
#include "HTMLInputElement.h"

using namespace WebCore;
using namespace HTMLNames;

const int KMaxJumpPercent = 90;
const int KScrollWhenNotFound = 5;

WebTabbedNavigation::WebTabbedNavigation(WebView* webView)
{
    m_webView = webView;
    clear();
}

WebTabbedNavigation::~WebTabbedNavigation()
{
}

void WebTabbedNavigation::clear()
{
    // Different initialization for mirrored layouts?
    m_selectedElementRect.SetRect(0, 0, m_webView->Rect().Width(), 0);
    m_initializedForPage = false;
    m_firstNavigationOnPage = true;
    m_focusPosition = TPoint(0,0);
    m_node = NULL;
}

void WebTabbedNavigation::initializeForPage()
{
    if (!m_initializedForPage) {
        if (navigate(0, 1)) {
            m_initializedForPage = true;
            m_firstNavigationOnPage = false;
        }
    }
}

void WebTabbedNavigation::updateCursorPosition(const TPoint& pos)
{
    m_focusPosition = pos;
    m_selectedElementRect.SetRect(pos.iX, pos.iY, pos.iX + 1, pos.iY + 1);
}

void WebTabbedNavigation::focusedElementChanged(Element* element)
{
    m_initializedForPage = true;
    m_firstNavigationOnPage = false;
    m_node = element;
    m_selectedElementRect = element->getRect().Rect();
    m_focusPosition = StaticObjectsContainer::instance()->webCursor()->position();
}


void WebTabbedNavigation::resetNavigationIfNeeded(TPoint& contentPos, TSize& contentSize, 
                                                  Frame* focusedFrame, int horizontalDir, int verticalDir)
{
    TPoint docBrViewCoord = kit(focusedFrame)->frameView()->frameCoordsInViewCoords(
                                           TPoint(contentSize.iWidth, contentSize.iHeight));
    TRect docRect = TRect(TPoint(0,0), docBrViewCoord);
    if (!docRect.Contains(m_focusPosition)) {
        TInt viewW = m_webView->Rect().Width();
        TInt viewH = m_webView->Rect().Height();
        if (m_focusPosition.iX > contentSize.iWidth) {
            m_focusPosition.iX = (horizontalDir == -1) ? contentPos.iX + viewW : contentPos.iX;
        }
        
        if (m_focusPosition.iY > contentSize.iHeight) {
            m_focusPosition.iY = (verticalDir == -1) ? contentPos.iY + viewH : contentPos.iY;
        }
    
        m_selectedElementRect.SetRect(m_focusPosition.iX, m_focusPosition.iY, 
                                      m_focusPosition.iX, m_focusPosition.iY);
        m_node = NULL;    
    }
}


TPoint WebTabbedNavigation::focusPointFromFocusedNode(Frame* frame, int horizontalDir, int verticalDir)
{
    TPoint oldFocusPoint(m_focusPosition);
    TPoint focusPosition(m_focusPosition);
    TRect  selectedElementRect(m_selectedElementRect);
    
    if (frame->document()) {
        Node* focusedNode = frame->document()->focusedNode();
        if (focusedNode) {
            m_node = focusedNode;
            selectedElementRect = focusedNode->getRect().Rect();
            Frame* frame = focusedNode->document()->frame();
            selectedElementRect = kit(frame)->frameView()->frameCoordsInViewCoords(selectedElementRect);

            // Move the focus to the visible edge of the current object
            TRect elemVisibleRect = selectedElementRect;
            if (elemVisibleRect.Intersects(m_webView->Rect())) {
                 
                elemVisibleRect.Intersection(m_webView->Rect());
                if (horizontalDir == -1) {
                    focusPosition.iX = elemVisibleRect.iTl.iX;
                }
                else if (horizontalDir == 1) {
                    focusPosition.iX = elemVisibleRect.iBr.iX;
                }
                
                if (verticalDir == -1) {
                    focusPosition.iY = elemVisibleRect.iTl.iY;
                }
                else if (verticalDir == 1) {
                    focusPosition.iY = elemVisibleRect.iBr.iY;
                }
                
                if ((verticalDir == 0) && (horizontalDir == 0)) {
                    focusPosition = elemVisibleRect.Center();
                }
                m_focusPosition = focusPosition;
            }
            m_selectedElementRect = selectedElementRect;
        }
    }
    return oldFocusPoint;
}


void WebTabbedNavigation::calcSearchViewRect(int horizontalDir, int verticalDir, TRect& view)
{
    
    TPoint br;
    TPoint viewBr = m_webView->Rect().iBr;
    br.iX = viewBr.iX * KMaxJumpPercent / m_webView->scalingFactor();
    br.iY = viewBr.iY * KMaxJumpPercent / m_webView->scalingFactor();
    // define the view rect where we are looking for a match
    int x, y;
    if (horizontalDir == -1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY - br.iY, 
                m_focusPosition.iX , m_focusPosition.iY + br.iY);
    }
    else if (horizontalDir == 1) {
        view.SetRect(m_focusPosition.iX, m_focusPosition.iY - br.iY, 
                m_focusPosition.iX + br.iX, m_focusPosition.iY + br.iY);
    }
    else if (verticalDir == -1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY - br.iY, 
                     m_focusPosition.iX + br.iX, m_focusPosition.iY);
    }
    else if (verticalDir == 1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY, 
                     m_focusPosition.iX + br.iX, m_focusPosition.iY + br.iY);
    }
}

TPoint WebTabbedNavigation::updateCursorPosAfterScroll(Frame* frame, int horizontalDir, int verticalDir)
{
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    TPoint oldPos = cursor->position();
    
    focusPointFromFocusedNode(frame, horizontalDir, verticalDir);
    if (m_node && !m_selectedElementRect.Intersects(m_webView->Rect())) {
        static_cast<Node*>(m_node)->document()->setFocusedNode(NULL);
        m_node = NULL;
    }
    
    cursor->setPosition(m_focusPosition - TPoint(horizontalDir, verticalDir));
    return oldPos;
}

bool WebTabbedNavigation::navigate(int horizontalDir, int verticalDir)
{
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    if (handleSelectElementScrolling(m_webView, verticalDir)) {
        cursor->cursorUpdate(true);
        return true;
    }
    Frame* mainFrame = core(m_webView->mainFrame());
    FocusController* focusController = m_webView->page()->focusController();
    Frame* focusedFrame = focusController->focusedOrMainFrame();        
    TSize contentSize = kit(focusedFrame)->frameView()->contentSize();
    TPoint contentPos = kit(focusedFrame)->frameView()->contentPos();
    
    // DOM can be changed so check if we are still inside the document
    // If not reset tabbed navigation parameters to the closest point in document.
    resetNavigationIfNeeded(contentPos, contentSize, focusedFrame, horizontalDir, verticalDir);
    
    
    bool ret = m_firstNavigationOnPage;
    
    if (focusedFrame == NULL) focusedFrame = mainFrame;
    TPoint oldFocusPoint =  focusPointFromFocusedNode(focusedFrame, horizontalDir, verticalDir);
    TRect view;
    calcSearchViewRect(horizontalDir, verticalDir, view);

    // walk all focusable nodes    
    TPoint selectedPoint(0, 0);
    TRect selectedRect(0, 0, 0, 0);
    Node* selectedNode = bestFitFocusableNode(mainFrame, view, horizontalDir, verticalDir, 
                                              selectedPoint, selectedRect);
    
    // Remember new selection
    contentPos = kit(mainFrame)->frameView()->contentPos();
    if (selectedNode) {
        // Found an element to jump to
        m_selectedElementRect = selectedRect;
        m_focusPosition = selectedPoint;
        m_node = selectedNode;
        selectedNode->document()->setFocusedNode(selectedNode);
        m_webView->page()->focusController()->setFocusedFrame(selectedNode->document()->frame());
        
        // And scroll to the selected element
        RenderLayer *layer = selectedNode->renderer()->enclosingLayer();
        if (layer) {
            layer->scrollRectToVisible(selectedNode->getRect(), RenderLayer::gAlignCenterIfNeeded, RenderLayer::gAlignCenterIfNeeded);
            WebFrameView* fv = kit(selectedNode->document()->frame())->frameView();
            TRect newRect = fv->frameCoordsInViewCoords(selectedNode->getRect().Rect());
            selectedRect = newRect;
            selectedPoint = potentialFocusPoint(horizontalDir, verticalDir, newRect);
            m_selectedElementRect = selectedRect;
            m_focusPosition = selectedPoint;
                        
            cursor->updatePositionAndElemType(m_focusPosition);
            // special handling for Select-Multi
            if (m_webView->focusedElementType() == TBrCtlDefs::EElementSelectMultiBox) {
                handleMultiSelect(horizontalDir, verticalDir);
            }
            
            m_webView->sendMouseEventToEngine(TPointerEvent::EMove, cursor->position(), mainFrame);
            ret = true;
        }
    }
    else {
        if (!m_firstNavigationOnPage) {
            TInt vWidth = m_webView->Rect().Width();
            TInt vHeight = m_webView->Rect().Height();
            kit(mainFrame)->frameView()->scrollTo(contentPos + 
                                                  TPoint(horizontalDir * vWidth / KScrollWhenNotFound, 
                                                         verticalDir * vHeight / KScrollWhenNotFound));
          
            cursor->updatePositionAndElemType(m_focusPosition - TPoint(horizontalDir, verticalDir));
        }
    }
    cursor->cursorUpdate(true);
    return ret;
}



Node* WebTabbedNavigation::bestFitFocusableNode(Frame* topFrame, TRect& viewRect, int horizontalDir, int verticalDir, //input
                                                TPoint& selectedPoint, TRect& selectedRect ) //output
{
   Node* selectedNode = NULL;
   Frame* f = topFrame;
    while ( f ) {
        PassRefPtr<HTMLCollection> elements = f->document()->all();   
        TRect frameRect = kit(f)->frameView()->rectInGlobalCoords();
        for (Node* n = elements->firstItem(); n; n = elements->nextItem()) {
            if (n == m_node) continue;
            if (n->isFocusable() && n->isElementNode() && !n->hasTagName(iframeTag) && !n->hasTagName(frameTag)) {
                // Does the node intersect with the view rect?
                TRect nodeRect = n->getRect().Rect();
                nodeRect = kit(f)->frameView()->frameCoordsInViewCoords(nodeRect);
                if (nodeRect.Intersects(viewRect) && 
                    shouldConsiderRect(nodeRect, viewRect, horizontalDir, verticalDir)) {
                    // Compare nodes and select the best fit
                    TPoint newFocusPoint = potentialFocusPoint(horizontalDir, verticalDir, nodeRect);
                    if (selectNode(horizontalDir, verticalDir, selectedRect, nodeRect, selectedPoint, newFocusPoint)) {
                        // found a better fit
                        selectedNode = n;
                        selectedRect.SetRect(nodeRect.iTl, nodeRect.iBr);
                        selectedPoint = newFocusPoint;
                    }
                } // if (nodeRect.Intersects(rect[i])
            } // if (n->isFocusable() && n->isElementNode())
        } // for (Node* n = elements->firstItem(); n; n = elements->nextItem())
        f = f->tree()->traverseNext();
    } // while ( f )
    return selectedNode;
}


bool WebTabbedNavigation::shouldConsiderRect(TRect& rect, TRect& searchRect, int horizontalDir, int verticalDir)
{
    bool considerX = false;
    bool considerY = false;
    
    if (horizontalDir == 1) {
        considerX = (rect.iTl.iX >= searchRect.iTl.iX);
    }
    else if (horizontalDir == -1) {
        considerX = (rect.iBr.iX <= searchRect.iBr.iX);
    }
    
    if (verticalDir == 1) {
        considerY = (rect.iTl.iY >= searchRect.iTl.iY);
    }
    else if (verticalDir == -1) {
        considerY = (rect.iBr.iY <= searchRect.iBr.iY);
    }
    
    return considerX || considerY;
    
}

void WebTabbedNavigation::handleMultiSelect(int horizontalDir, int verticalDir)
{
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    Node* n = static_cast<Node*>(m_node);
    WebFrameView* fv = kit(n->document()->frame())->frameView();
    Element* e = static_cast<Element*>(m_node);
    if (e->isControl()) {
        HTMLGenericFormElement* ie = static_cast<HTMLGenericFormElement*>( e );
        if (ie->type() == "select-multiple") {
            RenderListBox* render = static_cast<RenderListBox*>(e->renderer());
            HTMLSelectElement* selectElement = static_cast<HTMLSelectElement*>( e );
            TRect itemRect = render->itemRect(0, 0, render->indexOffset()).Rect();
            TRect itemRectViewCoord = fv->frameCoordsInViewCoords(itemRect);
            itemRectViewCoord.Move(m_selectedElementRect.iTl);
            itemRectViewCoord.Intersection(fv->topView()->Rect());           
            
            TPoint cursorPoint = itemRect.Center() + m_selectedElementRect.iTl;
            if (cursorPoint != cursor->position()) {
                cursor->setPosition(cursorPoint);
            }
        }
    }
}

bool WebTabbedNavigation::selectNode(int horizontalDir, int verticalDir, TRect& selectedRect, TRect& newNodeRect, TPoint& selectedPoint, TPoint& newFocusPoint)
{
    if (selectedPoint == TPoint(0, 0)) { // first selected node fixme: ensure not to divert direction too much
        return true;
    }
    int selectedDist = distanceFunction(horizontalDir, verticalDir, selectedRect, selectedPoint);
    int newDist = distanceFunction(horizontalDir, verticalDir, newNodeRect, newFocusPoint);

    return newDist < selectedDist;
}

TPoint WebTabbedNavigation::potentialFocusPoint(int horizontalDir, int verticalDir, TRect& newNodeRect)
{
    int x = 0;
    int y = 0;
    // Focus points must be inside the element and not on the border in order for the outline to be drawn.
    if (horizontalDir) { // horizontal
        if (horizontalDir == -1) { // left
            x = newNodeRect.iBr.iX - 1;
        }
        else if (horizontalDir == 1) { // right
            x = newNodeRect.iTl.iX + 1;
        }
        if (newNodeRect.iBr.iY < m_focusPosition.iY) {
            y = newNodeRect.iBr.iY - 1;
        }
        else if (newNodeRect.iTl.iY > m_focusPosition.iY) {
            y = newNodeRect.iTl.iY + 1;
        }
        else {
            y = m_focusPosition.iY;
        }
    }
    else { // vertical
        if (verticalDir == -1) { // up
            y = newNodeRect.iBr.iY - 1;
        }
        else if (verticalDir == 1) { // down
            y = newNodeRect.iTl.iY + 1;
        }
        if (newNodeRect.iBr.iX < m_focusPosition.iX) {
            x = newNodeRect.iBr.iX - 1;
        }
        else if (newNodeRect.iTl.iX > m_focusPosition.iX) {
            x = newNodeRect.iTl.iX + 1;
        }
        else {
            x = m_focusPosition.iX;
        }
    }
    return TPoint(x, y);
}

int WebTabbedNavigation::distanceFunction(int horizontalDir, int verticalDir, TRect& rect, TPoint& point)
{
    // Based on http://www.w3.org/TR/WICD/#focus-navigation
    TReal x, y, euclidianDistance;
    int sameAxisDist, otherAxisDist, overlap;

    Math::Pow(x, (m_focusPosition.iX - point.iX), 2.0);
    Math::Pow(y, (m_focusPosition.iY - point.iY), 2.0);
    Math::Sqrt(euclidianDistance, x + y);
    sameAxisDist = horizontalDir * (point.iX - m_focusPosition.iX) + verticalDir * (point.iY - m_focusPosition.iY);
    otherAxisDist = (horizontalDir) ? (point.iY - m_focusPosition.iY) : (point.iX - m_focusPosition.iX) ;
    otherAxisDist = (otherAxisDist < 0 ) ? (otherAxisDist * -1) : otherAxisDist;
    if (horizontalDir) { // horizontal
        if (rect.iBr.iY < m_selectedElementRect.iTl.iY || rect.iTl.iY > m_selectedElementRect.iBr.iY) {
            overlap = 0;
        }
        else {
            int top = max(m_selectedElementRect.iTl.iY, rect.iTl.iY);
            int bottom = std::min(m_selectedElementRect.iBr.iY, rect.iBr.iY);
            overlap = bottom - top;
            if (overlap == rect.Height()) {
                euclidianDistance = (euclidianDistance * rect.Height()) / m_selectedElementRect.Height();
                otherAxisDist = 0;
            }
        }
    }
    else { // vertical    
        if (rect.iBr.iX < m_selectedElementRect.iTl.iX || rect.iTl.iX > m_selectedElementRect.iBr.iX) {
            overlap = 0;
        }
        else {
            int top = max(m_selectedElementRect.iTl.iX, rect.iTl.iX);
            int bottom = std::min(m_selectedElementRect.iBr.iX, rect.iBr.iX);
            overlap = bottom - top;
            if (overlap == rect.Width()) {
                euclidianDistance = (euclidianDistance * rect.Width()) / m_selectedElementRect.Width();
                otherAxisDist = 0;
            }
        }
    }
    long ed, o;
    Math::Int(ed, euclidianDistance);
    Math::Int(o, sqrt(overlap));
    return ed + sameAxisDist + 2 * otherAxisDist - o;
}

