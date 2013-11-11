/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*/

#ifndef ALFCRPPLUGINCLIENT_H
#define ALFCRPPLUGINCLIENT_H

#include <e32base.h>
#include <w32std.h>

/**
 * Client for ALF window server extension
 */
NONSHARABLE_CLASS( CAlfCrpPluginClient ) : public CWsGraphic
    {
public:
    /**
     * Constructor.
     */
    CAlfCrpPluginClient();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Destructor.
     */
    virtual ~CAlfCrpPluginClient();
    
public:
    
    /**
     * Synchronizes drawing.
     */
    void Synchronize();

public: // from CWsGraphic
    
    virtual void HandleMessage(const TDesC8& aData);
    virtual void OnReplace();
        
    };

#endif // ALFCRPPLUGINCLIENT_H

// End of file
