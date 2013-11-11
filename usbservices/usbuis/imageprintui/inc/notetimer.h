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
* Description:  Header file for note timer
*
*/



#ifndef C_NOTETIMER_H
#define C_NOTETIMER_H

#include <e32base.h>

class CImagePrintUiAppUi;

/**
* Timer for showing notes in right time
*/
class CNoteTimer : public CTimer
    {
public:

    /**
    * Factory method NewL
    * @param aAppUi the pointer to the appui class
    * @return instance of notetimer        
    */
    static CNoteTimer* NewL( CImagePrintUiAppUi* aAppUi);
    
    /**
    * Factory method NewLC
    * @param aAppUi the pointer to the appui class
    * @return instance of notetimer        
    */
    static CNoteTimer* NewLC( CImagePrintUiAppUi* aAppUi);
    
   /**
    * Destructor.
    */
    ~CNoteTimer();

   
private:
    
   /**
    *   @see CActive
    */
    virtual void RunL();
    
   /**
    *   @see CActive
    */
    TInt RunError(TInt aError);

   /**
    * C++ default constructor
    */
    CNoteTimer();
    
   /**
    *   Second phase constructor. Operations which might leave should 
    *   be called here
    * @param aAppUi the pointer to the appui class
    */  
    void ConstructL(CImagePrintUiAppUi* aAppUi);

private:

    CImagePrintUiAppUi* iAppUi;
    };
    
#endif // C_NOTETIMER_H