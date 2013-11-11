/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Alf CRP ECOM plugin
*
*/


#ifndef AFLCRPPLUGIN_H
#define AFLCRPPLUGIN_H

// INCLUDES
#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/wsgraphicdrawer.h>

/**
 * Alf window server ECOM plugin class.
 */
NONSHARABLE_CLASS( CAlfCrpPlugin ) : public CWsGraphicDrawer
    {
public:

    /**
     * Creates a new instance of the plugin.
     * @return Created instance.
     */ 
    static CWsGraphicDrawer* CreateL();
    
    /**
     * Destructor.
     */ 
    ~CAlfCrpPlugin();
    
private:

    /**
     * From CWsGraphicDrawer.
     *
     * @param aGc
     * @param aRect
     * @param aData
     */ 
    void DoDraw( MWsGc& aGc, const TRect& aRect, const TDesC8& aData ) const;
    
    /**
     * Handles message from PreviewProviderClient.
     *
     * @param aData Message data.
     */ 
    void HandleMessage( const TDesC8& aData );

    /**
     * 2nd phase constructor.
     *
     * @param aEnv
     * @param aId
     * @param aOwner
     * @param aData
     */     
    void ConstructL( MWsGraphicDrawerEnvironment& aEnv, 
                     const TGraphicDrawerId& aId, 
                     MWsClient& aOwner, 
                     const TDesC8& aData );   

private:
    /**
     * Handles message from PreviewProviderClient.
     *
     * @param aData Message data.
     */     
    void DoHandleMessageL( const TDesC8& aData );
    
    };

#endif // AFLCRPPLUGIN_H

// End of file
