/*
 * Copyright (c) 2005, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Declares USB UI notifiers base class.
 *
 */
#ifndef USBUINOTIFIER_H
#define USBUINOTIFIER_H
// INCLUDES
#include <usbuinotif.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh> 
#include "usbuinotifdebug.h"
// CONSTANTS
// Literal resource filename 
_LIT(KResourceFileName, "usbuinotif.rsc");

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 *  This class is the base class for all notifiers.
 *
 *  @lib
 */
NONSHARABLE_CLASS(CUSBUINotifierBase) : public CBase, public MEikSrvNotifierBase2
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CUSBUINotifierBase();

protected:
    // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CUSBUINotifierBase();

    /**
     * Symbian 2nd phase constructor.
     */
    virtual void ConstructL();

protected:
    // New functions

    /**
     * Used in asynchronous notifier launch to store received parameters 
     * into members variables and make needed initializations.        
     * @param aBuffer A buffer containing received parameters
     * @param aReplySlot a Reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @return None.
     */
    virtual void StartDialogL(const TDesC8& aBuffer, TInt aReplySlot,
            const RMessagePtr2& aMessage)=0;

    /**
     * Check if message needs to be completed and complete it
     * @param aReason   The completetion code
     */
    void CompleteMessage(TInt aReason);

    /**
     * Initialize HbTextResolrer, if not initialized before
     * (checks it from TLS)
     */    
    void InitializeTextResolver();
   
protected:
    // Functions from base classes        

    /**
     * From MEikSrvNotifierBase2 Called when a notifier is first loaded 
     * to allow any initial construction that is required.
     * @param None.
     * @return A structure containing priority and channel info.
     */
    virtual TNotifierInfo RegisterL()=0;

    /**
     * From MEikSrvNotifierBase2 The notifier has been deactivated 
     * so resources can be freed and outstanding messages completed.
     * @param None.
     * @return None.
     */
   virtual void Cancel();

private:
    // Functions from base classes        

    /**
     * From MEikSrvNotifierBase2 Called when all resources allocated 
     * by notifiers should be freed.
     * @param None.
     * @return None.
     */
    virtual void Release();

    /**
     * From MEikSrvNotifierBase2 Return the priority a notifier takes 
     * and the channels it acts on.
     * @param None.
     * @return A structure containing priority and channel info.
     */
    virtual TNotifierInfo Info() const;

    /**
     * From MEikSrvNotifierBase2 Synchronic notifier launch.        
     * @param aBuffer Received parameter data.
     * @return A pointer to return value.
     */
    virtual TPtrC8 StartL(const TDesC8& aBuffer);

    /**
     * From MEikSrvNotifierBase2 Asynchronic notifier launch.
     * @param aBuffer A buffer containing received parameters
     * @param aReturnVal The return value to be passed back.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @return A pointer to return value.
     */
    virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot,
            const RMessagePtr2& aMessage);

    /**
     * From MEikSrvNotifierBase2 Updates a currently active notifier.
     * @param aBuffer The updated data.
     * @return A pointer to return value.
     */
    virtual TPtrC8 UpdateL(const TDesC8& aBuffer);

 
  
protected:
    // Data

    RMessagePtr2 iMessage; // Received message
    TInt iReplySlot; // Reply slot
    TBool iNeedToCompleteMessage; // Flag for releasing messages

    TInt iResourceFileFlag; // Flag for eikon env.
    TNotifierInfo iInfo; // Notifier parameters structure    
    CEikonEnv* iEikEnv; // Local eikonenv, not own
    TBool iTranslator ;
    };

#endif // USBUINOTIFIER_H
// End of File
