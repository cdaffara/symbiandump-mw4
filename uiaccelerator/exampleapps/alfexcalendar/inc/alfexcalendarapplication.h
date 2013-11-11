/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#ifndef __ALFEXCALENDAR_APPLICATION_H__
#define __ALFEXCALENDAR_APPLICATION_H__

#include <aknapp.h>


/*! 
  @class CAlfExCalendarApplication
  
  @discussion An instance of CAlfExCalendarApplication is the application part of the AVKON
  application framework for the alfexcalendar application
  */
class CAlfExCalendarApplication : public CAknApplication
    {
public:  // from CAknApplication

/*! 
  @function AppDllUid
  
  @discussion Returns the application DLL UID value
  @result the UID of this Application/Dll
  */
    TUid AppDllUid() const;

protected: // from CAknApplication
/*! 
  @function CreateDocumentL
  
  @discussion Create a CApaDocument object and return a pointer to it
  @result a pointer to the created document
  */
    CApaDocument* CreateDocumentL();
    };

#endif // __ALFEXCALENDAR_APPLICATION_H__
