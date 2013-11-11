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
* Description:   ?Description
*
*/



#ifndef C_ALFBRUSHHANDLERS_H
#define C_ALFBRUSHHANDLERS_H

#include <alf/alfextensionfactory.h>
#include <AknsConstants.h>

class CHuiBrush;
class THuiImage;
struct TAlfMetric;
struct TAlfXYMetric;
class TAlfImage;


/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfBrushHandler : public CBase, public MAlfExtension 
    {
    
protected:

    IMPORT_C CAlfBrushHandler(MAlfInterfaceProvider& aResolver);
    
    IMPORT_C void ConstructL( CHuiBrush* aBrush );
    IMPORT_C ~CAlfBrushHandler();

    /**
     * From MAlfExtension.
     */
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

public:
    IMPORT_C void SetOwnership(TBool aHolderOwnsBrush);

protected: // new member functions 
    IMPORT_C virtual void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

protected: // data

    CHuiBrush* iBrush;
    MAlfInterfaceProvider& iResolver;

private:
    TBool iOwnsBrush;
    TAny* iSpare;    
    };


/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfBorderBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TAlfXYMetric aThickness, 
        TAlfXYMetric aEdgeOffset);

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TInt aThicknessWidth, 
        TInt aThicknessHeight,
        TInt aEdgeOffsetX, 
        TInt aEdgeOffsetY );
        
    IMPORT_C CAlfBorderBrushHandler(MAlfInterfaceProvider& aResolver);

    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TAlfXYMetric aThickness, 
        TAlfXYMetric aEdgeOffset);
    
    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TInt aThicknessWidth, 
        TInt aThicknessHeight,
        TInt aEdgeOffsetX, 
        TInt aEdgeOffsetY );

protected:

    IMPORT_C ~CAlfBorderBrushHandler();

    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;

    };



/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfImageBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver, THuiImage aImage); 
        
    IMPORT_C CAlfImageBrushHandler(MAlfInterfaceProvider& aResolver);
    
    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush, THuiImage aImage); 

protected:

    IMPORT_C ~CAlfImageBrushHandler();

// from base class MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;
    };
/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfShadowBorderBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TInt aWidth );

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TAlfMetric aWidth );
        
    IMPORT_C CAlfShadowBorderBrushHandler(MAlfInterfaceProvider& aResolver);
    
    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TInt aWidth);

    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TAlfMetric aWidth );

protected:

    IMPORT_C ~CAlfShadowBorderBrushHandler();

// from base class MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;
    };

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfDropShadowBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TAlfMetric aDepth );

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver,
        TInt aDepth );
        
    IMPORT_C CAlfDropShadowBrushHandler(MAlfInterfaceProvider& aResolver);
    
    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TAlfMetric aDepth );

    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush,
        TInt aDepth );

protected:

    IMPORT_C ~CAlfDropShadowBrushHandler();

// from base class MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;
    };
/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfGradientBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( MAlfInterfaceProvider& aResolver );
    IMPORT_C CAlfGradientBrushHandler(MAlfInterfaceProvider& aResolver);
    IMPORT_C void ConstructL( CHuiBrush* aBrush );

protected:

    IMPORT_C ~CAlfGradientBrushHandler();

// from base class MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;
    };


/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAlfFrameBrushHandler : public CAlfBrushHandler
    {
public:

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver, const TAknsItemID& aIID, TAlfXYMetric aEdgeOffset); 

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver, const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY); 

    IMPORT_C static MAlfExtension* NewL( 
        MAlfInterfaceProvider& aResolver, const THuiImage& aImage, TAlfXYMetric aEdgeOffset); 
        
    IMPORT_C CAlfFrameBrushHandler(MAlfInterfaceProvider& aResolver);
    
    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush, const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY); 

    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush, const TAknsItemID& aIID, TAlfXYMetric aEdgeOffset); 

    IMPORT_C void ConstructL( 
        CHuiBrush* aBrush, const THuiImage& aImage, TAlfXYMetric aEdgeOffset); 

protected:

    IMPORT_C ~CAlfFrameBrushHandler();

// from base class MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

    IMPORT_C void BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    TAny* iSpare;
    };

#endif // C_ALFBRUSHHANDLERS_H
