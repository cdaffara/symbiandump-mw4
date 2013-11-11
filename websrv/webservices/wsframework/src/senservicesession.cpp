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
* Description:           
*
*/











#include <e32std.h>  // for RPointerArray

#include "SenServiceConnection.h" // session status constants
#include "senservicesession.h"
#include "SenXmlUtils.h"
#include "senpolicy.h"
#include "senserviceinvocationframework.h"
#include "senprovider.h"
#include "senidentifier.h"
#include "sentransportbase.h"

#include "msenlayeredproperties.h"

#include "msenremotehostlet.h" // internal

#include "sendebug.h"
#include "senlogger.h"

EXPORT_C CSenServiceSession::CSenServiceSession(TDescriptionClassType aType,
                                                MSIF& aFramework) :
    CSenWSDescription(aType),
    iStatus(KSenConnectionStatusNew),
    iFramework(aFramework),
    ipTransport(NULL)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenServiceSession::CSenServiceSession()");
    }

EXPORT_C CSenServiceSession::~CSenServiceSession()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenServiceSession::~CSenServiceSession()");
    // The REMOTE consumers are also sessions which are
    // owned by XMLDAO or ClientSession
    iConsumerList.Reset();
    iFacets.ResetAndDestroy();
    if (iTransportOwned)
        {
        delete ipTransport;
        }
    ipTransport = NULL;
    }

// Constructor offered to subclasses:
EXPORT_C void CSenServiceSession::BaseConstructL()
    {
	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenServiceSession::BaseConstructL - Version 2 [2006-05-09]");
        
    // Sets the local name to "ServiceDescription"
    // and initiates the inner ipElement
    CSenWSDescription::ConstructL();
    }

EXPORT_C TInt CSenServiceSession::InitializeFromL(MSenServiceDescription& aServiceDescription)
    { 
    _LIT8(KTouch, "touch");
    CSenWSDescription& sd = (CSenWSDescription&)aServiceDescription;
    const TDesC8* attrValue = sd.AsElement().AttrValue(KTouch);
    if(attrValue != NULL)
    	{
    	AsElement().AddAttrL(KTouch, *attrValue);
    	}
    _LIT8(KEndpointLocalname, "Endpoint");
    CSenElement* pEndpointElement = sd.AsElement().Element(KEndpointLocalname);
    TPtrC8 transportCue;
    if(pEndpointElement)
        {
        _LIT8(KCue, "cue");
        const TDesC8* cue = pEndpointElement->AttrValue(KCue);
        if(cue && cue->Length()>0)
            {
            transportCue.Set(*cue);
            SetTransportCueL(transportCue);
            }
        }

    if(IsLocalL())
        {
        // Initialize local service sessions by trying to lookup the hostlet plug-in
        // behind a local endpoint, and if such exists, by asking it to add (facet)
        // data into this session.

        // Check transport cue; if such XML attribute
        // has been set to <Endpoint element, it is
        // "stronger" than actual endpoint scheme

        TPtrC8 hostletCue = aServiceDescription.Endpoint();
        if( transportCue.Length() > 0 )
            {
            hostletCue.Set( transportCue );
            }



	    CSenProvider* pHostlet = NULL;
        TInt leaveCode(KErrNone);
	    TRAP(leaveCode, pHostlet = CSenProvider::NewL(hostletCue));
	    if(leaveCode == KErrNone && pHostlet)
	        {	
	        CleanupStack::PushL(pHostlet);

            // Inside InitServiceSessionL, the hostlet can add it's facets into this
            // session and possibly initialize some other data required.
            CSenWSDescription* pSD = CSenWSDescription::NewLC();
            pHostlet->DescribeServiceL(*pSD); // pHostlet->InitServiceSessionL(*this);
            this->InitializeFromL(*pSD);
            CleanupStack::PopAndDestroy(pSD);
	        

            /*
            CSenWSDescription* pSD = pHostlet->SessionDescriptionLC();
            // this->InitializeFacetsFromL(*pSD);
            
            // Copy the facets from local service provider  session description
            RFacetArray facets;
            pSD->FacetsL(facets);
            CleanupClosePushL(facets);
        
            TInt count(facets.Count());
            for (TInt i=0; i<count; i++)
                {
                SetFacetL(*facets[i]);
                }
        
            descFacets.ResetAndDestroy();
            CleanupStack::Pop(); // facets.Close()
            
            CleanupStack::PopAndDestroy(); // pSD
            */

	        CleanupStack::PopAndDestroy(); //pHostlet	                
	        }
        }
        
                
    RFacetArray descFacets;
    aServiceDescription.FacetsL(descFacets);
    CleanupClosePushL(descFacets);

    TInt count(descFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        SetFacetL(*descFacets[i]);
        }

    descFacets.ResetAndDestroy();
    CleanupStack::Pop(); // descFacets.Close()

    return KErrNone;
    }

EXPORT_C TInt CSenServiceSession::ParseResponseL(const TDesC8& aInput,
                                                 HBufC8*& aOutput)
    {
    aOutput = HBufC8::NewL(aInput.Length());
    aOutput->Des().Append(aInput); 
    return KErrNone;
    }

/**
 * Ask the ServiceManager to save a description
 * of this ServiceSession (in its DAO storage).
 *
 * This (convenience) method can be called from within other methods of
 * ServiceSession (and/or subclasses) and by ServiceInvocationFramework
 * implementations.
 *
 */
EXPORT_C void CSenServiceSession::SaveL()
    {
    iFramework.Manager().SaveL(*this);
    }

/**
 * Return the status of this ServiceSession.
 *
 * @return  TInt the status
 */
EXPORT_C TInt CSenServiceSession::StatusL()
    {
    return(iStatus);
    }



/**
 * SetStatus
 *
 * Compute the status of the ServiceSession. If the status
 * changes to, or from, "KSenConnectionStatusReady"
 * inform service consumers about the new status.
 *
 * @return TInt indicating the CHANGED status that was set
 *
 */
EXPORT_C TInt CSenServiceSession::SetStatusL()
    {

    TInt currentStatus = iStatus;
    iStatus = ComputeStatusL();
    if (    (currentStatus == KSenConnectionStatusReady
            && iStatus != KSenConnectionStatusReady)
            ||
            (currentStatus != KSenConnectionStatusReady
            && iStatus == KSenConnectionStatusReady)    )
        {

        // notify clients
        RServiceConsumerArray consumers;
        CleanupClosePushL(consumers);
        TInt err = Consumers(consumers);

        if(err==KErrNone)
            {
            TInt count = consumers.Count();
            for (TInt i=0; i<count; i++)
                {
                TRAPD(error, (consumers[i])->SetStatusL(iStatus));
                if(error!=KErrNone)
                    {
			TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CSenServiceSession::SetStatusL() \
                        failed to set status of %d consumer, error code (%d)"),
                        (i+1), error));
                    }
                }
            }
        CleanupStack::PopAndDestroy(); // close() consumers
        }
    return(iStatus);
    }

/**
 * Compute the current status of the ServiceSession.
 * Subclasses __will__ override this.
 * This method does not actually set the status of the ServiceSession
 * and does not notify consumers about changes.
 *
 * @return  TInt indicating the CURRENT computed status
 */
EXPORT_C TInt CSenServiceSession::ComputeStatusL()
    {
    TInt result = KSenConnectionStatusNew;
    TPtrC8 endpoint = Endpoint();

    if(endpoint.Length()>1)
        result = KSenConnectionStatusReady;

    return result;
    }

EXPORT_C MSenRemoteServiceConsumer* CSenServiceSession::RemoteConsumerL(
            const TDesC8& aSenderID)
    {
    TInt consumersCount(iConsumerList.Count());

    for(TInt i=0; i<consumersCount; i++)
        {
        if(iConsumerList[i]->Id() == aSenderID)
            {
            return iConsumerList[i];
            }
        }
    return NULL; // not found

    }


EXPORT_C TInt CSenServiceSession::SubmitL(const TDesC8& aMessage, 
                                          const TDesC8& aTransportProperties,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          HBufC8*& aResponse)
    {
//    CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenServiceSession::SubmitL");
    TPtrC8 endpoint = Endpoint();
    //////////////////////////////////////////////////////////////////////////
    //CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"***********************************************************");
    //CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
    //CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Request (%d bytes):"), aMessage.Length()));
    //CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( aMessage ));
    //CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"***********************************************************");
    //////////////////////////////////////////////////////////////////////////

    delete aResponse;
    aResponse = NULL;

    //MSenRemoteServiceConsumer* pConsumer = RemoteConsumerL(aSenderId);
    
    MSenTransport& transport = aConsumer.TransportL();
    TInt retVal( transport.SubmitL(endpoint, aMessage, aTransportProperties, aResponse,aConsumer) );
    if(retVal==KErrNone)
        {
        // attempt to parse the response. 

        HBufC8* parsedResponse = NULL;
        TInt leaveCode(KErrNone);
        TRAP(leaveCode, ParseResponseL(*aResponse, parsedResponse));
        if(leaveCode!=KErrNone)
            {
            // response is returned as it is: unparsed (for debugging reasons)
            retVal = leaveCode;
            }
        else
            {
            // parsing went OK. Release unparsed buffer
            delete aResponse;
            // assign the parsed buffer to aResponse
            aResponse = parsedResponse;
            }
        }
    // else directly return the error received from transport
    return retVal;
    }


/*
 * @see CServiceConnection.Send(...)
 */
// add txnid, transport properties
EXPORT_C TInt CSenServiceSession::SendL( const TDesC8& aMessage, 
                                         const TDesC8& aTransportProperties,
                                         MSenRemoteServiceConsumer& aConsumer,
                                         TInt& aTxnId,
                                         HBufC8*& /*aRevalidationError*/ )
    {
    //CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"CSenServiceSession::SendL:");
    TPtrC8 endpoint = Endpoint();
    //////////////////////////////////////////////////////////////////////////
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"***********************************************************");
    //CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Endpoint: %S"), &endpoint));
    //CSLOG_FORMAT((aConsumer.ConnectionId() , KMaxLogLevel, _L8("- Request (%d bytes):"), aMessage.Length()));
    //CSLOG_ALL(aConsumer.ConnectionId()  , KMaxLogLevel,( aMessage ));
    CSLOG_L(aConsumer.ConnectionId()  , KMaxLogLevel,"***********************************************************");
    //////////////////////////////////////////////////////////////////////////
    MSenTransport& t = aConsumer.TransportL();
    TInt retVal(t.SendL(endpoint, aMessage, aTransportProperties, *this, aConsumer, aTxnId));
    return retVal;
    }


/*
 * @see CServiceConnection.IsReadyL()
 * *** Note: in Java reference implementation method's was named "isValid()"!
 */
EXPORT_C TBool CSenServiceSession::IsReadyL()
    {
    return (iStatus==KSenConnectionStatusReady);
    }

EXPORT_C TInt CSenServiceSession::CompleteServerMessagesOnOffL(
                                                const TBool& aCompleteOnOff)
    {

    if (aCompleteOnOff)
        {
        CSenFacet* pFacet = CSenFacet::NewL();
        CleanupStack::PushL(pFacet);
        pFacet->SetNameL(KCompleteMessagesFacet);
        pFacet->SetValueL(KSenFacetValTrue);
        SetFacetL(*pFacet);
        CleanupStack::PopAndDestroy(); // pFacet
        }
    else
        {
        RemoveFacet(KCompleteMessagesFacet);
        }

    return KErrNone;
    }


EXPORT_C TInt CSenServiceSession::HasFacetL(const TDesC8& aURI, TBool& aHasFacet)
    {
    aHasFacet = EFalse;
    TInt count(iFacets.Count());

    for (TInt i=0;i<count && !aHasFacet;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            aHasFacet = ETrue;
            }
        }

    return KErrNone;
    }

EXPORT_C TInt CSenServiceSession::FacetValue(TDesC8& aURI, HBufC8*& aValueTo)
    {
    delete aValueTo;
    aValueTo = NULL;

    TInt retVal = KErrNotFound;
    TInt count(iFacets.Count());

    for (TInt i=0; i<count && retVal == KErrNotFound;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            aValueTo = iFacets[i]->Value().Alloc();
            if (aValueTo == NULL) 
                {
                retVal = KErrNoMemory;
                }
            else
                {
                retVal = KErrNone;
                }
            }
        }
    return retVal;
    }

EXPORT_C TInt CSenServiceSession::AddFacetL(const CSenFacet& aFacet)
    {
    // In XML SD, this does not equal with SetFacetL(), but checks
    // for duplicate, already reserved facet name and returns an
    // error if such exists.
    return SetFacetL(aFacet); 
    }

EXPORT_C TInt CSenServiceSession::SetFacetL(const CSenFacet& aFacet)
    {
    TBool hasFacet;
    HasFacetL(((CSenFacet&)aFacet).Name(),hasFacet);
    if (hasFacet)
        {
        RemoveFacet(((CSenFacet&)aFacet).Name());
        }
    CSenFacet* pNewFacet = CSenFacet::NewL(((CSenFacet&)aFacet));
    return iFacets.Append(pNewFacet);
    }

EXPORT_C TInt CSenServiceSession::RemoveFacet(const TDesC8& aURI)
    {
    TInt count(iFacets.Count());

    for (TInt i=0;i<count;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            delete iFacets[i];
            iFacets.Remove(i);
            return KErrNone;
            }
        }

    return KErrNotFound;
    }

EXPORT_C TInt CSenServiceSession::FacetsL(RFacetArray& aFacetArray)
    {
    CSenFacet* pFacet = NULL;
    TInt count(iFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        pFacet = CSenFacet::NewL(*iFacets[i]);
        aFacetArray.Append(pFacet);
        }
    return KErrNone;
    }

EXPORT_C TInt CSenServiceSession::ScoreMatchL(MSenServiceDescription& aPattern)
    {
    TInt score(0);
    
    if ( (aPattern.FrameworkId().Length() == 0) ||
         ( (aPattern.FrameworkId().Length() > 0) && (aPattern.FrameworkId() == FrameworkId()) ) )
        {
        if ((aPattern.Endpoint().Length() > 0) && (aPattern.Endpoint() == Endpoint()))
            {
            score = score + KSenServiceDescriptionBaseScore;
            }
        if ((aPattern.Contract().Length() > 0) && (aPattern.Contract() == Contract()))
            {
            score = score + KSenServiceDescriptionBaseScore;
            }
        }
    

    if ( score > 0 )
        {
        RFacetArray otherFacets;
        CleanupClosePushL(otherFacets); 
        aPattern.FacetsL(otherFacets);

        HBufC8* pFacetValue = NULL;
        TPtrC8 facetName;

        TInt count(otherFacets.Count());
        for (TInt i=0; i<count; i++)
            {
            facetName.Set(otherFacets[i]->Name());
            FacetValue(facetName,pFacetValue);
            if (pFacetValue && *pFacetValue == otherFacets[i]->Value()) score++;
            delete pFacetValue;
            pFacetValue = NULL;
            }

        otherFacets.ResetAndDestroy();
        CleanupStack::Pop();

        if ( ipHostlet )
            {
            score++;
            }
        }

    return score;
    }

EXPORT_C TBool CSenServiceSession::Matches(MSenServiceDescription& aOtherServiceDescription)
    {
    TPtrC8 patternEndpoint = aOtherServiceDescription.Endpoint();
    TPtrC8 thisEndpoint = Endpoint();

    if(patternEndpoint.Length()>0)
        {
        if(!(thisEndpoint.Length()>0 && patternEndpoint == thisEndpoint))
            {
            return EFalse;
            }
        }

    TPtrC8 patternContract = aOtherServiceDescription.Contract();
    TPtrC8 thisContract = Contract();

    if(patternContract.Length()>0)
        {
        if(!(thisContract.Length()>0 && patternContract == thisContract))
            {
            return EFalse;
            }
        }

    TPtrC8 patternFrameworkId = aOtherServiceDescription.FrameworkId();
    TPtrC8 thisFrameworkId = FrameworkId();
    if(patternFrameworkId.Length()>0)
        {
        if(!(thisFrameworkId.Length()>0 && patternFrameworkId == thisFrameworkId))
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CSenServiceSession::Matches() - Framework Id not same returning EFalse");
            return EFalse;
            }
        }


    TBool match(ETrue);
		TInt retVal(KErrNone);
		TRAP(retVal,
		    RFacetArray otherFacets;
		    CleanupClosePushL(otherFacets);
		    aOtherServiceDescription.FacetsL(otherFacets);
		
		    TPtrC8 facetName;
		    TInt count(otherFacets.Count());
		    for (TInt i=0; i<count && match; i++)
		        {
		        facetName.Set(otherFacets[i]->Name());
		        HasFacetL(facetName, match);
		        }
		    otherFacets.ResetAndDestroy();
		    CleanupStack::Pop(); // otherFacets
				);
    return match;
    }

// implement the MSenRemoteServiceSession:
EXPORT_C TPtrC8 CSenServiceSession::Endpoint()
    {
    return CSenWSDescription::Endpoint();
    }

EXPORT_C TPtrC8 CSenServiceSession::Contract()
    {
    return CSenWSDescription::Contract();
    }

// implement the MSenRemoteServiceSession:
EXPORT_C TPtrC8 CSenServiceSession::FrameworkId()
    {
    return CSenWSDescription::FrameworkId();
    }


EXPORT_C RFileLogger* CSenServiceSession::Log() const
    {
    return iFramework.Manager().Log();
    }

EXPORT_C void CSenServiceSession::WriteExtensionsAsXMLToL(RWriteStream& aWriteStream)
    {
    HBufC8* pFacetAsXML = NULL;

    TInt count(iFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        if (iFacets[i]->Name() != KCompleteMessagesFacet)
            {
            pFacetAsXML = iFacets[i]->AsXmlL();
            CleanupStack::PushL(pFacetAsXML);
            aWriteStream.WriteL(*pFacetAsXML);
            CleanupStack::PopAndDestroy(1); // pFacetAsXML
            }
        }
    }

// This base class implementation passes the properties directly to consumer's transport
EXPORT_C TInt CSenServiceSession::SetTransportPropertiesL( const TDesC8& aProperties,
                                                           MSenRemoteServiceConsumer& aConsumer )
    {
    
    return aConsumer.TransportL().SetPropertiesL(aProperties, MSenLayeredProperties::ESenConsumerSessionLayer, &aConsumer);
    }

EXPORT_C TInt CSenServiceSession::TransportPropertiesL(HBufC8*& aProperties,
                                             MSenRemoteServiceConsumer& aConsumer )
    {
    return aConsumer.TransportL().PropertiesL(aProperties);
    }

EXPORT_C MSenServiceDescription::TDescriptionClassType CSenServiceSession::DescriptionClassType()
    {
    return CSenWSDescription::DescriptionClassType();
    }



EXPORT_C MSenServiceDescription& CSenServiceSession::AsServiceDescription()
    {
    return *this;
    }

EXPORT_C TInt CSenServiceSession::RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
		CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel ,"CSenServiceSession::RemoveConsumerL");
    TInt count(iConsumerList.Count());
    TInt index(KErrNotFound);
    for ( TInt i = 0; i < count; i++ )
        {
        MSenRemoteServiceConsumer* consumer = iConsumerList[i];
        if ( consumer )
            {
            if ( consumer == &aConsumer )
                {
                // match
                index = i;
                break;
                }

            if ( consumer->Id() == aConsumer.Id() )
                {
                // match
                index = i;
                break;
                }
            }
        }
    if( index != KErrNotFound )
        {
        //CSLOG_FORMAT((aConsumer.ConnectionId() , KNormalLogLevel, _L8("- Removing consumer: %d/%d"), (index+1), iConsumerList.Count()));
        iConsumerList.Remove(index);
        index = KErrNone; // success
        //CSLOG_FORMAT((aConsumer.ConnectionId() , KNormalLogLevel, _L8("- Consumer count now: %d"), iConsumerList.Count()));
        //CSLOG_FORMAT((aConsumer.ConnectionId() , KNormalLogLevel, _L8("- Hostlet:            %d"), ipHostlet?1:0));
        //CSLOG_FORMAT((aConsumer.ConnectionId() , KNormalLogLevel, _L8("- Transport:          %d"), ipTransport?1:0));
        
        if(ipTransport && iConsumerList.Count() == 0 && !ipHostlet)
            {
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- this session has no consumer(s) and no hostlet; deleting the transport -- START:");
            delete ipTransport;
            CSLOG_L(aConsumer.ConnectionId()  , KMinLogLevel,"- deleting transport --- END");
            ipTransport = NULL;
            }
        }
    if (iTransportOwned)
        {
        delete ipTransport;
        ipTransport = NULL;
        }

    return index;
    }

EXPORT_C void CSenServiceSession::SetTransportL(CSenTransportBase* aTransport)
    {
    if (aTransport)
        {
    if(ipTransport && ipTransport != aTransport)
        {
        // delete the current transport, if it does not match with new one
        delete ipTransport; // this will naturally cancel all pending txns
        ipTransport = NULL;
        }
    ipTransport = aTransport;
    iTransportOwned = aTransport->SessionOwned();
    }
    }
EXPORT_C MSenTransport* CSenServiceSession::Transport() const
    {
    return ipTransport;
    }

EXPORT_C MSenTransport& CSenServiceSession::TransportL(MSenRemoteServiceConsumer& aRequestor)
    {
    if(!ipTransport)
        {
        MSenTransport& transport = aRequestor.TransportL();
        ipTransport = (CSenTransportBase*) &transport;
        }
    return *ipTransport;
    }

EXPORT_C TInt CSenServiceSession::SetHostletL(MSenRemoteHostlet* aHostlet)
    {
    ipHostlet = aHostlet;
    if(!ipHostlet && iConsumerList.Count() == 0)
        {
        delete ipTransport;
        ipTransport = NULL;
        }
    return KErrNone; // return value not used atm
    }

EXPORT_C MSenRemoteHostlet* CSenServiceSession::Hostlet() const
    {
    return ipHostlet;
    }

EXPORT_C TBool CSenServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWSDescription ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWSDescription::HasSuperClass( aType ); 
        }
    }

EXPORT_C TInt CSenServiceSession::SendToHostletL(MSenRemoteHostlet& aReceiver,
                                                 const TDesC8& aMessage,
                                                 const TInt aTxnId,
                                                 MSenRemoteServiceConsumer& aFrom,
                                                 MSenProperties* /* aProperties */)
    {
    // default impelementation routes the request directly to the hostlet
    // Handler aware framework could load the required handlers in here
    // to enable addressing / message correlation etc.
    return aReceiver.ProcessRequestFromConsumerL(aMessage, aTxnId, aFrom /*, aProperties */); 
    }

EXPORT_C TInt CSenServiceSession::ProvideHostletResponseL( MSenRemoteHostlet& /* aProvider */,
                                                           const TInt aTxnId,
                                                           const TInt aServiceCode,
                                                           const TDesC8& /* aRecipientsConsumerId */,
                                                           CSenChunk& /* aMessageChunk */ )
    {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CSenServiceSession::ProvideHostletResponseL");
    TInt retVal(KErrNone);
    MSenTransport* pTransport = Transport();
    if ( pTransport )
        {
		TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Completing the transaction %d"), aTxnId));
        retVal = pTransport->CompleteTransaction( aTxnId, aServiceCode );
        }
    else
        {
		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Fatal - Transport is NULL.");
        retVal = KErrSenInternal; 
        }
    return retVal;
    }

EXPORT_C TInt CSenServiceSession::SetTransportCueL(const TDesC8& aCue)
    {
    return CSenWSDescription::SetTransportCueL(aCue);
    }

EXPORT_C TPtrC8 CSenServiceSession::TransportCue()
    {
    return CSenWSDescription::TransportCue();
    }

EXPORT_C TInt CSenServiceSession::RefreshMTL(HBufC8*& /*aRevalidationError*/)
    {
    return KErrNotSupported;
    }

// End of File

