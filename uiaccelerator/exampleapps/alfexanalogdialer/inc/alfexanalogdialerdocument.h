/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application document class for Analog Dialer example application, which
*                implements rotating dial pad.
*
*/



#ifndef ANALOGDIALER_DOCUMENT_H
#define ANALOGDIALER_DOCUMENT_H

#include <akndoc.h>

// Forward references
class CAlfExAnalogDialerAppUi;
class CEikApplication;

/**
 *  Document class for Analog Dialer
 *
 *  @since S60 5.0
 */
class CAlfExAnalogDialerDocument : public CAknDocument
    {
public:

    
    /* Constructors and destructor. */

    /**
     * First phase constructor of CAlfExAnalogDialerDocument.
     * Construct a CAlfExAnalogDialerDocument for the AVKON application aApp 
     * using two phase construction, and return a pointer to the created object
     *
     * @since S60 5.0
     * @param aApp application creating this document
     * @result a pointer to the created instance of CAlfExAnalogDialerDocument
     */
    static CAlfExAnalogDialerDocument* NewL(CEikApplication& aApp);

    /**
     * First phase constructor of CAlfExAnalogDialerDocument.
     * Construct a CAlfExAnalogDialerDocument for the AVKON application aApp 
     * using two phase construction, and return a pointer to the created object
     *
     * @since S60 5.0
     * @param aApp application creating this document
     * @result a pointer to the created instance of CAlfExAnalogDialerDocument
     */
    static CAlfExAnalogDialerDocument* NewLC(CEikApplication& aApp);

    /**
     * Destructor of CAlfExAnalogDialerDocument
     *
     * @since S60 5.0
     */
    ~CAlfExAnalogDialerDocument();


    /* Methods */

// from base class CAknDocument

    /**
     * From CAknDocument
     * Create a CAlfExAnalogDialerAppUi object and return a pointer to it
     *
     * @since S60 5.0
     * @result a pointer to the created instance of the AppUi created
     */
    CEikAppUi* CreateAppUiL();

private:


    /* Constructors and destructor. */

    /**
     * Constructor of CAlfExAnalogDialerDocument.
     * Private as this object must be constructed via NewL() or NewLC()
     *
     * @since S60 5.0
     */
    CAlfExAnalogDialerDocument(CEikApplication& aApp);

    /**
     * Second phase constructor of CAlfExAnalogDialerDocument.
     * Private as this object must be constructed via NewL() or NewLC()
     *
     * @since S60 5.0
     */
    void ConstructL();

private:
    /**
     * Not yet member data
     */

    };


#endif // ANALOGDIALER_DOCUMENT_H
