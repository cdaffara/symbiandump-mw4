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



#ifndef __ALFEXCALENDAR_DOCUMENT_H__
#define __ALFEXCALENDAR_DOCUMENT_H__


#include <akndoc.h>

// Forward references
class CAlfExCalendarAppUi;
class CEikApplication;


/*! 
  @class CAlfExCalendarDocument
  
  @discussion An instance of class CAlfExCalendarDocument is the Document part of the AVKON
  application framework for the alfexcalendar application
  */
class CAlfExCalendarDocument : public CAknDocument
    {
public:

/*!
  @function NewL
  
  @discussion Construct a CAlfExCalendarDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp application creating this document
  @result a pointer to the created instance of CAlfExCalendarDocument
  */
    static CAlfExCalendarDocument* NewL(CEikApplication& aApp);

/*!
  @function NewLC
  
  @discussion Construct a CAlfExCalendarDocument for the AVKON application aApp 
  using two phase construction, and return a pointer to the created object
  @param aApp application creating this document
  @result a pointer to the created instance of CAlfExCalendarDocument
  */
    static CAlfExCalendarDocument* NewLC(CEikApplication& aApp);

/*!
  @function ~CAlfExCalendarDocument
  
  @discussion Destroy the object and release all memory objects
  */
    ~CAlfExCalendarDocument();

public: // from CAknDocument
/*!
  @function CreateAppUiL 
  
  @discussion Create a CAlfExCalendarAppUi object and return a pointer to it
  @result a pointer to the created instance of the AppUi created
  */
    CEikAppUi* CreateAppUiL();

private:

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CAlfExCalendarDocument object
  */
    void ConstructL();

/*!
  @function CAlfExCalendarDocument
  
  @discussion Perform the first phase of two phase construction 
  @param aApp application creating this document
  */
    CAlfExCalendarDocument(CEikApplication& aApp);

    };


#endif // __ALFEXCALENDAR_DOCUMENT_H__
