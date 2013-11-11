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

#include "BrCtl.h"
#include "Frame.h"
#include "Cache.h"
#include "DocLoader.h"
#include "IntPoint.h"
#include "string.h"
#include "HTMLNames.h"
#include "EventNames.h"
#include "HTMLInputElement.h"
#include "Document.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebCursor.h"
#include "WebView.h"
#include "FrameView.h"
#include "RenderImage.h"
#include "IntRect.h"
#include "FloatRect.h"
#include "Image.h"
#include "HTMLImageElement.h"
#include "StaticObjectsContainer.h"
#include "SelectionController.h"
#include "WebUtil.h"
#include "ErrorDefs.h"
#include "RenderListBox.h"
#include "RenderTextControl.h"
#include "RenderView.h"
#include "PlatformScrollbar.h"
#include "HTMLSelectElement.h"
#include "WebKitLogger.h"
#include <StringLoader.h>
#include <sslerr.h>
#include <httperr.h>
#include <inet6err.h>
#include <in_sock.h>

using namespace WebCore;
using namespace EventNames;
using namespace HTMLNames;

static const char* KTel = "tel:";
static const char* KMailto = "mailto:";
static const char* KWtai = "wtai:";
static const char* KWtaiMC = "wtai://wp/mc;";
static const char* KWtaiAP = "wtai://wp/ap;";
static const char* KWtaiSD = "wtai://wp/sd;";

_LIT(KPathBegin,"<!--framePathBegin ");
_LIT(KPathEnd," framePathEnd --!>");
_LIT(KMimeWBMP, "image/vnd.wap.wbmp");
_LIT(KMimeOTA, "image/vnd.nokia.ota-bitmap");

TBrCtlDefs::TBrCtlElementType nodeTypeB(Node* node, Frame* frame)
{
    if( !node || !node->isElementNode() )
        return TBrCtlDefs::EElementNone;

    TBrCtlDefs::TBrCtlElementType elType(TBrCtlDefs::EElementNone);
    HTMLElement *e = static_cast<HTMLElement *>(node);

    // get the right element type
    if ( e->hasLocalName(linkTag) ) {
        elType = TBrCtlDefs::EElementAnchor;
    }
    else if ( e->hasLocalName(aTag) ) {
        elType = TBrCtlDefs::EElementAnchor;

        String href = e->getAttribute( hrefAttr );
        if (!href.isNull()) {

            if( href.startsWith( KTel ) ) {
                elType = TBrCtlDefs::EElementTelAnchor;
            }
            else if( href.startsWith( KWtai ) ) {

                if( href.startsWith( KWtaiMC ) || href.startsWith(KWtaiAP) || href.startsWith(KWtaiSD) ) {
                    elType = TBrCtlDefs::EElementTelAnchor;
                }

            }
            else if( href.startsWith( KMailto ) ) {
                elType = TBrCtlDefs::EElementMailtoAnchor;
            }


        }
        else if (!e->getAttribute( ctiAttr ).isNull()) {
            elType = TBrCtlDefs::EElementTelAnchor;
        }
        else if( e->hasLocalName(areaTag) ) {

            elType = TBrCtlDefs::EElementInputBox;
        }
    }
    else if (e->isControl()) {
        HTMLGenericFormElement* ie = static_cast<HTMLGenericFormElement*>( e );
        // default for form element
        elType = TBrCtlDefs::EElementInputBox;
        if (ie->type() == "radio")
            elType = ((HTMLInputElement*)ie)->checked() ? TBrCtlDefs::EElementRadioButtonSelected : TBrCtlDefs::EElementRadioButtonUnSelected;
        else if (ie->type() == "checkbox")
            elType = ((HTMLInputElement*)ie)->checked() ? TBrCtlDefs::EElementCheckBoxChecked : TBrCtlDefs::EElementCheckBoxUnChecked;
        else if (ie->type() == "button" || ie->type() == "reset" || ie->type() == "submit")
            elType = TBrCtlDefs::EElementButton;
        else if (ie->type() == "file") {
            if (((HTMLInputElement*)ie)->value() == String())
                elType = TBrCtlDefs::EElementFileSelectionBoxNoContent;
            else
                elType = TBrCtlDefs::EElementFileSelectionBoxWithContent;
        }
        else if (ie->type() == "select-multiple")
            elType = TBrCtlDefs::EElementSelectMultiBox;
        else if ( ie->type() == "select-one") {
            if(!e->getAttribute(sizeAttr).isNull() && !(e->getAttribute(sizeAttr) == "1")) {
                elType = TBrCtlDefs::EElementSelectMultiBox;
            }
            else {
            elType = TBrCtlDefs::EElementSelectBox;
            }
        }
        else if (ie->type() == "textarea")
            elType = TBrCtlDefs::EElementTextAreaBox;
    }
    else if( e->hasLocalName(objectTag) || e->hasLocalName(embedTag) ) {
        if( e->renderer() && e->renderer()->isWidget() )
            if(node->active())
                elType = TBrCtlDefs::EElementActivatedObjectBox;
            else
            elType = TBrCtlDefs::EElementObjectBox;
        else
            elType = TBrCtlDefs::EElementNone;
    }

    // change to activated input box
    WebView* v = control(frame)->webView();
    if ((elType == TBrCtlDefs::EElementInputBox) && v && v->isEditable())
        elType = TBrCtlDefs::EElementActivatedInputBox;

    return elType;
}


WebFrame* frameAndPointUnderCursor(IntPoint& p_, WebView& v_)
{
    WebFrameView* mfv = v_.mainFrame()->frameView();
    p_ = StaticObjectsContainer::instance()->webCursor()->position();
    WebFrame* frame = v_.mainFrame()->frameAtPoint(p_);
    if (!frame)
        frame = v_.mainFrame();
    IntPoint rc = frame->frameView()->rectInGlobalCoords().iTl;
    p_ = frame->frameView()->viewCoordsInFrameCoords(p_);
    return frame;
}

String getNodeUrlAtPointInFrame(WebFrame& f_, IntPoint& p_)
{
    Document* doc = core(&f_)->document();
    if (doc) {
        Node* node = doc->elementFromPoint(p_.x(), p_.y());

        if (node) {
            if (node->hasTagName(areaTag)) {
                HTMLElement* e =  static_cast<HTMLElement*>(node);
                if(!e->getAttribute(hrefAttr).isNull()) {
                    return e->getAttribute(hrefAttr);
                }
            }
            // get the navigable node at this point
            Node* n = node;
            for (; n; n = n->parentNode())
                if (n->isFocusable() && n->isElementNode())
                    break;

            if (n && n->isFocusable() && n->isElementNode()) {
                HTMLElement* e =  static_cast<HTMLElement*>(n);
                if (!e->getAttribute(hrefAttr).isNull()) {
                    return e->getAttribute(hrefAttr);
                }
            }
        }
    }
    return String();
}


int imageCountInFrame(WebFrame& wf_, bool visibleOnly_)
{
    int count = 0;
    Frame* f = core(&wf_);

    if (!f || !f->document() || !f->document()->renderer())
        return count;

    Document* doc = f->document();
    RefPtr<WebCore::HTMLCollection> collection = doc->images();

    FrameView* v = doc->view();
    IntRect r1 = wf_.frameView()->topView()->mainFrame()->frameView()->rect();

    for (Node *n = collection->firstItem(); n; n = collection->nextItem()) {

        RenderImage* render = static_cast<RenderImage*>(n->renderer());
        if (render) {

            Image* img = render->image();
            if (!img->isNull() && img->data()) {
                //if visible only is true, check for intersection
                bool processing = true;
                if (visibleOnly_) {
                    IntRect imageRect;
                    IntPoint tl = wf_.frameView()->frameCoordsInViewCoords(n->getRect().topLeft());
                    IntPoint br = wf_.frameView()->frameCoordsInViewCoords(n->getRect().bottomRight());
                    imageRect.setLocation(tl);
                    imageRect.setSize(br-tl);

                    processing = r1.intersects(imageRect);
                }
                if (processing) {
                    String alttext = static_cast<HTMLImageElement*>(render->element())->altText();
                    // Check if alttext is "Nokia" or "No_save".
                    // Then do not count
                    if (!(equalIgnoringCase(alttext, "nokia") || equalIgnoringCase(alttext, "no_save")))
                        ++count;
                }
            }
        }

    }
    return count;
}

CArrayFixFlat<TBrCtlImageCarrier>* imagesInFrame(WebFrame& wf_, bool visibleOnly_)
{
    CArrayFixFlat<TBrCtlImageCarrier>* imglist = new CArrayFixFlat<TBrCtlImageCarrier>(10);
    Frame* f = core(&wf_);

    if (!f || !f->document() || !f->document()->renderer())
        return imglist;

    Document* doc = f->document();
    FrameView* v = doc->view();
    IntRect r1 = wf_.frameView()->topView()->mainFrame()->frameView()->rect();


    RefPtr<WebCore::HTMLCollection> collection = doc->images();
    for (Node* n = collection->firstItem(); n; n = collection->nextItem()) {

        RenderImage* render = static_cast<RenderImage*>(n->renderer());
        if (render) {
            Image* img = render->image();
            if (!img->isNull() && img->data()) {
                //if visible only is true, check for intersection
                TBool processing = ETrue;
                if (visibleOnly_) {
                    IntRect imageRect;
                    IntPoint tl = wf_.frameView()->frameCoordsInViewCoords(n->getRect().topLeft());
                    IntPoint br = wf_.frameView()->frameCoordsInViewCoords(n->getRect().bottomRight());
                    imageRect.setLocation(tl);
                    imageRect.setSize(br-tl);

                    processing = r1.intersects(imageRect);
                }
                if (processing) {
                    String alttext = static_cast<HTMLImageElement*>(render->element())->altText();
                    TBrCtlImageType type = EImageTypeAny;
                    if (img->getMimeType().find("image/vnd.wap.wbmp") == 0)
                        type = EImageTypeWbmp;
                    else if (img->getMimeType().find("image/vnd.nokia.ota-bitmap") == 0)
                        type = EImageTypeOta;
                    //
                    TBrCtlImageCarrier tImg(TPtrC8((const TUint8*)img->data()->data(),img->data()->size()),
                        static_cast<HTMLImageElement*>(render->element())->getAttribute(srcAttr),
                        alttext, type, img->getMimeType());

                    // Check if alttext is "Nokia" or "No_save".
                    // Then do not add the image to the list
                    if (!(equalIgnoringCase(alttext, "nokia") || equalIgnoringCase(alttext, "no_save"))) {
                        // Check if this image has been retrieved before.
                        // Image pointer should be the same for similar images
                        TBool imgexists = false;
                        for (int i=0; i<imglist->Count();i++) {
                              TBrCtlImageCarrier wcImage = imglist->At(i);
                              if (tImg.RawData() == wcImage.RawData()) {
                                      imgexists = true;
                                      break;
                              }

                        }
                        if (!imgexists)
                            TRAP_IGNORE( imglist->AppendL(tImg) );
                    }
                }
            }
        }
    }
    return imglist;
}

CArrayFixFlat<TBrCtlSubscribeTo>* findSubscribeToInFrame(WebFrame& wf_)
{
    CArrayFixFlat<TBrCtlSubscribeTo>* linkList = new CArrayFixFlat<TBrCtlSubscribeTo>(10);
    Frame* f = core(&wf_);

    if (!f || !f->document())
        return linkList;

    for (Node *n = f->document(); n; n = n->traverseNextNode()) {
        if (n->isElementNode() && n->hasTagName(linkTag)) {

            HTMLElement* e =  static_cast<HTMLElement*>(n);
            String type = e->getAttribute(typeAttr);

            if (type == "application/rss+xml" || type == "application/atom+xml" ||
                type == "text/xml" || type == "application/xml") {
                String href = e->getAttribute(hrefAttr);
                String title = e->getAttribute(titleAttr);

                TBrCtlSubscribeTo item(title.des(), href.des(), 0);
                TRAP_IGNORE( linkList->AppendL(item) );
            }
        }
    }
    return linkList;
}

int focusedImage(WebView* webView, TBrCtlImageCarrier*& imageCarrier)
{
    int ret = KErrNone;
    TBrCtlImageCarrier* t = NULL;
    RenderImage *r = renderImageUnderCursor(webView);
    if (r) {
        Image* img = r->image();
        CachedImage* ci = r->cachedImage();
        if( ci && !img->isNull()) {
            SharedBuffer* imgData = img->data();
            if (imgData) {
                TPtrC8 rawData((const unsigned char*)(imgData->data()), (imgData->size()));
                // Check if there is data in the image buffer.
                // Broken links do not have data in them
                if (rawData.Length() > 0) {
                    // Get the alt text
                    String alttext = static_cast<HTMLImageElement*>(r->element())->altText();
                    // Check if alttext is "Nokia" or "No_save".
                    // Then do not add the image to the list
                    if (!(equalIgnoringCase(alttext, "nokia") || equalIgnoringCase(alttext, "no_save"))) {
                        TBrCtlImageType brCtlTtype = EImageTypeAny;
                        if( img->getMimeType().des().Find(KMimeWBMP) != KErrNotFound ) {
                            brCtlTtype = EImageTypeWbmp;
                        }
                        else if ( img->getMimeType().des().Find(KMimeOTA) != KErrNotFound ) {
                            brCtlTtype = EImageTypeOta;
                        }
                        t = new TBrCtlImageCarrier(TPtrC8((const TUint8*)img->data()->data(),img->data()->size()),
                            static_cast<HTMLImageElement*>(r->element())->getAttribute(srcAttr),
                            alttext, brCtlTtype, img->getMimeType());
                        if (!t) {
                            ret = KErrNoMemory;
                        }
                        else {
                            ret = KErrNone;
                        }
                    }
                    else {
                        ret = KErrAccessDenied;
                    }
                } // if (rawData.Length() > 0)
                else {
                    ret = KErrNotFound;
                }
            }
            else {
                ret = KErrNotFound;
            }
        } // if (ci)
        else {
            ret = KErrNotFound;
        }
    } // if (r)
    else {
        ret = KErrNoMemory;
    }
    imageCarrier = t;
    return ret;
}

void loadFocusedImage(WebView* webView)
{
    RenderImage *r = renderImageUnderCursor(webView);
    if (r) {
        CachedImage* ci = r->cachedImage();
        if (ci && ci->status() == CachedResource::Unknown) {
            IntPoint p;
            WebFrame* frame = frameAndPointUnderCursor(p, *webView);
            cache()->loader()->load(core(frame)->document()->docLoader(), ci, true);
        }
    }
}

RenderImage* renderImageUnderCursor(WebView* webView)
{
    IntPoint p;
    WebFrame* frame = frameAndPointUnderCursor(p, *webView);
    Frame* f = core(frame);
    RenderImage* render = NULL;
    if (!f || !f->document() || !f->document()->renderer())
        return NULL;
    Document* doc = f->document();
    FrameView* v = doc->view();
    RefPtr<WebCore::HTMLCollection> collection = doc->images();
    for (Node* n = collection->firstItem(); n; n = collection->nextItem()) {
        render = static_cast<RenderImage*>(n->renderer());
        if (render) {
            Image* img = render->image();
            if (!img->isNull()) {
                //if visible only is true, check for intersection
                if (n->getRect().contains(p)) {
                    return render;
                }
            }
        }
    }
    return NULL;
}

HBufC* generateFrameName()
{
    HBufC* name = NULL;
    name = HBufC::New(60);
    if (name) {
        TPtr path = name->Des();
        path.Copy(KPathBegin);
        TBuf<20>timeBuf;
        TTime ttime;
        ttime.HomeTime();
        timeBuf.Num(ttime.Int64());
        path.Append(timeBuf);
        path.Append(KPathEnd);
    }
    return name;
}


void addOneMenuItemAfter(CEikMenuPane& menuPane, unsigned int after, int command, int resourceId, unsigned int commandBase)
{
    CEikMenuPaneItem::SData item;
    HBufC* buf = NULL;
    TRAP_IGNORE( buf = StringLoader::LoadL(resourceId));
    if(buf == NULL)
        {
        return;
        }
    item.iText.Copy(*buf);
    delete buf;
    buf = NULL;
    item.iCommandId = command + commandBase;
    item.iFlags = 0;
    item.iCascadeId = 0;
    TRAP_IGNORE( menuPane.AddMenuItemL(item, after) );
}

void insertOneMenuItem(CEikMenuPane& menuPane, int command, int resourceId, unsigned int commandBase)
{
    CEikMenuPaneItem::SData item;
    HBufC* buf = NULL;
    TRAP_IGNORE( buf = StringLoader::LoadL(resourceId));
    if(buf == NULL)
        {
        return;
        }
    item.iText.Copy(*buf);
    delete buf;
    buf = NULL;
    item.iCommandId = command + commandBase;
    item.iFlags = 0;
    item.iCascadeId = 0;
    TRAP_IGNORE( menuPane.InsertMenuItemL(item, 0) );
}

int textMultiplier(int fontLevel, int originalSize)
{
    int sizeMultiplier = originalSize;
    switch(fontLevel)
        {
        case TBrCtlDefs::EFontSizeLevelAllSmall:
            // show text 40% smaller than the declared size
            sizeMultiplier *= 0.6;
            break;
        case TBrCtlDefs::EFontSizeLevelSmaller:
            // show text 20% smaller than the declared size
            sizeMultiplier *= 0.8;
            break;
        default:
        case TBrCtlDefs::EFontSizeLevelNormal:
            break;
        case TBrCtlDefs::EFontSizeLevelLarger:
            // show text 25% larger than the declared size
            sizeMultiplier *= 1.25;
            break;
        case TBrCtlDefs::EFontSizeLevelAllLarge:
            // show text 50% larger than the declared size
            sizeMultiplier *= 1.50;
            break;
        }
    return sizeMultiplier;
}

static void handleSpecialSchemeL(String url, CBrCtl* brctl)
{
    // only the url is supported currently
    RArray<TUint> typeArray;
    CDesCArrayFlat* strArray = new CDesCArrayFlat( 1 );
    if (strArray) {
        // add url id
        typeArray.Append( EParamRequestUrl );
        // add url string
        TRAPD(err, strArray->AppendL( url.des() ));
        if (err == KErrNone) {
            TRAP_IGNORE(brctl->brCtlSpecialLoadObserver()->HandleRequestL( &typeArray, strArray ));
        }
        delete strArray;
    }
}

void addFocusedUrlToContacts(WebView* webView)
{
    if (webView->brCtl()->brCtlSpecialLoadObserver()) {
        IntPoint point;
        WebFrame* frame = frameAndPointUnderCursor(point, *webView);
        if (frame) {
            Document* doc = core(frame)->document();
            if (doc) {
                Node* node = doc->elementFromPoint(point.x(), point.y());
                if (node) {
                    // get the navigable node at this point
                    Node* n = node;
                    for (; n; n = n->parentNode())
                        if (n->isFocusable() && n->isElementNode())
                            break;

                    if (n && n->isFocusable() && n->isElementNode()) {
                        HTMLElement* e =  static_cast<HTMLElement*>(n);
                        String tel;
                        String email;
                        String telbook;
                        // href
                        String attr = e->getAttribute(hrefAttr);
                        if (!attr.isNull() && !attr.isEmpty()) {
                            if ( attr.startsWith( KTel ) )
                                tel = attr.substring(4);
                            else if( attr.startsWith(KWtaiMC) )
                                tel = attr.substring(13);
                            else if( attr.startsWith(KWtaiAP) )
                                tel = attr.substring(13);
                            else if( attr.startsWith(KWtaiSD) )
                                tel = attr.substring(13);
                            else if ( attr.startsWith( KMailto ) ) {
                                email = attr.substring(7);
                                // Find just the email address
                                int i = email.find('?');
                                if (i > 0 ) {
                                    email = email.left(i).stripWhiteSpace();
                                }
                            }
                        }
                        // cti
                        attr = e->getAttribute(ctiAttr);
                        if (!attr.isNull() && !attr.isEmpty() )
                            tel = attr;
                        // email
                        attr = e->getAttribute(emailAttr);
                        if (!attr.isNull() && !attr.isEmpty() )
                            email = attr;
                        // telbook
                        attr = e->getAttribute(telbookAttr);
                        if (!attr.isNull() && !attr.isEmpty() )
                            telbook = attr;
                        // Save to phone book
                        String url;
                        String makeCallUrlPrefix("wtai://wp/ap;");
                        char numberNameSeparator = ';';
                        int len = makeCallUrlPrefix.length() + 3; // NULL terminator and ';' separators
                        len += tel.length() + telbook.length() + email.length();
                        //url.reserve(len);
                        url = makeCallUrlPrefix;
                        url.append(tel);
                        url.append(numberNameSeparator);
                        url.append(telbook);
                        url.append(numberNameSeparator);
                        url.append(email);
                        TRAP_IGNORE( handleSpecialSchemeL(url, webView->brCtl()) );
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Map the Symbian errors to rainbow errors.
// -----------------------------------------------------------------------------
//
int mapHttpErrors(int err )
{
    if (err == KErrSSLAlertUserCanceled){
        return KBrowserCancelled;
    }
    // All SSL errors are mapped into this one, which gets displayed to the user
    if (err <= SSL_ERROR_BASE && err > SSL_ERROR_BASE - 200 ||
        err == KErrHttpCannotEstablishTunnel) {
        return KErrSSLAlertHandshakeFailure;
    }

    // Deal with DNS lookup errors
    if ((err <= KErrInet6NoDestination) && (err > (KErrInet6NoDestination - 200))) {
        return KBrowserHTTP502;
    }

    // Deal with HTTP errors
    if (err <= KHttpErrorBase && err > KHttpErrorBase - 200) {
        // Encode errors
        if (err <= KErrHttpEncodeDefault && err >= KErrHttpEncodeCookie2) {
            return KBrowserFailure;
        }
        // Auth errors
        if (err == KErrHttpDecodeWWWAuthenticate ||
            err == KErrHttpDecodeUnknownAuthScheme ||
            err == KErrHttpDecodeBasicAuth ||
            err == KErrHttpDecodeDigestAuth) {
            return KBrowserMissingAuthHeader;
        }

        // Decode errors
        if (err <= KErrHttpDecodeMalformedDate && err >= KErrHttpDecodeCookie) {
            return KBrowserBadContent;
        }
        switch (err)
        {
            case KErrHttpRedirectExceededLimit:
                {
                    return KBrowserTooManyRedirects;
                }
            case KErrHttpRedirectNoLocationField:
            case KErrHttpRedirectUseProxy:
                {
                    return KBrowserBadRedirect;
                }
            case KErrHttpInvalidUri:
                {
                    return KBrowserMalformedUrl;
                }
            default:
                {
                    return KBrowserFailure;
                }
        }
    }
    if (err <= KBrowserHTTP100 && err > KBrowserFailure)
        return err;
    switch (err)
    {
        case KErrHostUnreach:
            {
                return KBrowserTcpHostUnreachable;
            }
        case KErrAbort: //User has interrupted the loading process. We treat it as cancellation so that no error message shown.
        case KErrCancel:
            {
                return KBrowserCancelled;
            }
        case KErrBadName:
        case KErrPathNotFound:
            {
                return KBrowserFileNotFound;
            }
        case KErrCommsLineFail:
        case KErrNotReady:
            {
                return KBrowserConnFailed;
            }
        case KErrDiskFull:
            {
                return KBrowserFileDiskFullError;
            }
        case KErrTimedOut:
            {
                return KBrowserWaitTimeout;
            }
        case KErrCouldNotConnect:
            {
                return KBrowserMalformedUrl;
            }
        case KErrDisconnected:
            {
                return KBrowserHTTP504;
            }
        case KErrNoMemory:
            {
                return KBrowserOutOfMemory;
            }
        default:
            {
                return KBrowserFailure;
            }
    } // end of switch
}


bool handleSelectElementScrolling(WebView* webView, int tb)
{
    bool ret = false;
    bool isScrollList = false;
    if (webView->focusedElementType() == TBrCtlDefs::EElementSelectMultiBox && tb != 0) {
        IntPoint point;
        WebFrame* frame = frameAndPointUnderCursor(point, *webView);
        Element* e = core(frame)->document()->elementFromPoint(point.x(), point.y());
        if (e && e->isControl()) {
            HTMLGenericFormElement* ie = static_cast<HTMLGenericFormElement*>( e );
        	if (ie->type() == "select-multiple") {
        		isScrollList = true;
        	}
        	else if (ie->type() == "select-one") {
        		HTMLSelectElement* theSecletedElement = static_cast<HTMLSelectElement*>( e );
        		//check if the size is > 1
        		if (theSecletedElement->size() > 1) {
        			isScrollList = true;
        		} //select-one
        	}

        	//scroll only for the list which is either multiple or single but with more than one lines
            if (isScrollList) {
                RenderListBox* render = static_cast<RenderListBox*>(e->renderer());
                
                    HTMLSelectElement* selectElement = static_cast<HTMLSelectElement*>( e );
                    IntRect r = e->getRect();
                    int curIndex = render->listIndexAtOffset(point.x() - r.x(), point.y() - r.y());
                    int topIndex = render->indexOffset();
                    int bottomIndex = topIndex + render->getSize() - 1;
                    TPoint curPointInSelectBox(point.x() - r.x(), point.y() - r.y());
                    IntRect itemRect = render->itemRect(0, 0, curIndex);
                    int centerOfRect = itemRect.y() + (itemRect.height() * 1) / 4;
                    if (tb == -1) {
                        if (curIndex == topIndex) {
                            if (render->scroll(ScrollUp, ScrollByLine)) {
                                ret = true;
                            }
                        }
                        else {
                            int step = ((itemRect.height() + (curPointInSelectBox.iY - centerOfRect)) * webView->scalingFactor()) / 100;
                            if (step <= 0) step = (itemRect.height() * webView->scalingFactor()) / 100;
                            TPoint tp(StaticObjectsContainer::instance()->webCursor()->position().iX, StaticObjectsContainer::instance()->webCursor()->position().iY - step);
                            StaticObjectsContainer::instance()->webCursor()->updatePositionAndElemType(tp);
                            ret = true;
                        }
                    }
                    else {
                        if (curIndex == bottomIndex) {
                            if (render->scroll(ScrollDown, ScrollByLine)) {
                                ret = true;
                            }
                        }
                        else {
                            int step = ((itemRect.height() + (centerOfRect - curPointInSelectBox.iY)) * webView->scalingFactor()) / 100;
                            if (step <= 0) step = (itemRect.height() * webView->scalingFactor()) / 100;
                            TPoint tp(StaticObjectsContainer::instance()->webCursor()->position().iX, StaticObjectsContainer::instance()->webCursor()->position().iY + step);
                            StaticObjectsContainer::instance()->webCursor()->updatePositionAndElemType(tp);
                            ret = true;
                        }
                    }
                
            }//isScrollList
        } //isControl
    } //focusedElementType()

    return ret;
}

