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
* Description:   Control group sub session.
*
*/



#ifndef C_ALFSRVCONTROLGROUPSUBSESSION_H
#define C_ALFSRVCONTROLGROUPSUBSESSION_H

#include "alfsrvsubsessionbase.h"

class CAlfAppSrvSessionBase;
class CHuiControlGroup;

/**
 *  Control group sub session.
 *
 *  @lib alfappservercore.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfSrvControlGroupSubSession): public CAlfSrvSubSessionBase
    {

public:

    /**
     * Two-phased constructor
     *
     * @param aSession Session in which this sub session belongs into.
     * @param aId Group ID.
     * @return New object. Ownership transferred.
     */
    static CAlfSrvControlGroupSubSession* NewL( CAlfAppSrvSessionBase& aSession, 
                                                TInt aId );

    /**
     * Two-phased constructor. Leaves the objenct onto the cleanup stack.
     *
     * @param aSession Session in which this sub session belongs into.
     * @param aId Group ID.
     * @return New object on stack. Ownership transferred.
     */
    static CAlfSrvControlGroupSubSession* NewLC( CAlfAppSrvSessionBase& aSession,
                                                 TInt aId );

    /**
     * Destructor.
     */
    virtual ~CAlfSrvControlGroupSubSession();

    /**
     * Returns the used control group.
     *
     * @return Used control group.
     */
    CHuiControlGroup& ControlGroup();
    
    /**
     * Returns if the control group is shown
     *
     * @return ETrue if shown
     */
    TBool IsShown() const;
    
    /**
     * Set if the control group is shown
     *
     * @param aShown ETrue if shown.
     */
    void SetIsShown( TBool aShown );
    
    /**
     * Returns the host control.
     *
     * @return Host control. Always on the control group.
     */
    CHuiControl& HostControl() const;

// From CAlfSrvSubSessionBase

    /**
     * From CAlfSrvSubSessionBase
     * Can process the given message?
     *
     * @param aMessage The message.
     * @return ETrue if can process the message.
     */
    TBool CanProcessMessage(const RMessage2& aMessage) const;
    
    /**
     * From CAlfSrvSubSessionBase
     * Processes the message.
     *
     * @param aMessage The message.
     */
    void DoProcessMessageL(const RMessage2& aMessage);

    // defines whether group would like go to top or bottom of groups stack
    TInt& PreferredPos();

private:

    CAlfSrvControlGroupSubSession( CAlfAppSrvSessionBase& aSession,
                                   TInt aId );

    void ConstructL();

    void AppendL( const RMessage2& aMessage );
    void RemoveL( const RMessage2& aMessage );
    void EnableTransformationL( const RMessage2& aMessage );
    
    // from subsessionbase
    CHuiControlGroup* AsHuiControlCroup();

private: // data
    
    CHuiControlGroup* iControlGroup;
    TInt iId;
    TBool iIsShown;
    CHuiControl* iHostControl;
    TInt iPrefrerredPos;
    };

#endif // C_ALFSRVCONTROLGROUPSUBSESSION_H
