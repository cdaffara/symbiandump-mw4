/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Alf server visual owner
*
*/



#ifndef C_ALFSRVVISUALOWNER_H
#define C_ALFSRVVISUALOWNER_H

#include <uiacceltk/HuiVisual.h>

class CAlfAppSrvSessionBase;

/**
 *  Alf server visual owner
 *
 *  The alfred component that owns an HUI visual.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfSrvVisualOwner) : public CBase, public MHuiVisualOwner
    {
    
public:

    static CAlfSrvVisualOwner* NewL( 
        CAlfAppSrvSessionBase& aSession,
        MHuiVisualOwner* aRealOwner );

    virtual ~CAlfSrvVisualOwner();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    MHuiVisualOwner* RealOwner();
    
    void SetVisualLayoutUpdatedNotificationId( TInt aNotificationId );

protected:

    CHuiEnv& Env() const;
    CHuiControlGroup* ControlGroup() const;
    MHuiEventHandler* EventHandler();
    void AppendL(CHuiVisual* aVisual);
    void Remove(CHuiVisual* aVisual);
    void VisualDestroyed(CHuiVisual& aVisual);
    void VisualLayoutUpdated(CHuiVisual& aVisual);
    void VisualPrepareDrawFailed(CHuiVisual& aVisual, TInt aErrorCode);

private:

    CAlfSrvVisualOwner( 
        CAlfAppSrvSessionBase& aSession,
        MHuiVisualOwner* aRealOwner );

    void ConstructL();

private: // data

    CAlfAppSrvSessionBase& iSession;
    MHuiVisualOwner* iRealOwner;
    TInt iVisualLayoutUpdatedNotificationId;
    };

#endif // C_ALFSRVVISUALOWNER_H
