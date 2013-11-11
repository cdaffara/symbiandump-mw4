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
* Description:   definitions of alf built-in visual support
*
*/



#ifndef __ALFVISUALHANDLERS__
#define __ALFVISUALHANDLERS__

#include <alf/alfextensionfactory.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiMesh.h>

/**
 * Handler for the visual base class.
 */
class CAlfVisualHandler: public CBase, public MAlfExtension
    {
public: // new methods

    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    
    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler. 
     */
    IMPORT_C CAlfVisualHandler(MAlfInterfaceProvider& aResolver);
    
    /**
     * Second phase constructor.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */    
    IMPORT_C virtual void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfVisualHandler();

public: // From MAlfExtension
    /**
     * Release this visual handler.
     */
    IMPORT_C void Release();
    
    /**
     * Gets the hui visual handled by this class.
     * @param aInterface Return parameter containing the visual.
     */
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    
    /**
     * Handle a command for this visual that has come from the client.
     * @param aCommandId The TAlfredServerIPC enum specifying what command to execute.
     * @param aInputBuffer The input parameters for this command, will be cast to the correct types internally.
     * @param aResponse The return value for this command. Will be cast to correct type at the client side.
     * @see TAlfredServerIPC
     */
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); 
    
    // @internal
    void HuiVisualAboutToTerminate();

protected: // new methods
    IMPORT_C virtual void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

protected: // data
    CHuiVisual* iVisual; // not owned
    MAlfInterfaceProvider& iResolver; // not owned

private:
    
    struct TPrivateData;
    TPrivateData* iPrivateData;    
    };

/**
 * Handler for TextVisual.
 */
class CAlfTextVisualHandler: public CAlfVisualHandler
    {
public: // new methods
    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    
    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler. 
     */    
    IMPORT_C CAlfTextVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Second phase constructor.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfTextVisualHandler();

public: // From MAlfExtension
    /**
     * Release this visual handler.
     */
    IMPORT_C void Release();
    
    /**
     * Gets the hui visual handled by this class.
     * @param aInterface Return parameter containing the visual.
     */    
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);

    /**
     * Handle a command for this visual that has come from the client.
     * @param aCommandId The TAlfredServerIPC enum specifying what command to execute.
     * @param aInputBuffer The input parameters for this command, will be cast to the correct types internally.
     * @param aResponse The return value for this command. Will be cast to correct type at the client side.
     * @see TAlfredServerIPC
     */    
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);    

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };

//----------------------------
// LCTTextVisual
//----------------------------

class CHuiTextVisual;
class TAknTextLineLayout;

class CAlfLCTTextVisualHandler: public CAlfTextVisualHandler
    {
public: // new methods
    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);
    
    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler. 
     */        
    IMPORT_C CAlfLCTTextVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Second phase constructor.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfLCTTextVisualHandler();

public: // From MAlfExtension
    /**
     * Release this visual handler.
     */
    IMPORT_C void Release();
    
    /**
     * Gets the hui visual handled by this class.
     * @param aInterface Return parameter containing the visual.
     */    
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);

    /**
     * Handle a command for this visual that has come from the client.
     * @param aCommandId The TAlfredServerIPC enum specifying what command to execute.
     * @param aInputBuffer The input parameters for this command, will be cast to the correct types internally.
     * @param aResponse The return value for this command. Will be cast to correct type at the client side.
     * @see TAlfredServerIPC
     */    
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); 

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    void SetTextPaneL(CHuiTextVisual* aTextVisual, const TAknTextLineLayout& aLine);

private:
    class TLCTTextVisualPrivateData;
    TLCTTextVisualPrivateData* iLCTTextVisualPrivateData;
    };

//----------------------------
// ImageVisual
//----------------------------
class CAlfImageVisualHandler: public CAlfVisualHandler
    {
public: // new methods

    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);

    /**
     * Constructor.
     * @param aResolver The resolver for this visual handler. 
     */            
    IMPORT_C CAlfImageVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Second phase constructor.
     * @param aOwner The owner control of this visual.
     * @param aParentLayout The parent layout of this visual.
     */
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfImageVisualHandler();

public: // From MAlfExtension
    /**
     * Release this visual handler.
     */
    IMPORT_C void Release();
    
    /**
     * Gets the hui visual handled by this class.
     * @param aInterface Return parameter containing the visual.
     */    
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    
    /**
     * Handle a command for this visual that has come from the client.
     * @param aCommandId The TAlfredServerIPC enum specifying what command to execute.
     * @param aInputBuffer The input parameters for this command, will be cast to the correct types internally.
     * @param aResponse The return value for this command. Will be cast to correct type at the client side.
     * @see TAlfredServerIPC
     */        
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);     

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    TAny* iSpare;
    };
    
//----------------------------
// Line Visual
//----------------------------
class CAlfLineVisualHandler: public CAlfVisualHandler
    {
public: // new methods

    /**
     * Constructor.
     * @see CAlfVisualHandler::NewL()
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);

    /**
     * Constructor.
     * @see CAlfVisualHandler::CAlfVisualHandler()
     */    
    IMPORT_C CAlfLineVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Constructor.
     * @see CAlfVisualHandler::ConstructL()
     */    
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfLineVisualHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); 

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    class TLineVisualHandlerPrivateData;
    TLineVisualHandlerPrivateData* iLineVisualPrivateData;    
    };
    
//----------------------------
// Mesh Visual
//----------------------------
class CAlfMeshVisualHandler: public CAlfVisualHandler
    {
public: // new methods

    /**
     * Constructor.
     * @see CAlfVisualHandler::NewL()
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, THuiMeshType aMeshType, CHuiLayout* aParentLayout);
    
    /**
     * Constructor.
     * @see CAlfVisualHandler::CAlfVisualHandler()
     */        
    IMPORT_C CAlfMeshVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Constructor.
     * @see CAlfVisualHandler::ConstructL()
     */    
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, THuiMeshType aMeshType, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfMeshVisualHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    
    /**
     * Get interface.
     * @see CAlfVisualHandler::GetInterface()
     */        
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    
    /**
     * Handle command from client.
     * @see CAlfVisualHandler::HandleCmdL()
     */       
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); 

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    class TMeshVisualHandlerPrivateData;
    TMeshVisualHandlerPrivateData* iMeshVisualPrivateData;    
    };

//----------------------------
// Canvas Visual
//----------------------------
class CAlfCanvasVisualHandler: public CAlfVisualHandler
    {
public: // new methods

    /**
     * Constructor.
     * @see CAlfVisualHandler::NewL()
     */
    IMPORT_C static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);

    /**
     * Constructor.
     * @see CAlfVisualHandler::CAlfVisualHandler()
     */    
    IMPORT_C CAlfCanvasVisualHandler(MAlfInterfaceProvider& aResolver);

public: // from CAlfVisualHandler
    /**
     * Constructor.
     * @see CAlfVisualHandler::ConstructL()
     */    
    IMPORT_C void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

protected: // from CBase
    IMPORT_C ~CAlfCanvasVisualHandler();

public: // From MAlfExtension
    IMPORT_C void Release();
    IMPORT_C TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    IMPORT_C void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse); 

protected: // From CAlfVisualHandler
    IMPORT_C void VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:
    class TCanvasVisualHandlerPrivateData;
    TCanvasVisualHandlerPrivateData* iLineVisualPrivateData;    
    };

#endif

