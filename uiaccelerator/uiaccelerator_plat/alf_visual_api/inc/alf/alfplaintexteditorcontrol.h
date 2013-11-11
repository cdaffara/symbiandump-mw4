/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Concrete editor control for plain (descriptor content) text
*
*/



#ifndef C_ALFPLAINTEXTEDITORCONTROL_H
#define C_ALFPLAINTEXTEDITORCONTROL_H

#include <e32base.h>
#include <alf/alfcontrol.h>
#include <alf/alfeditorcontrol.h>
#include <eikon.hrh> // For Avkon key mappings

/* Forward declarations. */
class CAlfTextEditorVisualization;
class CAlfTextSource;
class CAlfEditorPlainTextModifier;
class CRichText;
class CAlfTextEditorFunctionalState;

/**
 * CAlfPlainTextEditorControl provides un-marked up text editing
 *
 * Usage:
 * @code
 *  //Create editor control
 *  CAlfPlainTextEditorControl* control = new (ELeave) CAlfPlainTextEditorControl;
 *  control->ConstructL( *iEnv );
 *  control->SetMaximumTextLength( KTestMaximumTextLength );
 * 
 *  //Read entered text
 *   HBufC* readText = control->GetTextInHBufL();
 * 
 *  //Access plain text buffer
 *  CPlainText& text = control->Text();
 *  TPtrC readText = text.Read(control->TextLength());
 * 
 * @endcode
 * @lib alfclient.lib
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CAlfPlainTextEditorControl) : public CAlfEditorControl
    {

public:

    /** @beginAPI */

    /**
    * Statically creates in instance of this class
    *
    * @param aEnv   Alfred environment object
    * @return   a fully-constructed instance
    */
    IMPORT_C static CAlfPlainTextEditorControl* NewL(CAlfEnv& aEnv);

    /**
    * Constructor.
    * 
    */
    CAlfPlainTextEditorControl();

    /**
    * 2nd stage constructor
    *
    * @param aEnv   The Alf Environment
    */
    void ConstructL(CAlfEnv& aEnv);
            	
    /**
      * Destructor.
      */
    virtual ~CAlfPlainTextEditorControl();

public:  // From CAlfControl
    
    /**
     * Called when the control's focus state changes.
     *
     * @param aDisplay  The display where this control is changing focus
     * @param aFocused  EFalse iff losing focus.
     */
    void FocusChanged( CAlfDisplay& aDisplay, TBool aFocused );

    /**
     * Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    
    /**
     * Notifies the control that its visible has been changed on a display.
     * This is the earliest time when the control knows the dimensions of
     * the display it is being shown on.
     *
     * @param aIsVisible  ETrue, if the control is now visible on the display.
     *                    EFalse, if the control is about to the hidden on the display.
     * @param aDisplay    The display on which the control's visibility is changing.
     */    
    virtual void NotifyControlVisibility( 
        TBool aIsVisible, 
        CAlfDisplay& aDisplay );

public:  // From MVisualOwner:
 
    virtual void VisualLayoutUpdated(CAlfVisual& aVisual);

public:   // From CAlfEditorControl
 
    virtual void VisualizeL(  CAlfLayout* aParentLayout );
    
 public: // New Methods
  
    /**
    * Copy a descriptor into the editor as the current text.
    * Note this method is not suitable for long texts since a copy of the text is made.
    *
    * @param aText  Text descriptor to be copied in
    */
    IMPORT_C void SetTextL( const TDesC& aText );

    /**
    * Transfer to the caller ownership of newly constructed copy of the text.
    * Note that this method is not recommended for long texts.
    *
    * The returned buffer will have have length TextLength() and will not have an appended paragraph marker.
    * 
    * @return A newly instantiated copy of the text in the editor. Owned by the caller.
    */
    IMPORT_C HBufC* GetTextInHBufL() const;
    
    /**
    * Access the plain text buffer
    *
    * The CPlainText object will always have an end of paragraph mark (0x2029) appended. This will be at character index
    * TextLength(). 
    * @see TextLength()
    * 
    * @return a reference to a CPlainText object containing the document.. No ownership is transferred.
    */
    IMPORT_C CPlainText& Text() const;

    /**
    * Access the current text length.
    *
    * The returned value does not count the end of paragraph mark ( 0x2029 ) which is found ad the end of the CPlainText object.
    *
    * @return The length of the text
    */
    IMPORT_C TInt TextLength() const;

    /**
    * Sets the maximum text length.
    * 
    * This method will panic if the passed-in value is greater than the current TextLength()
    *
    * @param maximum allowed length of text
    */
    IMPORT_C void SetMaximumTextLength( TInt aMaxTextLength );

    /**
    * Access the maximum text length
    *
    * @return maximum allowed length of text
    */
    IMPORT_C TInt MaximumTextLength() const;

    /**
    * Set the current input position within the document. It is possible to set both the pending input position (cursor position)
    * in logical order, and to set to which run of text that cursor should be associated with. 
    * 
    * In the following, the terms "next", "previous", "after" and "before" all refer to the logical ordering of characters in the text
    * buffer.  The terms "left" and "right" refer to visual positioning.
    * 
    * The valid range for the position is 0 to TextLength(), inclusive. A value of TextLength() indicates the cursor is positioned after 
    * the last character. The value of the cursor position can be though of as a "gap number", where the first gap is that before 
    * character 0, and the last gap is that after TextLength(). Another way of thinking of this is the cursor position represents
    * the 0-based index of a character that would be input at that point. More simply, it can be thought of as sharing an
    * index value with the next character in the buffer.
    *
    * The leading flag can be used to associate the cursor visually with the next character in logical order (aLeading = ETrue)
    * or with the previous character in logical order. This has an effect visually, if the previous and next characters are in runs of 
    * text with different directionality. If a cursor is at ( N, Trailing ), and character at index N-1 is in a left-to-right (L-R) run of text, 
    * then the cursor will be displayed to the right of character N-1. This would be the default for a cursor at the end of a run of 
    * L-R text.  However, if the cursor is set to ( N, Leading ) and the character at index N is in a right-to-left (R-L) text run, then 
    * the cursor will adhere to that block and be positioned to the right side of character N. Note that this cursor position could 
    * potentially be at some distance to the right from the (N, Trailing) visual position. 
    * 
    * Conversely if there is a run of R-L text followed (logically) by L-R text, and the curosr is positioned at N. (i.e. N is in the 
    * L-R text and N-1 is in the R-L). In this case, if the cursor is trailing, it trails character N-1, and therefore is 
    * positioned at the end (left side) of the R-L text - which is positioned visually between the runs of text. If the cursor is 
    * set to leading, then it leads character N, and is therefore at the left end of the L-R text, probably at the left end of the whole text. 
    * 
    * Note that the values of ( 0, Trailing ), and ( TextLength(), Leading ) are both legal, and behave as they are specified. 
    * SetCursorPos(0) is generally what one wants. It will cause the cursor to adhere to the end of text that is entered/pasted.
    * SetCursorPos( MaxLength(), ETrue ) will generally behave the same as SetCursorPos( MaxLength(), EFalse) since there
    * will still be no "next run" of text after input.
    * 
    * @param a "gap number" in the document. 
    * @param aLeading = EFalse iff the cursor is to be associated visually to the previous character's run of text. 
    */
    IMPORT_C void SetCursorPos( TInt aNewPosition, TBool aLeading = EFalse );
    
    /**
    * Gets the current input position within the document.
    * @see SetCursorPos( TInt, TBool );
    *
    * @param returns the logical position of the cursor
    * @param returns the leading/trailing flag
    */
    IMPORT_C void GetCursorPos( TInt& aPosition, TBool& aLeading );
    
    /**
    * Obtain the simple cursor position without leading/trailing information.
    * This API is appropriate in many cases that the visual positioning of the cursor is not a concern.
    * @see SetCursorPos( TInt, TBool );
    *
    * @return the gap number that the cursor is presently located in
    */
    IMPORT_C TInt CursorPos() const;
    
    /**
    * Set a selection on the editor contents.  The selection is single contiguous range of indices with a 
    * starting cursor position (the "Anchor") and an end position (the "Cursor"). It thus has a directionality.
    * The semantics of the selection is that it indicates what set of characters the next user input is going 
    * to affect. It is usually indicated to the user with a selection highlight. The ends of the selection may or 
    * may not be differentiated visually to the user.
    *
    * Both anchor and cursor follow the conventions of the the cursor in SetCursorPos. 
    * @see SetCursorPos( TInt, TBool )
    * Note however, that the selected set of characters comprises those characters between the anchor and cursor.
    * So if a selection is set at (M,N), and then the selection is deleted, the characters at indices M, M+1...N-1 will be deleted.
    *
    * The Set/GetCursorPos APIs interoperate with the selection-oriented APIs. The cursor position is merely the 
    * end point of the selection.
    * 
    * Setting a zero length selection is the same as setting a (non-selection) cursor position. This action leaves the 
    * leading/trailing flag state of the cursor unchanged. If the editor client needs to zero the selection and also change the 
    * leading/trailing flag, he must first call SetSelection with zero length, and then call SetCursorPos, explicitly setting the
    * leading/trailing flag.
    * 
    * Setting a non-zero length selection does affect the leading trailing flag, according to the following rules: 
    * If aCursorPosition > aAnchorPosition, then the flag is set to Trailing. 
    * If aCursorPosition < aAnchorPosition, then the flag is set to Leading.
    * 
    * Deletion or replacing the selection (by user activity or via the Text() interface leaves the leading trailing state 
    * unchanged
    * 
    * @param aAnchorPosition    Starting gap position of the selection
    * @param aCursorPosition    End gap position of the selection
    */
    IMPORT_C void SetSelection( TInt aAnchorPosition, TInt aCursorPosition );

    /**
    * Moves only the cursor position of the selection. The anchor position remains unchanged.
    * 
    * @param the cursor position to move the end of the selection to.
    */
    IMPORT_C void ExtendSelection( TInt aNewCursorPosition );

    /**
    * Obtain the current selection. 
    * @see SetSelection( TInt, TInt );
    * 
    * @param aAnchorPosition  Returns the anchor gap number
    * @param aCursorPosition  Returns the cursor gap number
    */
    IMPORT_C void GetSelection( TInt& aAnchorPosition, TInt& aCursorPosition )const;

    /**
     * Sets permitted cases for the editor.
     * Possible flags from @c uikon.hrh are @c EAknEditorAllCaseModes, 
     * @c EAknEditorUpperCase, @c EAknEditorLowerCase and 
     * @c EAknEditorTextCase. 
     * @c EAknEditorAllCaseModes is default.
     *
     * @param aPermittedCaseModes Case modes that are available in the editor.
     */
    IMPORT_C void SetPermittedCaseModes(TInt aPermittedCaseModes);

    /**
     * Sets default case for the editor. The case is used when the editor
     * is focused first time. Available case flags from @c uikon.hrh
     * are @c EAknEditorUpperCase, @c EAknEditorLowerCase and 
     * @c EAknEditorTextCase. @c EAknEditorTextCase is used if the initial 
     * case by default.
     *
     * @param aCase Initial case to be used in the editor.
     */
    IMPORT_C void SetDefaultCase(TInt aDefaultCase);
    
    /**
     * Updates editor case. FEP is automatically notified of the
     * case change in editor state and FEP takes new case to use immediately.
     * Available case flags from @c uikon.hrh are @c EAknEditorUpperCase, 
     * @c EAknEditorLowerCase and @c EAknEditorTextCase.
     * This method should not be used to set initial case for the editor.
     * Use @c SetAknEditorCase() method instead for setting initial case.
     *
     * @param aCase New case for the editor.
     */
    IMPORT_C void SetCurrentCase(TInt aCase);
        
    /**
     * Sets default input mode for the editor. The input mode is used when 
     * the editor is focused first time. 
     * @c EAknEditorTextInputMode is used if the initial input mode by default.
     *
     * @param aInputMode Initial input mode to be used in the editor.
     */
    IMPORT_C void SetDefaultInputMode( TInt aInputMode );
    
    /**
     * Updates editor input mode. FEP is automatically notified of the
     * new input mode in editor state and FEP takes new input mode to use 
     * immediatelly. Current inline editing is reset.
     * This method should not be used for setting initial input mode for the 
     * editor. Use @c SetAknEditorInputMode() method instead for setting 
     * initial input mode.
     *
     * @param aInputMode New input mode for the editor.
     */
    IMPORT_C void SetCurrentInputMode( TInt aInputMode );

    /**
     * Sets allowed input modes for the editor. 
     * All input modes are allowed (@c EAknEditorAllInputModes) if the value 
     * is not set from EDWIN resource @c allowed_input_modes or with this API.
     *
     * @param aInputModes Input modes that are allowed in the editor.
     */
    IMPORT_C void SetPermittedInputModes( TInt aInputModes );
    
    /**
     * Sets number mode key mapping for '*' and '#' keys. The key mapping is 
     * used in editors when numeric input mode is used. Key mapping characters
     * for '*' key are shown on special character table if also text input 
     * modes are allowed in the editor but the current input mode is numeric 
     * mode. @c EAknEditorStandardNumberModeKeymap is used in editors that 
     * allow only numeric input if the mapping is not set from EDWIN resource 
     * @c numeric_keymap or with this API. 
     * @c EAknEditorAlphanumericNumberModeKeymap is used as default if the 
     * editor allows also text input mode.
     *
     * @param aNumericKeymap Key mapping used in an editor with number input 
     *        mode.
     */
    IMPORT_C void SetNumericKeymap(
        TAknEditorNumericKeymap aNumericKeymap);
    
     /**
     * Sets the @c EOnlyASCIIChars flag. 
     *
     * @param aASCIIOnly If @c ETrue the EOnlyASCIIChars flag will be set. If 
     *        @c EFalse the EOnlyASCIIChars flag will be removed.
     */
    IMPORT_C void SetOnlyASCIIChars( TBool aASCIIOnly );
    
    /**
    * Autoselection of text. When set, this initially sets the editor to have the cursor position 
    * at the end of the document with the selection's anchor at the beginning. 
    * That is, the whole text is selected.
    *
    * If cursor positioning at the end is desired, but with no auto-selection, then the flag 
    * EAutoCursorAtEndIndex should be used.  But if selection is desired, then this flag only 
    * is needed.
    * 
    * This setting is looked at whenever SetTextL is called and not just at initial display of the editor.
    *
    * @param aAutoSelection     iff not EFalse, will cause autoselection
    */
    IMPORT_C void SetAutoSelection( TBool aAutoSelection );

    /**
    * Automatic setting of the cursor at the end of the document.
    * If set, this initially sets the editor to have the cursor position at the end of the document,
    * but with no selection.
    *
    * This setting is looked at whenever SetTextL is called and not just at initial display of the editor.
    *
    * @param aCursorAtEnd     iff not EFalse, will cause auto-positioning of the cursor at the end of the document
    */
    IMPORT_C void SetInitialCursorAtEnd( TBool aCursorAtEnd );
    
    /**
    * Access method for visualizations to get at the non editor content state
    * @return A pointer to a CAknEditorState object
    */ 
    CAlfTextEditorFunctionalState& State() const;
    
    /**
    * Set the text style for the text.  The Id passed in is either a pre-allocated
    * Alf text style ID or a derived ID using the TextStyleManager.
    *
    * @param aTextStyleId Alf platform-specifc text id
    */
    IMPORT_C void SetTextStyleId( TInt aTextStyleId );
    
protected:

private:

    /**
    * Create visuals for this editor in the given parent
    */
    void CreateVisualizationL( CAlfLayout* aParentLayout );
    
    void SetSingleAknEditorFlagAndReportIfChanged( TInt aSingleFlag, TBool aNewValue );
    
private:
    // @todo Not properly BC-proofed
    CAlfTextSource* iTextSource;    // Owned
    CAlfEditorPlainTextModifier* iTextModifier; // Owned
    TInt iMaximumTextLength;
    CAlfTextEditorVisualization* iTextVisualization; // Not Owned
    
    // Functional flags:
    CAlfTextEditorFunctionalState* iState;
    // textStyleId to be passed to the visualization 
    TInt iTextStyleId;

    TInt iSpare;
    };

#endif  // C_ALFPLAINTEXTEDITORCONTROL_H
