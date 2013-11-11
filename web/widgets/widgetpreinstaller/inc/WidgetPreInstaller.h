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
* Description:  This file contains the header file of the CWidgetPreInstaller class.
*

*
*/


#ifndef WIDGETPREINSTALLER_H
#define WIDGETPREINSTALLER_H

//  INCLUDES
#include <eikappui.h>
#include <SWInstUIPluginAPI.h>


// FORWARD DECLARATIONS
class CSWInstUIPluginAPI;


/**
*
* @since 3.1
*/
class CWidgetPreInstaller : public CEikAppUi
    {
    public:  // Constructors and destructor

         /**
         * C++ default constructor.
         */
        CWidgetPreInstaller();


        /**
        * Destructor.
        */
        virtual ~CWidgetPreInstaller();
        
    public: 

    protected:
    
        /**
        * 2nd phase constructor.
        */
	    void ConstructL();
	    
    private:
    
        /**
         * Perform the pre installation.
         */
        void DoInstallL();

    private: //  Data
    
        /**
         * File system session.
         */
        RFs iFs;
        
    };
    
#endif      // WIDGETPREINSTALLER_H

// End of File
