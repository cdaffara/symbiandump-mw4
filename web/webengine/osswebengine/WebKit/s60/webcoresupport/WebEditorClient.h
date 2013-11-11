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
* Description:   extension of WebCore::EditorClient
*
*/

#ifndef WebEditorClient_h
#define WebEditorClient_h

#include "EditorClient.h"


class WebView;

class WebEditorClient : public WebCore::EditorClient {
public:     
	enum TEdwinHotKeys 
		{
        /** Cut. */
		EHotKeyCut,

        /** Copy. */
		EHotKeyCopy,

        /** Paste. */
		EHotKeyPaste
		};		 
    WebEditorClient(WebView* webview);
     virtual ~WebEditorClient() {}
     void pageDestroyed();
    
     bool shouldDeleteRange(WebCore::Range*);
     bool shouldShowDeleteInterface(WebCore::HTMLElement*);
     bool smartInsertDeleteEnabled(); 
     bool isContinuousSpellCheckingEnabled();
     void toggleContinuousSpellChecking();
     bool isGrammarCheckingEnabled();
     void toggleGrammarChecking();
     int spellCheckerDocumentTag();
    
     bool selectWordBeforeMenuEvent();
     bool isEditable();

     bool shouldBeginEditing(WebCore::Range*);
     bool shouldEndEditing(WebCore::Range*);
     bool shouldInsertNode(WebCore::Node*, WebCore::Range*, WebCore::EditorInsertAction);
     bool shouldInsertText(WebCore::String, WebCore::Range*, WebCore::EditorInsertAction);
     bool shouldChangeSelectedRange(WebCore::Range* fromRange, WebCore::Range* toRange, WebCore::EAffinity, bool stillSelecting);
    
     bool shouldApplyStyle(WebCore::CSSStyleDeclaration*, WebCore::Range*);
     bool shouldMoveRangeAfterDelete(WebCore::Range*, WebCore::Range*);

     void didBeginEditing();
     void respondToChangedContents();
     void respondToChangedSelection();
     void didEndEditing();
     void didWriteSelectionToPasteboard();
     void didSetSelectionTypesForPasteboard();
    
     void registerCommandForUndo(PassRefPtr<WebCore::EditCommand>);
     void registerCommandForRedo(PassRefPtr<WebCore::EditCommand>);
     void clearUndoRedoOperations();

     bool canUndo() const;
     bool canRedo() const;
    
     void undo();
     void redo();

     void handleKeypress(WebCore::KeyboardEvent*);
     void handleInputMethodKeypress(WebCore::KeyboardEvent*);
             
     void textFieldDidBeginEditing(WebCore::Element*);
     void textFieldDidEndEditing(WebCore::Element*);
     void textDidChangeInTextField(WebCore::Element*);
     bool doTextFieldCommandFromEvent(WebCore::Element*, WebCore::KeyboardEvent*);
     void textWillBeDeletedInTextField(WebCore::Element*);
     void textDidChangeInTextArea(WebCore::Element*);
     void ignoreWordInSpellDocument(const WebCore::String&);
     void learnWord(const WebCore::String&);
     void checkSpellingOfString(const UChar*, int length, int* misspellingLocation, int* misspellingLength);
     void checkGrammarOfString(const UChar*, int length, Vector<WebCore::GrammarDetail>&, int* badGrammarLocation, int* badGrammarLength);
     void updateSpellingUIWithGrammarString(const WebCore::String&, const WebCore::GrammarDetail& detail);
     void updateSpellingUIWithMisspelledWord(const WebCore::String&);
     void showSpellingUI(bool show);
     bool spellingUIIsShowing();
     void getGuessesForWord(const WebCore::String&, Vector<WebCore::String>& guesses);     
     void setInputMethodState(bool enabled);

private:
    void handleInsertText(WebCore::Frame* frame, const WebCore::String& text);
    void handleDeleteText(WebCore::Frame* frame);

private:
    WebView* m_webView;
    bool     m_shouldEndEditing;
};


#endif // WebEditorClient_h
