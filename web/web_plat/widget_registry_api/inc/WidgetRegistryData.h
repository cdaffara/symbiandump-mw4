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
* Description:  This file contains the CWidgetData and CWidgetInfo classes.
*
*
*/

#ifndef WIDGETREGISTRYDATA_H
#define WIDGETREGISTRYDATA_H

// INCLUDES
#include <f32file.h>
#include "WidgetRegistryConstants.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*
*  @since 3.1
*/
class CWidgetInfo : public CBase
    {
    public:
        /**
        * Constructor
        */
        inline CWidgetInfo()
            {
            iUid = TUid::Uid(0);
            iFileSize = 0;
            iBundleName = HBufC::New( KWidgetRegistryVal + 1 );
            iDriveName = HBufC::New( KMaxDriveName + 1 );
            }

        /**
        * Destructor.
        */
        inline virtual ~CWidgetInfo()
            {
            delete iBundleName;
            delete iDriveName;
            }

    public:
        TUid        iUid;
        TInt        iFileSize;
        HBufC*      iBundleName;// widget bundle name
        HBufC*      iDriveName;	// drive name
    };


#endif  //WIDGETREGISTRYDATA_H
