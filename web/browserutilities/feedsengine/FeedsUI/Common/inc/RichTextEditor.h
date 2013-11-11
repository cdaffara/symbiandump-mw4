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



#ifndef RICH_TEXT_EDITOR_H
#define RICH_TEXT_EDITOR_H


// INCLUDES
#include <eikrted.h>
#include <eikedwin.h>
#include <frmtlay.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Specialization of CEikRichTextEditor. Needed for enabling view 
*  by view scrolling.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CRichTextEditor : public CEikRichTextEditor
	{
    public:
        /**
        * Returns the total number of formatted lines in this text.
        *
        * @since 3.0
        * @return The number of lines.
	    */
	    TInt GetNumberOfLines();
	    
	    /** 
	    * Handles the view by view scrolling.
	    *
        * @since 3.0
	    * @param aMovement The desired cursor movement. Only EFPageDown and EFPageUp
	    *                  are allowed.
	    * @param aSelect see CEikEdwin.
        * @return void.
	    */
	    void MoveCursorL(TCursorPosition::TMovementType aMovement, TBool aSelect);


    private:
        /**
        * Returns the line number of a given char position.
        *
        * @since 3.0
        * @param aPos A character position.
        * @return The line number of the character.
	    */
	    TInt GetLinePos(TInt aPos);

        /**
        * The 1st character in the given line.
        *
        * @since 3.0
        * @param aLinePos A line number.
        * @return The 1st character in that line.
	    */
	    TInt Get1stCharInLine(TInt aLinePos);	    
	};

#endif      // RICH_TEXT_EDITOR_H
            
// End of File