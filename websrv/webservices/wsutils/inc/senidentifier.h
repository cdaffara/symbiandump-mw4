/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef C_SEN_IDENTIFIER_H
#define C_SEN_IDENTIFIER_H

// INCLUDES

#include <e32base.h>
#include <flogger.h>
#include <ssl.h>

#include "senattributes.h"

// CONSTANTS
namespace SenIdentifier
    {
    enum TIdentifierType
        {
        EUnknown = 0,
        EEndpoint, // URL
        ECertInfo, // (Server) certificate info object - unique ID of X509 cert
        ESession,  // Symbian server session
        ELocalApp, // Symbian App GUID
        EProvider, // i.e., Liberty ProviderID
        ERole,     // Role ID (e.g., in form of SAML NameIdentifier)
        EIdentity, // Subject ID, e.g., SAML NameIdentifier
        EThread,   // Thread ID
        EProcess   // Process ID
        };
    }

// CLASS DEFINITIONS

/**
 * An Identifier is used to address (point to) a Principal.
 * The system uses a variety of actual types to identify different
 * parties. For example an URL endpoint can serve as an identifier
 * for a remote service; a (G)UID may identitfy a local application, and
 * an XML document or SAML NameIdentifier may identify a person.
 * 
 * NOTE: This is just like a java.security.Principal
 * and could potentially be extended to supply that kind of interface
 * to requesters
 * 
 * How are Identifiers related? One way is via the Proxy mechanism - but
 * an attribute (ie. SAML attribute is just an Identifier with a different 
 * name, and possibly multiple values)
 * 
 * Identifiers are useful for policy related to a Subject, but can also be 
 * used to index a session with a consumer application
 *  
 * <p>
 * (c) Copyright Nokia 2004, 2005
 * <p>
 */
class CSenIdentifier : public CBase
    {
    /*
    public final static CSenIdentifier NULL = new Identifier();
    */

    public: // Constructors and Destructors

        IMPORT_C static CSenIdentifier* NewL();

        IMPORT_C static CSenIdentifier* NewLC();

        IMPORT_C static CSenIdentifier* NewL(const CSenIdentifier& aParent);

        IMPORT_C static CSenIdentifier* NewLC(const CSenIdentifier& aParent);

        IMPORT_C static CSenIdentifier* NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                             const TDesC8& aIdentifierValue);

        IMPORT_C static CSenIdentifier* NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TDesC8& aIdentifierValue);


        IMPORT_C static CSenIdentifier* NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                             const TDesC8& aIdentifierValue, 
                                             const TDesC8& aFriendlyName);

        IMPORT_C static CSenIdentifier* NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TDesC8& aIdentifierValue, 
                                              const TDesC8& aFriendlyName);


        /**
         * 
         * Constructor taking TCertInfo type as value input, which will be
         * converted into string representation (TDesC8) where fields are 
         * delimited by KSenCASNDelimiter.
         * 
         * @since Series60 5.0
         * 
         * @param aIdentifierType The identifier type value, which typically would be ECertInfo.
         * @param aCertInfo The certificate info object that provides a unique
         * identification of a certificate.
         * @return a pointer to new CSenIdentifier instance
         * 
         */
        IMPORT_C static CSenIdentifier* NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                             const TCertInfo& aCertInfo);

        /**
         * 
         * Constructor taking TCertInfo type as value input, which will be
         * converted into string representation (TDesC8) where fields are 
         * delimited by KSenCASNDelimiter.
         * 
         * @since Series60 5.0
         * 
         * @param aIdentifierType The identifier type value, which typically would be ECertInfo.
         * @param aCertInfo The certificate info object that provides a unique
         * identification of a certificate.
         * @return  new CSenIdentifier instance pointer, which has been left on cleanup stack.
         * 
         */
        IMPORT_C static CSenIdentifier* NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TCertInfo& aCertInfo);


        IMPORT_C virtual ~CSenIdentifier();

        /**
        * Getter for Identifier value
        * @return the value of this identifier
        */
        IMPORT_C TPtrC8 Value() const;


        /**
        * Getter for Identifier type
        * @return TIdentifierType enumeration (type) of this identifier
        */
        IMPORT_C SenIdentifier::TIdentifierType Type() const;


        /**
         * Equality operation for two identifiers
         */
        IMPORT_C TBool Equals(const CSenIdentifier& otherIdentifier);

        /** 
         * This identifier is saying that it is acting as a proxy for the given (param) 
         * identifier. An example of proxying is if the PAOS filter or another piece of
         * local software acts on behalf of some remote network entity, forwarding a 
         * request message to Core Service Manager (and then onto the service instance).
         *
         * @param aIdentifier identifier of which self is a proxy for. That is,
         * aIdentifier becomes parent of self. Method takes ownership of aIdentifier.
         */
        IMPORT_C void ProxyFor( CSenIdentifier* aIdentifier );

        /**
         * Is the Identifier acting as the proxy for another Identifier?
         * 
         * @return TBool - indicating that the Identifier in question is 
         * acting as a proxy for some other Identifier
         * 
         */
        IMPORT_C TBool IsProxy() const;

        /**
         * Getter for parent identifier (ie. that which this identifier 
         * is proxying for. 
         * 
         * @return the identifier that is being proxied for, or NULL,
         *         if this identifier does not act as a proxy to any 
         *         other.
         */
        IMPORT_C const CSenIdentifier* Proxy() const;


        /**
         * Getter for top level proxy (parent) of the identifier chain.
         * If this identifier is not proxy for any other, then this
         * method returns itself.
         * @return pointer to root identifier or if this identifier is
         *         not a proxy, the pointer to this identifier itself.
         */
        IMPORT_C const CSenIdentifier* Root() const;


        /**
         * Add an Attribute statement to this Identifier. An attribute might say something like
         * "this Identifier was authenticated at such and such a time" or "this Identifier is a member
         * of group YYY"
         * 
         * Note: It is possible that multiple attributes with the same name, but different values
         * could have been added to the list. Currently there is no check on this in the Attribute
         * or Identifier code.
         * 
         * 
         * @param aAttribute
         * @see com.nokia.serene.utils.Attribute
         */
        IMPORT_C void AddAttributeL( CSenAttribute& aAttribute );	//codescannerwarnings


        /**
         * Get a reference to the attributes associated with this Identifier
         * 
         *
         * @since Series60 5.0
         * 
         * @param       none
         *
         * @return      CSenAttributes
         */
        IMPORT_C const CSenAttributes* Attributes() const;


        /**
        * Tests whether or not two CSenIdentifier instances are equal.
        * @since Series60 5.0
        * @param aIdentifier is the CSenIdentifier to be compared with this
        * @return   True if the two CSenIdentifier are equal. False if not.
        */
        IMPORT_C TBool operator==(const CSenIdentifier& aIdentifier);

    protected:



        void ConstructL(SenIdentifier::TIdentifierType aIdentifierType, 
                        const TDesC8& aIdentifierValue, 
                        const TDesC8& aFriendlyName);


        void ConstructL(const CSenIdentifier& aParent);


    private:

        CSenIdentifier();

        /**
        * Getter for parent identifier (which is being proxied by
        * this identifier, or if this identifier does not act as
        * proxy, it returns it self.
        * @return parent identifier. Unlike Proxy(), this method
        *         returns a reference to itself, if it does not
        *         have a parent.
        */
        const CSenIdentifier& Parent() const;

        /**
        * Getter for statement attribute list.
        * @return a list of attributes that containt statements of
        *         this identifier.
        */
        const CSenAttributes& Statements() const;


    protected: // Data

        SenIdentifier::TIdentifierType iType;
        HBufC8* ipValue; // owned

    private: // Data
        HBufC8* ipFriendlyName; // ownedd
        CSenIdentifier* ipParent; // not owned
        CSenAttributes* ipStatements; // owned


    };

#endif // C_SEN_IDENTIFIER_H

