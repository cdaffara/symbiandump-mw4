/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmFilteredAppList class implementation.
*
*/

#include "upnptmfilteredapplist.h"
#include "upnptmserverimpl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmfilteredapplistTraces.h"
#endif

//Constants
const TUint  KQuote            =     '"';
const TUint  KCommaSeparator   =     ',';

//Literals
_LIT8(KDoubleQuote,        "\"\"");
_LIT8(KIconElement,        "icon");

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------------
//
CUpnpTmFilteredAppList* CUpnpTmFilteredAppList::NewL( CUpnpTmServerImpl& aTmServerImpl )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_NEWL_ENTRY );
    CUpnpTmFilteredAppList* self = new (ELeave) CUpnpTmFilteredAppList( aTmServerImpl );   
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_NEWL_EXIT );
    return self;  
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::CUpnpTmFilteredAppList
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmFilteredAppList::CUpnpTmFilteredAppList( CUpnpTmServerImpl& aTmServerImpl )
                                                   :iTmServerImpl (aTmServerImpl )
     {

     }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmFilteredAppList::ConstructL()
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTL_ENTRY );
    iFilterInfo = CUpnpAppFilterInfo::NewL();
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::~CUpnpTmFilteredAppList
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmFilteredAppList::~CUpnpTmFilteredAppList()
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_CUPNPTMFILTEREDAPPLIST_ENTRY );
    delete iFilterInfo;
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_CUPNPTMFILTEREDAPPLIST_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::ParseAppFilterStringL
// Method parses the descriptor containing the filter string
// It parses the comma-separated list of A_ARG_TYPE_AppList schema 
// elements, attributes and their values
// eg: "name="*Audio*",description="*",icon@mimetype="*svg+xml*", remotingInfo@protocolID="*",
//     appInfo@appCategory="*",audioInfo@audioType="*",resourceStatus="free",signature="*""
// @param aAppFilter Buffer containing application filter string
// @param aErr[out]  Terminal Mode error code
// ---------------------------------------------------------------------------------
//
void CUpnpTmFilteredAppList::ParseAppFilterStringL( const TDesC8& aAppFilter, 
                                                     TTerminalModeErrorCode& aErr )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_PARSEAPPFILTERSTRINGL_ENTRY );
    // Validate the filter string
    aErr = ETerminalModeSuccess;
    TInt quotePos = aAppFilter.Locate( KQuote );
    if ( ( quotePos != 0 ) || ( aAppFilter.Find(KDoubleQuote) == KErrNotFound ))    
        {
        // corrupt filter string
        aErr = ETerminalModeInvalidArgument;
        OstTrace1( TRACE_ERROR, DUP2_CUPNPTMFILTEREDAPPLIST_PARSEAPPFILTERSTRINGL, "CUpnpTmFilteredAppList::ParseAppFilterStringL;quotePos=%d", quotePos );
        return;
        }
    RBuf8 filterBuffer;
    CleanupClosePushL(filterBuffer);
    /* Create a buffer having the content of AppFilter buffer but without 
       the leading quote(")  */ 
    filterBuffer.CreateL(aAppFilter.Mid(quotePos+1));
    TInt equalToQuoteToken;
    while( ( equalToQuoteToken = filterBuffer.Find(Keq)) != KErrNotFound )
        {
        // Fetch the full key string
        TPtrC8 key = filterBuffer.Left(equalToQuoteToken);
        // Check for the presence of sub element by checking the @ in the key string
        TInt atTokenPos = key.Find(KAtToken);
        TBool displayInfo(EFalse);
        if ( atTokenPos != KErrNotFound )
            {
            // @ is found in the key string
            // Now extract the parent element
            TPtrC8 parentKey = key.Left(atTokenPos);
            //Remove any leading and trailing whitespaces in the parent element
            const TDesC8& parentKeyWithoutSpace = RemoveWhiteSpace(parentKey);
            // Check if the parent elemet is one of desired element or not.
            // It should be one of the following :
            // <1> icon <2> remotingInfo <3> appInfo <4> displayInfo <5> audioInfo
            if ( ( parentKeyWithoutSpace.Compare(KIconElement) != KErrNone ) &&
                ( parentKeyWithoutSpace.Compare(KRemotingInfo) != KErrNone ) &&
                ( parentKeyWithoutSpace.Compare(KAppInfo) != KErrNone ) &&
                ( parentKeyWithoutSpace.Compare(KDisplayInfo) != KErrNone ) &&
                ( parentKeyWithoutSpace.Compare(KAudioInfo) != KErrNone ) )
                {
                // parent element is not proper
                aErr = ETerminalModeInvalidArgument;   
                break;
                }
            if ( parentKeyWithoutSpace.Compare(KDisplayInfo) == KErrNone )
                {
                // The parent key element is displayInfo
                displayInfo = ETrue;   
                }
            // Fetch the actual key name ( child element )
            key.Set(key.Mid(atTokenPos+1));
            }
        
        //Remove any leading and trailing whitespaces in the key 
        const TDesC8& keyWithoutSpace = RemoveWhiteSpace(key);
        if ( (filterBuffer.Mid(equalToQuoteToken) ).Locate(KQuote) != 1 )
            {
            // Missing quote(") ahead of the value
            aErr = ETerminalModeInvalidArgument;
            break;    
            }
        TPtrC8 bufPtr = filterBuffer.Mid(equalToQuoteToken+2);
        quotePos = bufPtr.Locate( KQuote );
        if ( quotePos == KErrNotFound )
            {
            // missing quote (") at the end of the value
            aErr = ETerminalModeInvalidArgument;
            break;
            }
        
        /* Add the filter info as key-value pairs.
        // Also check if the parent key is display info.
           If display info flag is true then use the method with non default parameter */
        if ( displayInfo )
            {
            iFilterInfo->SetFilterInfoL( keyWithoutSpace, bufPtr.Left(quotePos), aErr, ETrue );
            }
        else
            {
            // Use the method with default parameter
            iFilterInfo->SetFilterInfoL( keyWithoutSpace, bufPtr.Left(quotePos), aErr );   
            }
        if ( aErr != ETerminalModeSuccess )
            {
            // Return the error code in case the key element is not as per the schema
            aErr = ETerminalModeInvalidArgument;
            break;
            }
        // Skip the quote position and set the buffer
        bufPtr.Set(bufPtr.Mid(quotePos+1));
        if ( ( bufPtr.Locate(KCommaSeparator) != 0 ) && ( bufPtr.Locate(KQuote) != 0 ) )
            {
            //  missing quote (") or comma (,) following the quote.
            //  Expected to be something of this kind ( ", or "" )
            aErr = ETerminalModeInvalidArgument;
            break;      
            }
        //Copy the residual content skipping two characters(", or "" ) in the actual buffer
        filterBuffer.Copy(bufPtr.Mid(UpnpString::KLinefeedLength));
        }
    CleanupStack::PopAndDestroy(&filterBuffer);
    OstTrace1( TRACE_FLOW, CUPNPTMFILTEREDAPPLIST_PARSEAPPFILTERSTRINGL, "CUpnpTmFilteredAppList::ParseAppFilterStringL;aErr=%d", aErr );
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_PARSEAPPFILTERSTRINGL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::ConstructFilteredGetAppListResponseL
// Constructs the GetApplicationList action response buffer satisfying 
// the AppListingFilter criteria
// @param aAppIndex Index the registered application 
// ---------------------------------------------------------------------------------
//
void CUpnpTmFilteredAppList::ConstructFilteredGetAppListResponseL( TInt aAppIndex )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTFILTEREDGETAPPLISTRESPONSEL_ENTRY );
    const TDesC8& filterAppName = iFilterInfo->AppName();
    const TDesC8& filterAppDesc = iFilterInfo->AppDescription();
    if ( ( ( filterAppName.Length() == 0 ) || 
          ((iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).AppName()).MatchC(filterAppName)  
          != KErrNotFound )) && (( filterAppDesc.Length() == 0 ) ||
          ((iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).AppDescription()).MatchC(filterAppDesc) 
            != KErrNotFound )))
            {
            /**
             * 1. Either required appName element is unavailable in the filter string or the desired appName matches.
             * 2. Either optional appDescription element is unavailable in the filter string
             *    or the desired appDescription element matches
             */ 
            const RPointerArray<CUpnpTmInfoElement>& tmInfoElementList = 
                          iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).TmInfoElementList();  
            TInt tmInfoCount = tmInfoElementList.Count();
            // Check if terminal mode info is also available in the filter string
            TInt match(KErrNone);
            for ( TInt i(0); i < tmInfoCount; i++ )
                {
                if ( TerminalModeInfoMatchingL( *tmInfoElementList[i]) )
                    {
                    match++;
                    }
                else
                    {
                    break;
                    }
                }
            if ( match == tmInfoCount )
                {      
                // Check if resource status and xm signature elements are available in the filter
                const TDesC8& filterRscStatus = iFilterInfo->ResourceStatus();
                const TDesC8& filterSignature = iFilterInfo->Signature(); 
                if ( (( filterRscStatus.Length() == 0 ) || 
                    ((iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).ResourceStatus()).MatchC(filterRscStatus) 
                      != KErrNotFound)) && ( ( filterSignature.Length() == 0 ) ||
                      ((iTmServerImpl.XmlSignature()).MatchC(filterSignature) != KErrNotFound)) )
                    {
                    IconMatchingL( aAppIndex );
                    }                
                }
            }
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTFILTEREDGETAPPLISTRESPONSEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::TerminalModeInfoMatchingL
// The method checks if the terminal mode info element is present in the filter string
// If available then it compares with each of the desired key-value of a particular 
// info element.
// @param aTmInfoElement Reference to Terminal Mode Info Element object
// @return Returns a boolean value ( ETrue/EFalse )
// ---------------------------------------------------------------------------------
//
TBool CUpnpTmFilteredAppList::TerminalModeInfoMatchingL( const CUpnpTmInfoElement& aTmInfoElement )const
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_TERMINALMODEINFOMATCHING_ENTRY );
    TBool match(EFalse);
    // First check for terminal mode info type
    const TDesC8& infoType = aTmInfoElement.TerminalModeInfoType();
    /**
     * info Type must be either one of these
     * <1> Remoting Info <2> App Info <3> Display Info <4> Audio Info
     * 
     */
    if ( ( infoType.Compare(KRemotingInfo) == KErrNone ) ||( infoType.Compare(KAppInfo) == KErrNone )
       || ( infoType.Compare(KDisplayInfo) == KErrNone ) || ( infoType.Compare(KAudioInfo) == KErrNone ))
        {
        for ( TInt j(0); j < aTmInfoElement.TmInfoElementCount(); j++ )
            {
            match = TmInfoElementMatchingL( aTmInfoElement,j );
            if ( !match )
                {
                break;
                }
            }
        }
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_TERMINALMODEINFOMATCHING_EXIT );
    return match;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::TmInfoElementMatchingL
// Method is used to compare with each of the desired key-value of a particular
// info element.
// @param aTmInfoElement Reference to Terminal Mode Info Element object.
// @return Returns the boolean value ( ETrue/EFalse )
// ---------------------------------------------------------------------------------
//
TBool CUpnpTmFilteredAppList::TmInfoElementMatchingL( const CUpnpTmInfoElement& aTmInfoElement,
                                                                            TInt aIndex )const
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_TMINFOELEMENTMATCHINGL_ENTRY );
    TBool match(EFalse);
    RBuf8 elementName;
    CleanupClosePushL(elementName);
    RBuf8 elementValue;
    CleanupClosePushL(elementValue);
    aTmInfoElement.GetTmInfoElementL(elementName,elementValue,aIndex );
    // Check for required "protocolID" element
    if ( elementName.Compare(KProtocolId()) == KErrNone ) 
     {
     const TDesC8& filterProtocol = iFilterInfo->ProtocolId();
     if ( ( filterProtocol.Length() == KErrNone) || 
         elementValue.MatchC(filterProtocol) != KErrNotFound )
         {
         // Either protocolID( required element )is unavailable or desired 
         // protocolID matches
         match = ETrue;     
         }
     }
    // Check for optional "format" element
    else if ( elementName.Compare(KFormat()) == KErrNone ) 
         {
         const TDesC8& filterFormat = iFilterInfo->Format();
         if ( ( filterFormat.Length() == KErrNone) || 
             elementValue.MatchC(filterFormat) != KErrNotFound )  
             {
             // Either format ( optional element )is unavailable or desired "format" matches
             match = ETrue;   
             }
         }
    // Check for optional "direction" element
    else if ( elementName.Compare(KRemotingDirection()) == KErrNone ) 
         {
         const TDesC8& filterDirection = iFilterInfo->Direction();
         if ( ( filterDirection.Length() == KErrNone) || 
             elementValue.MatchC(filterDirection) != KErrNotFound )  
             {
             // Either direction ( optional element )is unavailable or desired "direction" matches
             match = ETrue;      
             }
         }
    // Check for optional "appCategory" element
    else if ( elementName.Compare(KAppCategory()) == KErrNone ) 
         {
         const TDesC8& filterAppCategory = iFilterInfo->AppCategory(); 
         if ( ( filterAppCategory.Length() == KErrNone) || 
             elementValue.MatchC(filterAppCategory) != KErrNotFound )
             {
             // Either appCategory ( optional element )is unavailable or desired "appCategory" matches
             match = ETrue;     
             }
         }
    // Check for optional "trustLevel" element for App Info Type
    else if ( ( elementName.Compare(KTrustLevel()) == KErrNone ) &&
                           ( aTmInfoElement.TerminalModeInfoType() == KAppInfo()) )
         {
         const TDesC8& filterAppTrustLevel = iFilterInfo->AppTrustLevel();
         if ( ( filterAppTrustLevel.Length() == KErrNone) || 
             elementValue.MatchC(filterAppTrustLevel) != KErrNotFound )
             {
             // Either trustLevel ( optional element )is unavailable or desired "trustLevel" matches
             match = ETrue;    
             }
     }
    // Check for optional "contentCategory" element
    else if ( elementName.Compare(KContCategory()) == KErrNone ) 
         {
         const TDesC8& filterContCategory = iFilterInfo->ContentCategory();
         if ( ( filterContCategory.Length() == KErrNone) || 
             elementValue.MatchC(filterContCategory) != KErrNotFound )
             {
             // Either contentCategory ( optional element )is unavailable or desired
             // "contentCategory" matches
             match = ETrue;     
             }
         }
    // Check for optional "contentRules" element
    else if ( elementName.Compare(KContentRules()) == KErrNone ) 
         {
         const TDesC8& filterContentRules = iFilterInfo->ContentRules();
         if ( ( filterContentRules.Length() == KErrNone) || 
             elementValue.MatchC(filterContentRules) != KErrNotFound )
             {
             // Either contentRules ( optional element )is unavailable or desired "contentRules" matches
             match = ETrue;  
             }
         }
    // Check for optional "trustLevel" element for Display Info type
    else if ( ( elementName.Compare(KTrustLevel()) == KErrNone ) &&
                        ( aTmInfoElement.TerminalModeInfoType() == KDisplayInfo()) )
         {
         const TDesC8& filterContTrustLevel = iFilterInfo->ContentTrustLevel();
         if ( ( filterContTrustLevel.Length() == KErrNone) || 
             elementValue.MatchC(filterContTrustLevel) != KErrNotFound )
             {
             // Either trustLevel ( optional element )is unavailable or desired "trustLevel" matches
             match = ETrue;  
             }
         }
    // Check for optional "audioType" element
    else if ( elementName.Compare(KAudioType()) == KErrNone ) 
         {
         const TDesC8& filterAudioType = iFilterInfo->AudioType();
         if ( ( filterAudioType.Length() == KErrNone) || 
             elementValue.MatchC(filterAudioType) != KErrNotFound )
             {
             // Either audioType ( optional element )is unavailable or desired "audioType" matches
             match = ETrue;     
             }
         }
    CleanupStack::PopAndDestroy(2,&elementName);
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_TMINFOELEMENTMATCHINGL_EXIT );
    return match;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::IconMatchingL
// The method checks if the optional icon element is present in the filter string
// If present then it compares with the icon of the given application
// @param aAppIndex index of the registered app
// @param aAppStore reference to CUpnpRemotableAppStore object
// ---------------------------------------------------------------------------------
//
void CUpnpTmFilteredAppList::IconMatchingL( TInt aAppIndex )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_ICONMATCHINGL_ENTRY );
    const TDesC8& mimeType = iFilterInfo->MimeType();
    const TDesC8& width = iFilterInfo->Width();
    const TDesC8& height = iFilterInfo->Height();
    const TDesC8& depth = iFilterInfo->Depth(); 
    const RPointerArray<CUpnpTerminalModeIcon>& iconList = 
                           iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).IconList();
    TInt iconCount = iconList.Count();
    if( ( iconCount == 0 ) || 
            (( mimeType.Length() == 0 ) && ( width.Length() == 0 ) && 
                              ( height.Length() == 0 ) && ( depth.Length() == 0 )))
         {
         /* Match is found and the given application needs to be there 
            in the response with no optional icon element */
         ConstructFilteredResponseL( aAppIndex );
         }
    else 
         {
         for ( TInt k(0); k < iconCount; k++ )
             {   
             if ( IconLookUp( *iconList[k] ))
                 {
                 /* Match is found and the given application needs to be there 
                    in the response with the icon element to be present */
                 ConstructFilteredResponseL( aAppIndex );
                 break;
                 }
             }
         }   
    OstTraceFunctionExit0( DUP1_CUPNPTMFILTEREDAPPLIST_ICONMATCHINGL_EXIT );
    }
                              
// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::ConstructFilteredResponseL
// Constructs the filtered soap response buffer for GetApplicationList action
// @param aAppIndex index the registered application 
// @param aAppStore  Reference to CUpnpRemotableAppStore object
// ---------------------------------------------------------------------------------
//
void CUpnpTmFilteredAppList::ConstructFilteredResponseL( TInt aAppIndex )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTFILTEREDRESPONSEL_ENTRY );
    iTmServerImpl.ConstructLeadingSoapBufferL(aAppIndex);
    const TDesC8& appDesc = iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).AppDescription();
    if (  ( iFilterInfo->AppDescription().Length() != KErrNone )  
                                                && ( appDesc.Length() != KErrNone ) )
         {
         // Appends the optional application desciption element
         iTmServerImpl.AppendDataL(KStartNameDesc);
         iTmServerImpl.AppendDataL(appDesc);
         iTmServerImpl.AppendDataL(KEndNameDesc);   
         }  
    const TDesC8& mimeType = iFilterInfo->MimeType();
    const TDesC8& width = iFilterInfo->Width();
    const TDesC8& height = iFilterInfo->Height();
    const TDesC8& depth = iFilterInfo->Depth(); 
    if (( mimeType.Length() != KErrNone ) || ( width.Length() != KErrNone ) || 
                         ( height.Length() != KErrNone ) || ( depth.Length() != KErrNone ))
        {  
        // Even if any one of the matching optional child element of the icon element is present 
        // in the filter string,we will return the complete icon information related to that icon
        const RPointerArray<CUpnpTerminalModeIcon>& iconList = 
                                 iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).IconList();
        TInt iconCount = iconList.Count();
        if ( iconCount > KErrNone )
            {
            // Appends the optional icon element
            iTmServerImpl.AppendDataL(KStartIconList);
            for ( TInt i(0); i < iconCount; i++ )
                {   
                if ( IconLookUp( *iconList[i] ))
                    {
                    iTmServerImpl.SetRemotableAppIconInfoL( *iconList[i] );
                    }
                }
            iTmServerImpl.AppendDataL(KEndIconList);
            }
        }
    // Appends allowed profile ID list
    if ( iFilterInfo->AllowedProfileIdList().Length() != KErrNone )
        {
        iTmServerImpl.SetAllowedProfileIdListL(aAppIndex);
        }
    
    // Appends the different associated terminal mode info elements
    const TDesC8& filterAppCategory = iFilterInfo->AppCategory(); 
    const TDesC8& filterAppTrustLevel = iFilterInfo->AppTrustLevel();
    const TDesC8& filterContCategory = iFilterInfo->ContentCategory();
    const TDesC8& filterContentRules = iFilterInfo->ContentRules();
    const TDesC8& filterContTrustLevel = iFilterInfo->ContentTrustLevel();
    const TDesC8& filterAudioType = iFilterInfo->AudioType();
    
    const RPointerArray<CUpnpTmInfoElement>& tmInfoElementList = 
                      iTmServerImpl.iAppStore->FetchRemotableApp(aAppIndex).TmInfoElementList();
    /**
     * Case 1. In case of matching child elements of remotingInfo ,all the child elements of the
     *         remotingInfo element are sent in the response as protocolID is a required element.
     * Case 2. In case of matching child elements of appInfo,displayInfo or audioInfo ,if one or 
     *         more of the optional child element is available in the filter string,then all
     *         the optional child elements for that info element will be sent in the response.
     */  
    for ( TInt j(0); j < tmInfoElementList.Count(); j++)
        {
        /**
         * No checking for availabilty of child elements of remotingInfo is needed , because 
         * protocolID( child element of remotingInfo ) is required/must element in the response
         * and as per the rule even if one child element is present in the filter string,
         * all other optional child elements of their parent info element too will be sent.
         */  
        if ( ( tmInfoElementList[j]->TerminalModeInfoType() == KRemotingInfo ) ||
            ( ( tmInfoElementList[j]->TerminalModeInfoType() == KAppInfo ) && 
              (( filterAppCategory.Length() != KErrNone ) || (filterAppTrustLevel.Length() != KErrNone ))) ||
            ( ( tmInfoElementList[j]->TerminalModeInfoType() == KDisplayInfo ) && 
             (( filterContCategory.Length() != KErrNone ) || (filterContentRules.Length() != KErrNone ) || 
             (filterContTrustLevel.Length() != KErrNone ) )) || ( ( tmInfoElementList[j]->TerminalModeInfoType() == KAudioInfo ) 
               && ( filterAudioType.Length() != KErrNone )))
            {
            // Called for each terminal mode element associated with the App
            iTmServerImpl.SetTerminalModeInfoElementL( *tmInfoElementList[j] );
            }
        }
    // Appends resource status element for the app if it is available
    if ( iFilterInfo->ResourceStatus().Length() != KErrNone ) 
        {
        iTmServerImpl.SetResourceStatusL(aAppIndex);
        }
    // End of an application
    iTmServerImpl.AppendDataL(KEndApp);
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_CONSTRUCTFILTEREDRESPONSEL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::IconLookUp
// Performs the icon look up
// Checks if the requested icon and given icon matches
// @param aIcon Reference to the Terminal Mode Icon object
// @return Returns boolean value(true or false)
// ---------------------------------------------------------------------------------
//
TBool CUpnpTmFilteredAppList::IconLookUp( CUpnpTerminalModeIcon& aIcon  )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_ICONLOOKUP_ENTRY );
    TBool matchFound(EFalse);
    const TDesC8& filterMimeType = iFilterInfo->MimeType();
    const TDesC8& filterWidth = iFilterInfo->Width();
    const TDesC8& filterHeight = iFilterInfo->Height();
    const TDesC8& filterDepth = iFilterInfo->Depth(); 

    // Check whether the icon is meant to be used only at launch time
    TBuf8<UpnpString::KShortStringLength> widthBuf;
    widthBuf.Num(aIcon.Width());
    TBuf8<UpnpString::KShortStringLength> heightBuf;
    heightBuf.Num(aIcon.Height());
    TBuf8<UpnpString::KShortStringLength> depthBuf;
    depthBuf.Num(aIcon.Depth());
    /* Check if the icon in provided in the input filter string, and if so
       then does it match with the desired icon ( type and dimension ) */
    if ( (( filterMimeType.Length() == 0 ) ||( (aIcon.MimeType()).MatchC(filterMimeType) != KErrNotFound))
         && (( filterWidth.Length() == 0 ) ||( widthBuf.MatchC(filterWidth) != KErrNotFound))
         && (( filterHeight.Length() == 0 ) ||( heightBuf.MatchC(filterHeight) != KErrNotFound))
         && (( filterDepth.Length() == 0 ) ||( depthBuf.MatchC(filterDepth) != KErrNotFound)))
            {
            matchFound = ETrue;
            }
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_ICONLOOKUP_EXIT );
    return matchFound;
    }

// ---------------------------------------------------------------------------------
// Method is used to check whether signature element is needed in the response
// to GetApplicationList action or not
// ---------------------------------------------------------------------------------
const TDesC8& CUpnpTmFilteredAppList::SignatureElement()const
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_GETSIGNATUREELEMENT_ENTRY );
    return iFilterInfo->Signature(); 
    }

// ---------------------------------------------------------------------------------
// CUpnpTmFilteredAppList::RemoveWhiteSpace
// Method is used to remove leading and trailing whitespaces from the descriptor's data
// @param aData Descriptor's data from which whitespaces have to be removed
// @return Returns the descriptor having no white spaces
// ---------------------------------------------------------------------------------
//
const TDesC8& CUpnpTmFilteredAppList::RemoveWhiteSpace( TPtrC8& aData )
    {
    OstTraceFunctionEntry0( CUPNPTMFILTEREDAPPLIST_REMOVEWHITESPACE_ENTRY );
    // Removes the leading white spaces
    TInt length = aData.Length();
    TInt i = 0;
    TBool done = EFalse;
    while( !done && i < length )
        {
        TChar current = aData[i];
        done  = !current.IsSpace();
        if( !done )
        ++i;
        }
    aData.Set(aData.Mid(i)); /* aData now does not have any white space character 
                                ahead of its actual data */
    
    // Removes the trailing white spaces
    length = aData.Length();
    i = 0;
    done = EFalse;
    while( !done && i < length )
        {
        TChar current = aData[(length-1) - i];
        done  = !current.IsSpace();
        if( !done )
        ++i;
        }
    aData.Set(aData.Left(aData.Length() - i)); /* aData now does not have any white space character
                                                following the actual data */
    OstTraceFunctionExit0( CUPNPTMFILTEREDAPPLIST_REMOVEWHITESPACE_EXIT );
    return aData;
    }

// End of File

