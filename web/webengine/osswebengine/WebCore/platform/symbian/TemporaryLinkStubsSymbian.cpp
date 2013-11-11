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
* Description:  
*
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
#include "Event.h"
#include "FileChooser.h"
#include "Font.h"
#include "IntPoint.h"
#include "Widget.h"
#include "GraphicsContext.h"
#include "Cursor.h"
#include "loader.h"
#include "KURL.h"
#include "ScrollBar.h"
#include "ScrollBar.h"
#include "Path.h"
#include "PlatformScrollbar.h"
#include "PlatformMouseEvent.h"
#include "CookieJar.h"
#include "Screen.h"
#include "History.h"
#include "Language.h"
#include "LocalizedStrings.h"
#include "RenderTheme.h"
//#include "BrowserExtensionWin.h"
//#include "RenderThemeWin.h"
#include "TextBoundaries.h"
#include "AXObjectCache.h"
//#include "RenderPopupMenuWin.h"
#include "Icon.h"
#include "IconDatabase.h"
#include "Editor.h"
#include "ClipboardAccessPolicy.h"
#include "Clipboard.h"
#include "Sound.h"
#include "PageCache.h"
#include "SystemTime.h"
#include "AffineTransform.h"
#include "Path.h"
#include "ContextMenu.h"
#include "HitTestResult.h"
#include "ScrollView.h"
#include "SearchPopupMenu.h"
#include "Frame.h"
#include "FrameView.h"
#include "AffineTransform.h"
#include "DragData.h"
#include "DocumentFragment.h"

#include "../bidi.h"
#include <StringLoader.h>
#include <webkit.rsg>


namespace WebCore {

static void notImplemented() { /*puts("Not yet implemented"); */}

void Widget::removeFromParent() { }

Cursor::Cursor(Image*, const IntPoint&) { notImplemented(); }

String searchableIndexIntroduction() { notImplemented(); return String(); }

int findNextSentenceFromIndex(UChar const*,int,int,bool) { notImplemented(); return 0; }
void findSentenceBoundary(UChar const*,int,int,int*,int*) { notImplemented(); }
int findNextWordFromIndex(UChar const*,int,int,bool) { notImplemented(); return 0; }

Vector<char> ServeSynchronousRequest(Loader*, DocLoader*, const ResourceRequest&, ResourceResponse&) { notImplemented(); return Vector<char>(); }
void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect, int thickness) { notImplemented(); }

int screenDepthPerComponent(const Widget*) { notImplemented(); return 0; }
bool screenIsMonochrome(const Widget*) { notImplemented(); return false; }

/********************************************************/
/* Completely empty stubs (mostly to allow DRT to run): */
/********************************************************/
static Cursor localCursor;
const Cursor& moveCursor() { return localCursor; }

bool AXObjectCache::gAccessibilityEnabled = false;

//bool historyContains(DeprecatedString const&) { return false; }
String submitButtonDefaultLabel() { return "Submit"; }
String inputElementAltText() { return DeprecatedString(); }
String resetButtonDefaultLabel() { return "Reset"; }
String defaultLanguage() { return "en"; }

void findWordBoundary(UChar const* str,int len,int position,int* start, int* end) {*start=position; *end=position; }

bool CheckIfReloading(DocLoader*) { return false; }
void CheckCacheObjectStatus(DocLoader*, CachedResource*) { }
int NumberOfPendingOrLoadingRequests(DocLoader*) { return 0; }

void GraphicsContext::setShadow(IntSize const&,int,Color const&) { }
void GraphicsContext::clearShadow() { }
void GraphicsContext::beginTransparencyLayer(float) { }
void GraphicsContext::endTransparencyLayer() { }
void GraphicsContext::clearRect(const FloatRect&) { }
void GraphicsContext::strokeRect(const FloatRect&, float) { }
void GraphicsContext::setLineCap(LineCap) { }
void GraphicsContext::setLineJoin(LineJoin) { }
void GraphicsContext::setMiterLimit(float) { }
void GraphicsContext::setAlpha(float) { }
void GraphicsContext::setCompositeOperation(CompositeOperator) { }
void GraphicsContext::clip(const Path&) { }
void GraphicsContext::translate(float x, float y) { }
void GraphicsContext::rotate(float) { }
void GraphicsContext::scale(const FloatSize&) { }


void Path::moveTo(const FloatPoint&) { }
void Path::addQuadCurveTo(const FloatPoint&, const FloatPoint&) { }
void Path::addBezierCurveTo(const FloatPoint&, const FloatPoint&, const FloatPoint&) { }
void Path::addArcTo(const FloatPoint&, const FloatPoint&, float) { }
void Path::addArc(const FloatPoint&, float, float, float, bool) { }
void Path::apply(void* info, PathApplierFunction) const { }


Color focusRingColor() { return 0xFF0000FF; }
void setFocusRingColorChangeFunction(void (*)()) { }

String signedPublicKeyAndChallengeString(unsigned keySizeIndex, const String& challengeString, const KURL&) { return String(); }
Vector<String> supportedKeySizes() { return Vector<String>(); }

Icon::Icon() { notImplemented(); }
Icon::~Icon() { notImplemented(); }
PassRefPtr<Icon> Icon::newIconForFile(const String& filename) { notImplemented(); return PassRefPtr<Icon>(new Icon()); }
void Icon::paint(GraphicsContext*, const IntRect&) { notImplemented(); }

void Widget::invalidate() { }
void Widget::invalidateRect(const IntRect& r) { }

String fileButtonChooseFileLabel() 
    { 
    String chooseLabel;
    HBufC* loadStr = StringLoader::LoadLC(R_QTN_BROWSER_CHOOSE_FILE_SYM);
    chooseLabel.append(*loadStr);
    CleanupStack::PopAndDestroy();
    return chooseLabel;
    }

// editor
PassRefPtr<Clipboard> Editor::newGeneralClipboard(ClipboardAccessPolicy policy) { return 0; }


void systemBeep() { }
float userIdleTime()                                       { notImplemented(); return 0.0; }
IntSize dragImageSize(void *)                              { notImplemented(); return IntSize(); }
void deleteDragImage(void *)                               { notImplemented(); }

// AffineTransform
AffineTransform::AffineTransform()                                  { notImplemented(); }
AffineTransform& AffineTransform::scale(double sx, double sy)       { notImplemented(); return *this; }
AffineTransform& AffineTransform::rotate(double sx)                 { notImplemented(); return *this; }
AffineTransform& AffineTransform::translate(double, double)         { notImplemented(); return *this; }

// Path
void Path::transform(const AffineTransform&)               { notImplemented(); }

// ContextMenu
ContextMenu::ContextMenu(const HitTestResult& h) : m_hitTestResult(h)   { notImplemented(); }
ContextMenu::~ContextMenu()                                             { notImplemented(); }
void ContextMenu::setPlatformDescription(void* )                        { notImplemented(); }
String ContextMenuItem::title() const									{ notImplemented(); return String(); }
ContextMenuAction ContextMenuItem::action() const						{ notImplemented(); return ContextMenuItemTagNoAction; }


void ContextMenuItem::setEnabled(bool)									{ notImplemented(); }
void ContextMenuItem::setChecked(bool)									{ notImplemented(); }
ContextMenuItemType ContextMenuItem::type() const						{ notImplemented(); return ActionType; }
ContextMenuItem::ContextMenuItem(ContextMenuItemType, ContextMenuAction, const String&, ContextMenu *)  { notImplemented(); }
ContextMenuItem::~ContextMenuItem()                                     { notImplemented(); }
void ContextMenuItem::setSubMenu(ContextMenu *)                { notImplemented(); }
void ContextMenu::appendItem(ContextMenuItem &)                { notImplemented(); }

String contextMenuItemTagShowSpellingPanel(bool show)                   { notImplemented(); return String(); }
String contextMenuItemTagCheckSpelling()                                { notImplemented(); return String(); }
String contextMenuItemTagCheckSpellingWhileTyping()                     { notImplemented(); return String(); }
String contextMenuItemTagCheckGrammarWithSpelling()                     { notImplemented(); return String(); }
String contextMenuItemTagBold()                                         { notImplemented(); return String(); }
String contextMenuItemTagItalic()                                       { notImplemented(); return String(); }
String contextMenuItemTagUnderline()                                    { notImplemented(); return String(); }
String contextMenuItemTagOutline()                                      { notImplemented(); return String(); }
String contextMenuItemTagNoGuessesFound()                               { notImplemented(); return String(); }
String contextMenuItemTagOpenFrameInNewWindow()                         { notImplemented(); return String(); }
String contextMenuItemTagStop()                                         { notImplemented(); return String(); }
String contextMenuItemTagGoForward()                                    { notImplemented(); return String(); }
String contextMenuItemTagGoBack()                                       { notImplemented(); return String(); }
String contextMenuItemTagCopy()                                         { notImplemented(); return String(); }
String contextMenuItemTagSearchWeb()                                    { notImplemented(); return String(); }
String contextMenuItemTagCopyImageToClipboard()                         { notImplemented(); return String(); }
String contextMenuItemTagDownloadImageToDisk()                          { notImplemented(); return String(); }
String contextMenuItemTagOpenImageInNewWindow()                         { notImplemented(); return String(); }
String contextMenuItemTagCopyLinkToClipboard()                          { notImplemented(); return String(); }
String contextMenuItemTagDownloadLinkToDisk()                           { notImplemented(); return String(); }
String contextMenuItemTagOpenLinkInNewWindow()                          { notImplemented(); return String(); }
String contextMenuItemTagOpenLink()                                     { notImplemented(); return String(); }
String contextMenuItemTagRightToLeft()                                  { notImplemented(); return String(); }
String contextMenuItemTagLeftToRight()                                  { notImplemented(); return String(); }
String contextMenuItemTagDefaultDirection()                             { notImplemented(); return String(); }
String contextMenuItemTagPaste()                                        { notImplemented(); return String(); }
String contextMenuItemTagIgnoreGrammar()                                { notImplemented(); return String(); }
String contextMenuItemTagIgnoreSpelling()                               { notImplemented(); return String(); }
String contextMenuItemTagReload()                                       { notImplemented(); return String(); }
String contextMenuItemTagLearnSpelling()                                { notImplemented(); return String(); }
String contextMenuItemTagCut()                                          { notImplemented(); return String(); }
String contextMenuItemTagWritingDirectionMenu()                         { notImplemented(); return String(); }
String contextMenuItemTagFontMenu()                                     { notImplemented(); return String(); }
String contextMenuItemTagInspectElement()                               { notImplemented(); return String(); }
String searchMenuNoRecentSearchesText()                                 { notImplemented(); return String(); }
String searchMenuRecentSearchesText()                                   { notImplemented(); return String(); }
String searchMenuClearRecentSearchesText()                              { notImplemented(); return String(); }
String unknownFileSizeText()                                            { notImplemented(); return String(); }


// Drag stuff
void* createDragImageFromImage(class Image *)                  { notImplemented(); return 0; }
void* dissolveDragImageToFraction(void *, float)               { notImplemented(); return 0; }
void* createDragImageIconForCachedImage(class CachedImage *)  { notImplemented(); return 0; }
DragImageRef scaleDragImage(DragImageRef, FloatSize )                   { notImplemented(); return 0; }

PlatformScrollbar* ScrollView::scrollbarUnderMouse(const PlatformMouseEvent &) { notImplemented(); return 0; }

// Frame
void Frame::setNeedsReapplyStyles()                                     { notImplemented(); }

// FrameView
void FrameView::updateBorder()                                          { notImplemented(); }

String DragData::asURL(WebCore::String*) const							{ notImplemented(); return String(); }
void DragData::asFilenames(Vector<String>&) const                       { notImplemented(); }

Clipboard* DragData::createClipboard(WebCore::ClipboardAccessPolicy) const	{ notImplemented(); return 0; }
String DragData::asPlainText() const									{ notImplemented(); return String(); }
bool DragData::canSmartReplace() const									{ notImplemented(); return false; }
Color DragData::asColor() const											{ notImplemented(); return Color(); }
bool DragData::containsColor() const									{ notImplemented(); return false; }
bool DragData::containsPlainText() const								{ notImplemented(); return false; }
bool DragData::containsURL() const										{ notImplemented(); return false; }
PassRefPtr<DocumentFragment> DragData::asFragment(WebCore::Document*) const	{ notImplemented(); return 0; }
bool DragData::containsCompatibleContent() const						{ notImplemented(); return false; }
bool DragData::containsFiles() const                                    { notImplemented(); return false; }


void ScrollView::wheelEvent(WebCore::PlatformWheelEvent&)				{ notImplemented(); }

void AffineTransform::map(double, double, double*, double*) const		{ notImplemented(); }

//Threading
void callOnMainThread(void (*)()) { notImplemented(); }

}
