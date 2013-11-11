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
* Description:   WebFepEditor.h
*
*/



#include <Browser_platform_variant.hrh>

#include <e32base.h>
#include <fepbase.h>
#include <coeinput.h>
#include <fepipext.h>
#include <frmtlay.h>
#include <aknextendedinputcapabilities.h> 
#include <eikccpu.h>
#include <baclipb.h>

#include "PlatformString.h"
#include "Node.h"
#include "Frame.h"

#ifndef WEBFEPTEXTEDITOR_H
#define WEBFEPTEXTEDITOR_H

using namespace WebCore;

class WebView;
class WebTextFormatMask;
 
class CWebFepTextEditor : public CBase,
                          public MObjectProvider,
                          public MCoeFepAwareTextEditor,
                          public MCoeFepAwareTextEditor_Extension1,
                          public TCoeInputCapabilities::MCoeFepSpecificExtensions,
                          public MEikCcpuEditor
    {
public:
    CWebFepTextEditor(WebView* aView);    
    ~CWebFepTextEditor();   
        
    void UpdateEditingMode();
    void CancelEditingMode();
    void ActivatePenInputRequest();
    void DeactivatePenInputRequest();
    TCoeInputCapabilities InputCapabilities();            
    
public:        
    // from MCoeFepAwareTextEditor    
    virtual void StartFepInlineEditL(const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
    virtual void UpdateFepInlineTextL(const TDesC& aNewInlineText, TInt aPositionOfInsertionPointInInlineText);
    virtual void SetInlineEditingCursorVisibilityL(TBool aCursorVisibility);
    virtual void CancelFepInlineEdit();
    virtual TInt DocumentLengthForFep() const;
    virtual TInt DocumentMaximumLengthForFep() const;
    virtual void SetCursorSelectionForFepL(const TCursorSelection& aCursorSelection);
    virtual void GetCursorSelectionForFep(TCursorSelection& aCursorSelection) const;
    virtual void GetEditorContentForFep(TDes& aEditorContent, TInt aDocumentPosition, TInt aLengthToRetrieve) const; // must cope with aDocumentPosition being outside the range 0 to DocumentLengthForFep()
    virtual void GetFormatForFep(TCharFormat& aFormat, TInt aDocumentPosition) const;
    virtual void GetScreenCoordinatesForFepL(TPoint& aLeftSideOfBaseLine, TInt& aHeight, TInt& aAscent, TInt aDocumentPosition) const;
    virtual void DoCommitFepInlineEditL();
    virtual MCoeFepAwareTextEditor_Extension1* Extension1(TBool& aSetToTrue);
    virtual void MCoeFepAwareTextEditor_Reserved_2();    

    // from MCoeFepAwareTextEditor_Extension1
    void SetStateTransferingOwnershipL(CState* aState, TUid aTypeSafetyUid);
    CState* State(TUid aTypeSafetyUid); // this function does *not* transfer ownership    
    virtual void StartFepInlineEditL(TBool& aSetToTrue, const TCursorSelection& aCursorSelection, const TDesC& aInitialInlineText, TInt aPositionOfInsertionPointInInlineText, TBool aCursorVisibility, const MFormCustomDraw* aCustomDraw, MFepInlineTextFormatRetriever& aInlineTextFormatRetriever, MFepPointerEventHandlerDuringInlineEdit& aPointerEventHandlerDuringInlineEdit);
    virtual void SetCursorType(TBool& aSetToTrue, const TTextCursor& aTextCursor);
    virtual void MCoeFepAwareTextEditor_Extension1_Reserved_3(){};
    virtual void MCoeFepAwareTextEditor_Extension1_Reserved_4(){};

    // from TCoeInputCapabilities::MCoeFepSpecificExtensions
    virtual TBool IsValidCharacter(TInt aChar);
    virtual void MCoeFepSpecificExtensions_Reserved_1(){};
    virtual void MCoeFepSpecificExtensions_Reserved_2(){};

    // from MObjectProvider
	virtual TTypeUid::Ptr MopSupplyObject(TTypeUid aId);	
	
    // from MEikCcpuEditor
	virtual TBool CcpuIsFocused() const;
	virtual TBool CcpuCanCut() const;
	virtual void CcpuCutL();
	virtual TBool CcpuCanCopy() const;
	virtual void CcpuCopyL();
	virtual TBool CcpuCanPaste() const;
	virtual void CcpuPasteL();
	virtual TBool CcpuCanUndo() const{return EFalse;}
	virtual void CcpuUndoL(){	}
	   
public:

    void UpdateFlagsState(TUint flags);
    void UpdateInputModeState(TUint inputMode, TUint permittedInputModes, TUint numericKeyMap);
    void UpdateCaseState(TUint currentCase, TUint permittedCase);
    bool GetStateFromFormatMask(TUint& currentCase, TUint& permittedCase, TUint& inputMode, TUint& permittedInputModes, TUint& flags, TUint& numericKeyMap);

    void HandleUpdateCursor();
    void UpdateInlineText(const TDesC& aText);
    void ClearInlineText();    

    void CreateTextFormatMask();        
    bool validateTextFormat();

    TBool IsDirectionRTL();
    
#if defined(BRDO_BROWSER_50_FF)
    void SetAlignment(CAknExtendedInputCapabilities::TInputCapabilities aAlignment);
#endif	// BRDO_BROWSER_50_FF

    bool IsTextAreaFocused() const;
    
    void EnableCcpu(TBool aSupport);
    void DoCcpuCanPasteL() const;
    void PlaceDataOnClipboardL();
    void RetrieveDataFromClipboardL();
    void PasteFromStoreL(CStreamStore& aStore,CStreamDictionary& aDict);
    void CopyToStoreL(CStreamStore& aStore,CStreamDictionary& aDict);    
    void HandleMaskedInsertText(WebCore::Frame* frame, const String& text);
    void HandleMaskedDeleteText(WebCore::Frame* frame);
    bool IsWapMaskedModeInput(WebCore::Frame* frame);

private:
    void  findPrevSiblingTextLen(Node*, TInt&) const;
    Node* findTextNodeForCurPos(Node* aNode, TInt& aPos) const;
    void setSCTAvailability(bool aAvailable);
       
private:
    CState* m_state;        
    WebView* m_webView;
    WebTextFormatMask* m_textFormatMask;    
    HBufC* m_inlineEditText;
    String m_inputTextColor;
    CAknExtendedInputCapabilities* m_ExtendedInputCapabilities;
    };

#endif
