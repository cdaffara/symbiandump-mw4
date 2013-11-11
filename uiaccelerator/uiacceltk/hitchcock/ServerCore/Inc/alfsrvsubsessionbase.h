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
* Description:   Sub session base
*
*/



#ifndef C_ALFSRVSUBSESSIONBASE_H
#define C_ALFSRVSUBSESSIONBASE_H

#include <e32base.h>

class CAlfAppSrvSessionBase;
class MHuiVisualOwner;
class CHuiVisual;
class CHuiLayout;
class CHuiBrush;
class CHuiControl;
class CHuiControlGroup;
class MHuiMappingFunction;
class CAlfBrushHandler;
class MAlfExtension;
class CHuiDisplay;

/**
 *  Sub session base.
 *
 *  @lib alfappservercore.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfSrvSubSessionBase) : public CObject
    {

public:

    /**
    * Public constructor
    */
    CAlfSrvSubSessionBase( CAlfAppSrvSessionBase& aSession );

    /**
    * static c-tor
    */
    static CAlfSrvSubSessionBase* NewLC(const RMessage2& aMessage, 
                                       TInt aImplementationUid,  
                                       TInt aImplementationId,
                                       CAlfAppSrvSessionBase& aSession);

    /**
     * Destructor
     */
    ~CAlfSrvSubSessionBase();
    
    /**
     * Processes message.
     *
     * @param aMessage
     */
    void ProcessMessageL(const RMessage2& aMessage);

    // Interfaces:
    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual MHuiVisualOwner* AsVisualOwner();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiControl* AsHuiControl();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiVisual* AsHuiVisual();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiLayout* AsHuiLayout();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiBrush* AsHuiBrush();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual MHuiMappingFunction* AsMappingFunction();

    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiControlGroup* AsHuiControlCroup();
    
    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CHuiDisplay* AsHuiDisplay();
 
    /**
     * needed for visual factory, only subsessions having instance implementing the interface 
     * should implement this virtual 
     */
    virtual CAlfBrushHandler* AsBrushHandler();

    /**
     * Request to complete pending asynchornous request(s)  
     * Initiated by client
     */
    virtual void CancelAsynchRequests();

    virtual MAlfExtension* AsCommandHandler();

    /**
    * Public member for session to check what factory was used to create this
    */
    TInt iFactoryUid;

protected:
    
    /**
     * Processes message. Meant for child class to implement.
     *
     * @param aMessage
     */
    virtual void DoProcessMessageL(const RMessage2& aMessage);
    
    /**
     * Returns the session this sub session belongs to.
     *
     * @return Session.
     */
    CAlfAppSrvSessionBase& Session();

    /**
     * must be base-called from deriving classes to finalize construction
     */
    virtual void ConstructL();

private:
    void ConstructL(const RMessage2& aMessage, 
                    TInt aImplementationUid,  
                    TInt aImplementationId );

    
private: // data

    CAlfAppSrvSessionBase& iSession; 

    class TPrivateData;                          
    TPrivateData* iData;
    };


#endif // C_ALFSRVSUBSESSIONBASE_H
