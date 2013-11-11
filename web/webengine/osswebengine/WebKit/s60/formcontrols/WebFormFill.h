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
* Description:  FormFill
*
*/


#ifndef WEBFORMFILL_H
#define WEBFORMFILL_H

//  INCLUDES
#include <e32base.h>
#include "FormFillCallback.h"


// CONSTANTS

// FORWARD DECLARATIONS


// CLASS DECLARATION

class WebView;
class MFormFillPopup;
class CFont;

namespace WebCore {
    class FormState;
    class Element;
}

/**
*  Displays the Page overview
*
*  @lib browserview.lib
*  @since Series 60 3.0
*/
class WebFormFill : public MFormFillCallback
    {
    public:  // Constructors and destructor

        /**
        * Constructor
        */
        WebFormFill( WebView* view );

        /**
        * Destructor.
        */
        virtual ~WebFormFill();
    public: // New functoins

        void willSubmitForm(WebCore::FormState* formState);
        void textChanged(WebCore::Element* inputElement);
        void updatePopupView();

    public: // from MFormFillCabbback
        void queryUserAboutAutoFillL(bool& saveNow, bool& neverSave);
        MFormFillPopup* createFormFillPopup(CFont* font);
        void autoComplete(TPtrC text);
        void cancelPopup();
    private:    // Data

        WebView* m_webView;
    };

#endif      // WEBFORMFILL_H

// End of File
