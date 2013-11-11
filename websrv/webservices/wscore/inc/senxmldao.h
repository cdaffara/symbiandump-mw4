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








#ifndef SEN_XML_DAO_H
#define SEN_XML_DAO_H

//  INCLUDES
#include <flogger.h>
#include <e32std.h>

#include <SenXmlReader.h>
#include <SenBaseFragment.h>
#include "senwsdescription.h"
#include <MSenServiceDescription.h>
#include "senpointermap.h"

#include "msendao.h"

// CONSTANTS
const TInt KStateParsingFramework = 4;          // even ones ignore(0), odd ones save(1)
const TInt KStateParsingServiceDescription = 6;
const TInt KStateParsingTransport = 8;

// DATA TYPES
enum TFileOperationState
    {
    ESenIdle = 0,
    ESenReading,
    ESenSaveNeeded,
    ESenSaving
    };

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class RSenServiceManager;
class MSenCoreServiceManager;
class CSIF;
class CSenSecurityMechanism;
class CSenProvider;
class CSenHostletLookupInfo;

// TYPEDEF
typedef RSenPointerMap<TDesC8, HBufC8> RTransportMap;
typedef RPointerArray<CSenProvider> RProviderArray;
typedef RSenPointerMap<CSenHostletLookupInfo, CSenProvider> RProviderMap;

// CLASS DECLARATION
class CSenXMLDAO :  public CSenBaseFragment,
                    public MSenDAO
    {
    public: // Constructors and destructor
        static CSenXMLDAO* NewL(MSenCoreServiceManager& aManager);
        static CSenXMLDAO* NewLC(MSenCoreServiceManager& aManager);
    
        virtual ~CSenXMLDAO();

        // New functions
        
        virtual MSIF* Framework(const TDesC8& aFramework);
        virtual TInt Add(CSenWSDescription& aDescription);
        virtual TInt Add(CSIF& aFramework);
        virtual TBool Remove(CSenWSDescription& aDescription);
        virtual void Remove(CSIF& aFramework);

        /**
        * The ownership of the returned ServiceDescription is NOT transferred  
        */
        virtual CSenWSDescription* FindMatchingServiceDescriptionL( CSenWSDescription& aSd ); // codescannerwarnings

        virtual CSenWSDescription* FindMatchingSDAndBestScoreL(
                                            CSenWSDescription& aSd,
                                            TInt &aBestScore);	//CodeScannerWarnings

        virtual CSenWSDescription* FindScoreMatchingServiceDescriptionL(
                                            CSenWSDescription& aSd,
                                            TInt aBestScore);	//CodeScannerWarnings

        virtual CSenWSDescription* FindExactServiceDescriptionL( CSenWSDescription& aSd );	//CodeScannerWarnings

        virtual TInt AddServiceDescriptionToFrameworksL( CSenWSDescription& aPattern,
                                                         MSenRemoteServiceConsumer& aRemoteConsumer,
                                                         
                                                         HBufC8*& aErrorMsg );

        virtual TInt NotifyFrameworksL(const TDesC8& aFrameworkID,
                                       const TInt aEvent,
                                       TAny* aArgument);

        virtual TInt FindAllMatchingServiceDescriptions(RWSDescriptionArray& aMatches,
                                                        const TDesC8& aContract);

        virtual TInt FindAllMatchingServiceDescriptions(RWSDescriptionArray& aMatches,
                                                        MSenServiceDescription& aPattern);

        virtual TInt ContainsServiceDescriptionL(TBool& aContains,
                                                 CSenWSDescription& aPattern);

        virtual TInt SaveL(const CSenServiceSession& aServiceSession);
        virtual TInt SaveL(const CSIF& aServiceInvocationFramework);
        virtual CSenXmlReader* XMLReader();
        void Load();
        TInt ProxyPort();
        const TDesC8& ProxyHost();
        CDesC8Array& SupportedFrameworksL();

        MSIF* CSenXMLDAO::DefaultFrameworkL(const TDesC8& aFrameworkID, 
                                            const TDesC8& aCue);

        TBool Owns(CSenWSDescription* aSD);

        TPtrC8 TransportPluginCueBySchemeL(const TPtrC8& aScheme);

        virtual MSenProvider& LookupHostletForL(const TDesC8& aHostletEndpoint,
                                                 const TDesC& aReqThreadId,
                                                 const TDesC8& aReqConsumerId);

        virtual TInt ReleaseHostletL(const MSenProvider* aHostlet,
                                      const TDesC& aReqThreadId,
                                      const TDesC8& aReqConsumerId);


		/** Improved heuristics from Session DB removal / cleanups 
		*
		* It finds all matching ServiceDescription elements using the service description 
		* passed argument and add the touch attribute to all matched ServiceDescription
		* elements with current tick count value.
		* If touch attribute already there it will be replaced with new current 
		* tick count value.
		*
		* @param asd   is the new established session service description.
		* Leave codes: Symbian system-wide error codes.  
		*/
		void UpdateTouchSessionDBL(MSenServiceDescription& asd);
    protected:
    
        // New functions
        
        void StartElementL(const TDesC8& aURI,
                           const TDesC8& aLocalName,
                           const TDesC8& aName,
                           const RAttributeArray& aAttributes);

        void EndElementL(const TDesC8& aURI,
                         const TDesC8& aLocalName,
                         const TDesC8& aName);

    private:
    
        /**
        * C++ default constructor.
        */
        CSenXMLDAO(MSenCoreServiceManager& aManager);
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions
        TInt Save();
        void WriteL();
        void ReadL();
        RFileLogger* Log() const;

        TInt SizeOfSessionsXmlInBytesL();
        TInt SizeOfFrameworksXmlInBytesL();

        TInt CheckDefaultFrameworks();
        TInt CheckDefaultTransports();
        /** Improved heuristics from Session DB removal / cleanups
	    *
	    * Checks touch attribute value for all ServiceDescription elements.
	    * If that exceeds the tick counts value for two weeks (Max tick count)
	    *  or If it is greater than current tick count, it removes the corresponding 
	    * ServiceDescription element from the SenConfiguration element child list.  
	    * 
	    * 1 millisec = 1 tick
		* 86400 sec = 1 day
		* 86400*1000 millisec = 1 day
		* 86400*14*1000 = 1209600000 millisec = 2 weeks
		* So if attr val >= 1209600000 or > current tick count.
		* Note: always attr val should be <= current tick count.
		*
	    * eg;
	    * Before call to CleanupUnusedSessionDBL()
	    * <SenConfiguration xmlns="urn:com.nokia.Sen.config.1.0">
	    * ........
	    * ........
  		* <ServiceDescription framework="ID-WSF" touch="2000000000">
  		* ........
  		*.........
  		* </ServiceDescription>
		* <ServiceDescription framework="ID-WSF" touch="20000">
		* ..........
		* .......
  		* </ServiceDescription>
  		* <ServiceDescription framework="ID-WSF">
		* ..........
		* .......
  		* </ServiceDescription>
  		* </SenConfiguration>
		* 
		* After call to CleanupUnusedSessionDBL()
	    * <SenConfiguration xmlns="urn:com.nokia.Sen.config.1.0">
	    * ........
	    * ........
  		* <ServiceDescription framework="ID-WSF" touch="20000">
		* ..........
		* .......
  		* </ServiceDescription>
  		* <ServiceDescription framework="ID-WSF">
		* ..........
		* .......
  		* </ServiceDescription>
  		* </SenConfiguration>
  		* 
		* Leave codes: Symbian system-wide error codes.
	    */
        void CleanupUnusedSessionDBL();

    private: // Data
        MSenCoreServiceManager& iManager;
        TFileOperationState iReadingConfig;
        TInt iErrorState;
        TFileName iFileName;
        RPointerArray<CSenWSDescription> iSessions;
        RPointerArray<CSIF> iFrameworks;
        CSenWSDescription* iServiceDescription;
        CSenBaseFragment* iFrameworkConfigParser;
        TBool iDefaultFrameworkCheckedToExist;
        HBufC8* iProxyHost;     // owned
        TInt    iProxyPort;
        RTransportMap iTransportMap;
        TBool iDefaultTransportMapChecked;
        RProviderArray iSharableProviders;
        RProviderMap iUnsharableProviders;
        CSenDomFragment* ipTransportDelegate;
    };


/** 
* THostletRequester class is used to identify the identity
* requesting service (looking up) certain hostlet (a local
* service provider provider).
* Class instance constructs from full thread name (unique id #1)
* and unique consumer id; typically some UNR (unique id # 2).
* Equals operator is overloaded providing quick matching 
* mechanism for hostlet-requester -type searches performed within
* a pointer map.
*/
class CSenHostletLookupInfo : public CBase
    {
    public:
        /** 
        * @param aThreadIdAsFullName is unique identifier for some
        *        thread, which is requesting service from certain
        *        hostlet (local service provider).
        * @param aConsumerIdUrn is unique identifier for some
        *        service consumer, which is requesting certain
        *        hostlet (local service provider).
        */
        static CSenHostletLookupInfo* NewL(const TDesC& aThreadIdFullName,
                                          const TDesC8& aConsumerIdUrn);
        /** 
        * @param aThreadIdAsFullName is unique identifier for some
        *        thread, which is requesting service from certain
        *        hostlet (local service provider).
        * @param aConsumerIdUrn is unique identifier for some
        *        service consumer, which is requesting certain
        *        hostlet (local service provider).
        */
        static CSenHostletLookupInfo* NewLC(const TDesC& aThreadIdFullName,
                                           const TDesC8& aConsumerIdUrn);


        /** 
        * C++ destructor
        */
        ~CSenHostletLookupInfo();

        /** 
        * Getter for thread id (full thread name) of the first requester/invoker
        */
        TPtrC ThreadId() const;

        /** 
        * Getter for consumer id (typically some URN) of the first requester/invoker
        */
        TPtrC8 ConsumerId() const;

        /** 
        * Getter for current lookup count (active requesters)
        */
        TInt LookupCount() const;

        /** 
        * Increase method for lookup count (called once per lookup)
        */
        void IncrementLookupCount();

        /** 
        * Decrease method for lookup count (called once per release)
        */
        void DecrementLookupCount();
        
        /** 
        * Implement equals operation, so that pointer map can be used.
        */
        TBool operator==(const CSenHostletLookupInfo& aHostletRequestor);

    private:

        /** 
        * C++ constructor
        */
        CSenHostletLookupInfo();
        
        /** 
        * @param aThreadIdAsFullName is unique identifier for some
        *        thread, which is requesting service from certain
        *        hostlet (local service provider).
        * @param aConsumerIdUrn is unique identifier for some
        *        service consumer, which is requesting certain
        *        hostlet (local service provider).
        */
        void ConstructL(const TDesC& aThreadIdFullName,
                        const TDesC8& aConsumerIdUrn);

    private:
        HBufC* ipFullThreadName;     // owned
        HBufC8* ipUniqueConsumerId;  // owned
        TInt* ipLookupCount;           // count of currently active requesters
    };

#endif // SEN_XML_DAO_H

// End of File
