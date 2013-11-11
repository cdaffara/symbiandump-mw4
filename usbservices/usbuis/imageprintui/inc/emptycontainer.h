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
* Description:  Header file for emptycontainer
*
*/

#ifndef CEMPTYCONTAINER_H
#define CEMPTYCONTAINER_H


// INCLUDES
#include <coecntrl.h>


// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;


/**
* Container class for emptyview
*/
class CEmptyContainer : public CCoeControl
    {


public:

    /**
     * Factory method NewL
     * @param aRect Container area 
     * @return   The new object. 
     */ 
    static CEmptyContainer* NewL( const TRect& aRect );

    /**
     * Factory method NewLC
     * @param aRect Container area 
     * @return   The new object. 
     */ 
    static CEmptyContainer* NewLC( const TRect& aRect );


    /**
    * Destructor.
    */
    virtual ~CEmptyContainer();
    

private: // Methods derived from CCoeControl


   /**
    * From CCoeControl Draw screen
    */
    void Draw( const TRect& aRect ) const;	
    
   /**
    * From CCoeControl Handles a chage of client area size.
    */
    void SizeChanged(); 	

   /**
    * From CCoeControl, Handles a change to the control's resources.
    */
    void HandleResourceChange( TInt aType ); 	

   
private:

   /**
    * C++ default constructor
    */
    CEmptyContainer();

   /**
    *   Second phase constructor. Operations which might leave should 
    *   be called here
    */ 
    void ConstructL( const TRect& aRect );


private: // data

    CAknsBasicBackgroundControlContext* iSkinContext; // skin data

    };

#endif // CEMPTYCONTAINER_H
