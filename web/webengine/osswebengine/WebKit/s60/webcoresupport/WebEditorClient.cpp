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
* Description:   Implementation of WebEditorClient
*
*/

#include "config.h"
#include "../../bidi.h"
#include "WebEditorClient.h"

#include "EditCommand.h"
#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "Document.h"
#include "KeyboardEvent.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "DebugStream.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFormFill.h"
#include "HTMLNames.h"
#include "WebFepTextEditor.h"
#include <EIKCOCTL.rsg>
#include <eikon.hrh>


using namespace WebCore;
using namespace HTMLNames;

//-----------------------------------------------------------------------------
// WebEditorClient::WebEditorClient
//-----------------------------------------------------------------------------
WebEditorClient::WebEditorClient(WebView* webview) : m_webView(webview)
{
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldDeleteRange
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldDeleteRange(Range*)
{
    return true;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldShowDeleteInterface
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldShowDeleteInterface(HTMLElement*)
{
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::isContinuousSpellCheckingEnabled
//-----------------------------------------------------------------------------
bool WebEditorClient::isContinuousSpellCheckingEnabled()
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::isGrammarCheckingEnabled
//-----------------------------------------------------------------------------
bool WebEditorClient::isGrammarCheckingEnabled()
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::spellCheckerDocumentTag
//-----------------------------------------------------------------------------
int WebEditorClient::spellCheckerDocumentTag()
{
    notImplemented();
    return 0;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldBeginEditing
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldBeginEditing(WebCore::Range*)
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldEndEditing
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldEndEditing(WebCore::Range*)
{
    return m_shouldEndEditing;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldInsertText
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldInsertText(String, Range*, EditorInsertAction)
{
    return true;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldChangeSelectedRange
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldChangeSelectedRange(Range* fromRange,
                                                Range* toRange,
                                                EAffinity,
                                                bool stillSelecting)
{
    return true;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldApplyStyle
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldApplyStyle(WebCore::CSSStyleDeclaration*,
                                       WebCore::Range*)
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldMoveRangeAfterDelete
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldMoveRangeAfterDelete(Range* range, Range* rangeToBeReplaced)
{
    /*
    return [[m_webView _editingDelegateForwarder] webView:m_webView
        shouldMoveRangeAfterDelete:kit(range) replacingRange:kit(rangeToBeReplaced)];
    */
    notImplemented();
    return true;
}

//-----------------------------------------------------------------------------
// WebEditorClient::didBeginEditing
//-----------------------------------------------------------------------------
void WebEditorClient::didBeginEditing()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::respondToChangedContents
//-----------------------------------------------------------------------------
void WebEditorClient::respondToChangedContents()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::respondToChangedSelection
//-----------------------------------------------------------------------------
void WebEditorClient::respondToChangedSelection()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::didEndEditing
//-----------------------------------------------------------------------------
void WebEditorClient::didEndEditing()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::didWriteSelectionToPasteboard
//-----------------------------------------------------------------------------
void WebEditorClient::didWriteSelectionToPasteboard()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::didSetSelectionTypesForPasteboard
//-----------------------------------------------------------------------------
void WebEditorClient::didSetSelectionTypesForPasteboard()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::selectWordBeforeMenuEvent
//-----------------------------------------------------------------------------
bool WebEditorClient::selectWordBeforeMenuEvent()
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::isEditable
//-----------------------------------------------------------------------------
bool WebEditorClient::isEditable()
{
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::registerCommandForUndo
//-----------------------------------------------------------------------------
void WebEditorClient::registerCommandForUndo(WTF::PassRefPtr<WebCore::EditCommand>)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::registerCommandForRedo
//-----------------------------------------------------------------------------
void WebEditorClient::registerCommandForRedo(WTF::PassRefPtr<WebCore::EditCommand>)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::clearUndoRedoOperations
//-----------------------------------------------------------------------------
void WebEditorClient::clearUndoRedoOperations()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::canUndo
//-----------------------------------------------------------------------------
bool WebEditorClient::canUndo() const
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::canRedo
//-----------------------------------------------------------------------------
bool WebEditorClient::canRedo() const
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::undo
//-----------------------------------------------------------------------------
void WebEditorClient::undo()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::redo
//-----------------------------------------------------------------------------
void WebEditorClient::redo()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::shouldInsertNode
//-----------------------------------------------------------------------------
bool WebEditorClient::shouldInsertNode(Node*, Range*, EditorInsertAction)
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::pageDestroyed
//-----------------------------------------------------------------------------
void WebEditorClient::pageDestroyed()
{
    delete this;
}

//-----------------------------------------------------------------------------
// WebEditorClient::smartInsertDeleteEnabled
//-----------------------------------------------------------------------------
bool WebEditorClient::smartInsertDeleteEnabled()
{
    return true;
}

//-----------------------------------------------------------------------------
// WebEditorClient::toggleContinuousSpellChecking
//-----------------------------------------------------------------------------
void WebEditorClient::toggleContinuousSpellChecking()
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::toggleGrammarChecking
//-----------------------------------------------------------------------------
void WebEditorClient::toggleGrammarChecking()
{
}

//-----------------------------------------------------------------------------
// WebEditorClient::handleKeypress
//-----------------------------------------------------------------------------
void WebEditorClient::handleKeypress(KeyboardEvent* event)
{
    if (!m_webView && !m_webView->page()) {
        return;
    }

    Frame* frame = m_webView->page()->mainFrame();
    if (!frame) {
        return;
    }

    m_shouldEndEditing = false;
    // Move to the right frame
    frame = m_webView->page()->focusController()->focusedOrMainFrame();

    const PlatformKeyboardEvent* kevent = event->keyEvent();
    if (!kevent->isKeyUp()) {
        const TInt modifiers=kevent->symbianEvent().iModifiers;
        TBool select=modifiers&EModifierShift;
        TBool magnify=modifiers&EModifierCtrl;
        // it's a rather hackish way to check if we are at the end of the editing field
        Position startPos = frame->selectionController()->start();
        Position endPos = frame->selectionController()->end();
        Node* startNode = startPos.node();
        if (startNode && startNode->isContentEditable()) {

            // The virtual ITU-T keyboard has a sync issue when we handle these
            // key events, so we call Fep()->HandleChangeInFocus() to get
            // the Fep in sync.
            int code = (kevent->keyIdentifier() == "Enter") ? EKeyEnter : kevent->symbianEvent().iCode;
            switch (code)
            {

            // AknFepManager calls SimulateKeyEventL(EKeyF20) to delete
            // previous char in secret text editor
            case EKeyF20:
            case EKeyBackspace:
                handleDeleteText(frame);
                m_webView->fepTextEditor()->HandleUpdateCursor();
                m_webView->fepTextEditor()->UpdateEditingMode();
                event->setDefaultHandled();
                break;

            case EKeyDelete:
                frame->editor()->deleteWithDirection(SelectionController::FORWARD,
                                                     CharacterGranularity, false, true);
                m_webView->fepTextEditor()->HandleUpdateCursor();
                m_webView->fepTextEditor()->UpdateEditingMode();
                event->setDefaultHandled();
                break;

            case EKeyLeftArrow:
                if (select) //If shift is pressed then highlight the selection
                {
                //Webview Passes EEventKeyDown and EEventKey due to which 2 characters are getting selected in one key press
                //Avoiding one Event so that for each key press it selects 1 character only
                 if(kevent->isKeyDown())
                    break;
                 frame->editor()->execCommand("MoveLeftAndModifySelection");//from createCommandMap()
                }
                else
                {
                 frame->editor()->execCommand("MoveLeft");
                }
                m_webView->fepTextEditor()->HandleUpdateCursor();
                if (frame->selectionController()->start() != startPos &&
                    frame->selectionController()->end() != endPos) {
                    event->setDefaultHandled();
                }
                else {
                    m_shouldEndEditing = !m_webView->fepTextEditor()->IsTextAreaFocused();
                }
                break;

            case EKeyRightArrow:
                if (select)//If shift is pressed then highlight the selection
                {
                //Webview Passes EEventKeyDown and EEventKey due to which 2 characters are getting selected in one key press
                //Avoiding one Event so that for each key press it selects 1 character only
                 if(kevent->isKeyDown())
                    break;
                 frame->editor()->execCommand("MoveRightAndModifySelection");
                }
                else
                {
                 frame->editor()->execCommand("MoveRight");
                }
                m_webView->fepTextEditor()->HandleUpdateCursor();
                if (frame->selectionController()->start() != startPos &&
                    frame->selectionController()->end() != endPos) {
                    event->setDefaultHandled();
                }
                else {
                    m_shouldEndEditing = !m_webView->fepTextEditor()->IsTextAreaFocused();
                }
                break;

            case EKeyUpArrow:
                frame->editor()->execCommand("MoveUp");
                m_webView->fepTextEditor()->HandleUpdateCursor();
                if (frame->selectionController()->start() != startPos &&
                    frame->selectionController()->end() != endPos) {
                    event->setDefaultHandled();
                }
                else {
                    m_shouldEndEditing = true;
                }
                break;

            case EKeyDownArrow:
                frame->editor()->execCommand("MoveDown");
                m_webView->fepTextEditor()->HandleUpdateCursor();
                if (frame->selectionController()->start() != startPos &&
                    frame->selectionController()->end() != endPos) {
                    event->setDefaultHandled();
                }
                else {
                    m_shouldEndEditing = true;
                }
                break;
                
            case EKeyEnter:
            case EKeyDevice3:    
            	// If we are in a textarea, add a newline
                if (m_webView->fepTextEditor()->IsTextAreaFocused()) {
                    if (m_webView->fepTextEditor()->DocumentLengthForFep() <
                        m_webView->fepTextEditor()->DocumentMaximumLengthForFep()) {
                        frame->editor()->insertLineBreak();
                        m_webView->fepTextEditor()->UpdateEditingMode();
                        m_webView->fepTextEditor()->HandleUpdateCursor();
                        event->setDefaultHandled();
                    }
                }
                break;

            case EKeyF18:
                if (magnify)
                {
                    switch (kevent->symbianEvent().iScanCode)
                        {
                        case EEikCmdEditCut:
                            m_webView->fepTextEditor()->CcpuCutL();
                            frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                                                             CharacterGranularity, false, true);
                            m_webView->fepTextEditor()->HandleUpdateCursor();
                            break;
                        case EEikCmdEditCopy:
                            m_webView->fepTextEditor()->CcpuCopyL();
                            break;
                        case EEikCmdEditPaste:
                            m_webView->fepTextEditor()->CcpuPasteL();
                            break;
                        default:
                            break;
                        }
                }
                break;

// All of the diagonal KeyEvents are allowed to flow through the "default" case...
//
//          case EKeyRightUpArrow:        // Northeast
//          case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
//          case EKeyRightDownArrow:      // Southeast
//          case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
//          case EKeyLeftDownArrow:       // Southwest
//          case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
//          case EKeyLeftUpArrow:         // Northwest
//          case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
            default:
                TBuf<24> buf;
                if (select)
                    CCoeEnv::Static()->ReadResource(buf,R_EIK_EDWIN_SHIFT_CTRL_HOTKEYS);
                else
                    CCoeEnv::Static()->ReadResource(buf,R_EIK_EDWIN_CTRL_HOTKEYS);
                const TInt ret=buf.Locate(TChar(kevent->symbianEvent().iCode+'a'-1));
                switch (ret)
                    {
                case EHotKeyCut:
                    m_webView->fepTextEditor()->CcpuCopyL();
                    frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
                                                     CharacterGranularity, false, true);
                    m_webView->fepTextEditor()->HandleUpdateCursor();
                    break;
                case EHotKeyCopy:
                    m_webView->fepTextEditor()->CcpuCopyL();
                    break;
                case EHotKeyPaste:
                    m_webView->fepTextEditor()->CcpuPasteL();
                    break;
                    }

                if (TChar(kevent->symbianEvent().iCode).IsPrint()) {
                    if (m_webView->fepTextEditor()->DocumentLengthForFep() <
                        m_webView->fepTextEditor()->DocumentMaximumLengthForFep()) {
                        TText c = kevent->symbianEvent().iCode;
                        handleInsertText(frame, String(TPtrC(&c,1)));
                        m_webView->fepTextEditor()->UpdateEditingMode();
                    }
                    m_webView->fepTextEditor()->HandleUpdateCursor();
                    event->setDefaultHandled();
                }
                break;

            }   // End of switch
        }
    }
}

//-----------------------------------------------------------------------------
// WebEditorClient::handleInputMethodKeypress
//-----------------------------------------------------------------------------
void WebEditorClient::handleInputMethodKeypress(KeyboardEvent*)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::textFieldDidBeginEditing
//-----------------------------------------------------------------------------
void WebEditorClient::textFieldDidBeginEditing(Element*)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::textFieldDidEndEditing
//-----------------------------------------------------------------------------
void WebEditorClient::textFieldDidEndEditing(Element*)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::textDidChangeInTextField
//-----------------------------------------------------------------------------
void WebEditorClient::textDidChangeInTextField(Element* inputElement)
{
    if ( m_webView->formFill() )
        {
        m_webView->formFill()->textChanged(inputElement);
        }
}

//-----------------------------------------------------------------------------
// WebEditorClient::doTextFieldCommandFromEvent
//-----------------------------------------------------------------------------
bool WebEditorClient::doTextFieldCommandFromEvent(Element*, KeyboardEvent*)
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::textWillBeDeletedInTextField
//-----------------------------------------------------------------------------
void WebEditorClient::textWillBeDeletedInTextField(Element*)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::textDidChangeInTextArea
//-----------------------------------------------------------------------------
void WebEditorClient::textDidChangeInTextArea(Element*)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::ignoreWordInSpellDocument
//-----------------------------------------------------------------------------
void WebEditorClient::ignoreWordInSpellDocument(const String&)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::learnWord
//-----------------------------------------------------------------------------
void WebEditorClient::learnWord(const String&)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::checkSpellingOfString
//-----------------------------------------------------------------------------
void WebEditorClient::checkSpellingOfString(const UChar*, int length, int* misspellingLocation, int* misspellingLength)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::checkGrammarOfString
//-----------------------------------------------------------------------------
void WebEditorClient::checkGrammarOfString(const UChar*, int length, Vector<GrammarDetail>&, int* badGrammarLocation, int* badGrammarLength)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::updateSpellingUIWithGrammarString
//-----------------------------------------------------------------------------
void WebEditorClient::updateSpellingUIWithGrammarString(const String&, const GrammarDetail& detail)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::updateSpellingUIWithMisspelledWord
//-----------------------------------------------------------------------------
void WebEditorClient::updateSpellingUIWithMisspelledWord(const String&)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::showSpellingUI
//-----------------------------------------------------------------------------
void WebEditorClient::showSpellingUI(bool show)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::spellingUIIsShowing
//-----------------------------------------------------------------------------
bool WebEditorClient::spellingUIIsShowing()
{
    notImplemented();
    return false;
}

//-----------------------------------------------------------------------------
// WebEditorClient::getGuessesForWord
//-----------------------------------------------------------------------------
void WebEditorClient::getGuessesForWord(const String&, Vector<String>& guesses)
{
    notImplemented();
}

//-----------------------------------------------------------------------------
// WebEditorClient::setInputMethodState
//-----------------------------------------------------------------------------
void WebEditorClient::setInputMethodState(bool enabled)
{
    m_webView->setEditable(enabled);
}

//-----------------------------------------------------------------------------
// WebEditorClient::handleInsertText
//-----------------------------------------------------------------------------
void WebEditorClient::handleInsertText(Frame* frame,  const String& text)
{
    if (!m_webView->fepTextEditor()->IsWapMaskedModeInput(frame)) {
        frame->editor()->insertTextWithoutSendingTextEvent(text, false);    
    }
    else {
        m_webView->fepTextEditor()->HandleMaskedInsertText(frame, text);
    }
}

//-----------------------------------------------------------------------------
// WebEditorClient::handleDeleteText
//-----------------------------------------------------------------------------
void WebEditorClient::handleDeleteText(Frame* frame)
{
    if (!m_webView->fepTextEditor()->IsWapMaskedModeInput(frame)) {
        frame->editor()->deleteWithDirection(SelectionController::BACKWARD,
            CharacterGranularity, false, true);
    }
    else {
        m_webView->fepTextEditor()->HandleMaskedDeleteText(frame);
    }
}

