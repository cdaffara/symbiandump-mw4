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
* Description:   Class which detects the long pointer press.
*
*/



#ifndef C_HUILONGTAPDETECTOR_H
#define C_HUILONGTAPDETECTOR_H

#include <e32base.h>
#include "uiacceltk/HuiEvent.h"

class CHuiRoster;

/**
 *  This class is responsible of timing the long tap event and
 *  generating it back to the roster.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib hitchcock.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHuiLongTapDetector ): public CTimer
    {

public:

    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    static CHuiLongTapDetector* NewL(CHuiRoster& aRoster);
    

    /**
    * Destructor.
    */
    virtual ~CHuiLongTapDetector();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void OfferPointerEvent( const THuiEvent& aEvent );


private:

    CHuiLongTapDetector(CHuiRoster& aRoster);

    void ConstructL();
    
    // from base class CActive
    
    void RunL();
    
    TInt RunError( TInt aError );

private: // data

    /**
     * ?description_of_member
     */
    CHuiRoster& iRoster;
    THuiEvent iPreviousPointerEvent;
    };


#endif // C_HUILONGTAPDETECTOR_H
