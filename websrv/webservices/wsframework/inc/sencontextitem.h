/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef SEN_CONTEXT_ITEM_H
#define SEN_CONTEXT_ITEM_H

//  INCLUDES
#include <ecom/ecom.h>
#include <e32std.h>   // for RCriticalSection

#include "senwsdescription.h"

#include "msencontextitem.h" // internal Framework\inc
#include "msencontext.h"

// FORWARD DECLARE
class MSenCoreServiceManager;
class MSenRemoteServiceConsumer;
class MSenRemoteHostlet;
class MSenWSDescription;
class CSenWSDescription;
class CSenIdentityProvider;
class CSenWSPattern;
class CSenServiceSession;
class CSenWebServiceSession;
class CSenBasicWebServiceSession;
class CIdWSFServiceSession;
class CIdWsfServiceInstance;
class CSenCoreServiceConsumer;
class CSenSenInternalServiceConsumer;
class CIdWsfAuthenticationServiceClient;
class CIdWsfDiscoveryServiceClient;
class CSenRestServiceSession;
class CWSStarServiceSession;
class CWSStarSTSClient;

class CSenChunk;
class CSenElement;
class CSenMessageBase;
class CSenSoapEnvelope2;
class MSenMessage;
class CSenSoapEnvelope;
class CSenAtomEntry;
class CSenSoapMessage;
class CSenXmlReader;
class MSenProperties;
class CSenXmlProperties;
class CSenTransportProperties;
class CSenHttpTransportProperties;
class CSenVtcpTransportProperties;
class CSenLayeredXmlProperties;
class CSenLayeredTransportProperties;
class CSenLayeredHttpTransportProperties;
class CSenLayeredVtcpTransportProperties;

using namespace SenContext;

// CLASS DECLARATION

/**
* Class for wrapping objects so that they can be moved in 
* and out of context a context using 
* 
* Sub-classing actual Service Providers have to implement MSenContextItem
* interface.
*
*/       
class CSenContextItem : public CBase, public MSenContextItem
    {
    public: 
    
        // Constructors and destructor
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETAny
        * @param apValue is the data to be stored
        * @param aIsOwnedOrNot determines whether the value is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(TAny* apNotOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETAny
        * @param apOwned is owned 
        * @param aIsOwnedOrNot determines whether the value is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(TAny* apNotOwned);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ETDesC8
        * @param aRefToValue is copied (alloc)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(const TDesC8& aRefToValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETDesC8
        * @param aRefToValue is copied (alloc)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(const TDesC8& aRefToValue);
        
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETDesC8
        * @param apOwned is owned 
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(TDesC8* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETDesC8
        * @param apOwned is owned 
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(TDesC8* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ETInt
        * @param apValue is copied (alloc)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(const TInt apValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETInt
        * @param apValue is copied (alloc)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(const TInt apValue);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ETInt
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(TInt* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETInt
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(TInt* apOwned);
        

        /**
        * Two-phased constructor, data type is TContextItemDataType::ETUint
        * @param aValue is copied (alloc)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(const TUint aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETUint
        * @param aValue is copied (alloc)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(const TUint aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ETUint
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(TUint* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ETUint
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(TUint* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenCoreServiceManager
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(MSenCoreServiceManager& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenCoreServiceManager
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(MSenCoreServiceManager& aValue);




        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenRemoteServiceConsumer
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(MSenRemoteServiceConsumer& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenRemoteServiceConsumer
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(MSenRemoteServiceConsumer& aValue);


        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenRemoteHostlet
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(MSenRemoteHostlet& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenRemoteHostlet
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(MSenRemoteHostlet& aValue);
        
        
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenWSDescription
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenWSDescription& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenWSDescription
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenWSDescription& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenServiceDescription
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenWSDescription* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenServiceDescription
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenWSDescription* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenChunk
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenChunk& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenChunk
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenChunk& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenChunk
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenChunk* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenChunk
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenChunk* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenElement& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenElement& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenElement* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenElement* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenXmlReader& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlReader
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenXmlReader& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlReader
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenXmlReader* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlReader
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenXmlReader* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(MSenProperties& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(MSenProperties& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenXmlProperties* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenXmlProperties* apOwned);

        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenLayeredXmlProperties* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenProperties
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenLayeredXmlProperties* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenMessage
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(MSenMessage& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::EMSenMessage
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(MSenMessage& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenSoapEnvelope2
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenSoapEnvelope2* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenSoapEnvelope2
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenSoapEnvelope2* apOwned);

        /**
        * Two-phased constructor, data type is TContextItemDataType::CSenAtomEntry
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenAtomEntry* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::CSenAtomEntry
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewL(CSenAtomEntry* apOwned);
        
        
        
        
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenMessageBase
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenMessageBase* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenMessageBase
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenMessageBase* apOwned);


        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param aValue is referenced (not owned)
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenSoapMessage& aValue);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param aValue is referenced owned (not owned)
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenSoapMessage& aValue);

        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param apOwned is owned
        * @return pointer to new context item
        */
        IMPORT_C static CSenContextItem* NewL(CSenSoapMessage* apOwned);
        /**
        * Two-phased constructor, data type is TContextItemDataType::ECSenXmlElement
        * @param apOwned is owned
        * @return pointer to new context item
        * which is also left on cleanup stack.
        */
        IMPORT_C static CSenContextItem* NewLC(CSenSoapMessage* apOwned);


        // From MSenContextItem:

        /*
        * Method for resolving real C-class type (implemenation class).
        * Inherited from MSenContextItem.
        * @return the type of this class (EContextItem)
        */
        virtual MSenContextItem::TClass Type() const;            


        // New methods:

        /** 
        * 
        * Returns the pointer to the data that is wrapped inside this item.
        * @return pointer to wrapped data, but only if this context item 
        * was constructed with TAny -argument. In case that this item knows
        * the data type of the object (!=TAny), method will leave.
        * Supported data type(s):
        *       TAny (root) -- TContextItemDataType::ETAny
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual TAny* ToAnyL() const;

        /** 
        * Returns the 8-bit descriptor that is wrapped inside this item.
        * @return 8-bit descriptor, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       TDesC8 (root) -- TContextItemDataType::ETDesC8
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual TPtrC8 ToPtrC8L() const;

            
        /** 
        * Returns the 8-bit descriptor that is wrapped inside this item.
        * @return 8-bit descriptor, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       TDesC8 (root) -- TContextItemDataType::ETDesC8
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual const TDesC8* ToDesC8L() const;
        
        /** 
        * Returns the signed integer descriptor that is wrapped inside this item.
        * @return integer, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       TInt -- TContextItemDataType::ETInt
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual const TInt* ToIntL() const;

        /** 
        * Returns the unsigned integer descriptor that is wrapped inside this item.
        * @return integer, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       TUint -- TContextItemDataType::ETUint
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual const TUint* ToUintL() const;
        
        /** 
        * Returns MSenCoreServiceManager that is wrapped inside this item.
        * @return core, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       MSenCoreServiceManager -- TContextItemDataType::EMSenCoreServiceManager
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual MSenCoreServiceManager& ToSenCoreL() const;

        /** 
        * Returns MSenRemoteServiceConsumer that is wrapped inside this item.
        * @return consumer, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       MSenRemoteServiceConsumer -- TContextItemDataType::EMSenRemoteServiceConsumer
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual MSenRemoteServiceConsumer& ToSenRemoteConsumerL() const;

        /** 
        * Returns MSenRemoteHostlet that is wrapped inside this item.
        * @return hostlet, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       MSenRemoteHostlet (root) -- TContextItemDataType::EMSenRemoteHostlet
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual MSenRemoteHostlet& ToSenRemoteHostletL() const;

        /** 
        * Returns CSenWSDescription that is wrapped inside this item.
        * @return XML service description, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenWSDescription (root) -- TContextItemDataType::EMSenServiceDescription
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenWSDescription& ToSenServiceDescriptionL() const;

        /** 
        * Returns CSenServiceSession that is wrapped inside this item.
        * @return session, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenServiceSession -- TContextItemDataType::EMSenServiceDescription
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenServiceSession& ToSenServiceSessionL() const;

        /** 
        * Returns properties object that is wrapped inside this item.
        * @return XML properties, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenXmlProperties (root) -- TContextItemDataType::EMSenProperties
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenXmlProperties& ToSenXmlPropertiesL() const;

        /** 
        * Returns layered properties object that is wrapped inside this item.
        * @return layered XML properties, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenLayeredXmlProperties (root) -- TContextItemDataType::EMSenProperties
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenLayeredXmlProperties& ToSenLayeredXmlPropertiesL() const;

        /** 
        * Returns the XML element that is wrapped inside this item.
        * @return XML element, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenElement (root) -- TContextItemDataType::ECSenElement
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenElement& ToSenElementL() const;

        /** 
        * Returns the SOAP message that is wrapped inside this item.
        * @return SOAP message, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenElement (root) -- TContextItemDataType::ECSenSoapEnvelope
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenSoapMessage& ToSenSoapMessageL() const;

        /** 
        * Returns the message that is wrapped inside this item.
        * @return message, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenElement (root) -- TContextItemDataType::EMSenMessage
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual MSenMessage& ToSenMessageL() const;


        /** 
        * Returns the chunk that is wrapped inside this item.
        * @return message, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenElement (root) -- TContextItemDataType::ECSenChunk
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenChunk& ToSenChunkL() const;

        /** 
        * Returns the XML element that is wrapped inside this item.
        * @return XML element, but only if this context item was constructed
        * to hold such data. In case that this item holds some object of different
        * type instead, the method will leave.
        * Supported data type(s):
        *       CSenXmlReader -- TContextItemDataType::ECSenXmlReader
        * Leave codes:
        *       KErrSenContextItemCastMismatch if owned/referenced data type mismatches
        */
        virtual CSenXmlReader& ToSenXmlReaderL() const;

        /**
        * Function:    ~CSenContextItem
        *
        * Description: Destroy the object
        */
        IMPORT_C virtual ~CSenContextItem();

    private:

        CSenContextItem(TAny* apData, MSenContextItem::TData aType, TInt* apFlags);


    private: // Data

        // This pointer holds the actual data wrapped by this context item:´
        TAny* ipData;
        // This member specifies the item class-type:
        MSenContextItem::TData iType;
        // This pointer holds information about data; whether it is owned or not, etc.
        TInt* ipFlags;
    };
#endif // SEN_CONTEXT_ITEM_H

// End of File
