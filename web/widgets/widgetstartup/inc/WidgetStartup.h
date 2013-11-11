/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the CWidgetStartup class.
*

*
*/


#ifndef WIDGETSTARTUP_H
#define WIDGETSTARTUP_H

//  INCLUDES
#include <eikappui.h>

// FORWARD DECLARATIONS

/**
*
* @since 3.1
*/
class CWidgetStartup : public CEikAppUi
    {
    public:  // Constructors and destructor

         /**
         * C++ default constructor.
         */
        CWidgetStartup();


        /**
        * Destructor.
        */
        virtual ~CWidgetStartup();


    protected:
    
        /**
        * 2nd phase constructor.
        */
	    void ConstructL();
	    
    private:
 
        /**
         * Run the preinstall functionality
         */
        void DoPreInstallL();
        
    private: //  Data
        
    };
    
#endif      // WIDGETSTARTUP_H

// End of File
