/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for emptyview
*
*/

#ifndef CIMAGEPRINTEMPTYVIEW_H
#define CIMAGEPRINTEMPTYVIEW_H


#include <aknview.h>

// FORWARD DECLARATIONS
class CEmptyContainer;

const TUid KImagePrintEmptyViewId = { 1 };

/**
*  Class for showing empty view before capabilities has retrieved
*/
class CEmptyView : public CAknView
    {

public:

   /**
    * Factory method NewL
    * @return   The new object. 
    */ 
    static CEmptyView* NewL();

   /**
    * Factory method NewLC
    * @return   The new object. 
    */ 
    static CEmptyView* NewLC();

   /**
    * Destructor.
    */ 
    virtual ~CEmptyView();
    
private: // from base class CAknView

    /**
     * From CAknView 
     * Returns view id.
     * @param None.
     * @return View id.
     */
    TUid Id() const;

    /**
     * From CAknView 
     * Handles user commands.
     * @param aCommand A command id.        
     * @return None.
     */
    void HandleCommandL(TInt aCommand);

    /**
     * From CAknView 
     * Activates view.
     * @param aPrevViewId Id of previous view.
     * @param aCustomMessageId Custom message id.
     * @param aCustomMessage Custom message.
     * @return None.
     */
    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
                     const TDesC8& aCustomMessage);

    /**
     * From CAknView 
     * Deactivates view.        
     * @param None.
     * @return None.
     */
    void DoDeactivate();

   /**
    * C++ default constructor
    */
    CEmptyView();

    /**
     * Symbian 2nd phase constructor.
     * @param None
     * @return None
     */
    void ConstructL();

private: // data

    /**
     * Container control of this view
     * Own
     */
    CEmptyContainer* iContainer; 
 
    };

#endif // CIMAGEPRINTEMPTYVIEW_H
