/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Specialization of CEikRichTextEditor. Needed for enabling view 
*                by view scrolling.
*
*/



#include <frmtlay.h>

#include "FeedsViewMgr.hrh"
#include "RichTextEditor.h"


// -----------------------------------------------------------------------------
// CRichTextEditor::GetNumberOfLines
//
// Returns the total number of formatted lines in this text.
// -----------------------------------------------------------------------------
//
TInt CRichTextEditor::GetNumberOfLines()
    {
    return CEikEdwin::iLayout->NumFormattedLines();
    }


// -----------------------------------------------------------------------------
// CRichTextEditor::MoveCursorL
//
// Handles the view by view scrolling.
// -----------------------------------------------------------------------------
//
void CRichTextEditor::MoveCursorL(TCursorPosition::TMovementType aMovement, TBool aSelect)
    {
    TInt numberOfLines(GetNumberOfLines());
    TInt currentLine(GetLinePos(CursorPos()));
    TInt targetLine(0);

    switch(aMovement)
        {
        case TCursorPosition::EFPageDown:
            targetLine = currentLine + KNumberOfLinesInRichTextEditor;
    
            if (targetLine < numberOfLines)
                {
                SetCursorPosL(Get1stCharInLine(numberOfLines), aSelect);
                SetCursorPosL(Get1stCharInLine(targetLine), aSelect);
                }
            break;

        case TCursorPosition::EFPageUp:
            targetLine = currentLine - KNumberOfLinesInRichTextEditor;
    
            if (targetLine >= 0)
                {
                SetCursorPosL(0, aSelect);
                SetCursorPosL(Get1stCharInLine(targetLine), aSelect);
                }
            break;

        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// CRichTextEditor::GetLinePos
//
// Returns the line number of a given char position.
// -----------------------------------------------------------------------------
//
TInt CRichTextEditor::GetLinePos(TInt aPos) 
    { 
    return CEikEdwin::iLayout->GetLineNumber(aPos) + 1;
    }


// -----------------------------------------------------------------------------
// CRichTextEditor::Get1stCharInLine
//
// The 1st character in the given line.
// -----------------------------------------------------------------------------
//
TInt CRichTextEditor::Get1stCharInLine(TInt aLinePos) 
    {
    return CEikEdwin::iLayout->FirstCharOnLine(aLinePos);
    }


