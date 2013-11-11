/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for imageprintuidocument
*
*/



#ifndef IMAGEPRINTUIDOCUMENT_H
#define IMAGEPRINTUIDOCUMENT_H

#include <AknDoc.h>
#include <pictbridge.h>
#include <e32cmn.h>
#include <AiwGenericParam.h>
#include <GenericParamConsumer.h>
#include <badesca.h>



// FORWARD DECLARATIONS
class CEikAppUi; 
class CImagePrintUiEngine; 
/**
 *   
 *  S60 document class
 */
class CImagePrintUiDocument : public CAknDocument
    {

public:
    
    /**
     * Factory method NewL
     * @param aApp refernce to CEikApplication class   
     * @return The new object. 
     */
    static CImagePrintUiDocument* NewL(CEikApplication& aApp);

   /**
    * Destructor.
    */ 
    virtual ~CImagePrintUiDocument();


private:

    /**
     * Default contructor
     */
    CImagePrintUiDocument(CEikApplication& aApp);
    
    /**
     * Default contructor that may leave. 
     */
    void ConstructL();


private: //  From base class CAknDocument

    /**
      * Returns AppUi class
      * @return Initialized instance of CEikAppUi
      */
    CEikAppUi* CreateAppUiL();


public: // New methods

    /**
      * Returns Engine
      * @Return PictBridge Engine
      */
	CDpsEngine* Engine();
	
private: // data
    
    // Instance to the engine of this application, own
     CDpsEngine* iEngine;
    };

#endif //  C_IMAGEPRINTUIDOCUMENT_H
