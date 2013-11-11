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
* Description:   WebFepTextEditor.cpp
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtetext_internal.h>
#include <txtclipboard.h>
#include <txtetext_internal.h>
#endif

#include "config.h"
#include "../../bidi.h"
#include "StaticObjectsContainer.h"
#include "AtomicString.h"
#include "PlatformFontCache.h"
#include "BrCtl.h"
#include "WebFepTextEditor.h"
#include "WebEditorClient.h"
#include "WebTextFormatMask.h"
#include "WebView.h"
#include "WebFrame.h"
#include "Page.h"
#include "FocusController.h"
#include "WebFrameView.h"

#include "Frame.h"
#include "Editor.h"
#include "HTMLNames.h"
#include "EventHandler.h"
#include "HTMLInputElement.h"
#include "HTMLTextAreaElement.h"
#include "PlatformKeyboardEvent.h"
#include "CSSPropertyNames.h"
#include "CSSValueKeywords.h"

#include <coemain.h>
#include <eikpanic.h>
#include <aknedsts.h>
#include <AknUtils.h>
#include "Text.h"

using namespace WebCore;

static const int kInfinite = -1;
_LIT(KBlankDesC," ");

GLDEF_C void Panic(TEikPanic aPanic)
    {
    _LIT(KPanicCat,"WEBKIT-FEP");
    User::Panic(KPanicCat,aPanic);
    }

// -----------------------------------------------------------------------------
// CWebFepTextEditor
//
//
// -----------------------------------------------------------------------------
CWebFepTextEditor::CWebFepTextEditor(WebView* aView)
    : m_webView(aView),
      m_textFormatMask(NULL),
      m_inlineEditText(NULL)
{
    // Set up the extended capabilities
    TRAP_IGNORE(
        m_ExtendedInputCapabilities = CAknExtendedInputCapabilities::NewL();
        );

    // Set alignment on m_ExtendedInputCapabilities, yes it checks if NULL
    // The EInputEditorAlignXXX flags are defined for 5.0+ platforms
#if defined(BRDO_BROWSER_50_FF)
    SetAlignment( CAknExtendedInputCapabilities::EInputEditorAlignBidi );
#endif
	EnableCcpu(ETrue);
}

// -----------------------------------------------------------------------------
// ~CWebFepTextEditor
//
//
// -----------------------------------------------------------------------------
CWebFepTextEditor::~CWebFepTextEditor()
    {
    delete m_state;
    delete m_inlineEditText;
    delete m_textFormatMask;
	delete m_ExtendedInputCapabilities;
    }

// -----------------------------------------------------------------------------
// CreateTextFormatMask
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CreateTextFormatMask()
{
    if (!m_textFormatMask) {
        Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
        if (frame &&
            frame->document() &&
            frame->document()->focusedNode()) {
            RenderStyle* s = frame->document()->focusedNode()->renderStyle();
            if (s &&
            	(!s->wapInputFormat().isEmpty() || s->wapInputRequired())){
                m_textFormatMask = new WebTextFormatMask(s->wapInputFormat(), s->wapInputRequired());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// UpdateEditingMode
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateEditingMode()
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame) {
        Node *node = frame->document()->focusedNode();
        if (frame && frame->document() && node) {
            if (node->hasTagName(HTMLNames::inputTag)
                    && (static_cast<HTMLInputElement*>(node)->inputType() == HTMLInputElement::PASSWORD)
                        && !static_cast<HTMLInputElement*>(node)->readOnly()) {
                // Set the state as if it was the CEikSecretEditor
                CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
                if (state) {
                    state->SetFlags( EAknEditorFlagNoLRNavigation |
                                        EAknEditorFlagLatinInputModesOnly |
                                        EAknEditorFlagNoT9 |
                                        EAknEditorFlagUseSCTNumericCharmap );
                    state->SetDefaultInputMode(EAknEditorSecretAlphaInputMode);
                    state->SetCurrentInputMode(EAknEditorSecretAlphaInputMode);
                    state->SetPermittedCases(EAknEditorLowerCase|EAknEditorUpperCase);
                    state->SetCurrentCase(EAknEditorLowerCase);
                    state->SetPermittedInputModes(EAknEditorSecretAlphaInputMode | EAknEditorNumericInputMode);
                    state->SetDefaultCase(EAknEditorLowerCase);
                    state->SetSpecialCharacterTableResourceId(R_AVKON_SPECIAL_CHARACTER_TABLE_DIALOG_LATIN_ONLY);
                    state->SetNumericKeymap(EAknEditorPlainNumberModeKeymap);
                }
            }
            else {
                    CreateTextFormatMask();
                    TUint currentCase ( EAknEditorLowerCase ) ;
                    TUint permittedCase ( EAknEditorAllCaseModes ) ;
                    TUint inputMode( EAknEditorNullInputMode );
                    TUint permittedInputModes( EAknEditorAllInputModes );
                    TUint flags( EAknEditorFlagDefault );
                    TUint numericKeyMap( EAknEditorStandardNumberModeKeymap );

                    if (GetStateFromFormatMask(currentCase, permittedCase, inputMode, permittedInputModes, flags, numericKeyMap)) {
                        UpdateFlagsState(flags);
                        UpdateInputModeState(inputMode, permittedInputModes, numericKeyMap);
                        UpdateCaseState(currentCase, permittedCase);
                    }
                    else {
                        CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
                        if (state) {
                            state->SetDefaultCase(EAknEditorLowerCase);
                            state->SetPermittedInputModes(EAknEditorAllInputModes);
                            state->SetPermittedCases(EAknEditorAllCaseModes);//allow everything
                        }
                    }
            }
        }
    }   // End of if (frame)
}

// -----------------------------------------------------------------------------
// CancelEditingMode
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CancelEditingMode()
    {
    delete m_textFormatMask;
    m_textFormatMask = NULL;

    delete m_inlineEditText;
    m_inlineEditText = NULL;

    UpdateInputModeState(EAknEditorNullInputMode, EAknEditorAllInputModes,EAknEditorStandardNumberModeKeymap);
    UpdateFlagsState(EAknEditorFlagDefault);
    UpdateCaseState(EAknEditorLowerCase, EAknEditorAllCaseModes);

    CancelFepInlineEdit();
    }

// -----------------------------------------------------------------------------
// ActivatePenInputRequest
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::ActivatePenInputRequest()
{
    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
    if ( state ) {
        TRAP_IGNORE( state->ReportAknEdStateEventL(MAknEdStateObserver::EAknActivatePenInputRequest ) );
    }
}

// -----------------------------------------------------------------------------
// DeactivatePenInputRequest
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::DeactivatePenInputRequest()
    {
    }

// -----------------------------------------------------------------------------
// InputCapabilities
//
//
// -----------------------------------------------------------------------------
TCoeInputCapabilities CWebFepTextEditor::InputCapabilities()
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame) {
        SelectionController* sc = frame->selectionController();
        if (sc && sc->isContentEditable()) {

            // Set up the input capabilities, based on the <input> box
            TUint caps = TCoeInputCapabilities::ENavigation;

            if ( sc->isInPasswordField() ) {
                caps |= TCoeInputCapabilities::ESecretText;
                UpdateFlagsState(EAknEditorFlagNoT9);
            }
            else {

                CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
                if ( state ) {
                    if ( state->PermittedInputModes() == EAknEditorNumericInputMode ) {
                        caps |= (TCoeInputCapabilities::EWesternNumericIntegerPositive |
                                 TCoeInputCapabilities::EWesternNumericIntegerNegative)|
                                 TCoeInputCapabilities::EAllText;
                    }
                    else if (  state->PermittedInputModes() == EAknEditorTextInputMode ) {
                        caps |= TCoeInputCapabilities::EWesternAlphabetic;
                    }
                    else if (  state->PermittedInputModes() == (EAknEditorTextInputMode | EAknEditorNumericInputMode) ) {
                        caps |= (TCoeInputCapabilities::EWesternNumericIntegerPositive |
                                 TCoeInputCapabilities::EWesternNumericIntegerNegative |
                                 TCoeInputCapabilities::EWesternAlphabetic);
                    }
                    else {
                        caps |=  TCoeInputCapabilities::EAllText;
                    }
                }
            }

            // Set up the extended input capabilities, based on dir="rtl".
            // EInputEditorAlignXXX flags are only valid for 5.0+ platform.
#if defined(BRDO_BROWSER_50_FF)
            if ( IsDirectionRTL() ) {
                SetAlignment(CAknExtendedInputCapabilities::EInputEditorAlignRight );
            }
            else {
                SetAlignment(CAknExtendedInputCapabilities::EInputEditorAlignBidi );
            }
#endif
            // Set up the ObjectProvider, so ExtendedInputCapablities can be called by FEP.
            // MObjectProvider callback method MopSupplyObject().
            TCoeInputCapabilities inputCaps( caps, this, NULL, TUid::Uid(0x100056de), this );
            inputCaps.SetObjectProvider( this );

            return inputCaps;
        }
    }

    // Empty input capabilities
    TCoeInputCapabilities emptyCaps(TCoeInputCapabilities::ENone);
    emptyCaps.SetObjectProvider( this );

    return emptyCaps;
}


////////////////////////////////////////////////////////////////////////////////
// from MCoeFepAwareTextEditor
//

// -----------------------------------------------------------------------------
// StartFepInlineEditL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::StartFepInlineEditL(
      const TDesC& aInitialInlineText,
      TInt /*aPositionOfInsertionPointInInlineText*/,
      TBool /*aCursorVisibility*/,
      const MFormCustomDraw*,
      MFepInlineTextFormatRetriever&,
      MFepPointerEventHandlerDuringInlineEdit& /*aPointerEventHandlerDuringInlineEdit*/)
{
    CCoeEnv::Static()->ForEachFepObserverCall(FepObserverHandleStartOfTransactionL);
    ClearInlineText();
    UpdateInlineText(aInitialInlineText);
}

// -----------------------------------------------------------------------------
// UpdateFepInlineTextL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateFepInlineTextL( const TDesC& aNewInlineText,
								TInt /*aPositionOfInsertionPointInInlineText*/ )
{
    ClearInlineText();
    UpdateInlineText(aNewInlineText);
}

// -----------------------------------------------------------------------------
// SetInlineEditingCursorVisibilityL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::SetInlineEditingCursorVisibilityL(TBool /*aCursorVisibility*/)
{
}

// -----------------------------------------------------------------------------
// CancelFepInlineEdit
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CancelFepInlineEdit()
{
}

// -----------------------------------------------------------------------------
// DocumentLengthForFep
//
//
// -----------------------------------------------------------------------------
TInt CWebFepTextEditor::DocumentLengthForFep() const
{
    TInt length = 0;
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame &&
        frame->document() &&
        frame->document()->focusedNode()) {

        if (frame->document()->focusedNode()->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* ie = static_cast<HTMLInputElement*>(frame->document()->focusedNode());
            length = ie->value().length();
        }
        else if (frame->document()->focusedNode()->hasTagName(HTMLNames::textareaTag)) {
            HTMLTextAreaElement* ie = static_cast<HTMLTextAreaElement*>(frame->document()->focusedNode());
            length = ie->value().length();
        }
    }

    return length;
}

// -----------------------------------------------------------------------------
// DocumentMaximumLengthForFep
//
//
// -----------------------------------------------------------------------------
TInt CWebFepTextEditor::DocumentMaximumLengthForFep() const
{
    TInt length = KMaxTInt;

    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame &&
        frame->document() &&
        frame->document()->focusedNode() &&
        frame->document()->focusedNode()->hasTagName(HTMLNames::inputTag) ) {
        length = static_cast<HTMLInputElement*>(frame->document()->focusedNode())->maxLength();
    }

	// Override the length if a text format mask is set
	if ( m_textFormatMask && m_textFormatMask->getMultitude() > 0 )
        length = m_textFormatMask->getMultitude();

    // TextArea node has no member function maxLength(), so return KMaxTInt
    return length;
}

// -----------------------------------------------------------------------------
// SetCursorSelectionForFepL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection)
{
    // The other part of the rather hackish way to check if we are at the end of the editing field
    // see WebEditorClient::handleKeypress
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    Node*  focusedNode = frame->document()->focusedNode();
    if ( frame && focusedNode) {
        TInt lowPos = aCursorSelection.LowerPos();
        TInt highPos = aCursorSelection.HigherPos();
        if (focusedNode->hasTagName(HTMLNames::textareaTag)) {
            HTMLTextAreaElement* textArea = static_cast<HTMLTextAreaElement*>(focusedNode);
            textArea->setSelectionRange(lowPos, highPos);
		}
		else if (focusedNode->hasTagName(HTMLNames::inputTag)) {
		    HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(focusedNode);
		    inputElement->setSelectionStart(lowPos);
		    inputElement->setSelectionEnd(highPos);
		}
		HandleUpdateCursor();
    }
}

// -----------------------------------------------------------------------------
// GetCursorSelectionForFep
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const
{
    aCursorSelection.SetSelection(0,0);

    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    Node*  focusedNode = frame->document()->focusedNode();
    if (frame && focusedNode) {
        if (focusedNode->hasTagName(HTMLNames::textareaTag)) {
            HTMLTextAreaElement* textArea = static_cast<HTMLTextAreaElement*>(focusedNode);
            TInt anchorPos = textArea->selectionStart();
            TInt cursorPos = textArea->selectionEnd();
            aCursorSelection.SetSelection(cursorPos, anchorPos);
        }
        else if (focusedNode->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* inputElement = static_cast<HTMLInputElement*>(focusedNode);
            TInt anchorPos = inputElement->selectionStart();
            TInt cursorPos = inputElement->selectionEnd();
            aCursorSelection.SetSelection(cursorPos, anchorPos);
        }
    }
}

// -----------------------------------------------------------------------------
// GetEditorContentForFep
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::GetEditorContentForFep( TDes& aEditorContent,
												TInt aDocumentPosition,
												TInt aLengthToRetrieve ) const
{
    aEditorContent = KNullDesC;   	   // KNullDesC has length 0

    if (aLengthToRetrieve == 0) {
        return;
    }

    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame &&
        frame->document() &&
        frame->document()->focusedNode()) {

        if (frame->document()->focusedNode()->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* ie = static_cast<HTMLInputElement*>(frame->document()->focusedNode());
            aEditorContent = ie->value().substring(aDocumentPosition, aLengthToRetrieve);
        }
        else if (frame->document()->focusedNode()->hasTagName(HTMLNames::textareaTag)) {
            HTMLTextAreaElement* ie = static_cast<HTMLTextAreaElement*>(frame->document()->focusedNode());
            // Convert the newline to paragraph separator, because the FEP
            // input editors (vkb, etc) ignore newline
            String str(ie->value().substring(aDocumentPosition, aLengthToRetrieve));
            str.replace(EKeyLineFeed, CEditableText::EParagraphDelimiter);
            aEditorContent = str;
        }
    }
}

// -----------------------------------------------------------------------------
// GetFormatForFep
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::GetFormatForFep(TCharFormat& /*aFormat*/,TInt /*aDocumentPosition*/) const
{
}

// -----------------------------------------------------------------------------
// GetScreenCoordinatesForFepL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine,
                                                    TInt& aHeight,
                                                    TInt& aAscent,
                                                    TInt aDocumentPosition) const
{
Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
if (frame &&
    frame->document() &&
    frame->document()->focusedNode()){
    if ( frame->document()->focusedNode()->hasTagName(HTMLNames::inputTag) ||
		 frame->document()->focusedNode()->hasTagName(HTMLNames::textareaTag)){
		HTMLGenericFormElement*  ie = static_cast<HTMLGenericFormElement*>(frame->document()->focusedNode());
		SelectionController* sc = frame->selectionController();
		int xPos(0);
		int yPos(0);
		if ( sc ){
			IntRect rect = sc->caretRect();
			Node* editNode = sc->focusNode();
			TPoint viewPoint = kit(frame)->frameView()->frameCoordsInViewCoords(editNode->getRect().Rect().iBr);
			xPos = viewPoint.iX;
			yPos = viewPoint.iY;
			String str;
			if ( editNode &&
				 editNode->isTextNode() ) {
				WebCore::Text* aText = (WebCore::Text*)editNode;
				str = aText->data();
				aDocumentPosition =  aText->length();
				TInt position = aDocumentPosition - ( str.reverseFind(KBlankDesC(), aDocumentPosition )+1);
				String word(str);
				if( position > 0 ){
					word = str.left( position );
					}
				RenderStyle* s = frame->document()->focusedNode()->renderStyle();
				PlatformFontCache* cache = StaticObjectsContainer::instance()->fontCache();
				CFont* sFont =  cache->zoomedFont( s->fontDescription(), cache->fontZoomFactor());
				TInt sizePix =	sFont->MeasureText( word.des() );
				xPos -= sizePix;
				}
			}
		aLeftSideOfBaseLine.SetXY( xPos,yPos );
		}
	}
	aAscent = 0;
	aHeight = 0;
}

// -----------------------------------------------------------------------------
// DoCommitFepInlineEditL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::DoCommitFepInlineEditL()
{
    ClearInlineText();

    if (m_inlineEditText && DocumentLengthForFep() < DocumentMaximumLengthForFep()) {
        Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
        if (frame){
            if(IsWapMaskedModeInput(frame)) {
                HandleMaskedInsertText(frame, (String(*m_inlineEditText)));
            }
            else {
                frame->editor()->insertTextWithoutSendingTextEvent(String(*m_inlineEditText), false);
            }
        }
    }
    //delete the m_inlineEditText since text is commited
    delete m_inlineEditText;
    m_inlineEditText = NULL;

    HandleUpdateCursor();
    UpdateEditingMode();
}

// -----------------------------------------------------------------------------
// Extension1
//
//
// -----------------------------------------------------------------------------
MCoeFepAwareTextEditor_Extension1* CWebFepTextEditor::Extension1(TBool& aSetToTrue)
{
    aSetToTrue=ETrue;
    return STATIC_CAST(MCoeFepAwareTextEditor_Extension1*, this);
}

// -----------------------------------------------------------------------------
// MCoeFepAwareTextEditor_Reserved_2
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::MCoeFepAwareTextEditor_Reserved_2()
{
}

////////////////////////////////////////////////////////////////////////////////
// from MCoeFepAwareTextEditor_Extension1

// -----------------------------------------------------------------------------
// SetStateTransferingOwnershipL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::SetStateTransferingOwnershipL(CState* aState, TUid /*aTypeSafetyUid*/)
{
    delete m_state;
    m_state = aState;
}

// -----------------------------------------------------------------------------
// State
//
//
// -----------------------------------------------------------------------------
MCoeFepAwareTextEditor_Extension1::CState* CWebFepTextEditor::State(TUid /*aTypeSafetyUid*/)
{
    if (!m_state) {
        CAknEdwinState* state = new CAknEdwinState();
        state->SetObjectProvider(m_webView);
        m_state = state;
        }
    return m_state;
}

// -----------------------------------------------------------------------------
// StartFepInlineEditL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::StartFepInlineEditL(
        TBool& aSetToTrue,
        const TCursorSelection& aCursorSelection,
        const TDesC& aInitialInlineText,
        TInt aPositionOfInsertionPointInInlineText,
        TBool aCursorVisibility,
        const MFormCustomDraw* aCustomDraw,
        MFepInlineTextFormatRetriever& aInlineTextFormatRetriever,
        MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit)
{
    aSetToTrue=ETrue;
    SetCursorSelectionForFepL(aCursorSelection);
    StartFepInlineEditL(aInitialInlineText, aPositionOfInsertionPointInInlineText, aCursorVisibility, aCustomDraw, aInlineTextFormatRetriever, aPointerEventHandlerDuringInlineEdit);
}

// -----------------------------------------------------------------------------
// SetCursorType
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::SetCursorType(TBool& /*aSetToTrue*/, const TTextCursor& /*aTextCursor*/)
{
}

////////////////////////////////////////////////////////////////////////////////
// from TCoeInputCapabilities::MCoeFepSpecificExtensions

// -----------------------------------------------------------------------------
// IsValidCharacter
//
//
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::IsValidCharacter(TInt aChar)
{
     return ETrue;
}

////////////////////////////////////////////////////////////////////////////////
// from MObjectProvider

// -----------------------------------------------------------------------------
// MopSupplyObject
// Fep Manager will call us to get pointers to our bits. We currently support
// returning our ExtendedInputCapabilities.
// -----------------------------------------------------------------------------
TTypeUid::Ptr CWebFepTextEditor::MopSupplyObject(TTypeUid aId)
{
    if ( aId.iUid == CAknExtendedInputCapabilities::ETypeId )
    {
        return aId.MakePtr( m_ExtendedInputCapabilities );
    }

    return aId.Null();
}


////////////////////////////////////////////////////////////////////////////////
// New public methods
//

// -----------------------------------------------------------------------------
// UpdateFlagsState
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateFlagsState(TUint flags)
{
    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));

    if ( IsTextAreaFocused() ) {
    	// If in a TextArea, allow "enter" key presses to be newline/paragraph
    	state->SetFlags( flags | EAknEditorFlagUseSCTNumericCharmap
    					 | EAknEditorFlagAllowEntersWithScrollDown );
    	}
    else {
        state->SetFlags(flags | EAknEditorFlagUseSCTNumericCharmap);
    	}

    state->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateFlagsUpdate);
}

// -----------------------------------------------------------------------------
// UpdateInputModeState
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateInputModeState(TUint inputMode, TUint permittedInputModes, TUint numericKeyMap)
{

    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));

    if (permittedInputModes != EAknEditorNumericInputMode) {
        EVariantFlag variant = AknLayoutUtils::Variant();
        if (variant == EApacVariant) {
              permittedInputModes |= EAknEditorTextInputMode |
              EAknEditorHalfWidthTextInputMode | EAknEditorFullWidthTextInputMode |
              EAknEditorKatakanaInputMode | EAknEditorFullWidthKatakanaInputMode |
              EAknEditorHiraganaKanjiInputMode | EAknEditorHiraganaInputMode;

        }
    }

    state->SetDefaultInputMode(inputMode);
    state->SetCurrentInputMode(inputMode);
    state->SetPermittedInputModes(permittedInputModes);
    state->SetNumericKeymap(static_cast<TAknEditorNumericKeymap>(numericKeyMap));
    state->ReportAknEdStateEventL(MAknEdStateObserver::EAknSyncEdwinState);
    state->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateInputModeUpdate);
}

// -----------------------------------------------------------------------------
// UpdateCaseState
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateCaseState(TUint currentCase, TUint permittedCase)
{
    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));

    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame){

        if (frame->editor()->canEditRichly()) {
            state->SetDefaultCase(EAknEditorTextCase);
        }
        else {
            state->SetDefaultCase(currentCase);
            state->SetCurrentCase(currentCase);
            state->SetPermittedCases(permittedCase);
        }
    }

    state->ReportAknEdStateEventL(MAknEdStateObserver::EAknEdwinStateCaseModeUpdate);
}

// -----------------------------------------------------------------------------
// HandleUpdateCursor
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::HandleUpdateCursor()
{
    // ReportAknEdStateEventL, for events see aknedstsobs.h
    // MAknEdStateObserver::EAknCursorPositionChanged

    CAknEdwinState* state = static_cast<CAknEdwinState*>(State(KNullUid));
    if ( state ) {
        TRAP_IGNORE( state->ReportAknEdStateEventL( MAknEdStateObserver::EAknCursorPositionChanged ) );
    }
}

// -----------------------------------------------------------------------------
// GetStateFromFormatMask
//
//
// -----------------------------------------------------------------------------
bool CWebFepTextEditor::GetStateFromFormatMask(TUint& currentCase,
                                                TUint& permittedCase,
                                                TUint& inputMode,
                                                TUint& permittedInputModes,
                                                TUint& flags,
                                                TUint& numericKeyMap)
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame && m_textFormatMask) {

        TInt cursorpos = DocumentLengthForFep();
        TInputFormatMaskType fm = m_textFormatMask->getInputFormatMaskType(frame, cursorpos);
        if (!cursorpos) {
            while(fm == EStatic) {
                fm = m_textFormatMask->getInputFormatMaskType(frame, ++cursorpos);
            }
        }
        setSCTAvailability(true);
        switch( fm ) {
            case ELeUpSymPuc:       //A any upper case letter or symbolic
                flags = EAknEditorFlagNoT9 | EAknEditorFlagFixedCase;
                currentCase = EAknEditorUpperCase;
                permittedCase = EAknEditorUpperCase;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes = EAknEditorTextInputMode;
            break;
            case ELeLoSymPuc:       //a any lower case letter or symbolic
                flags = EAknEditorFlagNoT9 | EAknEditorFlagFixedCase;
                currentCase = EAknEditorLowerCase;
                permittedCase= EAknEditorLowerCase;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes= EAknEditorTextInputMode;
            break;
            case ELeUpNumSymPuc:    //X any upper case, number or symbolic
                flags = EAknEditorFlagNoT9 | EAknEditorFlagFixedCase;
                currentCase = EAknEditorUpperCase;
                permittedCase= EAknEditorUpperCase;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes= EAknEditorTextInputMode;
            break;
            case ELeLoNumSymPuc:    //x any lower case, number or symbolic
                flags = EAknEditorFlagNoT9 | EAknEditorFlagFixedCase;
                currentCase = EAknEditorLowerCase;
                permittedCase= EAknEditorLowerCase;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes= EAknEditorTextInputMode | EAknEditorNumericInputMode;
            break;
            case EAnyLow:           //m any lower character can be changed to upper
                flags = EAknEditorFlagNoT9;
                currentCase = EAknEditorLowerCase;
                permittedCase= EAknEditorAllCaseModes;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes= EAknEditorAllInputModes;
            break;
            case EAnyUpper:         //M any upper character can be changed to lower
                flags = EAknEditorFlagNoT9;
                currentCase = EAknEditorUpperCase;
                permittedCase= EAknEditorAllCaseModes;
                inputMode = EAknEditorTextInputMode;
                permittedInputModes= EAknEditorAllInputModes;
            break;
            case ENumSymPuc:        //n any number or symbolic
                flags = EAknEditorFlagNoT9;
                currentCase = EAknEditorUpperCase;
                permittedCase = EAknEditorAllCaseModes;
                inputMode = EAknEditorNumericInputMode;
                permittedInputModes= EAknEditorAllInputModes;
            break;
            case ENumChar:          //N any number
                flags = EAknEditorFlagNoT9;
                currentCase = EAknEditorUpperCase;
                permittedCase = EAknEditorAllCaseModes;
                inputMode = EAknEditorNumericInputMode;
                permittedInputModes= EAknEditorNumericInputMode;
                numericKeyMap = EAknEditorPlainNumberModeKeymap;
                setSCTAvailability(false);
            break;
            case EStatic:
                return EFalse;
            break;
            case ENoFormat:
                return EFalse;
            break;
            default:
            return EFalse;
        }

        return ETrue;
    }

    return EFalse;
}

// -----------------------------------------------------------------------------
// validateTextFormat
//
//
// -----------------------------------------------------------------------------
bool CWebFepTextEditor::validateTextFormat()
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (!frame || !frame->document()->focusedNode())
        return true;

    RenderStyle* s = frame->document()->focusedNode()->renderStyle();
    if (!m_textFormatMask) {
        return true;
    }

    Node* n = frame->document()->focusedNode();
    if (n->hasTagName(HTMLNames::inputTag)) {
        HTMLInputElement* input = static_cast<HTMLInputElement*>(n);
        WebTextFormatMask::ErrorBlock eb;
        CSSStyleDeclaration* style = input->style();
        ExceptionCode ec = 0;

        String inputColor = style->getPropertyValue(CSS_PROP_COLOR);
        if ( inputColor.lower() != "red" )
        {
            m_inputTextColor = inputColor;
        }

        if (!m_textFormatMask->checkText(input->value(), eb)) {
            style->setProperty(CSS_PROP_COLOR, "red", false, ec);
            return false;
        }
        else
        {
            style->setProperty(CSS_PROP_COLOR, m_inputTextColor, false, ec);
            CancelEditingMode();
            return true;
        }
    }
    else if ( n->hasTagName(HTMLNames::textareaTag)) {
        CancelEditingMode();
    }

    return true;
}

// -----------------------------------------------------------------------------
// IsDirectionRTL
// Walk through the DOM and find if the focus node or a parent (table, html,
// body, etc) has dir attribute set to RTL.
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::IsDirectionRTL()
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if ( frame &&
         frame->document() &&
         frame->document()->focusedNode() ) {

        Node* node = frame->document()->focusedNode();
        RenderStyle* renderStyle = node->renderStyle();
        if ( renderStyle ) {
            TextDirection txtDir = renderStyle->direction();
            if ( txtDir == RTL ) {
                return ETrue;
            }
        }
    }

    return EFalse;
}

#if defined(BRDO_BROWSER_50_FF)
// -----------------------------------------------------------------------------
// SetAlignment
// The EInputEditorAlignXXX flags are only supported in 5.0+ platforms
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::SetAlignment(CAknExtendedInputCapabilities::TInputCapabilities aAlignment)
{
    if ( !m_ExtendedInputCapabilities ) {
        return;
    }

    // Clear the old alignment
    TUint capabilities = m_ExtendedInputCapabilities->Capabilities();
    capabilities &= ~( CAknExtendedInputCapabilities::KAknEditorAlignMask );

    // Set the new alignment
    capabilities |= aAlignment;
    m_ExtendedInputCapabilities->SetCapabilities( capabilities );
}
#endif

// -----------------------------------------------------------------------------
// UpdateInlineText
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::UpdateInlineText(const TDesC& aText)
{
    delete m_inlineEditText;
    m_inlineEditText = NULL;

    if (DocumentLengthForFep() >= DocumentMaximumLengthForFep())
        return;

    m_inlineEditText = aText.Alloc();

    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();

    if (!frame)
        return;


    RenderStyle* r = NULL;
    if ( Node *n  = frame->selectionController()->selection().start().node() ) {
        r = n->renderStyle();
    }

    bool textSecurity = r && r->textSecurity() != TSNONE;

    RefPtr<CSSMutableStyleDeclaration> oldStyle;
    if (textSecurity) {
        oldStyle = frame->typingStyle();
        RefPtr<CSSMutableStyleDeclaration> style = new CSSMutableStyleDeclaration;
        style->setProperty(CSS_PROP__WEBKIT_TEXT_SECURITY, CSS_VAL_NONE);
        frame->computeAndSetTypingStyle(style.get(), EditActionTyping);
    }

    if(!DocumentLengthForFep() && IsWapMaskedModeInput(frame)) {
        HandleMaskedInsertText(frame, String(*m_inlineEditText));
    }
    else {
        frame->editor()->insertTextWithoutSendingTextEvent(String(*m_inlineEditText), false);
    }

    if (textSecurity) {
        if (oldStyle)
            frame->setTypingStyle(oldStyle.get());
        else
            frame->clearTypingStyle();
    }
}

// -----------------------------------------------------------------------------
// ClearInlineText
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::ClearInlineText()
{
    TInt oldlen = m_inlineEditText ? m_inlineEditText->Length() : 0;

    TKeyEvent keyEvent = { EKeyBackspace, EKeyBackspace, 0, 0 };
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    if (frame) {
        while ( oldlen-- ) {
            frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                                                 CharacterGranularity, false, true);
        }
    }
}

// -----------------------------------------------------------------------------
// IsTextAreaFocused
//
//
// -----------------------------------------------------------------------------
bool CWebFepTextEditor::IsTextAreaFocused() const
{
    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
    return ( frame && frame->document()->focusedNode() &&
    		 frame->document()->focusedNode()->hasTagName(HTMLNames::textareaTag));
}

// -----------------------------------------------------------------------------
// CcpuIsFocused
//
//
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::CcpuIsFocused() const
{
    return ETrue;
}

// -----------------------------------------------------------------------------
// CcpuCanCut
//
//
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::CcpuCanCut() const
{
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
    return selection.Length();
}

// -----------------------------------------------------------------------------
// CcpuCutL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CcpuCutL()
{
    PlaceDataOnClipboardL();
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
}

// -----------------------------------------------------------------------------
// CcpuCanCopy
//
//
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::CcpuCanCopy() const
{
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
    return selection.Length();
}

// -----------------------------------------------------------------------------
// CcpuCopyL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CcpuCopyL()
{
    PlaceDataOnClipboardL();
}

// -----------------------------------------------------------------------------
// CcpuCanPaste
//
//
// -----------------------------------------------------------------------------
TBool CWebFepTextEditor::CcpuCanPaste() const
{
    TRAPD(err, DoCcpuCanPasteL());
    return err == KErrNone;
}

// -----------------------------------------------------------------------------
// CcpuPasteL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CcpuPasteL()
{
    RetrieveDataFromClipboardL();
}

// -----------------------------------------------------------------------------
// DoCcpuCanPasteL
//
//
// -----------------------------------------------------------------------------
void  CWebFepTextEditor::DoCcpuCanPasteL() const
{ 
    CClipboard* cb=CClipboard::NewForReadingL(CCoeEnv::Static()->FsSession());
    CleanupStack::PushL(cb);
    TStreamId streamId=cb->StreamDictionary().At(KClipboardUidTypePlainText);
    if (streamId==KNullStreamId)
        User::Leave(KErrNotFound);
    CleanupStack::PopAndDestroy(); // allowedChars, cb
}

// -----------------------------------------------------------------------------
// PlaceDataOnClipboardL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::PlaceDataOnClipboardL()
{
    CancelFepInlineEdit();
    CClipboard* cb=CClipboard::NewForWritingLC(CCoeEnv::Static()->FsSession());
    CopyToStoreL(cb->Store(),cb->StreamDictionary());
    cb->CommitL();
    CleanupStack::PopAndDestroy();
}

// -----------------------------------------------------------------------------
// CopyToStoreL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::CopyToStoreL(CStreamStore& aStore,CStreamDictionary& aDict)
{
	if ( DocumentLengthForFep()== 0)
		return ;
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
    TInt len = DocumentLengthForFep();

	HBufC* buf = HBufC::NewLC(len);
	TPtr ptr(buf->Des());

    GetEditorContentForFep(ptr,0,len);
    CPlainText* text = CPlainText::NewL(CPlainText::EFlatStorage);

	text->InsertL(0,*buf);
	text->CopyToStoreL(aStore, aDict, selection.LowerPos(), selection.Length());

	delete text;
    CleanupStack::PopAndDestroy();
}

// -----------------------------------------------------------------------------
// HandleMaskedInsertText
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::HandleMaskedInsertText(WebCore::Frame *frame, const String& text)
{
    TInt pos = DocumentLengthForFep();
    if (!pos) {
        while(m_textFormatMask->getInputFormatMaskType(frame, pos) == EStatic) {
            MaskStatic* ms = static_cast<MaskStatic*>(m_textFormatMask->getMask(pos));
            UChar mask(ms->getStatic());
            frame->editor()->insertTextWithoutSendingTextEvent(String(&mask,1), false);
            ++pos;
        }
        frame->editor()->insertTextWithoutSendingTextEvent(text, false);
    }
    else {
        frame->editor()->insertTextWithoutSendingTextEvent(text, false);
        while(m_textFormatMask->getInputFormatMaskType(frame, ++pos) == EStatic) {
            TCursorSelection selection;
            GetCursorSelectionForFep(selection);
            const TInt cursorPos=selection.LowerPos();
            if (cursorPos>=pos) {
                MaskStatic* ms = static_cast<MaskStatic*>(m_textFormatMask->getMask(pos));
                UChar mask(ms->getStatic());
                frame->editor()->insertTextWithoutSendingTextEvent(String(&mask,1), false);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// HandleMaskedDeleteText
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::HandleMaskedDeleteText(WebCore::Frame* frame)
{
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
    TInt cursorPos=selection.LowerPos();

    if (cursorPos == DocumentLengthForFep()) {
        while (m_textFormatMask->getInputFormatMaskType(frame, --cursorPos) == EStatic) {
            frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                    CharacterGranularity, false, true);
        }
    }
    if (cursorPos >=0)
        frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                CharacterGranularity, false, true);
}

// -----------------------------------------------------------------------------
// IsWapMaskedModeInput
//
//
// -----------------------------------------------------------------------------
bool CWebFepTextEditor::IsWapMaskedModeInput(WebCore::Frame* frame)
{
    bool maskedInput(false);
    if (m_textFormatMask && frame->document() && frame->document()->focusedNode()) {
        RenderStyle* s = frame->document()->focusedNode()->renderStyle();
        if (s && (!s->wapInputFormat().isEmpty() || s->wapInputRequired())){
            maskedInput = true;
        }
    }
    return maskedInput;
}

// -----------------------------------------------------------------------------
// RetrieveDataFromClipboardL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::RetrieveDataFromClipboardL()
{
    m_webView->fepTextEditor()->CancelFepInlineEdit();
    CClipboard* cb=NULL;
    TRAPD(err,cb=CClipboard::NewForReadingL(CCoeEnv::Static()->FsSession()));
    CleanupStack::PushL(cb);
    User::LeaveIfError(err);
    TStreamId streamId=cb->StreamDictionary().At(KClipboardUidTypePlainText);
    PasteFromStoreL(cb->Store(), cb->StreamDictionary());
    CleanupStack::PopAndDestroy();  // cb
}

// -----------------------------------------------------------------------------
// PasteFromStoreL
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::PasteFromStoreL(CStreamStore& aStore,CStreamDictionary& aDict)
{
    //Read the content into Text
    CancelFepInlineEdit();
    TStreamId streamId=aDict.At(KClipboardUidTypePlainText);
    TCursorSelection selection;
    GetCursorSelectionForFep(selection);
    const TInt cursorPos=selection.LowerPos();

    TInt len = DocumentLengthForFep();
	HBufC* buf1 = HBufC::NewLC(len);
	TPtr ptr1(buf1->Des());

	CPlainText* text = CPlainText::NewL(CPlainText::EFlatStorage);

    GetEditorContentForFep(ptr1,0,len);

    text->InsertL(0,*buf1);

    TInt charPasted = text->PasteFromStoreL(aStore,aDict,cursorPos);

    HBufC* buf = HBufC::NewLC(charPasted);
    TPtr ptr(buf->Des());
    text->Extract(ptr,cursorPos,charPasted);

    //remove Paragraph Delimiter
    TInt position = ptr.Mid(0).LocateReverse(TChar(CEditableText::EParagraphDelimiter));
    while (position != KErrNotFound ){
        ptr.Delete( position, 1 );
        position = ptr.Left(position).LocateReverse(TChar(CEditableText::EParagraphDelimiter));
        }

    Frame* frame = m_webView->page()->mainFrame();
	frame = m_webView->page()->focusController()->focusedOrMainFrame();
    frame->editor()->insertTextWithoutSendingTextEvent(String(ptr), false);

    delete text;
    CleanupStack::PopAndDestroy(2);
}

// -----------------------------------------------------------------------------
// EnableCcpu
//
//
// -----------------------------------------------------------------------------
void CWebFepTextEditor::EnableCcpu(TBool aSupport)
{
    CAknEdwinState* edwinState = static_cast<CAknEdwinState*>(this->State(KNullUid));
    if(aSupport)
        {
        edwinState->SetCcpuState(this);
        }
    else
        {
        edwinState->SetCcpuState(NULL);
        }
}

// -----------------------------------------------------------------------------
// findPrevSiblingTextLen
//
// Walk the previous text nodes and add up the len of each text node, so we can
// calculate the total length from first text node to current text node cursor
// position.
// NOTES:
// 1. Each "inserted" newline creates a new node.
// 2. Text area's with newline characters prepopulated (not "entered" by users,
//    but in the original html) do not create separate nodes.
// -----------------------------------------------------------------------------
void CWebFepTextEditor::findPrevSiblingTextLen(Node* aNode, TInt& aLen) const
{
	String str;
	while ( aNode ) {
		aNode = aNode->previousSibling();
		if ( aNode && aNode->isTextNode() ) {
			WebCore::Text* text = (WebCore::Text*)aNode;
			str = text->data();
			aLen +=  str.length();
		}
	}
}

// -----------------------------------------------------------------------------
// findTextNodeCurPos
//
// Returns the text node that contains the current text (cursor) position and
// returns the summed length (aPos) of all the text nodes before the current
// text node.
// NOTE: The length (aPos) doesn't include the current text node's length.
// -----------------------------------------------------------------------------
Node* CWebFepTextEditor::findTextNodeForCurPos(Node* aNode, TInt& aPos) const
{
	// Get the first text node
	Node* firstTextNode = NULL;
	Node* retNode = aNode;
	while ( aNode ) {
		firstTextNode = aNode;
		aNode = aNode->previousSibling();
	}

	TInt len( 0 );
	String str;
	aNode = firstTextNode;
	for ( ; aNode; aNode=aNode->nextSibling() ) {
		if ( aNode && aNode->isTextNode() ) {
			WebCore::Text* aText = (WebCore::Text*)aNode;
			str = aText->data();
			len +=  str.length();
			if ( len >= aPos ) {
				// We found the text node at aPos, calculate the length of all
				// previous text nodes
				retNode = aNode;
				aPos = len - str.length();
				break;
			}
		}
	}

	return retNode;
}

// -----------------------------------------------------------------------------
// SetSCTAvailability
//
// Set availibility of the special character table.
// -----------------------------------------------------------------------------
void CWebFepTextEditor::setSCTAvailability(bool aAvailable)
{
    if (m_ExtendedInputCapabilities) {
        TUint capabilities = m_ExtendedInputCapabilities->Capabilities();
        if (!aAvailable) {
            capabilities |= CAknExtendedInputCapabilities::EDisableSCT;
        }
        else {
            capabilities &= ~(CAknExtendedInputCapabilities::EDisableSCT);
        }
        m_ExtendedInputCapabilities->SetCapabilities(capabilities);
    }
}

