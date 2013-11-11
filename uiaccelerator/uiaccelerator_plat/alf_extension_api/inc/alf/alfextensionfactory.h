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
* Description:   interface for creating custom extensions
*
*/



#ifndef __M_ALF_EXTENSION_FACTORY__
#define __M_ALF_EXTENSION_FACTORY__

#include <e32base.h>

class CAlfLayoutManager;
class CHuiEnv;

// binary
enum THuiInterfaceSupport
    {                                   // Cast to:
    EHuiObjectTypeVisual = 0x0001,      // CHuiVisual
    EHuiObjectTypeLayout = 0x0002,      // CHuiLayout   
    EHuiObjectTypeControl = 0x0004 ,    // CHuiControl
    EHuiObjectTypeControlGroup = 0x0008,// CHuiControlGroup
    EHuiObjectTypeBrush = 0x0010,       // CHuiBrush 
    EHuiObjectTypeDisplay = 0x0020,     // CHuiDisplay
    EHuiObjectTypeCustom = 0x0040,      // For extension to decide
    EHuiObjectTypeCurvePathProvider = 0x0080, // Implemented by object which shares curve path
    
    EHuiInterfaceMappingFunction = 0x00010000, // MHuiMappingFunction
    EHuiInterfaceVisualOwner = 0x00020000,      // MHuiVisualOwner
    EAlfBrushHandler = 0x00040000,  // CAlfBrushHandler, needed for controlling brush ownership
    EAlfTextStyleHandler = 0x00080000,	// TAlfTextStyleHandler, needed for mapping client and server side text styles    
    EAlfVisualHandler = 0x00100000	// CAlfVisualHandler
    };



class MAlfInterfaceProvider
    {
public:
    /**
    * Request to pointer of given type matching to given handle. 
    * Leaves if object not found i.e. always returns pointer on completion
    */
    virtual TAny* GetInterfaceL(const THuiInterfaceSupport& aType, TInt aHandle) = 0;

    /**
    * Request identifier for pointer of given type 
    * On error returns KErrNotFound
    */
    virtual TInt GetHandleFromInterface(const THuiInterfaceSupport& aType, TAny* aInterface) = 0;

    /**
    * Returns common layoutmanager or null
    */
    virtual CAlfLayoutManager* LayoutManager() const = 0;

    /**
    * Returns common hitchcock environment or null
    */
    virtual CHuiEnv* SharedHuiEnv() const = 0;
    
    /**
    * Request to make current command (during MAlfExtension::HandleCmd) asynchronous
    * returns command id that is needed for completing the command later on
    *
    * exntension should take references to buffers (in/out) in case it wants to modify client message
    * before completing it
    */
    virtual TInt HandleCurrentCommanndAsynch() = 0;

    /**
    * Complete asynch command, if MAlfExtension implementing object utilizes asynch commands
    * it should also take care that those messages are completed at latest when the extension is being 
    * destoryed. FW will panic the client in case of orphean messages
    */    
    virtual void CompleteCmd(TInt aCommadIdentifier, TInt aResult = KErrNone) = 0;

    /** ! Future proofing */
    virtual void AlfInterfaceProviderExtension(const TUid& aExtensionUid, TAny** aExtensionParams) = 0;
    };


class MAlfExtension
    {
    public:
        /**
        * Free resources for instance, usually delete this
        */
        virtual void Release() = 0;

        /**
        * Returns pointer of correct type or null
        * @param aInterface     type of interface requested
        * @return               Pointer or null depending whether interface is implemented 
        */
        virtual TAny* GetInterface(const THuiInterfaceSupport& aInterface) = 0;

        /**
        * Handles command from controller
        * @param aCommandId     Command identifier, values below EAlfLastReservedOpCmd (= 10000)
        *                       are reserved for tookit fw and should not be utilized in app 
        *                       specific extensions.
        * @param aInputBuffer   Additional data attached to command
        * @param aResponse      Response to be passed back to commander
        * @leave                Any system wide error code
        */
        virtual void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse) = 0;

    };


class MAlfExtensionFactory
    {
    public:
        /**
        * Creates new instace of required type (unique for factory)
        * Must not take ownership of created object
        * 
        * @param aObjectId      Object type required
        * @param aInputBuffer   Additional data attached to object construction
        * @param aResolver      Reference to Interface resolver 
        * @leave                Any system wide error code
        */
        virtual MAlfExtension* CreateExtensionL(const TInt aObjectId, const TDesC8& aInitialParams, MAlfInterfaceProvider& aResolver) = 0;

        /**
        * Free resources for instance, usually delete this
        */
        virtual void Release() = 0; 

    };

#endif
