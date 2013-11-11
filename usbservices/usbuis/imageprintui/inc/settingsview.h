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
* Description:  Header file for settingsview
*
*/


#ifndef CSETTINGSVIEW_H
#define CSETTINGSVIEW_H


#include <aknview.h>

class CSettingsContainer;

const TUid KImagePrintSettingsViewId = { 2 };


/**
*   View class to present application main view
*/
class CSettingsView : public CAknView
    {

public:

   /**
    *   Two phase constructor
    *   @return a CSettingsView instance
    */
    static CSettingsView* NewL();
    
   /**
    *   Two phase constructor
    *   @return a CSettingsView instance
    */
    static CSettingsView* NewLC();

   /**
    *   Destructor
    */
    virtual ~CSettingsView();
        
       
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
     *   Default constructor
     */    
    CSettingsView();

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
    CSettingsContainer* iContainer; 
 
    };

#endif // CSETTINGSVIEW_H
