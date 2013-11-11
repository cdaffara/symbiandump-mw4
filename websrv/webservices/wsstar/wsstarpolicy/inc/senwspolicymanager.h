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








#ifndef SEN_WS_POLICY_MANAGER_H
#define SEN_WS_POLICY_MANAGER_H

// INCLUDES
#include "msenwspolicymanager.h"

#include <e32std.h>
#include <flogger.h>    // RFileLogger

#include <SenFragmentBase.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>

// FORWARD DECLARATIONS
class MSenCoreServiceManager;
class CSenWSPolicyIdentifier;
class CSenWSPolicyProperties;
class CSenParser;
class CSenWSPolicyLoader;
class CNormalizedPolicyConatiner;
class CExpiredPolicy;
class CConsumerContainer;
// CLASS DECLARATION
typedef RPointerArray<CNormalizedPolicyConatiner> RNormalizedArray;
typedef RPointerArray<CExpiredPolicy> RExpiredPolicyArray;
typedef RPointerArray<CConsumerContainer> RPolicyConsumerArray;

class CSenWSPolicyManager: public CBase , public MSenWsPolicyManager
    {
    public:
        IMPORT_C static CSenWSPolicyManager* NewL(MSenCoreServiceManager& aManager);

        IMPORT_C static CSenWSPolicyManager* NewLC(MSenCoreServiceManager& aManager);

        IMPORT_C virtual ~CSenWSPolicyManager();

        IMPORT_C TInt RegisterWsPolicyL(CSenWSDescription* aPattern,
                                                            TInt& aErrorTo);
        
        IMPORT_C TInt UnRegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);
                                                                
        //add a new Policy to existing SD whose endpoints are already registered by Register
        IMPORT_C TInt AddWsPolicyL(CSenWSDescription* aPattern, const TDesC8& aUri, 
                                               const TDesC8& aPolicy, TInt& aErrorTo);
                                                        
        //remove a policy from existing SD. The Metadataendpoint will not be removed
        IMPORT_C TInt RemoveWsPolicyL( CSenWSDescription* aPattern, 
                                                        const TDesC8& aUri, TInt& aErrorTo);    


        IMPORT_C CSenElement*  WsPolicyL( CSenWSDescription* aPattern, 
                                                        const TDesC8& aUri, TInt& aErrorTo);

        IMPORT_C TInt ExpiredPoliciesL(RExpiredPolicyArray& aArray,  TInt& aErrorTo);
        
        IMPORT_C TInt AddPolicyConsumerL(const TDesC8& aUri, const TDesC8& aConsumerId);
        
        IMPORT_C TInt RemovePolicyConsumerL(const TDesC8& aUri, const TDesC8& aConsumerId);
        


    //from MSenWsPolicyManager
        virtual TInt AddWsPolicy1L(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo);

        virtual TInt RemoveWsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);
                                                                                                                                                                                               
        virtual TInt RegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);

        virtual TInt UnRegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);
                                                    
        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);

        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                TInt& aErrorTo);

        virtual CSenElement*  WsPolicy1L( const TDesC8& aProviderId, TInt& aErrorTo);
        
        virtual TInt AddPolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId);

        virtual TInt RemovePolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId);


    private:

        /**
        * C++ default constructor.
        */
        CSenWSPolicyManager(MSenCoreServiceManager& aManager);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        CSenElement* PolicyFromUri(CSenWSDescription* aPolicy, const TDesC8& aUri);
        CSenElement* PolicyFromSDL(CSenWSDescription* aPolicy);  //codescannerwarnings
        CSenElement* PolicyByProviderId(const TDesC8& aProviderId, TInt&  aErrorTo);
        TInt IsContainerL(CSenInternalWsPolicy* aInternalPolicy);	//codescannerwarnings
        TInt CreatePolicyContainerL(CSenInternalWsPolicy* aInternalPolicy);  //codescannerwarnings
        TInt RefreshPolicyContainerL();  //codescannerwarnings
        TInt UpdatePolicyContainerL(CSenInternalWsPolicy* aInternalPolicy);
        TInt PolicyConsumerCountL(CSenInternalWsPolicy* aInternalPolicy); //codescannerwarnings
        TInt IsConsumerExist(const TDesC8& aEndpoint, const TDesC8& aConsumerId);
        RFileLogger* Log();
    private:
        MSenCoreServiceManager&     iManager;
        RFileLogger*                iLog;                   // not owned (no create/close)    
        CSenWSPolicyLoader* iPolicDataLoader;
        RNormalizedArray iPolicyArray;
        RPolicyConsumerArray iConsumerArray;
    
    };
//Policy container holding the Normalized Policy
class CNormalizedPolicyConatiner : public CBase
    {
    public:
        static CNormalizedPolicyConatiner* NewL(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri);

        static CNormalizedPolicyConatiner* NewLC(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri);

        virtual ~CNormalizedPolicyConatiner();
        
        TBool IsSameContainer(const TDesC8& aUri);
        TBool UpdateL(CSenInternalWsPolicy* aInternalPolicy);
        
        void CountInc();
        void CountDec();
        void SetConsumerCount(TInt aCount);
        TInt ConsumerCount();
        TPtrC8  Contract();
        TPtrC8  Endpoint();
        TPtrC8  ProviderId();        
        TPtrC8  URI();
        TTime  ValidUntil();
        CSenElement* Policy();
        
    private:

        /**
        * C++ default constructor.
        */
        CNormalizedPolicyConatiner();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri);    
    
    private:
        HBufC8* iContract;
        HBufC8* iEndpoint;
        HBufC8* iProviderId;
        HBufC8* iUri; //contents
        TTime iValidUntil; //contents
        TInt    iCount;
        CSenElement* iPolicy;    
    };
//for loading and saving the DB    
class CSenWSPolicyLoader : public CSenBaseFragment 
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenWSPolicyLoader* NewL(MSenCoreServiceManager& aManager);

        static CSenWSPolicyLoader* NewLC(MSenCoreServiceManager& aManager);

        virtual ~CSenWSPolicyLoader();

        /**
        * Load and parse Policies from the file.
        * Statuscodes:
        *   KErrNone                Ok
        *  Other codes are system error codes.
        * @return status code.
        */
        TInt LoadDBL();	//codescannerwarnings

        /**
        * Save Policies as an XML document into
        * the file.
        * Statuscodes:
        *   KErrNone                Ok
        *  Other codes are system error codes.
        * @return status code.
        */
        TInt SaveDB();
        TInt MergeToL();
        // From CSenBaseFragment
        /**
        * Callback function which implements the XML content handler interface.
        * @since Series60 3.0
        * @param aNsURI     The namespace URI of the new element
        * @param aLocalName The local name of the new element
        * @param aQName     The qualified name of the new element
        * @param aAttributes    The attributes of the new element
        */
        virtual void StartElementL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);

        // From CSenBaseFragment 
        /**
        * Callback function which implement the XML content handler interface.
        * @since Series60 3.0
        * @param aNsUri     The namespace URI of the new element
        * @param aLocalName The local name of the new element
        * @param aQName     The qualified name of the new element
        */
        virtual void EndElementL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                      const TDesC8& aQName);

//new Methods  

		RSenWSPolicyPtr AddWsPolicyL(CSenInternalWsPolicy* apPolicy,
		                                          TInt& aErrorTo);
                                                  
        TInt RegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);
        
                                                    
        
        TInt AddWsPolicyL(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo);


        TInt RemoveWsPolicyL( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);    

        CSenElement*  WsPolicyL( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);
                                                

        TInt InternalWsPoliciesL(RSenWSPolicyArray& aPolicies);
        TInt UnRegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);
                                                       

	protected:
	    RFileLogger* Log();

    private:

        /**
        * C++ default constructor.
        */
        CSenWSPolicyLoader(MSenCoreServiceManager& aManager);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions

        /**
        * Initialize this SenPolicyManager by parsing
        * the given file with the given XMLReader.
        * The file is expected to contain valid XML that
        * follows the example given above.
        * @param file
        * @param reader
        */
        void LoadFromL(const TDesC& aFile);

        /**
        * Save the current state of the SenPolicyManager as an XML
        * document into a file.
        * Status codes:
        *   KErrNone                Ok
        *   Other codes are system error codes.
        * @return status code.
        */
        TInt SaveToL( const TDesC& aFile );

	    virtual TInt NextId();
	    
	    virtual void UpdateMaxId();
	    CSenElement* CreatePolicyElementL(const TDesC8& aUri, const TDesC8& aPolicy);
	    CSenElement* GetExpiryTagL(CSenElement& aPolicy);
	    TInt EvaluateExpiryL(CSenElement* aExpiry, TTime& aExpiryTime);
	    TInt ProcessNewPolicyL( const TDesC8& aUri, CSenElement& aPolicy);
        TInt InternalWsPolicyL(CSenWSDescription& aPattern,
								  RSenWSPolicyArray& aPolicies);
        TInt InternalWsPolicyL(const TDesC8& aUri, RSenWSPolicyArray& aPolicies);

        TInt RegisterWsPolicyL(CSenInternalWsPolicy* aInternalPolicy,
                                                    TInt& aErrorTo);
        
        TInt AddWsPolicyL(const TDesC8& aUri, CSenElement* aPolicy, TInt& aErrorTo);
        TInt VerifyPolicyContentsL(CSenInternalWsPolicy* apInternalPolicy, TInt& aErrorTo); //codescannerwarnings
								  

    private: // Data
        MSenCoreServiceManager&     iManager;
        RFileLogger*                iLog;                   // not owned (no create/close)    
		RSenPolicyPtrArray      iPolicyArray;       // owned
		
		CSenBaseFragment*           ipPolicyContainer;  // owned
		CSenWSPolicyIdentifier*   ipPolicyIdentifier; // owned
		CSenInternalWsPolicy*     ipWSpolicy;           // owned
		
		TInt                        iMaxPolicyID;
    };
    
//for Storing the Active consumer information
class CConsumerContainer :public CBase
    {
    public:    
        static CConsumerContainer* NewL(const TDesC8& aUri, const TDesC8& aConsumerId);
        static CConsumerContainer* NewLC(const TDesC8& aUri, const TDesC8& aConsumerId);
        virtual ~CConsumerContainer();
        TPtrC8  Uri();
        TPtrC8  ConsumerId();
    private:
        CConsumerContainer();
        void ConstructL(const TDesC8& aUri, const TDesC8& aConsumerId);

        HBufC8* iConsumerId;
        HBufC8* iURI;
    };    
//for getting the expired policies information    
class CExpiredPolicy :public CBase
    {
    public:
        IMPORT_C static CExpiredPolicy* NewL(const TDesC8& aContract, const TDesC8& aEndpoint, 
                                            const TDesC8& aUri, const TDesC8& aProviderId);
        IMPORT_C static CExpiredPolicy* NewLC(const TDesC8& aContract, const TDesC8& aEndpoint, 
                                            const TDesC8& aUri, const TDesC8& aProviderId);
        IMPORT_C virtual ~CExpiredPolicy();


        IMPORT_C TPtrC8  Contract();
        IMPORT_C TPtrC8  Endpoint();
        TPtrC8  ProvideId();
        IMPORT_C TPtrC8  Uri();
        IMPORT_C TPtrC8  Contents();

        IMPORT_C TInt    SetContentsL(const TDesC8& aContent);
        
    private:
        CExpiredPolicy();
        void ConstructL(const TDesC8& aContract, const TDesC8& aEndpoint, const TDesC8& aUri, const TDesC8& aProviderId);
        
    private:
        HBufC8* iContract; //service contract
        HBufC8* iEndpoint; //service contract
        HBufC8* iProvideId; //service contract
        HBufC8* iURI; //uri to download
        HBufC8* iContents; //contents
    };
#endif // SEN_WS_POLICY_MANAGER_H

// End of File
