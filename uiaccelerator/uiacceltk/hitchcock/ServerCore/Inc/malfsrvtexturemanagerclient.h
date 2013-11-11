/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Client interface for server side texture manager
*
*/



#ifndef M_ALFSRVTEXTUREMANAGERCLIENT_H
#define M_ALFSRVTEXTUREMANAGERCLIENT_H

#include <e32std.h>

/**
 *  Client interface for server side texture manager.
 *
 *  This interface is used by server side texture manager for 
 *  auto size textures. @c GetTextureSize is used to query what is
 *  size of texture is being/has been delivered to client. 
 *  @c SetTextureSize is used to request new size for texture.
 *  Once a sequence of @c SetTextureSize calls have finished,
 *  server side texture manager calls @c HandleAllTextureSizesInformed.
 *
 *  @since S60 v5.0
 */
class MAlfSrvTextureManagerClient
    {
public:

    /**
     * Gets texture information from client
     *
     * @param aTextureId texture id.
     * @param aTextureSize current texture size being delivered to client.
     * @param aHasBeenDelivered ETrue if texture size has already been 
     *        delivered to client, EFalse otherwise.
     * @return ETrue if information found, EFalse otherwise.
     */
    virtual TBool GetTextureSize( 
        TInt aTextureId, 
        TSize& aTextureSize,
        TBool& aHasBeenDelivered ) = 0;

    /**
     * Sets new size for texture to be delivered to client.
     * @param aTextureId texture id.
     * @param aTextureSize new texture size.
     * @param aPriority priority for texture size to be delivered.
     *        negative: deliver urgently, 
     *        0: deliver immediately, 
     *        positive: deliver as soon as possible.
     */
    virtual void SetTextureSize(
        TInt aTextureId,
        const TSize& aTextureSize,
        TInt aPriority ) = 0;

    /**
     * Informs client that texture size does not need to be delivered
     * to client side.
     * @param aTextureId texture id.
     */
    virtual void RemoveTextureSize( TInt aTextureId ) = 0;
    
    /**
     * Informs client that all texture size information has been given 
     * (for now).
     */
    virtual void TextureSizeChangesCompleted() = 0;

    };

#endif // M_ALFSRVTEXTUREMANAGERCLIENT_H
