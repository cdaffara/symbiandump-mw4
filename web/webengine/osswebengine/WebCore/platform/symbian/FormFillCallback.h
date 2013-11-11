/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef FORMFILLCALLBACK_H
#define FORMFILLCALLBACK_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFont;
class MFormFillPopup;

// CLASS DECLARATION

class MFormFillCallback {
    public:
        virtual void queryUserAboutAutoFillL(bool& saveNow, bool& neverSave) = 0;
        virtual MFormFillPopup* createFormFillPopup(CFont* font) = 0;
        virtual void autoComplete(TPtrC text) = 0;
        virtual void cancelPopup() = 0;
};


#endif      // FORMFILLCALLBACK_H

// End of File
