/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/




#ifndef __ALFEXSTICKERS_DOCUMENT_H__
#define __ALFEXSTICKERS_DOCUMENT_H__


#include <akndoc.h>

/* Forward declarations. */
class CAlfExStickersAppUi;
class CEikApplication;


/** 
 * @class CAlfExStickersDocument
  
 * @discussion An instance of class CAlfExStickersDocument is the Document part of the AVKON
 * application framework for the AlfExStickers example application
 */
class CAlfExStickersDocument : public CAknDocument
    {
public:

	/**
	 * @function NewL 
	 * @discussion Construct a CAlfExStickersDocument for the AVKON application aApp 
	 * using two phase construction, and return a pointer to the created object
	 * @param aApp application creating this document
	 * @result a pointer to the created instance of CAlfExStickersDocument
	 */
    static CAlfExStickersDocument* NewL(CEikApplication& aApp);
    
    /**
     * @function NewLC 
     * @discussion Construct a CAlfExStickersDocument for the AVKON application aApp 
     * using two phase construction, and return a pointer to the created object
     * @param aApp application creating this document
     * @result a pointer to the created instance of CAlfExStickersDocument
     */
    static CAlfExStickersDocument* NewLC(CEikApplication& aApp);

    /**
     * @function ~CAlfExStickersDocument 
     * @discussion Destroy the object and release all memory objects
     */
    ~CAlfExStickersDocument();

public: // from CAknDocument
	/**
	 * @function CreateAppUiL 
	 * @discussion Create a CAlfExStickersAppUi object and return a pointer to it
	 * @result a pointer to the created instance of the AppUi created
	 */
    CEikAppUi* CreateAppUiL();

private:

	/**
	 * @function ConstructL 
	 * @discussion Perform the second phase construction of a CAlfExStickersDocument object
	 */
    void ConstructL();
    
    /**
     * @function CAlfExStickersDocument
     * @discussion Perform the first phase of two phase construction 
     * @param aApp application creating this document
     */
    CAlfExStickersDocument(CEikApplication& aApp);

    };


#endif // __ALFEXSTICKERS_DOCUMENT_H__
