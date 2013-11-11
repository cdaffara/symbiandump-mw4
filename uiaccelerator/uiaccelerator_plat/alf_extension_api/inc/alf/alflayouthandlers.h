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
* Description:   definitions of alf built-in layout support
*
*/



#ifndef __ALFLAYOUTHANDLERS__
#define __ALFLAYOUTHANDLERS__

#include <alf/alfvisualhandlers.h>

//----------------------------
// basic layout
//----------------------------
class CAlfLayoutHandler: public CAlfVisualHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension    

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };

//----------------------------
// anchor layout
//----------------------------
class CAlfAnchorLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfAnchorLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfAnchorLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension   

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };

//----------------------------
// anchor layout
//----------------------------

class TAknWindowLineLayout;
class TAknTextLineLayout;
class CHuiAnchorLayout;
    
class CAlfLCTAnchorLayoutHandler: public CAlfAnchorLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfLCTAnchorLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfLCTAnchorLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension

private:
    TBool IsParentRelative(TInt aVal) const;
    void SetAnchors(CHuiAnchorLayout* aAnchor, TInt aOrdinal, const TAknWindowLineLayout& aLine, const THuiTimedPoint& aTimedPoint);
    void DoSetAnchors(CHuiAnchorLayout* aAnchor, TInt aOrdinal, TInt aLeft, TInt aTop, TInt aRight, TInt aBottom, const THuiTimedPoint& aTimedPoint);

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };

//----------------------------
// grid layout
//----------------------------
class CAlfGridLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfGridLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfGridLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };
    
//----------------------------
// LCT grid layout
//----------------------------
class CAlfLCTGridLayoutHandler: public CAlfGridLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfLCTGridLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfLCTGridLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension
    
protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };
    
//----------------------------
// deck layout
//----------------------------
class CAlfDeckLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfDeckLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension


protected: // from CBase
    IMPORT_C ~CAlfDeckLayoutHandler();
    
protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };
    
//----------------------------
// flow layout
//----------------------------
class CAlfFlowLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfFlowLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfFlowLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };

//----------------------------
// curve path layout
//----------------------------
class CAlfCurvePathLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfCurvePathLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfCurvePathLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    struct TCurvePathLayoutPrivateData;
    TCurvePathLayoutPrivateData* iCurvePathLayoutPrivateData;    
    };

//----------------------------
// viewport layout
//----------------------------
class CAlfViewportLayoutHandler: public CAlfLayoutHandler
    {
public: // new methods
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    IMPORT_C CAlfViewportLayoutHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfViewportLayoutHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); public: // From MAlfExtension

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };




#endif
