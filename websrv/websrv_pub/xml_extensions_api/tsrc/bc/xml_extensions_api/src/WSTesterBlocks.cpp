/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include <SenNameSpace.h>
#include "WSTester.h"
#include "SAXTester.h"
#include <commdb.h>




// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWSTester::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CWSTester::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CWSTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CWSTester::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "_S_NewXMLServiceDescription", CWSTester::_S_NewXMLServiceDescription),
        ENTRY( "_S_NewServicePattern", CWSTester::_S_NewServicePattern),
        ENTRY( "_S_RegisterServiceDescription", CWSTester::_S_RegisterServiceDescription),
        ENTRY( "_S_UnregisterServiceDescription", CWSTester::_S_UnregisterServiceDescription),
  		ENTRY( "_S_InstallFramework", CWSTester::_S_InstallFramework),
        ENTRY( "_S_EndPoint", CWSTester::_S_EndPoint),
        ENTRY( "_S_Contract", CWSTester::_S_Contract),
        ENTRY( "_S_ConsumerIapId", CWSTester::_S_ConsumerIapId),
        ENTRY( "_C_ProviderIDToService", CWSTester::_C_ProviderIDToService),
        ENTRY( "_S_FrameworkId", CWSTester::_S_FrameworkId),
        ENTRY( "_S_NewServiceConnection", CWSTester::_S_NewServiceConnection),
        ENTRY( "_C_Body", CWSTester::_C_Body),
        ENTRY( "_C_Dump", CWSTester::_C_Dump),
        ENTRY( "_S_NewServiceManager", CWSTester::_S_NewServiceManager),
        ENTRY( "_S_NewIdentityProvider", CWSTester::_S_NewIdentityProvider),
        ENTRY( "_S_ProviderID", CWSTester::_S_ProviderID),
        ENTRY( "_S_UserInfo", CWSTester::_S_UserInfo),
        ENTRY( "_S_RegisterIdentityProvider", CWSTester::_S_RegisterIdentityProvider),
        ENTRY( "_S_UnregisterIdentityProvider", CWSTester::_S_UnregisterIdentityProvider),
        ENTRY( "_S_AssociateService", CWSTester::_S_AssociateService),
        ENTRY( "_S_DissociateService", CWSTester::_S_DissociateService),
        ENTRY( "_S_ServiceDescriptions", CWSTester::_S_ServiceDescriptions),
 		ENTRY( "_S_ServiceDescriptionsByUri", CWSTester::_S_ServiceDescriptionsByUri),
		ENTRY( "_C_ParseDomFragment", CWSTester::_C_ParseDomFragment),
        ENTRY( "_C_ParseXMLDocument", CWSTester::_C_ParseXMLDocument),
        ENTRY( "_C_FromXmlDateTime", CWSTester::_C_FromXmlDateTime),
        ENTRY( "_C_ToXmlDateTimeUtf", CWSTester::_C_ToXmlDateTimeUtf),
        ENTRY( "_X_WaitForResponse", CWSTester::_X_WaitForResponse),
        ENTRY( "_X_Reference", CWSTester::_X_Reference),
        ENTRY( "_X_ReferenceFragment", CWSTester::_X_ReferenceFragment),
        ENTRY( "_X_LogResult", CWSTester::_X_LogResult),
        ENTRY( "_X_Init", CWSTester::_X_Init),
        ENTRY( "_X_DeleteFile", CWSTester::_X_DeleteFile),
		ENTRY( "_X_TimeOut", CWSTester::_X_TimeOut),
        ENTRY( "_X_Flag", CWSTester::_X_Flag),
        ENTRY( "_X_NonIncludance", CWSTester::_X_NonIncludance),
        ENTRY( "_X_Includance", CWSTester::_X_Includance),
        ENTRY( "_X_DeBase64", CWSTester::_X_DeBase64),
   		ENTRY( "_X_CheckDir", CWSTester::_X_CheckDir),
		ENTRY( "_X_CheckFile", CWSTester::_X_CheckFile),
		ENTRY( "_S_CompleteServerMessagesOnOff", CWSTester::_S_CompleteServerMessagesOnOff),
		ENTRY( "_X_SOAPRequest", CWSTester::_X_SOAPRequest),
		ENTRY( "_X_NonExistence", CWSTester::_X_NonExistence ),
   		ENTRY( "_S_NewSAXFragment", CWSTester::_S_NewSAXFragment ),
		ENTRY( "_S_SAXTest", CWSTester::_S_SAXTest ),
        		ENTRY( "_C_ParseToElement", CWSTester::_C_ParseToElement),
		ENTRY( "_S_LocalName", CWSTester::_S_LocalName),
		ENTRY( "_S_NamespaceURI", CWSTester::_S_NamespaceURI),
		ENTRY( "_S_SetNamespace", CWSTester::_S_SetNamespace),
		ENTRY( "_S_NsPrefix", CWSTester::_S_NsPrefix),
        ENTRY( "_S_SetPrefix", CWSTester::_S_SetPrefix),
		ENTRY( "_S_HasContent", CWSTester::_S_HasContent),
		ENTRY( "_S_Content", CWSTester::_S_Content),
		ENTRY( "_S_ContentUnicode", CWSTester::_S_ContentUnicode),
		ENTRY( "_S_SetContent", CWSTester::_S_SetContent),
		ENTRY( "_S_AddNamespace", CWSTester::_S_AddNamespace),
		ENTRY( "_S_AttrValue", CWSTester::_S_AttrValue),
		ENTRY( "_S_AddAttr", CWSTester::_S_AddAttr),
		ENTRY( "_C_Namespace", CWSTester::_C_Namespace),
		ENTRY( "_C_Elements", CWSTester::_C_Elements),
		ENTRY( "_C_Attributes", CWSTester::_C_Attributes),
		ENTRY( "_C_Namespaces", CWSTester::_C_Namespaces),
		ENTRY( "_C_Parent", CWSTester::_C_Parent),
		ENTRY( "_C_Root", CWSTester::_C_Root),
		ENTRY( "_C_SetParent", CWSTester::_C_SetParent),
		ENTRY( "_C_Element", CWSTester::_C_Element),
		ENTRY( "_C_CreateElement", CWSTester::_C_CreateElement),
		ENTRY( "_C_InsertElement", CWSTester::_C_InsertElement),
		ENTRY( "_S_AddElement", CWSTester::_S_AddElement),
		ENTRY( "_S_RemoveElement", CWSTester::_S_RemoveElement),
		ENTRY( "_C_ReplaceElement", CWSTester::_C_ReplaceElement),
		ENTRY( "_S_AsXml", CWSTester::_S_AsXml),
		ENTRY( "_S_AsXmlUnicode", CWSTester::_S_AsXmlUnicode),
		ENTRY( "_C_AsElement", CWSTester::_C_AsElement),
		ENTRY( "_C_CopyFrom", CWSTester::_C_CopyFrom),
		ENTRY( "_C_Detach", CWSTester::_C_Detach),
		ENTRY( "_C_Child", CWSTester::_C_Child),
		ENTRY( "_C_AllocAttrValue", CWSTester::_C_AllocAttrValue),
		ENTRY( "_S_SetAttributes", CWSTester::_S_SetAttributes),
		ENTRY( "_S_Set", CWSTester::_S_Set),
		ENTRY( "_S_WriteAsXMLTo", CWSTester::_S_WriteAsXMLTo),
		ENTRY( "_S_WriteNamespacesTo", CWSTester::_S_WriteNamespacesTo),
		ENTRY( "_S_ContentWriteStream", CWSTester::_S_ContentWriteStream),
   		ENTRY( "_S_TransactionCompleted", CWSTester::_S_TransactionCompleted),
		ENTRY( "_S_StartTransaction", CWSTester::_S_StartTransaction),
   		ENTRY( "_X_Iteration", CWSTester::_X_Iteration),
   		ENTRY( "_X_IAPList", CWSTester::_X_IAPList),
        ENTRY( "_X_CheckResultFL", CWSTester::_X_CheckResultFL),
   		
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/*################################################
*
*
*   SERVICE MANAGER DLL
*
*
*################################################*/

/*-----------------------------------------------
            Service Manager
-----------------------------------------------*/

TInt CWSTester::_S_NewServiceManager( CStifItemParser& aItem ){
        iLog->Log(_L("*******NewServiceManager -> Started!********"));
		iLog->Log(_L("Creating service manager instance..."));

        if ( iServiceManager!= NULL ){
            delete iServiceManager;
	        iServiceManager = NULL;
        }
    	iServiceManager = CSenServiceManager::NewL();
        if (iServiceManager == NULL){
            return KErrNotFound;
        }
        iLog->Log(_L("Service manager instance created!"));
        return KErrNone;
}

TInt CWSTester::_S_RegisterIdentityProvider( CStifItemParser& aItem ){

        iLog->Log(_L("*******"));
		iLog->Log(_L("Registering identity provider 1 with values: "));
		LogResultL( iProvider->Endpoint() );
		LogResultL( iProvider->Contract() );
		LogResultL( iProvider->AdvisoryAuthnID() );
		LogResultL( iProvider->AuthzID() );
		LogResultL( iProvider->Password());
		LogResultL( iProvider->ProviderID());

        TInt error = iServiceManager->RegisterIdentityProviderL( *iProvider);

		if (error != KErrNone )
		{
			// Test case not passed
			iLog->Log(_L("Registering identity provider 1 failed. Error: %d"), error );
			if(error == -30321)
			{
				iWriter.WriteL(_L8("Registering identity provider 1 failed. End-user denies permission."));
				iWriter.CommitL();
			}
			return error;
		}
        return KErrNone;
}
TInt CWSTester::_S_UnregisterIdentityProvider( CStifItemParser& aItem ){
        iLog->Log(_L("*******"));
		iLog->Log(_L("Unregistering identity provider with values: "));
		iLog->Log( iProvider->Endpoint() );
		iLog->Log( iProvider->Contract() );
		iLog->Log( iProvider->AdvisoryAuthnID() );
		iLog->Log( iProvider->AuthzID() );
		iLog->Log( iProvider->Password());
		iLog->Log( iProvider->ProviderID());

        TInt error = iServiceManager->UnregisterIdentityProviderL( *iProvider);

		if (error != KErrNone )
		{
			// Test case not passed
			iLog->Log(_L("Unregistering identity provider 1 failed. Error: %d"), error );
			return error;
		}
        return KErrNone;
}
TInt CWSTester::_S_RegisterServiceDescription( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("Registering Service Description"));
	TInt error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
	if (error != KErrNone )
	{
		iLog->Log(_L("Registering Service Description failed. Error: %d"), error );
		if(error == -30321)
		{
			iWriter.WriteL(_L8("Registering Service Description failed.End-user denies permission"));
			iWriter.CommitL();
		}
		return error;
	}
    return KErrNone;
}
TInt CWSTester::_S_UnregisterServiceDescription( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("Unregistering Service Description"));
	TInt error = iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);
	if (error != KErrNone )
	{
		iLog->Log(_L("Unregistering Service Description failed. Error: %d"), error );
		return error;
	}
    return KErrNone;
}

TInt CWSTester::_S_AssociateService( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("Associate service"));

    TInt result;
    TPtrC8 arg1 = GetParC(aItem);
    TPtrC8 arg2 = GetParC(aItem);
    result = iServiceManager->AssociateServiceL(arg1, arg2);
    PopAndDestroyArgc();
    return result;
}
TInt CWSTester::_S_DissociateService( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("Dissociate service"));

    TInt result;
    TPtrC8 arg1 = GetParC(aItem);
    TPtrC8 arg2 = GetParC(aItem);
    result = iServiceManager->DissociateServiceL(arg1, arg2);
    PopAndDestroyArgc();
    return result;
}
TInt CWSTester::_S_ServiceDescriptions( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("ServiceDescriptions"));
    RServiceDescriptionArray arr;
    TInt error = iServiceManager->ServiceDescriptionsL(*iSenXmlServiceDescription, arr);
  	TInt SDs = arr.Count();
    TBuf8<20> buf;
    _LIT8(KFound, "Found: %d");
    buf.Format(KFound, SDs);
    LogResultL(buf);
    arr.ResetAndDestroy();
    //PopAndDestroyArgc();
    return error;
}
TInt CWSTester::_S_ServiceDescriptionsByUri( CStifItemParser& aItem ){
    iLog->Log(_L("*******"));
	iLog->Log(_L("ServiceDescriptions"));
    const TPtrC8 arg = GetParC(aItem);
    RServiceDescriptionArray arr;
    TInt error = KErrNone;
    //TRAP(error, iServiceManager->ServiceDescriptionsL(arg, arr));
	error = iServiceManager->ServiceDescriptionsL(arg, arr);
    if (error == KErrNone){
        TInt SDs = arr.Count();
        TBuf8<20> buf;
        _LIT8(KFound, "Found: %d");
        buf.Format(KFound, SDs);
        LogResultL(buf);
    }  	
    arr.ResetAndDestroy();
    PopAndDestroyArgc();
    return error;
}


TInt CWSTester::_S_InstallFramework( CStifItemParser& aItem ) {

	iLog->Log(_L("*******_S_InstallFramework -> Started!********"));

	TPtrC arg;
	aItem.GetNextString(arg);

	HBufC8* frmID = SenXmlUtils::ToUtf8LC(arg);

	TInt error(KErrNone);
/*	TInt error = ((CSenServiceManagerImpl*)iServiceManager)->InstallFrameworkL( *frmID );*/

	if (error != KErrNone)
	{
		// Test case not passed
		iLog->Log(_L("Installing framework failed" ) );
		LogResultL(_L8("Could not install framework!"));
		CleanupStack::PopAndDestroy(1);
		return error;
    }

	// Test case passed
	LogResultL(_L8("InstallFramework passed"));
	CleanupStack::PopAndDestroy(1);

	iLog->Log(_L("*******_S_InstallFramework -> Finished!********"));

	return KErrNone;
}

/*################################################
*
*
*   SERVICE CONNECTION DLL
*
*
*################################################*/

/*-----------------------------------------------
            Service Connection
-----------------------------------------------*/
TInt CWSTester::_S_NewServiceConnection( CStifItemParser& aItem ){
    iLog->Log(_L("###NewServiceConnection -> started###"));
	if ( iSenServiceConnection != NULL )
	{
    	delete iSenServiceConnection ;
	    iSenServiceConnection  = NULL;
	}


  	TPtrC arg;
    if(!(aItem.GetNextString(arg)))
    {
		HBufC8* aContract = SenXmlUtils::ToUtf8LC(arg);
    	iSenServiceConnection = CSenServiceConnection::NewL(*this, *aContract );
		CleanupStack::PopAndDestroy(1);
    }
    else iSenServiceConnection =	CSenServiceConnection::NewL(*this, *iSenXmlServiceDescription);


    if ( iSenServiceConnection != NULL )
	{
	    iLog->Log(_L("Connection received"));
	}
	else
	{
        //to do> to refrence file
		iLog->Log(_L("Connection NULL!"));
        return KErrNotFound;
    }


    return KErrNone;
}
TInt CWSTester::_S_CompleteServerMessagesOnOff( CStifItemParser& aItem ) {

	iLog->Log(_L("###Setting CompleteServerMessagesOnOff flag -> started###"));

	if ( iSenServiceConnection == NULL )
	{
		return KErrGeneral;
	}
    TPtrC string;
    if (!(aItem.GetNextString ( string )))
		{
        if (string.Compare(_L16("TRUE")) == 0)
			{
			TBool aTrue = ETrue;
			TInt retVal = iSenServiceConnection->CompleteServerMessagesOnOff(aTrue);
			return retVal;
			}
		else
			{
			TBool aTrue = EFalse;
			TInt retVal = iSenServiceConnection->CompleteServerMessagesOnOff(aTrue);
			return retVal;
			}
	}
	else
	{
		return KErrGeneral;
	}


}

TInt CWSTester::_S_StartTransaction( CStifItemParser& aItem ) {

	iLog->Log(_L("### _S_StartTransaction -> started###"));

	TInt retVal = iSenServiceConnection->StartTransaction();
	
	iLog->Log(_L("### _S_StartTransaction -> ended: %d"), retVal);
	return retVal;
}

TInt CWSTester::_S_TransactionCompleted( CStifItemParser& aItem ) {

	iLog->Log(_L("### _S_TransactionCompleted -> started###"));

	TInt retVal = iSenServiceConnection->TransactionCompleted();
	
	iLog->Log(_L("### _S_TransactionCompleted -> ended: %d"), retVal);
	return retVal;
}

/*################################################
*
*
*   SERVICE DESCRIPTION DLL
*
*
*################################################*/



/*-----------------------------------------------
            XML Service Description
-----------------------------------------------*/

TInt CWSTester::_S_NewXMLServiceDescription( CStifItemParser& aItem ){
    iLog->Log(_L("###NewXMLServiceDescription -> started###"));
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    iLog->Log(_L("  created new XML service description###"));
    if (iSenXmlServiceDescription != NULL)
        return KErrNone;
    else
        return KErrGeneral;
}

TInt CWSTester::_S_EndPoint( CStifItemParser& aItem ){
    iLog->Log(_L("###EndPoint -> started###"));
    iSenXmlServiceDescription->SetEndPointL(GetParC(aItem));//(TDesC8)
    PopAndDestroyArgc();
   return KErrNone;
}
TInt CWSTester::_S_ConsumerIapId( CStifItemParser& aItem ){
 	if (!iSenXmlServiceDescription)
	    return KErrGeneral;
    iLog->Log(_L("###IAP -> started###"));
    TInt iap = 0;
    aItem.GetNextInt( iap );
    //in fact servicePattern instance
	((CSenServicePattern*)iSenXmlServiceDescription)->SetConsumerIapIdL(iap);
    return KErrNone;
}

TInt CWSTester::_S_Contract( CStifItemParser& aItem ){
    iLog->Log(_L("###Contract -> started###"));

    iSenXmlServiceDescription->SetContractL(GetParC(aItem));//(TDesC8)
    PopAndDestroyArgc();
   return KErrNone;
}

TInt CWSTester::_S_FrameworkId( CStifItemParser& aItem ){
    iLog->Log(_L("###FrameworkId -> started###"));

    iSenXmlServiceDescription->SetFrameworkIdL(GetParC(aItem));
    PopAndDestroyArgc();
    return KErrNone;
}
TInt CWSTester::_C_ProviderIDToService( CStifItemParser& aItem ){
    iLog->Log(_L("###Setting providerId for service -> started###"));

    iSenXmlServiceDescription->AsElement().AddElementL(_L8("ProviderID")).SetContentL(GetParC(aItem));
    PopAndDestroyArgc();
    return KErrNone;
}

/*-----------------------------------------------
            Service PAttern
-----------------------------------------------*/
TInt CWSTester::_S_NewServicePattern( CStifItemParser& aItem ){
    iLog->Log(_L("###NewServicePattern -> started###"));
    iSenXmlServiceDescription = CSenServicePattern::NewL();

    if (iSenXmlServiceDescription != NULL)
        return KErrNone;
    else
        return KErrGeneral;
}
/*-----------------------------------------------
            IdentityProvider
-----------------------------------------------*/


TInt CWSTester::_S_NewIdentityProvider( CStifItemParser& aItem ){

    iLog->Log(_L("###_S_NewIdentityProvider -> started###"));

    TPtrC8 arg1 = GetParC(aItem);
    TPtrC8 arg2 = GetParC(aItem);
    iProvider = CSenIdentityProvider::NewL( arg1, arg2);
    PopAndDestroyArgc();
    return KErrNone;
}


TInt CWSTester::_S_ProviderID( CStifItemParser& aItem ){

    iLog->Log(_L("###_S_ProviderID -> started###"));

    iProvider->SetProviderID( GetParC(aItem));
    PopAndDestroyArgc();
    return KErrNone;
}
TInt CWSTester::_S_UserInfo( CStifItemParser& aItem ){
    iLog->Log(_L("###_S_UserInfo -> started###"));

    TPtrC8 arg1 = GetParC(aItem);
    TPtrC8 arg2 = GetParC(aItem);
    TPtrC8 arg3 = GetParC(aItem);

	iProvider->SetUserInfoL( arg1,
                            arg2,
                            arg3);
    PopAndDestroyArgc();
    return KErrNone;
}
/*################################################
*
*
*   UTILS
*
*
*################################################*/

/*-----------------------------------------------
            SoapEnvelope
-----------------------------------------------*/

TInt CWSTester::_C_Body( CStifItemParser& aItem ){
        iLog->Log(_L("### Body -> started ###"));
        TBuf8<255> pRequestBody;
		TPtrC arg;
        aItem.GetNextString(arg);
        //iRequestBody->Des().Append(arg);
        pRequestBody.Append(arg);

		while (!(aItem.GetNextString(arg)))
		{
			//iRequestBody->Des().Append(_L(" "));
			//iRequestBody->Des().Append(arg);
       		pRequestBody.Append(_L(" "));
			pRequestBody.Append(arg);

		}

        //iLog->Log(*requestBody);
        iLog->Log(pRequestBody);
        iSOAPMessage = CSenSoapMessage::NewL();
        iLog->Log(_L("Creating SOAP message.."));
        iSOAPMessage->SetBodyL(pRequestBody);
	    iLog->Log(_L("SOAP message created.."));

        return KErrNone;

    //Append(_L8("<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:CARDS/ab:CARD[ab:N/ab:FAMILY=\"Doe\" and ab:N/ab:GIVEN=\"John\"]</ab:Select></ab:QueryItem></ab:Query>"));
}

/*-----------------------------------------------
            SoapDateUtils

-----------------------------------------------*/

TInt CWSTester::_C_FromXmlDateTime( CStifItemParser& aItem ){
    TPtrC string;
    TPtrC stringTime;
    TTime ttime;
    iLog->Log(_L("### FromXMLDateTime -> started ###"));

    aItem.GetNextString ( string );
    aItem.GetNextString ( stringTime );
    if (string.Compare(_L16("16")) == 0){
        HBufC16* writeBuf=HBufC16::New(stringTime.Length());
        CleanupStack::PushL(writeBuf);
        TPtr16 ptr = writeBuf->Des();
        ptr.Copy(stringTime);
        iLog->Log(ptr);
		//API removed
        /*TRAPD(errorConverter, ttime = SenDateUtils::FromXmlDateTimeL(ptr));
	    if (errorConverter != KErrNone) {
            CleanupStack::PopAndDestroy(1);
            return errorConverter;
        }*/
    }else{
        HBufC8* writeBuf=HBufC8::New(stringTime.Length());
        CleanupStack::PushL(writeBuf);
        TPtr8 ptr = writeBuf->Des();
        ptr.Copy(stringTime);
        iLog->Log(ptr);
        TRAPD(errorConverter, ttime = SenDateUtils::FromXmlDateTimeL(ptr));
	    if (errorConverter != KErrNone) {
            CleanupStack::PopAndDestroy(1);
            return errorConverter;
        }

    }

    TBuf<30> dateString;
    _LIT(KDateString1,"%E%D%X%N%Y %1 %2 %3");
    ttime.FormatL(dateString,KDateString1);
    LogResultL(dateString);
    _LIT(KDateString4,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
    ttime.FormatL(dateString,KDateString4);
    LogResultL(dateString);
    CleanupStack::PopAndDestroy(1);
    return KErrNone;

}
TInt CWSTester::_C_ToXmlDateTimeUtf( CStifItemParser& aItem ){
    TPtrC string;
    TPtrC stringTime;
    TPtrC stringBufforLength;

    HBufC16* writeBufTime;
    TTime ttime;
    iLog->Log(_L("### ToXmlDateTimeUtf -> started ###"));

    aItem.GetNextString ( string );
    aItem.GetNextString ( stringBufforLength );
    aItem.GetNextString ( stringTime );

    writeBufTime = HBufC16::New(stringTime.Length());
    TPtr16 ptr = writeBufTime->Des();
	iLog->Log(_L("...copying"));
    ptr.Copy(stringTime);
    iLog->Log(_L("ok"));
	iLog->Log(ptr);
    iLog->Log(_L("...parsing"));
	TInt error  = ttime.Parse(ptr);
    iLog->Log(_L("ok"));
	if ( writeBufTime!= NULL ){
        delete writeBufTime;
	    writeBufTime = NULL;
    }

    if (error < KErrNone) {
        return error;
    }
    if (string.Compare(_L16("16")) == 0){
        HBufC16* writeBuf;
        if (stringBufforLength.Compare(_L16("over")) == 0){
            writeBuf=HBufC16::New(SenDateUtils::KXmlDateTimeMaxLength + 5);
        }else if(stringBufforLength.Compare(_L16("under")) == 0){
            writeBuf=HBufC16::New(SenDateUtils::KXmlDateTimeMaxLength - 20);
        }else if(stringBufforLength.Compare(_L16("equal")) == 0){
            writeBuf=HBufC16::New(SenDateUtils::KXmlDateTimeMaxLength);
        }else{
            return KResultFailedFound;
        }

        TPtr16 ptrResult = writeBuf->Des();
        iLog->Log(_L("converting to XmlDateTime"));
		//API removed
        /*TRAPD(errorConverter, SenDateUtils::ToXmlDateTimeL(ptrResult, ttime));
	    if (errorConverter != KErrNone) {
			iLog->Log(_L("...fail"));
            return errorConverter;
        }*/
        LogResultL(ptrResult);
		if ( writeBuf!= NULL ){
            delete writeBuf;
	        writeBuf = NULL;
        }
    }else{
        HBufC8* writeBuf;
        if (stringBufforLength.Compare(_L16("over")) == 0){
            writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength + 5);
        }else if(stringBufforLength.Compare(_L16("under")) == 0){
            writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength - 20);
        }else if(stringBufforLength.Compare(_L16("equal")) == 0){
            writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength);
        }else{
            return KResultFailedFound;
        }
        TPtr8 ptrResult = writeBuf->Des();
        iLog->Log(_L("converting to XmlDateTime"));
        TRAPD(errorConverter, SenDateUtils::ToXmlDateTimeUtf8L(ptrResult, ttime));
        if (errorConverter != KErrNone) {
      		iLog->Log(_L("...fail"));
            return errorConverter;
        }
        LogResultL(ptrResult);
	    if ( writeBuf!= NULL ){
            delete writeBuf;
	        writeBuf = NULL;
        }
      
    }
    return KErrNone;
}
/*################################################
*
*
*   XML DLL
*
*
*################################################*/

/*-----------------------------------------------
            SAX based parsing
-----------------------------------------------*/
TInt CWSTester::_S_NewSAXFragment( CStifItemParser& aItem )
	{

    iLog->Log(_L("### _S_NewSAXFragment -> started ###"));


    TPtrC string;
    if (!(aItem.GetNextString ( string )))
	{
        if (string.Compare(_L16("ConstructLocal")) == 0)
			{
			iSaxTester = CSAXTester::NewL( KLName8() );
			iSaxTester->SetReader( *iXmlReader );
		    }
		else if(string.Compare(_L16("ConstructLocalNs")) == 0)
			{
			iSaxTester = CSAXTester::NewL( KDefaultNS(), KLName8() );
			iSaxTester->SetReader( *iXmlReader );
			}
		else if(string.Compare(_L16("ConstructLocalNsQ")) == 0)
			{
			iSaxTester = CSAXTester::NewL( KDefaultNS(), KLName8(), KQName() );
			iSaxTester->SetReader( *iXmlReader );
			}

		else if(string.Compare(_L16("ConstructLocalNsQAttr")) == 0)
			{
			RAttribute justAttribute;
			CleanupClosePushL(justAttribute);
			TRAPD(err, justAttribute.Open(
					iStringPool.OpenStringL(KNullDesC8()),
					iStringPool.OpenStringL(KNullDesC8()),
					iStringPool.OpenStringL(KAttribute()),
					iStringPool.OpenStringL(KAttributeValue())));

		if (err != KErrNone) 
			{
			iLog->Log(_L("Retrieving the attribute failed. Error: %d"), err );
			CleanupStack::PopAndDestroy();
			return err;
	    }

			// make a new array for all attributes including namespace (to be added)
			RAttributeArray justAttributes;

			// append the namespace attribute (declaration)
			justAttributes.Append(justAttribute);

			iSaxTester = CSAXTester::NewL( KDefaultNS(), KLName8(), KQName(), justAttributes);
			iSaxTester->SetReader( *iXmlReader );

			justAttributes[0].Close();
			CleanupStack::Pop(); // justAttribute has been safely closed


			// close the actual array
			justAttributes.Close();
		}
		else if(string.Compare(_L16("ConstructLocalNsQAttrElem")) == 0)
			{
			RAttribute justAttribute;
			CleanupClosePushL(justAttribute);
			TRAPD(err,justAttribute.Open(
					iStringPool.OpenStringL(KNullDesC8()),
					iStringPool.OpenStringL(KNullDesC8()),
					iStringPool.OpenStringL(KAttribute()),
					iStringPool.OpenStringL(KAttributeValue())));

		if (err != KErrNone) 
			{
			iLog->Log(_L("Retrieving the attribute failed. Error: %d"), err );
			CleanupStack::PopAndDestroy();
			return err;
	    }

			RAttributeArray justAttributes;
			justAttributes.Append(justAttribute);

			iFrg = CSenBaseFragment::NewL(_L8("BaseFrag"));
			iFrg->SetReader( *iXmlReader );
			iFrg->ParseL(KElementXml);

			CSenElement &elem = iFrg->AsElement();

			iSaxTester = CSAXTester::NewL( KDefaultNS(), KLName8(), KQName(), justAttributes, elem );
			iSaxTester->SetReader( *iXmlReader );

			HBufC8* xmlBuffer = iSaxTester->AsElement().Parent()->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			CleanupStack::PopAndDestroy(); // xmlBuffer

			justAttributes[0].Close();
			CleanupStack::Pop(); // justAttribute has been safely closed


			// close the actual array
			justAttributes.Close();
		}
		else if(string.Compare(_L16("ConstructElem")) == 0)
			{

			CSenBaseFragment* frg = CSenBaseFragment::NewL(_L8("BaseFrag"));
			frg->SetReader( *iXmlReader );
			frg->ParseL(KElementXml);

			CSenElement &elem = frg->AsElement();

			iSaxTester = CSAXTester::NewL( elem );
			iSaxTester->SetReader( *iXmlReader );

			delete frg;
		}




	}

    iLog->Log(_L("### _S_NewSAXFragment -> finished ###"));

	return KErrNone;
	}
TInt CWSTester::_S_SAXTest( CStifItemParser& aItem )
	{

    iLog->Log(_L("### _S_SAXTest -> started ###"));

	HBufC8* xmlBuffer = iSaxTester->AsXmlL();
	CleanupStack::PushL( xmlBuffer );
	iLog->Log( xmlBuffer->Des() );
	CleanupStack::PopAndDestroy(); // xmlBuffer


    TPtrC string;
    if (!(aItem.GetNextString ( string )))
	{
        if (string.Compare(_L16("KParseLocal")) == 0)
			{
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 85 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseDifferenLocal")) == 0)
			{
			iSaxTester->ParseL( KParseDifferentLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 9 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseLocalNs")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNs() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 72 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseLocalNsDif")) == 0)
			{
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 33 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}

		else if(string.Compare(_L16("KParseLocalNsQ")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseLocalNsQNOK")) == 0)
			{
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 49 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseLocalNsQAttr")) == 0)
			{
			// Yep. This is ok. Atributes are not checked.
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 119 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseLocalNsQAttrElem")) == 0)
			{
			// Yep. This is ok. Atributes are not checked.
			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			iSaxTester->ParseL( KParseLocalNsQ() );


			xmlBuffer = iSaxTester->AsElement().Parent()->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length2 = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 119
					&& length2 == 11)
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KParseElem")) == 0)
			{
			// Yep. This is ok. Atributes are not checked.
			iSaxTester->ParseL( KParseElemMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 52 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KCheckGetters")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				TBool passed = ETrue;

				const TDesC8 &localName = iSaxTester->LocalName();
				iLog->Log( localName );
				if ( localName != KLName8 )
						passed = EFalse;

				const TDesC8 &nsUri = iSaxTester->NsUri();
				iLog->Log( nsUri );
				if ( nsUri != KDefaultNS )
						passed = EFalse;

				const TDesC8 &nsPrefix = iSaxTester->NsPrefix();
				iLog->Log( nsPrefix );
				if ( nsPrefix != KNsPrefix )
						passed = EFalse;


				if ( passed )
					{
					return KErrNone;
					}
				else
					{
					return KErrGeneral;
					}
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KCheckElement")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				CSenElement &elem = iSaxTester->AsElement();
				HBufC8* elemAsXml = elem.AsXmlL();
				if (elemAsXml->Length() != length )
					{
					delete elemAsXml;
					return KErrGeneral;
					}
				delete elemAsXml;
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KCheckExtract")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				CSenElement* elem = iSaxTester->ExtractElement();
				HBufC8* elemAsXml = elem->AsXmlL();
				if (elemAsXml->Length() != length )
					{
					delete elemAsXml;
					delete elem;
					return KErrGeneral;
					}
				delete elemAsXml;
				delete elem;
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KCheckReader")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				CSenXmlReader* reader = iSaxTester->Reader();
				if (reader != iXmlReader)
					{
					return KErrGeneral;
					}
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KCheckBuildFrom")) == 0)
			{
			TInt err = iSaxTester->BuildFrom( KParseLocalMsg() );
			if ( err != KErrNone )
				{
				return KErrGeneral;
				}
			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 85 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KDelegateParsing")) == 0)
			{
			iSaxTester->SetRunCase(KDelegateParsing );
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,1) && length == 54 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KDelegateParsing2")) == 0)
			{
			iSaxTester->SetRunCase( KDelegateParsing2 );
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,1) && length == 85 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KParseWith")) == 0)
			{
			iSaxTester->ParseWithL( *iXmlReader );
			iSaxTester->ParseL( KParseLocalMsg() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 85 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KSetOwner")) == 0)
			{
			iSaxTester->ParseL( KParseLocalMsg() );

			CSenBaseFragment *frg = CSenBaseFragment::NewL( KLName8() );
			frg->SetReader( *iXmlReader );
			frg->ParseL(KParseLocalMsg() );
			frg->SetOwner( *iSaxTester );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(3,3,2) && length == 85 )
				{
				delete frg;
				return KErrNone;
				}
			else
				{
				delete frg;
				return KErrGeneral;
				}
			}

		else if(string.Compare(_L16("KNameSpaceGetter")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// not owned. null check tells if found(!)
			CSenNamespace* ns = iSaxTester->Namespace( KNsPrefix() );
			iLog->Log( ns->Prefix() );
			iLog->Log( ns->URI() );

			if ( ns->URI() != KDefaultNS() )
				{
				return KErrGeneral;
				}

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
		else if(string.Compare(_L16("KEnsureNS")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNsQ() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			iSaxTester->EnsureNamespace( KNsPrefix() );

			// not owned
			CSenNamespace* ns = iSaxTester->Namespace( KNsPrefix() );
			iLog->Log( ns->Prefix() );
			iLog->Log( ns->URI() );

			if ( ns->URI() != KDefaultNS() )
				{
				return KErrGeneral;
				}

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 96 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}
        else if (string.Compare(_L16("KDetach")) == 0)
			{

			iSaxTester->SetRunCase( KDetach );
			iSaxTester->ParseL( KParseLocalNs() );

			// not owned, before detaching
			CSenBaseFragment* frg = iSaxTester->GetDelegate();			
			xmlBuffer = frg->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// not owned
			CSenBaseFragment* frg2 = iSaxTester->GetDetachedDelegate();
			
			xmlBuffer = frg2->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length2 = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			
				// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			if ( iSaxTester->CheckParsingStatus(2,2,0) && length == 31 && length2 == 55 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}

			}
        else if (string.Compare(_L16("KResetContent")) == 0)
			{
			iSaxTester->ParseL( KParseLocalNs() );

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			TInt length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			iSaxTester->ResetContentL();

			xmlBuffer = iSaxTester->AsXmlL();
			CleanupStack::PushL( xmlBuffer );
			iLog->Log( xmlBuffer->Des() );
			length = xmlBuffer->Length();
			CleanupStack::PopAndDestroy(); // xmlBuffer

			// using hard coded numbers. Do not change the PARSED MESSAGES!!!
			// AFTER RESETTING CONTENT
			if ( iSaxTester->CheckParsingStatus(2,2,1) && length == 33 )
				{
				return KErrNone;
				}
			else
				{
				return KErrGeneral;
				}
			}

	}

    iLog->Log(_L("### _S_SAXTest -> ended ###"));
	return KErrNone;
	}

/*-----------------------------------------------
            DOM based parsing
-----------------------------------------------*/

CSenElement* CWSTester::ParseToElementL( HBufC8* pTestXml )
{
	CSenDomFragment* pDom = CSenDomFragment::NewL();
    CleanupStack::PushL(pDom);
	pDom->SetReader(*iXmlReader);

	iXmlReader->SetContentHandler(*pDom);

	//parse xml fragment
	iXmlReader->ParseL( *pTestXml );

	//parsed document to SenElement
	CSenElement* element = pDom->ExtractElement();
    CleanupStack::PopAndDestroy();
	return element;
}

TInt CWSTester::_C_ParseToElement( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_ParseToElement -> Started!********"));
	iLog->Log(_L("Parse XML document"));

	TPtrC arg;
	TBuf8<3072> xmlDoc;
	aItem.GetNextString(arg);

	xmlDoc.Append(arg);

	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		xmlDoc.Append(_L(" "));
		xmlDoc.Append(arg);
	}

	HBufC8* pTestXml = HBufC8::NewLC(
		xmlDoc.Length() );

	pTestXml->Des().Append(xmlDoc);

	iLog->Log( _L(" Delete old iElement") );

	//removing old iElement
	if ( iElement!= NULL ){
        delete iElement;
	    iElement = NULL;
    }

	//adding parsed document to SenElement
    iLog->Log( _L(" Parsing message....") );
	
	TRAPD(rParse, iElement = ParseToElementL(pTestXml));
	if (rParse != KErrNone) {
		iLog->Log(_L("Parsing failed. Error: %d"), rParse );
		CleanupStack::PopAndDestroy();
		return rParse;
	}

//===============

    CleanupStack::PopAndDestroy();

	iLog->Log(_L("******* _C_ParseToElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_LocalName( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_LocalName -> Started!********"));

	LogResultL(iElement->LocalName());

	iLog->Log(_L("******* _S_LocalName -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_NamespaceURI( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_NamespaceURI -> Started!********"));

	const TDesC8* nsUri = &(iElement->NamespaceURI());
	LogResultL(*nsUri);

	iLog->Log(_L("******* _S_NamespaceURI -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_SetNamespace( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_SetNamespace -> Started!********"));

	//1. argument - NsUri, 2. argument - NsPrefix
	TPtrC8 arg1 = GetParC(aItem);
	TPtrC arg2;
	TBuf8<128> nsUri;
	nsUri.Append(arg1);

	if (!(aItem.GetNextString(arg2))) {
		TBuf8<128> nsPrefix;
		nsPrefix.Append(arg2);
		iElement->SetNamespaceL(nsPrefix, nsUri);
	} else
		iElement->SetNamespaceL(nsUri);

	CleanupStack::PopAndDestroy();
	iLog->Log(_L("******* _S_SetNamespace -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_NsPrefix( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_NsPrefix -> Started!********"));

	LogResultL(iElement->NsPrefix());

	iLog->Log(_L("******* _S_NsPrefix -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_SetPrefix ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_SetPrefix -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> prefix;
	prefix.Append(arg);

	((CSenBaseElement*)iElement)->SetPrefixL(prefix);

	iLog->Log(_L("******* _S_SetPrefix -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_HasContent( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_HasContent -> Started!********"));

	if (iElement->HasContent() == EFalse)
		error = KErrNotFound;

	iLog->Log(_L("******* _S_HasContent -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_Content( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_Content -> Started!********"));

	LogResultL(iElement->Content());

	iLog->Log(_L("******* _S_Content -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_ContentUnicode( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_ContentUnicode -> Started!********"));

	HBufC16* xmlBuffer = iElement->ContentUnicodeL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	iLog->Log(_L("******* _S_ContentUnicode -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_SetContent( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_SetContent -> Started!********"));

	TPtrC arg;
	TBuf8<1024> content;
    aItem.GetNextString(arg);
	content.Append(arg);

	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		content.Append(_L(" "));
		content.Append(arg);
	}

	iElement->SetContentL(content);

	iLog->Log(_L("******* _S_SetContent -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AddNamespace( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_AddNamespace -> Started!********"));

	//1. argument - NsUri, 2. argument - NsPrefix, 3. argument - CheckInParent (TRUE/FALSE) - it causes call overloaded function
	TPtrC8 arg1 = GetParC(aItem);
	TPtrC arg2;
	TPtrC arg3;
	TBuf8<128> nsUri;
	nsUri.Append(arg1);
	TBuf8<128> nsPrefix;
	aItem.GetNextString(arg2);
	nsPrefix.Append(arg2);

	if (!(aItem.GetNextString(arg3))) {
		TBool checkInParent = FALSE;
		if (arg3.Compare(_L("TRUE")) == 0) checkInParent = TRUE;
		CSenNamespace* ns = CSenNamespace::NewLC(nsPrefix, nsUri);
		iElement->AddNamespaceL(*ns, checkInParent);
		CleanupStack::PopAndDestroy();
	} else
		iElement->AddNamespaceL(nsPrefix, nsUri);

	CleanupStack::PopAndDestroy();

	iLog->Log(_L("******* _S_AddNamespace -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AttrValue( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_AttrValue -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	const TDesC8* attr = iElement->AttrValue(name);
	if (attr != NULL)
		LogResultL(*attr);

	iLog->Log(_L("******* _S_AttrValue -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AddAttr( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_AddAttr -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);
    aItem.GetNextString(arg);
	TBuf8<1024> value;
	value.Append(arg);
	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		value.Append(_L(" "));
		value.Append(arg);
	}

	iElement->AddAttrL(name, value);

	iLog->Log(_L("******* _S_AddAttr -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Namespace( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Namespace -> Started!********"));

	const CSenNamespace* ns = iElement->Namespace();

	TBuf8<1024> output;
	if (ns) {
		output.Append(ns->URI());
		output.Append(_L8(", "));
		output.Append(ns->Prefix());
	} else
		output.Append(_L8("no namespaces"));
	LogResultL(output);

	iLog->Log(_L("******* _C_Namespace -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Elements( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Elements -> Started!********"));

	RPointerArray<CSenElement> elements = iElement->ElementsL();

	TBuf8<1024> output;
	TInt size = elements.Count();
	for (TInt i = 0; i < size; i++) {
		output.Append(_L("\n"));
		output.Append(elements[i]->LocalName());
	}
	LogResultL(output);

	iLog->Log(_L("******* _C_Elements -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Attributes( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Attributes -> Started!********"));

	RPointerArray<CSenBaseAttribute> attributes = iElement->AttributesL();

	TBuf8<1024> output;
	TInt size = attributes.Count();
	for (TInt i = 0; i < size; i++) {
		output.Append(_L("\n"));
		output.Append(attributes[i]->Name());
	}
	LogResultL(output);

	iLog->Log(_L("******* _C_Attributes -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Namespaces( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Namespaces -> Started!********"));

	RPointerArray<CSenNamespace> namespaces = iElement->NamespacesL();

	TBuf8<1024> output;
	TInt size = namespaces.Count();
	for (TInt i = 0; i < size; i++) {
		output.Append(_L("\n"));
		output.Append(namespaces[i]->URI());
		output.Append(_L8(", "));
		output.Append(namespaces[i]->Prefix());
	}
	LogResultL(output);

	iLog->Log(_L("******* _C_Namespaces -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Parent( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Parent -> Started!********"));


	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	//set pointer to child
	CSenElement* childElement;
	childElement = iElement->Element(name);

	//get parent from child
	if (!(childElement->Parent() == iElement))
		error = KErrGeneral;

	iLog->Log(_L("******* _C_Parent -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Root( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Root -> Started!********"));

	TPtrC arg;
	CSenElement* secondElement;

	//optional argument
	if (!(aItem.GetNextString(arg))) {
		TBuf8<128> name;
		name.Append(arg);
		//set pointer to child
		secondElement = iElement->Element(name);
	} else
		secondElement = iElement;

	//get root
	if (!(&secondElement->Root() == iElement))
		error = KErrGeneral;

	iLog->Log(_L("******* _C_Root -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_SetParent( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_SetParent -> Started!********"));

	//new element
	CSenBaseElement* parentElement = CSenBaseElement::NewL(_L8("NewRoot"));
	CleanupStack::PushL(parentElement);
	//add namespace
	parentElement->SetNamespaceL(_L8("nok"), _L8("http://www.nokia.com"));

	//set parent
	iElement->SetParent(parentElement);

	HBufC8* xmlBuffer = iElement->Parent()->AsXmlL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	xmlBuffer = iElement->AsXmlL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	CleanupStack::PopAndDestroy(); // parentElement

	iLog->Log(_L("******* _C_SetParent -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Element ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Element -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	CSenElement* element = iElement->Element(name);

	if (element != NULL)
	{
		HBufC8* xmlBuffer = element->AsXmlL();
		CleanupStack::PushL( xmlBuffer );
		LogResultL( *xmlBuffer );
		CleanupStack::PopAndDestroy(); // xmlBuffer
	}

	iLog->Log(_L("******* _C_Element -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_CreateElement ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_CreateElement -> Started!********"));

	TPtrC arg1, arg2;
	//first param
    aItem.GetNextString(arg1);
	TBuf8<128> localName;
	localName.Append(arg1);
	//second param
    aItem.GetNextString(arg2);
	TBuf8<128> nsPrefix;
	nsPrefix.Append(arg2);

	//create element and dump it
	CSenElement* element = iElement->CreateElementL(nsPrefix, localName);
	CleanupStack::PushL(element);

	if (element != NULL)
	{
		HBufC8* xmlBuffer = element->AsXmlL();
		CleanupStack::PushL( xmlBuffer );
		LogResultL( *xmlBuffer );
		CleanupStack::PopAndDestroy(); // xmlBuffer
	}

	CleanupStack::PopAndDestroy();

	iLog->Log(_L("******* _C_CreateElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_InsertElement ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_InsertElement -> Started!********"));

	TPtrC arg;
	//param
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	//create new element
	CSenElement* element1 = CSenBaseElement::NewL(_L8("NewElement"));
	CSenElement* element2 = iElement->Element(name);

	//insert element
	iElement->InsertElementL(*element1, *element2);

	iLog->Log(_L("******* _C_InsertElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AddElement ( CStifItemParser& aItem )
{
	iLog->Log(_L("*******_S_AddElement -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	TRAPD(error, iElement->AddElementL(name));

	iLog->Log(_L("******* _S_AddElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_RemoveElement ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_RemoveElement -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	CSenElement* element;
	element = iElement->RemoveElement(name);

	if ( element!= NULL ){
        delete element;
	    element = NULL;
    }

	iLog->Log(_L("******* _S_RemoveElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_ReplaceElement ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_ReplaceElement -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	CSenElement* oldElement;
	CSenElement* element = CSenBaseElement::NewL(name);

	oldElement = iElement->ReplaceElementL(*element);

	CleanupStack::PushL(oldElement);
	CleanupStack::PopAndDestroy(); // oldElement
	iLog->Log(_L("******* _C_ReplaceElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AsXml( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_AsXml -> Started!********"));

	HBufC8* xmlBuffer = iElement->AsXmlL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	iLog->Log(_L("******* _S_AsXml -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_AsXmlUnicode( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_AsXmlUnicode -> Started!********"));

	HBufC16* xmlBuffer = iElement->AsXmlUnicodeL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	iLog->Log(_L("******* _S_AsXmlUnicode -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_AsElement( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_AsElement -> Started!********"));

	HBufC8* xmlBuffer = iElement->AsElement()->AsXmlL();
	CleanupStack::PushL( xmlBuffer );
	LogResultL( *xmlBuffer );
	CleanupStack::PopAndDestroy(); // xmlBuffer

	iLog->Log(_L("******* _C_AsElement -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_CopyFrom( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_CopyFrom -> Started!********"));

	TPtrC arg;
	TBuf8<3072> xmlDoc;
	aItem.GetNextString(arg);

	xmlDoc.Append(arg);

	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		xmlDoc.Append(_L(" "));
		xmlDoc.Append(arg);
	}

	HBufC8* pTestXml = HBufC8::NewLC( xmlDoc.Length() );
	pTestXml->Des().Append(xmlDoc);
	CSenElement* newElement = NULL;

	//adding parsed document to SenElement and copy it to iElement

	TRAPD(rParse, newElement = ParseToElementL(pTestXml));
	if (rParse != KErrNone) {
		iLog->Log(_L("Parsing failed. Error: %d"), rParse );
		CleanupStack::PopAndDestroy();
		return rParse;
	}
	CleanupStack::PushL(newElement);

	iElement->CopyFromL( *newElement );

    CleanupStack::PopAndDestroy(2);
	iLog->Log(_L("******* _C_CopyFrom -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Detach ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Detach -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	//pointer to the child
	CSenElement* element = iElement->Element(name);
	CleanupStack::PushL(element);

	element->DetachL();

	CleanupStack::PopAndDestroy();

	iLog->Log(_L("******* _C_Detach -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_Child ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_Detach -> Started!********"));

	TInt arg;
    aItem.GetNextInt(arg);

	CSenElement* element = iElement->Child(arg);

	if (element)
	{
		HBufC8* xmlBuffer = element->AsXmlL();
		CleanupStack::PushL( xmlBuffer );
		LogResultL( *xmlBuffer );
		CleanupStack::PopAndDestroy(); // xmlBuffer
	}

	iLog->Log(_L("******* _C_Child -> Ended! ********"));
	return error;
}


TInt CWSTester::_S_Set ( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_Set -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> nsUri;
	nsUri.Append(arg);

    aItem.GetNextString(arg);
	TBuf8<128> localName;
	localName.Append(arg);

    aItem.GetNextString(arg);
	TBuf8<128> qName;
	qName.Append(arg);

	iElement->Set(nsUri, localName, qName);

	iLog->Log(_L("******* _S_Set -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_WriteAsXMLTo( CStifItemParser& aItem ){
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_WriteAsXMLTo -> Started!********"));

	iElement->WriteAsXMLToL(iWriter);

	iWriter.CommitL();

	iLog->Log(_L("******* _S_WriteAsXMLTo -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_WriteNamespacesTo( CStifItemParser& aItem ){
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_WriteNamespacesTo -> Started!********"));

	iElement->WriteNamespacesToL(iWriter);

	iWriter.CommitL();

	iLog->Log(_L("******* _S_WriteNamespacesTo -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_ContentWriteStream( CStifItemParser& aItem ){
	TInt error = KErrNone;
	iLog->Log(_L("*******_S_ContentWriteStream -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<1024> content;
	content.Append(arg);

	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		content.Append(_L(" "));
		content.Append(arg);
	}

	RWriteStream writer = iElement->ContentWriteStreamL();

	writer.WriteL(content);

	writer.CommitL();
	writer.Close();

	iLog->Log(_L("******* _S_ContentWriteStream -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_AllocAttrValue ( CStifItemParser& aItem )
{
	//NOT COMPLETED!
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_AllocAttrValue -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);

	/*RAttributeArray apAttrs;

	HBufC8* newAttr;

	TRAPD(r, newAttr = ((CSenBaseElement*)iElement)->AllocAttrValueL(apAttrs, name) );
	if (r != KErrNone) {
		iLog->Log(_L("AllocAttrValueL failed. Error: %d"), r );
		return r;
	}

	if ( newAttr!= NULL ){
        delete newAttr;
	    newAttr = NULL;
    }*/

	iLog->Log(_L("******* _C_AllocAttrValue -> Ended! ********"));
	return error;
}

TInt CWSTester::_S_SetAttributes ( CStifItemParser& aItem )
{
	//NOT COMPLETED!
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_AllocAttrValue -> Started!********"));

	TPtrC arg;
    aItem.GetNextString(arg);
	TBuf8<128> name;
	name.Append(arg);
	RAttributeArray apAttrs;
	RStringPool stringPool;
/*
	RAttribute nsAttribute;
	CleanupClosePushL(nsAttribute);
	nsAttribute.Open(stringPool.OpenStringL(_L8("uri")),
					stringPool.OpenStringL(_L8("prefix")), //iStringPool.OpenStringL(KNullDesC8()),  //iXmlNsPrefix.Copy(),
					stringPool.OpenStringL(_L8("prefix")),
					stringPool.OpenStringL(_L8("uri")));

	apAttrs.Append(nsAttribute);

	((CSenBaseElement*)iElement)->SetAttributesL(apAttrs);
*/
	iLog->Log(_L("******* _C_AllocAttrValue -> Ended! ********"));
	return error;
}


/*################################################
            MService Consumer implementation
################################################*/

void CWSTester::HandleMessageL(const TDesC8& aMessage)
	{
	_LIT8(	KEnv, "<S:Envelope");
	_LIT8(	KBody, "<ab:QueryResponse");
	TInt aFound = 0;

    iLog->Log(_L("### HandleMessageL -> Started! ###"));
    iLog->Log(_L("Retrieved: "));
    //LogResultL( _L8("=================message===================") );
	LogResultL( aMessage );



	if ( iStarted )
		{
		// Received a response from server. This do not take any statement about
		// the received message.
		//iErrorsOccured = EFalse;
	// Here is the Switch - Case for checking the error values for cases. The
	// iCasePassed is checked in
		switch( iRunCase )
			{
			case KNoResourceID:
				iLog->Log( _L("The message from service received. Ok." ));
				iCasePassed = TRUE;
				break;
			case KDSEcrypted:
				iLog->Log( _L("The message from service received. Ok." ));
				iCasePassed = TRUE;
				break;

			case KEndpointMovedTest:
				iLog->Log( _L("The message from service received. Ok." ));
				iCasePassed = TRUE;
				break;

			case KMultippleROs:
				iLog->Log( _L("The message from service received. Ok." ));
				iCasePassed = TRUE;
				break;
			case KNoCredentials:
				iLog->Log( _L("The message from service received. Ok." ));
				iCasePassed = TRUE;
				break;
		    case KSendSOAP:
			    iLog->Log( _L("Received a whole SOAPEnvelope." ));
                iCasePassed = TRUE;
			    //TestParsing( aMessage );
			    break;
   		    case KSendSOAPRequest:
			    iLog->Log( _L("The message from service received. Ok." ));
                iCasePassed = TRUE;
			    break;
   		    case KSendBUCF:
			    iLog->Log( _L("Received a response from server. Thats enough." ));
                iCasePassed = TRUE;
			    //TestParsing( aMessage );
			    break;
		    case KSendYomi:
			    iLog->Log( _L("Received a response from server. Thats enough." ));
                iCasePassed = TRUE;
			    //TestParsing( aMessage );
			    break;
		    case KSendBUCFBody:
				iLog->Log( _L("Received a response from server. Checking if it starts correctly..." ));
			    aFound = aMessage.Find( KBody );
				if ( aFound == 0)
					{
					iCasePassed = TRUE;
					}
				else
					{
					iCasePassed = FALSE;
					}
			    break;
		    case KSendBUCFEnvelope:
				iLog->Log( _L("Received a response from server. Checking if it starts correctly..." ));
				aFound = aMessage.Find( KEnv() );
				if ( aFound == 0)
					{
					iCasePassed = TRUE;
					}
				else
					{
					iCasePassed = FALSE;
					}
			    break;
		    case KSendBUCFEnvelope2:
				iLog->Log( _L("Received a response from server. Checking if it starts correctly..." ));
				aFound = aMessage.Find( KEnv() );
				if ( aFound == 0)
					{
					iCasePassed = TRUE;
					}
				else
					{
					iCasePassed = FALSE;
					}
			    break;
   		    case KPerformanceSOAPSubmit:
			    iLog->Log( _L("The message from service received. Ok." ));
                iCasePassed = TRUE;
			    break;	
   		    case KCompleteServerMessagesOnOff_1:
   		        iLog->Log( _L("The message from service received. Ok." ));
   		        iCasePassed = TRUE;
   		        break;  			                    
            case KCompleteServerMessagesOnOff_3:
                iLog->Log( _L("The message from service received. Ok." ));
                iCasePassed = TRUE;
                break;                                  
            
            default:
				iCasePassed = FALSE;
				break;
			}
    HBufC8* aSD;

    iLog->Log( _L("Getting the services description....") );
	iSenServiceConnection->ServiceDescriptionL( aSD );
    //LogResultL( _L8("=================description===================") );
    //LogResultL( *aSD );

	delete aSD;

		iLog->Log(_L("Stopping active scheduler..."));
		CActiveScheduler::Stop();
		iStarted = EFalse;
		}

	iLog->Log(_L("******* HandleMessageL -> Finished! ********"));

	}
void CWSTester::HandleErrorL(const int aErrorCode, const TDesC8& aError)
//void CWSTester::HandleErrorL(const TDesC8& aError)
	{
	iLog->Log(_L("******* HandleErrorL -> Started! ********"));

	iLog->Log(_L("Run case: %d"), iRunCase );
	iLog->Log(_L("Retrieved: "));
  LogResultL(_L8("HandleErrorL"));
	//LogResultL( aError );
    TBuf8<20> buf;
    _LIT8(KError, "Error code: %d");
    buf.Format(KError, aErrorCode);
    LogResultL(buf);


	// Here is the Switch - Case for checking the error values for cases. The
	// iCasePassed is checked after the ActiveSceduler is stopped in _X_WaitForResponse
	// Note that every case now fails because the ErrorCodes are not cheked. All the
	// cases must be checked with codes (Niko).
	if ( iStarted )
		{
		switch( iRunCase ) {
			case KSendInvalidXML:
				iLog->Log( _L("Invalid XML send. Should become an html error..." ));
				if ( aErrorCode == 500 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = TRUE;
					}
				else
					{
					// Failed. Invalid code.
					iCasePassed = FALSE;
					}
				break;
			case KSendInvalidSOAP:
				iLog->Log( _L("The service is unreachable for now! Thats ok" ));
				if ( aErrorCode == KErrSenSoapFault )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = TRUE;
					}
				else
					{
					iCasePassed = FALSE;
					}
				break;
			case KInvalidHost:
				iLog->Log( _L("The service is unreachable for now! Thats ok" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KInvalidLogin:
				iLog->Log( _L("The user should have cancelled the login dialog" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KSendASAbort:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KTruncate:
				iLog->Log( _L("The AS aborted now" ));
				iCasePassed = EFalse;
				break;
			case KLowercase:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KUppercase:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KUppercaseAllowed:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KAllowUpperTrun:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KAllowedUppercase:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KAllowed:
				iLog->Log( _L("The AS should have been aborted now" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KInvalidCredentials:
				iLog->Log( _L("The error should have been received when Credentials are invalid" ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KUnexistantDS:
				iLog->Log( _L("Unexistant DS case. Error should have been received..." ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KDSFailed:
				iLog->Log( _L("DS returns fail code..." ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KDSFailedResult:
				iLog->Log( _L("DS returns fail code..." ));
				if ( aErrorCode == -1 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = ETrue;
					}
				else
					{
					iCasePassed = EFalse;
					}
				break;
			case KInvalidWSP:
				iLog->Log( _L("WSP returns fail code..." ));
				if ( aErrorCode == 502 )//2.8 versin( aErrorCode == -5120 )
					{
					iLog->Log( _L("The received error was correct!" ));
					iCasePassed = TRUE;
					}
				else
					{
					iCasePassed = FALSE;
					}
				break;
			case KNonSupportedAM:
					iLog->Log( _L("The service is unreachable for now! Thats ok" ));
					if ( aErrorCode == -1 )
						{
						iLog->Log( _L("The received error was correct!" ));
						iCasePassed = TRUE;
						}
					else
						{
						iCasePassed = FALSE;
						}
					break;
			    case KServerError500:
				    iLog->Log( _L("WSP returns server error 500..." ));
				    if ( aErrorCode == 500 )
					    {
					    iLog->Log( _L("The received error 500 from WSP was expected!" ));
					    iCasePassed = TRUE;
					    }
				    else
					    {
					    iCasePassed = FALSE;
					    }
				    break;
			    case KServerError404:
				    iLog->Log( _L("WSP returns server error 404..." ));
				    if ( aErrorCode == 404 )
					    {
					    iLog->Log( _L("The received error 404 from WSP was expected!" ));
					    iCasePassed = TRUE;
					    }
				    else
					    {
					    iCasePassed = FALSE;
					    }
				    break;
	            case KCompleteServerMessagesOnOff_1:
	                iLog->Log( _L("The message from service received. NOT Ok." ));
	                iCasePassed = FALSE;
	                break;                                  
	            case KCompleteServerMessagesOnOff_3:
	                iLog->Log( _L("The message from service received. NOT Ok." ));
	                iCasePassed = FALSE;
	                break;                                  
				   
			default:
				iCasePassed = EFalse;
				break;
	      }
		iLog->Log(_L("Stopping active scheduler..."));
		CActiveScheduler::Stop();
		iStarted = EFalse;
		}

	iLog->Log(_L("******* HandleErrorL -> Finished! ********"));
	}
void CWSTester::SetStatus(const TInt aStatus)
	{

		iLog->Log(_L("******* SetStatus -> Started! ********"));
		iLog->Log(_L("Status: %d"), aStatus);

	switch( aStatus )
		{
		case 0: // new connection
			iLog->Log( _L("New Connection (status==0)" ));
			break;

		case 1: // connection initialized
			iLog->Log( _L("Connection initialized (status==1)" ));
			if ( iRunCase == KSendBUCF )
				{
				iLog->Log( _L("KSendBUCF: connection initialized!" ));
				SendBUCFTest();
				}
			else if ( iRunCase == KEndpointMovedTest )
				{
				iLog->Log( _L("KEndpointMovedTest: connection initialized!" ));
				SendBUCFTest();
				}
			else if ( iRunCase == KSendInvalidSOAP)
				{
				iLog->Log( _L("KSendInvalidSOAP: connection initialized!" ));
				SendInvalidSOAPTest();
				}
			else if ( iRunCase == KTruncate)
				{
				iLog->Log( _L("KTruncate: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KSendInvalidXML )
				{
				iLog->Log( _L("KSendInvalidXML: connection initialized!" ));
				SendInvalidTest();
				}
			else if ( iRunCase == KNoResourceID )
				{
				iLog->Log( _L("KNoResourceID: connection initialized!" ));
				SendBUCFBodyTest();
				}
			else if ( iRunCase == KDSEcrypted )
				{
				iLog->Log( _L("KDSEcrypted: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KFacets )
				{
				iLog->Log( _L("KFacets: connection initialized!" ));
				TInt value = TestFacets();
				if (value == KErrNotFound)
					{
					iLog->Log( _L("KFacets: FAILED!" ));
					iCasePassed = FALSE;
					}
				else
					{
					iLog->Log( _L("KFacets: SUCCES!" ));
					iCasePassed = TRUE;
					}
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KSendBUCFBody )
				{
				iLog->Log( _L("KSendBUCFBody: connection initialized!" ));
				SendBUCFBodyTest();
				}
			else if ( iRunCase == KMultippleROs )
				{
				iLog->Log( _L("KConnectAS: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KSendBUCFEnvelope )
				{
				iLog->Log( _L("KSendBUCFEnvelope: connection initialized!" ));
				SendBUCFEnvelopeTest();
				}
			else if ( iRunCase == KSendBUCFEnvelope2 )
				{
				iLog->Log( _L("KSendBUCFEnvelope: connection initialized!" ));
				SendBUCFEnvelopeTest2();
				}
			else if ( iRunCase == KConnectAS )
				{
				iLog->Log( _L("KConnectAS: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KLowercase )
				{
				iLog->Log( _L("KLowercase: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KUppercase )
				{
				iLog->Log( _L("KUppercase: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KAllowed )
				{
				iLog->Log( _L("KAllowed: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KAllowedUppercase )
				{
				iLog->Log( _L("KAllowedUppercase: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KUppercaseAllowed )
				{
				iLog->Log( _L("KUppercaseAllowed: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KAllowUpperTrun )
				{
				iLog->Log( _L("KAllowUpperTrun: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KInitialOK )
				{
				iLog->Log( _L("KInitialOK: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KBasicDisco )
				{
				iLog->Log( _L("KBasicDisco: connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
						else if ( iRunCase == KInvalidWSP )
				{
				iLog->Log( _L("KInvalidWSP : connection initialized!" ));
				SendBUCFTest();

				}
			else if ( iRunCase == KNoCredentials )
				{
				iLog->Log( _L("KNoCredentials : connection initialized!" ));
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KSendYomi)
				{
				iLog->Log( _L("KSendYomi : connection initialized!" ));
				SendYomiTest();
				}
			else if ( iRunCase == KServerError404)
				{
				SendSOAPRequest(NULL);
				}
			else if ( iRunCase == KServerError500)
				{
				SendSOAPRequest(NULL);
				}
			else if ( iRunCase == KCredentialsExpires)
				{
				SendSOAPRequest(310); // argument delay time for credential to expire (value on the tgx should be smaller)
				}
			else if ( iRunCase == KSendSOAP)
				{
                iCasePassed = TRUE;
				SendSOAPTest();
				}
			else if ( iRunCase == KSendSOAPRequest)
				{
				SendSOAPRequest(NULL);
				}
			else if ( iRunCase == KSubmitSOAPRequest)
				{
				SubmitSOAPRequest();
				}
			else if ( iRunCase == KGetConnectionFooContaract)
				{
				 SendSOAPRequest(NULL);
				}
			else if ( iRunCase == KHasFacet)
				{
				CheckConnection();
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KIsReady)
				{
				CheckConnection();
				iCasePassed = TRUE;
				CActiveScheduler::Stop();
				}
			else if ( iRunCase == KCompleteServerMessagesOnOff_1)
				{
                iCasePassed = TRUE;
				iSenServiceConnection->CompleteServerMessagesOnOff(TRUE);
				SendSOAPRequest(NULL);
				}
			else if ( iRunCase == KCompleteServerMessagesOnOff_2)
				{
                iCasePassed = TRUE;
				iSenServiceConnection->CompleteServerMessagesOnOff(TRUE);
				SubmitSOAPRequest();
				}
			else if ( iRunCase == KSubmitSOAP)
				{
                iCasePassed = TRUE;
				SubmitSOAPTest();
				}
			else if ( iRunCase == KCompleteServerMessagesOnOff_3)
				{
                iCasePassed = TRUE;
				iSenServiceConnection->CompleteServerMessagesOnOff(TRUE);
				SendSOAPTest();
				}
			else if ( iRunCase == KCompleteServerMessagesOnOff_4)
				{
                iCasePassed = TRUE;
				iSenServiceConnection->CompleteServerMessagesOnOff(TRUE);
				SubmitSOAPTest();
                }
   			else if ( iRunCase == KPerformanceSOAPSubmit)
				{
                iCasePassed = TRUE;
				PerformanceSOAPSubmit();
                }
			else
				{
                iLog->Log(_L("Stopping active scheduler..."));
				CActiveScheduler::Stop();
				}
			break;
		case 2: // Connection expired
			iLog->Log( _L("Connection Expired(status==2)" ));
			break;
		}
	iLog->Log(_L("******* SetStatus -> Finished! ********"));
	}
/*################################################
            Complex
################################################*/

TInt CWSTester::_C_ParseDomFragment( CStifItemParser& aItem )
{
	TInt error = KErrNone;
	iLog->Log(_L("*******_C_ParseDomFragment -> Started!********"));
	iLog->Log(_L("Parse XML document"));

	TPtrC arg;
	TBuf8<3072> xmlDoc;
	aItem.GetNextString(arg);

	xmlDoc.Append(arg);

	//concatenating args to one string
	while (!(aItem.GetNextString(arg)))
	{
		xmlDoc.Append(_L(" "));
		xmlDoc.Append(arg);
	}

	HBufC8* pTestXml = HBufC8::NewLC(
		xmlDoc.Length() );

	TPtr8 ptr = pTestXml->Des();
	ptr.Append(xmlDoc);

	CSenDomFragment* pDom = CSenDomFragment::NewL();
    CleanupStack::PushL(pDom);
	pDom->SetReader(*iXmlReader);

	iXmlReader->SetContentHandler(*pDom);

	iLog->Log( _L(" Parsing message....") );

	TRAPD(rParse, iXmlReader->ParseL( *pTestXml ));
	if (rParse != KErrNone) {
		error = rParse;
		iLog->Log(_L("Parsing failed. Error: %d"), rParse );
		CleanupStack::PopAndDestroy(2);
		return error;
	}
//AsXML "UTF8"
	HBufC8* pBuf = NULL;

	TRAPD(rAsXml, pBuf = pDom->AsXmlL());
	if (rAsXml != KErrNone)
		iLog->Log(_L("Converting to text failed. Error: %d"), rAsXml );

	CleanupStack::PushL(pBuf);

	iLog->Log( _L("Parsed message:") );
	iLog->Log(*pBuf);
	LogResultL( *pBuf );
//AsXMLUnicode
    HBufC16* pBuf2 = NULL;

	TRAPD(rAsXmlUnicode, pBuf2 = pDom->AsXmlUnicodeL());
	if (rAsXmlUnicode != KErrNone)
		iLog->Log(_L("Converting to text failed. Error: %d"), rAsXmlUnicode );

	CleanupStack::PushL(pBuf2);

	iLog->Log( _L("Parsed message Unicode:") );
	iLog->Log(*pBuf2);
	LogResultL( *pBuf2 );
//===============


    CleanupStack::PopAndDestroy(4);

	iLog->Log(_L("******* _C_ParseDomFragment -> Ended! ********"));
	return error;
}

TInt CWSTester::_C_ParseXMLDocument( CStifItemParser& aItem )
{
	iLog->Log(_L("******* _C_ParseXMLDocument Method ********"));
	iLog->Log(_L("Parse XML document"));
	// Get file nmae from script
	TPtrC srcFile;
	aItem.GetNextString(srcFile);
    // Create DOM Element
//__UHEAP_MARK;
	CSenDomFragment* pDom = CSenDomFragment::NewL();
    CleanupStack::PushL(pDom);
	pDom->SetReader(*iXmlReader);
	iXmlReader->SetContentHandler(*pDom);
	// Open session to file
	RFs fsSession;
	CleanupClosePushL(fsSession);
	// Check if FS is aviable
	TRAPD(rFile, fsSession.Connect(););
	if (rFile != KErrNone) {
		// Test case not passed
	    // Unable to connect to FS
		iLog->Log(_L("Unable to connect to FS. Error: %d"), rFile);
		LogResultL(_L8("Unable to connect to FS" ));
		CleanupStack::PopAndDestroy(2);
		return KErrGeneral;
	}
	// Parse the document specified by file
	TRAPD(rParse, iXmlReader->ParseL(fsSession, srcFile));
	if (rParse != KErrNone) {
		// Test case not passed
	    // Parser error
		TBuf8<25> buf;
		_LIT8(KFound, "Parse error: %d");
		buf.Format(KFound, rParse);
		iLog->Log(buf);
		LogResultL(buf);
		CleanupStack::PopAndDestroy(2);
		return KErrNone;
	}
#ifdef _PROFILE
	_X_TimeOut(aItem);
#endif

	// close fsSession
	CleanupStack::PopAndDestroy();
	// Store parsed documed in file
	HBufC8* pBuf = NULL;
	// Convert document
	TRAPD(rAsXml, pBuf = pDom->AsXmlL());
	if (rAsXml != KErrNone)
		iLog->Log(_L("Converting to text failed. Error: %d"), rAsXml );
	//Write information to log files
	CleanupStack::PushL(pBuf);
	iLog->Log( _L("Parsed message:") );
	iLog->Log(*pBuf);
	LogResultL( *pBuf );
	// Clean stack
	CleanupStack::PopAndDestroy(2);
//__UHEAP_MARKEND;
	// Return function
	iLog->Log(_L("******* _C_ParseXMLDocument -> Ended! ********"));
		return KErrNone;
}

/*################################################
            Common
################################################*/

TInt CWSTester::_X_DeBase64( CStifItemParser& aItem )
    {

	_LIT8(KEndOfLine,"\n");


    TPtrC aSrcFile;
    TPtrC aRefFile;
    iLog->Log(_L("******* _X_DeBase64-> Started! ********"));
    if (iErrorsOccured==TRUE) {
        iLog->Log(_L("### iErrorsOccured==TRUE###"));
        return KErrGeneral;
    }
    aItem.GetNextString ( aSrcFile );
    aItem.GetNextString ( aRefFile );

	RFile srcFile;
    RFile referenceFile;


    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    TFileName referenceFileName;
	referenceFileName.Copy(aRefFile);

    TInt nResult = KResultOK;


    if(srcFile.Open(iFsSession, srcFileName, EFileRead) == KErrNone)
	    {
		// Does reference file exist.
		if(referenceFile.Open(iFsSession, referenceFileName, EFileRead) == KErrNone)
		    {
	        // Integer variables for compare to length of files (result and reference).
	        TInt resSize;
	        TInt refSize;
	        srcFile.Size(resSize);
	        referenceFile.Size(refSize);

			HBufC8* resBuf = HBufC8::NewLC(resSize) ;
			HBufC8* refBuf = HBufC8::NewLC(refSize) ;

			TPtr8 resPtr = resBuf->Des();
			TPtr8 refPtr = refBuf->Des();

			nResult = KResultOK;
			// Read result file
			srcFile.Read(resPtr);

			// Read reference file
			referenceFile.Read(refPtr);
			// Checking the <data> value and modify the refPtr
			// to match the case

			HBufC8* pDecodedBuf8 = DeBase64DescL(resPtr); // ownership transfered!!
			CleanupStack::PushL( pDecodedBuf8 );

			if ( pDecodedBuf8 == NULL )
				{
				nResult = KResultFailedFound;
				CleanupStack::PopAndDestroy(3); //pDecodedBuf8, refBuf, resBuf
				referenceFile.Close();
				srcFile.Close();
				return nResult;
				}

			TPtr8 pDecoded = pDecodedBuf8->Des();


			TInt endOfLine;
			TInt startPos;
			TInt delDength;

			// Do the checking
			while (TRUE)
				{
				endOfLine = refPtr.Find(KEndOfLine);
				if ( refPtr.Length() == 0 )
					{
					break;
					}
				TPtr8 searchPtr = refPtr.AllocLC()->Des();
				if ( endOfLine != KErrNotFound )
					{
					startPos = endOfLine - 1;
					delDength = refSize - startPos;

					searchPtr.Delete(startPos, delDength );
					}
				TInt found = pDecoded.Find(searchPtr);
				if ( found == KErrNotFound )
					{
					CleanupStack::PopAndDestroy(); //"searchPtr"
					nResult = KResultFailedFound;
					break;
					}

				CleanupStack::PopAndDestroy(); //"searchPtr"
				if (endOfLine != KErrNotFound )
					{
					refPtr.Delete(0, endOfLine + 1);
					}
				else
					{
					refPtr.Delete(0, refPtr.Length());
					}
				}
            srcFile.Close();
            referenceFile.Close();
			CleanupStack::PopAndDestroy(3); //refBuf, resBuf
			}
		else
			{
			nResult = KResultFailedNoReference;
			srcFile.Close();
			}
		}
	else
		{
		nResult = KResultFailedNoResult;
		}
	return nResult;
	}

/***************
 * This function does the Base64 decoding.
 ****************/
HBufC8* CWSTester::DeBase64DescL(TDesC8& aMessage)
{
	_LIT8(KStartData,"<sa:Data>");
	_LIT8(KEndData,"</sa:Data>");

	TInt foundStart = aMessage.Find(KStartData());
	TInt foundEnd = aMessage.Find(KEndData());
	if ( foundStart == KErrNotFound || foundEnd == KErrNotFound )
		{
		return NULL;
		}

	HBufC8* pSource8 = aMessage.AllocLC();
	TPtr8 source8 = pSource8->Des();
	source8.Delete(0, foundStart + KStartData().Length() );
	foundEnd = source8.Find(KEndData());
	source8.Delete(foundEnd, source8.Length() - foundEnd );

	HBufC8* pResult8 = HBufC8::NewLC( source8.Length() );
	TPtr8 result8 = pResult8->Des();
	iBase64Codec.Decode(source8	, result8);
	aMessage = result8;
	CleanupStack::Pop(); // pResult8
	CleanupStack::PopAndDestroy(); // pSource8
	return pResult8;
}

TInt CWSTester::_X_NonIncludance( CStifItemParser& aItem )
	{
    TPtrC srcFile;
    TPtrC refFile;
    iLog->Log(_L("******* _X_NonIncludance-> Started! ********"));
    if (iErrorsOccured==TRUE)
		{
        iLog->Log(_L("### iErrorsOccured==TRUE###"));
        return KErrGeneral;
		}
    aItem.GetNextString ( srcFile );
    aItem.GetNextString ( refFile );
   	TBool answer;
    iFsSession.IsFileOpen(srcFile, answer);
    if (answer == TRUE)
		{
        iResultFile.Flush();
	    iResultFile.Close();
		}
    return CheckNonIncludanceL(srcFile, refFile);
	}

TInt CWSTester::CheckNonIncludanceL(TPtrC aSrcFile, TPtrC aReferenceFile)
    {

	_LIT8(KEndOfLine,"\n");

	RFile srcFile;
    RFile referenceFile;


    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    TFileName referenceFileName;
	referenceFileName.Copy(aReferenceFile);

    TInt nResult = KResultOK;


    if(srcFile.Open(iFsSession, srcFileName, EFileRead) == KErrNone)
	    {
		// Does reference file exist.
		if(referenceFile.Open(iFsSession, referenceFileName, EFileRead) == KErrNone)
		    {
	        // Integer variables for compare to length of files (result and reference).
	        TInt resSize;
	        TInt refSize;
	        srcFile.Size(resSize);
	        referenceFile.Size(refSize);

			HBufC8* resBuf = HBufC8::NewLC(resSize) ;
			HBufC8* refBuf = HBufC8::NewLC(refSize) ;

			TPtr8 resPtr = resBuf->Des();
			TPtr8 refPtr = refBuf->Des();

			nResult = KResultOK;
			// Read result file
			srcFile.Read(resPtr);

			// Read reference file
			referenceFile.Read(refPtr);

			TInt endOfLine;
			TInt startPos;
			TInt delDength;

			// Do the checking
			while (TRUE)
				{
				endOfLine = refPtr.Find(KEndOfLine);
				if ( refPtr.Length() == 0 )
					{
					break;
					}
				TPtr8 searchPtr = refPtr.AllocLC()->Des();
				if ( endOfLine != KErrNotFound )
					{
					startPos = endOfLine - 1;
					delDength = refSize - startPos;

					searchPtr.Delete(startPos, delDength );
					}
				TInt found = resPtr.Find(searchPtr);
				if ( found != KErrNotFound )
					{
					CleanupStack::PopAndDestroy(); //"searchPtr"
					nResult = KResultFailedFound;
					break;
					}

				CleanupStack::PopAndDestroy(); //"searchPtr"
				if (endOfLine != KErrNotFound )
					{
					refPtr.Delete(0, endOfLine + 1);
					}
				else
					{
					refPtr.Delete(0, refPtr.Length());
					}
				}
			CleanupStack::PopAndDestroy(2); //refBuf, resBuf
            referenceFile.Close();
            srcFile.Close();
			}
		else
			{
			nResult = KResultFailedNoReference;
			}
		srcFile.Close();
		}
	else
		{
		nResult = KResultFailedNoResult;
		}
	return nResult;
    }
TInt CWSTester::_X_NonExistence( CStifItemParser& aItem )
	{
	TPtrC aSrcFile;
	iLog->Log(_L("******* _X_NonExistence-> Started! ********"));
	aItem.GetNextString ( aSrcFile );

	RFile srcFile;

    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    if(srcFile.Open(iFsSession, srcFileName, EFileRead) == KErrNone)
		{
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}
TInt CWSTester::_X_Includance( CStifItemParser& aItem )
	{
	TPtrC srcFile;
	TPtrC refFile;
	iLog->Log(_L("******* _X_Includance-> Started! ********"));
	if (iErrorsOccured==TRUE)
		{
		iLog->Log(_L("### iErrorsOccured==TRUE###"));
		return KErrGeneral;
		}
	aItem.GetNextString ( srcFile );
	aItem.GetNextString ( refFile );
	TBool answer;
	iFsSession.IsFileOpen(srcFile, answer);
	if (answer == TRUE)
		{
		iResultFile.Flush();
		iResultFile.Close();
		}
   	return CheckIncludanceL(srcFile, refFile);
	}


TInt CWSTester::CheckIncludanceL(TPtrC aSrcFile, TPtrC aReferenceFile)
    {
	_LIT8(KEndOfLine,"\n");

	RFile srcFile;
    RFile referenceFile;


    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    TFileName referenceFileName;
	referenceFileName.Copy(aReferenceFile);

    TInt nResult = KResultOK;


    if(srcFile.Open(iFsSession, srcFileName, EFileRead) == KErrNone)
	    {
		// Does reference file exist.
		if(referenceFile.Open(iFsSession, referenceFileName, EFileRead) == KErrNone)
		    {
	        // Integer variables for compare to length of files (result and reference).
	        TInt resSize;
	        TInt refSize;
	        srcFile.Size(resSize);
	        referenceFile.Size(refSize);

			HBufC8* resBuf = HBufC8::NewLC(resSize) ;
			HBufC8* refBuf = HBufC8::NewLC(refSize) ;

			TPtr8 resPtr = resBuf->Des();
			TPtr8 refPtr = refBuf->Des();

			nResult = KResultOK;
			// Read result file
			srcFile.Read(resPtr);

			// Read reference file
			referenceFile.Read(refPtr);

			TInt endOfLine;
			TInt startPos;
			TInt delDength;

			// Do the checking
			// Searches for EndOfLine. This algorithm reads line by line
			// from the file and searches the pattern from the target file.
			// The file may or may not end to a EndOfLine. Both are
			// accepted.
			while (TRUE)
				{
				endOfLine = refPtr.Find(KEndOfLine);
				if ( refPtr.Length() == 0 )
					{
					break;
					}
				TPtr8 searchPtr = refPtr.AllocLC()->Des();
				if ( endOfLine != KErrNotFound )
					{
					startPos = endOfLine - 1;
					delDength = refSize - startPos;
					searchPtr.Delete(startPos, delDength );
					}
				TInt found = resPtr.Find(searchPtr);
				if ( found == KErrNotFound )
					{
					CleanupStack::PopAndDestroy(); //"searchPtr"
					nResult = KResultFailedNotIdentical;
					break;
					}

				CleanupStack::PopAndDestroy(); //"searchPtr"
				if (endOfLine != KErrNotFound )
					{
					refPtr.Delete(0, endOfLine + 1);
					}
				else
					{
					refPtr.Delete(0, refPtr.Length());
					}
				}
			CleanupStack::PopAndDestroy(2); //refBuf, resBuf
            referenceFile.Close();
            srcFile.Close();
			}
		else
			{
			nResult = KResultFailedNoReference;
			}
		srcFile.Close();
		}
	else
		{
		nResult = KResultFailedNoResult;
		}
	return nResult;
    }

TInt CWSTester::_X_WaitForResponse( CStifItemParser& aItem )
	{
    iLog->Log(_L("### WaitForResponse -> started ###"));
    iStarted = TRUE;
	iCasePassed = EFalse;
    CActiveScheduler::Start();
	iLog->Log(_L("### WaitForResponse -> finished ###"));
    iLog->Log(_L("### Checking if everything went ok... ###"));
	if ( iCasePassed == TRUE )
		{
        iLog->Log(_L("### CasePassed is TRUE###"));
		return KErrNone;
		}
	else
		{
        iLog->Log(_L("### CasePassed is FALSE###"));
        return KErrGeneral;
		}
	}


TInt CWSTester::_X_Reference( CStifItemParser& aItem ){
    TPtrC srcFile;
    TPtrC refFile;
    iLog->Log(_L("******* _X_Reference-> Started! ********"));
    if (iErrorsOccured==TRUE) {
        iLog->Log(_L("### iErrorsOccured==TRUE###"));
        return KErrGeneral;
    }
    aItem.GetNextString ( srcFile );
    aItem.GetNextString ( refFile );
   	TBool answer;
#ifndef _PERFORMANCE   	
    iFsSession.IsFileOpen(srcFile, answer);
    if (answer == TRUE){
    	iWriter.Close();
        //iResultFile.Flush();
	    //iResultFile.Close();
    }
    return CheckResultL(srcFile, refFile);
#else 
    return KErrNone;
#endif

}
TInt CWSTester::_X_ReferenceFragment( CStifItemParser& aItem ){
    TPtrC srcFile;
    TPtrC refFile;
    iLog->Log(_L("******* _X_ReferenceFragment-> Started ! ********"));
    if (iErrorsOccured==TRUE) {
        iLog->Log(_L("### iErrorsOccured==TRUE###"));
        return KErrGeneral;
    }
    aItem.GetNextString ( srcFile );
    aItem.GetNextString ( refFile );
   	TBool answer;
    iFsSession.IsFileOpen(srcFile, answer);
    if (answer == TRUE){
		iWriter.Close();
        //iResultFile.Flush();
	    //iResultFile.Close();
    }
    return CheckResultFragmentL(srcFile, refFile);
}
TInt CWSTester::_X_LogResult( CStifItemParser& aItem ){
    //TPtrC resFile;
    TInt err;
	TInt size = 0;
    aItem.GetNextString ( iResFile );
    TFileName resultFileName;
#ifndef _PERFORMANCE
    resultFileName.Copy(iResFile);
    TInt res = iResultFile.Create(iFsSession, resultFileName, EFileStreamText|EFileWrite);
    err = iResultFile.Size(size);
    if (err != KErrNone){
        return err;
    }
	iWriter.Attach(iResultFile, (size));
    return res;
#else 
    return KErrNone;
#endif
}
TInt CWSTester::_X_Init( CStifItemParser& aItem ){
    TPtrC srcFile;
    TPtrC dstFile;
    TInt result;
    aItem.GetNextString ( srcFile );
    aItem.GetNextString ( dstFile );
    CFileMan* fileMan = CFileMan::NewL(iFsSession);
    CleanupStack::PushL(fileMan);
    result = fileMan->Copy(srcFile, dstFile, CFileMan::EOverWrite);
    CleanupStack::PopAndDestroy();
   	return result;
}
TInt CWSTester::_X_DeleteFile( CStifItemParser& aItem ){
    TPtrC file;
    TInt result;
    aItem.GetNextString ( file );
    CFileMan* fileMan = CFileMan::NewL(iFsSession);
    CleanupStack::PushL(fileMan);
    result = fileMan->Delete(file);
        if (result == KErrNotFound){
        result = KErrNone;
    }
    CleanupStack::PopAndDestroy();
   	return result;
}

TInt CWSTester::_X_TimeOut( CStifItemParser& aItem ){

	iLog->Log(_L("### TimeOut -> started ###"));
	RTimer timer;  // The synchronous timer
	TInt result( KErrNone );
	TInt aMicroSecs (10); // default value
	TRequestStatus timerStatus; // associated with timer

    aItem.GetNextInt( aMicroSecs );
	result = timer.CreateLocal();   // Always created for this thread.
	timer.After(timerStatus,aMicroSecs*1000000);
	User::WaitForRequest(timerStatus);
	timer.Close();
	iLog->Log(_L("### TimeOut -> finished  ###"));
	return result;
}
TInt CWSTester::_X_Flag(CStifItemParser& aItem ){
    iLog->Log(_L("### Setting flag -> started ###"));

    TPtrC string;
    if (!(aItem.GetNextString ( string ))){
        if (string.Compare(_L16("KSendSOAP")) == 0){
            iRunCase = KSendSOAP;
        }else if(string.Compare(_L16("KSendYomi")) == 0){
            iRunCase = KSendYomi;
        }else if(string.Compare(_L16("KEndpointMovedTest")) == 0){
            iRunCase = KEndpointMovedTest;
        }else if(string.Compare(_L16("KSendInvalidSOAP")) == 0){
            iRunCase = KSendInvalidSOAP;
        }else if(string.Compare(_L16("KSubmitSOAP")) == 0){
            iRunCase = KSubmitSOAP;
        }else if (string.Compare(_L16("KSendSOAPRequest")) == 0){
            iRunCase = KSendSOAPRequest;
        }else if (string.Compare(_L16("KSubmitSOAPRequest")) == 0){
            iRunCase = KSubmitSOAPRequest;
        }else if(string.Compare(_L16("KSendInvalidXML")) == 0){
            iRunCase = KSendInvalidXML;
        }else if(string.Compare(_L16("KNoResourceID")) == 0){
            iRunCase = KNoResourceID;
		}else if(string.Compare(_L16("KDSEcrypted")) == 0){
            iRunCase = KDSEcrypted;
        }else if(string.Compare(_L16("KConnectAS")) == 0){
            iRunCase = KConnectAS;
        }else if(string.Compare(_L16("KInvalidLogin")) == 0){
            iRunCase = KInvalidLogin;
        }else if (string.Compare(_L16("KInvalidHost")) == 0){
            iRunCase = KInvalidHost;
        }else if(string.Compare(_L16("KBasicDisco")) == 0){
            iRunCase = KBasicDisco;
        }else if(string.Compare(_L16("KDSFailedResult")) == 0){
            iRunCase = KDSFailedResult;
        }else if(string.Compare(_L16("KDSFailed")) == 0){
            iRunCase = KDSFailed;
        }else if(string.Compare(_L16("KSendBUCF")) == 0){
            iRunCase = KSendBUCF;
        }else if(string.Compare(_L16("KSendBUCFBody")) == 0){
            iRunCase = KSendBUCFBody;
        }else if(string.Compare(_L16("KSendBUCFEnvelope2")) == 0){
            iRunCase = KSendBUCFEnvelope2;
        }else if(string.Compare(_L16("KSendBUCFEnvelope")) == 0){
            iRunCase = KSendBUCFEnvelope;
        }else if(string.Compare(_L16("KSendASAbort")) == 0){
            iRunCase = KSendASAbort;
        }else if(string.Compare(_L16("KLowercase")) == 0){
            iRunCase = KLowercase;
        }else if(string.Compare(_L16("KUppercase")) == 0){
            iRunCase = KUppercase;
        }else if(string.Compare(_L16("KTruncate")) == 0){
            iRunCase = KTruncate;
        }else if(string.Compare(_L16("KInvalidWSP")) == 0){
            iRunCase = KInvalidWSP;
        }else if(string.Compare(_L16("KInitialOK")) == 0){
            iRunCase = KInitialOK;
        }else if(string.Compare(_L16("KUppercaseAllowed")) == 0){
            iRunCase = KUppercaseAllowed;
        }else if(string.Compare(_L16("KAllowedUppercase")) == 0){
            iRunCase = KAllowedUppercase;
        }else if(string.Compare(_L16("KAllowed")) == 0){
            iRunCase = KAllowed;
		}else if(string.Compare(_L16("KAllowUpperTrun")) == 0){
            iRunCase = KAllowUpperTrun;
        }else if(string.Compare(_L16("KInvalidCredentials")) == 0){
            iRunCase = KInvalidCredentials;
        }else if(string.Compare(_L16("KUnexistantDS")) == 0){
            iRunCase = KUnexistantDS;
        }else if(string.Compare(_L16("KMultippleROs")) == 0){
            iRunCase = KMultippleROs;
        }else if(string.Compare(_L16("KFacets")) == 0){
            iRunCase = KFacets;
        }else if(string.Compare(_L16("KNoCredentials")) == 0){
            iRunCase = KNoCredentials;
        }else if(string.Compare(_L16("KNonSupportedAM")) == 0){
	          iRunCase = KNonSupportedAM;
        }else if(string.Compare(_L16("KCredentialsExpires")) == 0){
        	iRunCase = KCredentialsExpires;
        }else if(string.Compare(_L16("KServerError500")) == 0){
        	iRunCase = KServerError500;
        }else if(string.Compare(_L16("KServerError404")) == 0){
        	iRunCase = KServerError404;
	    }else if(string.Compare(_L16("KHasFacet")) == 0){
        	iRunCase = KHasFacet;
	    }else if(string.Compare(_L16("KIsReady")) == 0){
        	iRunCase = KIsReady;
	    }else if(string.Compare(_L16("KCompleteServerMessagesOnOff_1")) == 0){
        	iRunCase = KCompleteServerMessagesOnOff_1;
	    }else if(string.Compare(_L16("KCompleteServerMessagesOnOff_2")) == 0){
        	iRunCase = KCompleteServerMessagesOnOff_2;
	    }else if(string.Compare(_L16("KCompleteServerMessagesOnOff_3")) == 0){
        	iRunCase = KCompleteServerMessagesOnOff_3;
	    }else if(string.Compare(_L16("KCompleteServerMessagesOnOff_4")) == 0){
        	iRunCase = KCompleteServerMessagesOnOff_4;
	    }else if(string.Compare(_L16("KPerformanceSOAPSubmit")) == 0){
        	iRunCase = KPerformanceSOAPSubmit;	    
	    }

        return KErrNone;
    }
    return KErrNotFound;
}
TInt CWSTester::_X_CheckDir(CStifItemParser& aItem ){
    iLog->Log(_L("### Openning a directory -> started ###"));

	RDir dir;
	TPtrC dirName;
	aItem.GetNextString ( dirName );
	TInt error = KErrNone;
	error = dir.Open(iFsSession, dirName, KEntryAttNormal);
	dir.Close();
    return error;
}

TInt CWSTester::_X_CheckFile(CStifItemParser& aItem ){
    iLog->Log(_L("### Openning a file -> started ###"));

	RFile file;
	TPtrC fileName;
	aItem.GetNextString ( fileName );
	//TBuf8<1> buf;
	TInt error = KErrNone;
	error = file.Open(iFsSession, fileName, EFileRead);
	//file.Read(buf);
	file.Close();
    return error;
}

TInt CWSTester::_C_Dump(CStifItemParser& aItem ){
    iLog->Log(_L("### Dumping -> started ###"));
	TPtrC part;
    TPtrC way;

    aItem.GetNextString(part);
    aItem.GetNextString(way);
    //===============ENVELOPE
    if (part.Compare(_L16("Envelope")) == 0){
        if (way.Compare(_L16("Xml")) == 0){
            HBufC8* xml = iSOAPMessage->AsXmlL();
            LogResultL( xml->Des() );
            delete xml;
            xml = NULL;
        }else if (way.Compare(_L16("String")) == 0){
            iLog->Log(_L("### wrong combination###"));
            return KErrGeneral;
        }
    }
    //===============HEADER

    else if (part.Compare(_L16("Header")) == 0){
        MSenElement* header = iSOAPMessage->HeaderL().AsElement();
        if (way.Compare(_L16("Xml")) == 0){
            HBufC8* xml = header->AsXmlL();
            LogResultL( xml ->Des());
            delete xml;
            xml = NULL;
        }else if (way.Compare(_L16("String")) == 0){
            iLog->Log(_L("### wrong combination###"));
            return KErrGeneral;
        }
    }
    //===============BODY
    else if (part.Compare(_L16("Body")) == 0){
        MSenElement* body = iSOAPMessage->BodyL().AsElement();
        if (way.Compare(_L16("Xml")) == 0){
            HBufC8* xml = body->AsXmlL();
            LogResultL( xml ->Des() );
            delete xml;
            xml = NULL;
        }else if (way.Compare(_L16("String")) == 0){
            HBufC8* bodyAsString = iSOAPMessage->BodyAsStringL();
            LogResultL( bodyAsString ->Des());
            delete bodyAsString;
            bodyAsString = NULL;
        }
    }else{
        iLog->Log(_L("### wrong combination###"));
        return KErrGeneral;
    }
    iLog->Log(_L("### Dumping -> finished###"));
    return KErrNone;
}

TInt CWSTester::_X_Iteration( CStifItemParser& aItem ){
        iLog->Log(_L("### TestItteration seter -> started ###"));
		//TInt iTestIteracion (1); // default value
    	aItem.GetNextInt( iTestIteracion );
	    iLog->Log(_L("TestItteration set"));
        return KErrNone;

}
/*################################################
            IAP Selection
################################################*/

TInt CWSTester::_X_IAPList(CStifItemParser& aItem ){
	TInt error = KErrNone;
	//Get all existing IAPs
    CCommsDatabase* CommDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(CommDb);
    CCommsDbTableView* tableIAP = CommDb->OpenTableLC(TPtrC(IAP));
    iLog->Log(_L("### Openning a CommDB -> started ###"));
    if (tableIAP->GotoFirstRecord()==KErrNone)
    {         
       do {
            TUint32 uid(0);
            tableIAP->ReadUintL(TPtrC(COMMDB_ID),uid);
    		TBuf<KCommsDbSvrMaxColumnNameLength> bufName;
            tableIAP->ReadTextL(TPtrC(COMMDB_NAME),bufName);
            
             TBuf<KCommsDbSvrMaxColumnNameLength + 10> buf;
    		_LIT(KFormat,"%S - %d\n");
    		buf.Format(KFormat, &bufName, uid);
    		LogResultL(buf);
        }
        while(tableIAP->GotoNextRecord()==KErrNone);
    }
    else 
    {
    	error = tableIAP->GotoFirstRecord();
    }

    CleanupStack::PopAndDestroy(tableIAP);
    CleanupStack::PopAndDestroy(CommDb);
    return error;
}

TInt CWSTester::_X_SOAPRequest( CStifItemParser& aItem ){
        iLog->Log(_L("### RequestQuery -> started ###"));
        TBuf8<255> pRequestBody;
		TPtrC arg;
        aItem.GetNextString(arg);
        pRequestBody.Append(arg);

		while (!(aItem.GetNextString(arg)))
		{
       		pRequestBody.Append(_L(" "));
			pRequestBody.Append(arg);

		}

        iLog->Log(pRequestBody);

		iSOAPRequest = pRequestBody;
		//LogResultL( iSOAPRequest );


	    iLog->Log(_L("RequestQuery created.."));

        return KErrNone;

}
void CWSTester::SendSOAPRequest(TInt aDelay){

    iLog->Log(_L("### SendRequest -> started ###"));
    if(aDelay !=NULL)
    {
    	iLog->Log(_L("### Delay -> started ###"));
		RTimer timer;  // The synchronous timer
		TInt aMicroSecs (aDelay); // default value
		TRequestStatus timerStatus; // associated with timer
		timer.CreateLocal();   // Always created for this thread.
		timer.After(timerStatus,aMicroSecs*1000000);
		User::WaitForRequest(timerStatus);
		timer.Close();
		iLog->Log(_L("### Delay -> finished  ###"));
    }

	iSenServiceConnection->SendL(iSOAPRequest);

	iLog->Log(_L("******* SendRequest -> Ended! ********"));

	}

void CWSTester::PerformanceSOAPSubmit(){

    iLog->Log(_L("### Performance - SubmitSOAP -> started ###"));
	HBufC8* pResponse = NULL;    
	for(TInt i = 0; i < iTestIteracion; i++)
	{
		iSenServiceConnection->SubmitL(iSOAPRequest, pResponse);
		if(pResponse)
			{
			iLog->Log( _L("Received response:") );
			}
		else
			{
			iLog->Log( _L(" Response NULL! ") );
        	iErrorsOccured = TRUE;
			}		
	}
	iCasePassed = TRUE;
	CActiveScheduler::Stop();
	iLog->Log(_L("******* Performance - SubmitSOAP -> Ended! ********"));

	}	

void CWSTester::SubmitSOAPRequest(){

    iLog->Log(_L("### SubmitRequest -> started ###"));

	HBufC8* pResponse = NULL;

	iSenServiceConnection->SubmitL(iSOAPRequest, pResponse);

	if(pResponse)
		{
		CleanupStack::PushL(pResponse);
		//HBufC* pUnicode = SenXmlUtils::ToUnicodeLC( pResponse->Des() );
		iLog->Log( _L("Received response:") );
		//iLog->Log( pUnicode->Des() );
		//LogResultL( pUnicode->Des() );
        LogResultL( *pResponse );
		//CleanupStack::PopAndDestroy(); // pUnicode
		CleanupStack::PopAndDestroy(); // pResponse
		}
	else
		{
		iLog->Log( _L(" Response NULL! ") );
        iErrorsOccured = TRUE;
		}

	iCasePassed = TRUE;
	CActiveScheduler::Stop();
	iLog->Log(_L("******* SubmitSOAPRequest -> Ended! ********"));

	}


void CWSTester::SendSOAPTest()
	{
	iLog->Log(_L("******* SendSOAPTest -> Started! ********"));

    if(iSOAPMessage)
		{
        //change serene
        //HBufC16* pInitRequest = iSOAPMessage->AsXmlUnicodeL();	// as xml{
		HBufC8* pInitRequest = iSOAPMessage->AsXmlL();	// as xml

		if(pInitRequest)
			{
			CleanupStack::PushL(pInitRequest);
			iLog->Log(_L("Sending:"));
            //LogResultL(*pInitRequest);
			CleanupStack::PopAndDestroy(); // pInitRequest
			}

		iSenServiceConnection->SendL( *iSOAPMessage );
		iLog->Log(_L("SOAP message sent!"));
    }else{
        iSOAPMessage = CSenSoapMessage::NewL();
        iLog->Log(_L("Creating empty SOAP message.."));
        iSenServiceConnection->SendL( *iSOAPMessage  );
		iLog->Log(_L("empty SOAP message sent!"));
    }


	iLog->Log(_L("******* SendSOAPTest -> Ended! ********"));

	}
void CWSTester::SendInvalidSOAPTest()
	{
	iLog->Log(_L("******* SendInvalidSOAPTest -> Started! ********"));

	iLog->Log( _L("SendBUCFTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<ab:Querykikka xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItemJorma><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItemJorma></ab:Querykikka>");


	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendInvalidSOAPTest -> Ended! ********"));

	}
void CWSTester::SendInvalidTest()
	{
	iLog->Log(_L("******* SendInvalidTest -> Started! ********"));

	iLog->Log( _L("SendBUCFTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItem></ab:Query>");


	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendBUCFTest -> Ended! ********"));

	}
void CWSTester::SendYomiTest()
	{
	iLog->Log(_L("******* SendYomiTest -> Started! ********"));

	iLog->Log( _L("SendBUCFTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<GetStationInfoRequest xmlns=\"http://radio.aol.com/xsd/2.0/ns/\" ><Stations><Station><Id>6546</Id></Station></Stations><Fields/></GetStationInfoRequest>");

	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendYomiTest -> Ended! ********"));

	}
void CWSTester::SendBUCFTest()
	{
	iLog->Log(_L("******* SendBUCFTest -> Started! ********"));

	iLog->Log( _L("SendBUCFTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItem></ab:Query>");


	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendBUCFTest -> Ended! ********"));

	}
void CWSTester::SendBUCFEnvelopeTest2()
	{
	iLog->Log(_L("******* SendBUCFEnvelopeTest2 -> Started! ********"));

	iLog->Log( _L("SendBUCFEnvelopeTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItem></ab:Query>");

	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendBUCFEnvelopeTest2 -> Ended! ********"));

	}

void CWSTester::SendBUCFEnvelopeTest()
	{
	iLog->Log(_L("******* SendBUCFEnvelopeTest -> Started! ********"));

	iLog->Log( _L("SendBUCFEnvelopeTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItem></ab:Query>");

	TBool completeOn = ETrue;

	TInt err = iSenServiceConnection->CompleteServerMessagesOnOff( completeOn );
	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendBUCFEnvelopeTest -> Ended! ********"));

	}
void CWSTester::SendBUCFBodyTest()
	{
	iLog->Log(_L("******* SendBUCFBodyTest -> Started! ********"));

	iLog->Log( _L("SendBUCFTest() SENDING REQUEST.... ") );

	_LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[ab:N/ab:FAMILY='Smith' and ab:N/ab:GIVEN='John']</ab:Select></ab:QueryItem></ab:Query>");

	TBool completeOn = EFalse;

	TInt err = iSenServiceConnection->CompleteServerMessagesOnOff( completeOn );
	iSenServiceConnection->SendL(KRequest);

	iLog->Log(_L("******* SendBUCFTest -> Ended! ********"));

	}

void CWSTester::SubmitSOAPTest()
	{
    TInt error = KErrNone;
	iLog->Log(_L("******* SubmitSOAPTest -> Started! ********"));
	if(iSOAPMessage)
		{
		HBufC8* pInitRequest = iSOAPMessage->AsXmlL();	// as xml

		if(pInitRequest)
			{
			CleanupStack::PushL(pInitRequest);
			iLog->Log(_L("Submitting:"));
			LogResultL( *pInitRequest );
			CleanupStack::PopAndDestroy(); // pInitRequest
			}

		}

	HBufC8* pResponse = NULL;

	error = iSenServiceConnection->SubmitL(*iSOAPMessage, pResponse);

    if (error == KErrSenSoapFault){
        LogResultL(_L8("KErrSenSoapFault"));
        iLog->Log( _L("Received Fault response:") );
	    iErrorsOccured = TRUE;
	
    }else if (error != KErrNone){
        LogResultL(_L("error"));
        iErrorsOccured = TRUE;
    }

	if(pResponse)
		{
		CleanupStack::PushL(pResponse);
		//HBufC* pUnicode = SenXmlUtils::ToUnicodeLC( pResponse->Des() );
		iLog->Log( _L("Received Normal Body response:") );
		//LogResultL( pUnicode->Des() );
        LogResultL(*pResponse);
		//CleanupStack::PopAndDestroy(); // pUnicode
		CleanupStack::PopAndDestroy(); // pResponse
		}
	else
		{
		iLog->Log( _L(" Response NULL! ") );
        iErrorsOccured = TRUE;
		}

	CActiveScheduler::Stop();
	iLog->Log(_L("******* SubmitSOAPTest -> Ended! ********"));
	}
void CWSTester::CheckConnection(){
	TBool pIsReady = TRUE;
	if ( iRunCase == KHasFacet )
	{
		TBool pHasFacet = TRUE;

		_LIT8(KFacet, "urn:liberty:id-sis-pp");
		iSenServiceConnection->HasFacetL(KFacet,pHasFacet);

		if(pHasFacet == TRUE) LogResultL(_L8("Has defined Facet"));
	}
	else
	{
   	    TInt retVal = iSenServiceConnection->IsReady( pIsReady );
        if(pIsReady == TRUE) LogResultL(_L8("Connection received"));


	    iLog->Log(_L("###Validating service connection IsReady -> started###"));


	    TInt retVal2 = iSenServiceConnection->IsReady( pIsReady );

	    if(pIsReady == TRUE)
	    {
		    HBufC8* aSD;
		    iLog->Log(_L("Connection validated"));
		    iLog->Log( _L("Getting the services description....") );
		    iSenServiceConnection->ServiceDescriptionL( aSD );
    	    LogResultL( *aSD );
    	    delete aSD;
	    } else iLog->Log(_L("Connection NOT validated!!!"));
	}
}

TInt CWSTester::CheckResultFragmentL(TPtrC aSrcFile, TPtrC aReferenceFile){
    RFile srcFile;
    RFile referenceFile;


    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    TFileName referenceFileName;
	referenceFileName.Copy(aReferenceFile);
    TInt nResult = KResultFailedNotIdentical;
    if(srcFile.Open(iFsSession, srcFileName, EFileStreamText|EFileRead) == KErrNone)
	    {
		// Does reference file exist.
		if(referenceFile.Open(iFsSession, referenceFileName, EFileStreamText|EFileRead) == KErrNone)
		    {
	        // Integer variables for compare to length of files (result and reference).
	        TInt resSize;
	        TInt refSize;
	        srcFile.Size(resSize);
	        referenceFile.Size(refSize);

	        TBuf8<1> resBuf;
	        TBuf8<1> refBuf;
	        nResult = KResultFailedNotIdentical;
            TInt j = 0;
	        for(TInt i = 0; i < resSize; i++)
		    {
		        // Read result file
		        srcFile.Read(resBuf);
			        // Read reference file
    	        referenceFile.Read(refBuf);
			        // Compare single letter at the time.

		        if(resBuf[0] == refBuf[0])
			    {
                    j++;
                    if (refSize == j) {
                        nResult = KResultOK;
                        break;//ok
                    }
			    }else{
                    TInt zero = 0;
                    referenceFile.Seek(ESeekStart, zero);//reset
                    j=0;
                }
		    }
            referenceFile.Close();
            srcFile.Close();
			}
		else
			{
			nResult = KResultFailedNoReference;
			}
		srcFile.Close();
		}
	else
		{
		nResult = KResultFailedNoResult;
		}
	return nResult;
}
TInt CWSTester::CheckResultL(TPtrC aSrcFile, TPtrC aReferenceFile)
    {
    RFile srcFile;
    RFile referenceFile;


    TFileName srcFileName;
	srcFileName.Copy(aSrcFile);

    TFileName referenceFileName;
	referenceFileName.Copy(aReferenceFile);

    TInt nResult = KResultOK;


    if(srcFile.Open(iFsSession, srcFileName, EFileStreamText|EFileRead) == KErrNone)
	    {
		// Does reference file exist.
		if(referenceFile.Open(iFsSession, referenceFileName, EFileStreamText|EFileRead) == KErrNone)
		    {
	        // Integer variables for compare to length of files (result and reference).
	        TInt resSize;
	        TInt refSize;
	        srcFile.Size(resSize);
	        referenceFile.Size(refSize);

            // Next compare one letter at the time, but only if files have same length.
            if(resSize == refSize)
		        {
		        TBuf8<1> resBuf;
		        TBuf8<1> refBuf;
		        nResult = KResultOK;
		        for(TInt i = 0; i < resSize; i++)
			        {
			        // Read result file
			        srcFile.Read(resBuf);

			        // Read reference file
			        referenceFile.Read(refBuf);

			        // Compare single letter at the time.

			        if(resBuf[0] != refBuf[0])
				        {
				        nResult = KResultFailedNotIdentical;
				        break;
				        }
			        }
		        }
	        else
		        {
		        nResult = KResultFailedNotIdentical;
		        } // if(resSize==refSize)
            referenceFile.Close();
            srcFile.Close();
			}
		else
			{
			nResult = KResultFailedNoReference;
			}
		srcFile.Close();
		}
	else
		{
		nResult = KResultFailedNoResult;
		}
	return nResult;
    }
    
TInt CWSTester::_X_CheckResultFL(CStifItemParser& aItem)
{
    TPtrC asrcFile;
    TPtrC arefFile;
    iLog->Log(_L("******* _X_CheckResultF-> Started! ********"));
    if (iErrorsOccured==TRUE) {
        iLog->Log(_L("### iErrorsOccured==TRUE###"));
        return KErrGeneral;
    }
    aItem.GetNextString ( asrcFile );
    aItem.GetNextString ( arefFile );
    TBool answer;
   
    iFsSession.IsFileOpen(asrcFile, answer);
    if (answer == TRUE){
        iWriter.Close();
        //iResultFile.Flush();
        //iResultFile.Close();
    }
    RFile srcFile;
    RFile referenceFile;

    TFileName srcFileName;
    srcFileName.Copy(asrcFile);

    TFileName referenceFileName;
    referenceFileName.Copy(arefFile);

    TInt nResult = KResultOK;

//    TDes8& refBuf;
    //TDes8& resBuf;

    if(srcFile.Open(iFsSession, srcFileName, EFileStreamText|EFileRead) == KErrNone)
        {
         // Does reference file exist.
         if(referenceFile.Open(iFsSession, referenceFileName, EFileStreamText|EFileRead) == KErrNone)
             {
             TInt resSize;
             TInt refSize;
             srcFile.Size(resSize);
             referenceFile.Size(refSize);
             
             HBufC8* hresBuf = HBufC8::NewL( resSize ); // create new dynamic buffer (can be reallocated with ReAlloc() )
             CleanupStack::PushL(hresBuf);             
             TDes8 resBuf = hresBuf->Des();
                         
             HBufC8* hrefBuf = HBufC8::NewL( refSize ); // create new dynamic buffer (can be reallocated with ReAlloc() )
             CleanupStack::PushL(hrefBuf);
             TDes8 refBuf = hrefBuf->Des();

             srcFile.Read(resBuf);
             referenceFile.Read(refBuf);
             
             nResult = resBuf.MatchF(refBuf);
             
             }
         else
             {
             nResult = KResultFailedNoReference;             
             }
        }
    if(nResult < 0)
        {
            nResult = KResultFailedNoResult;
        }
    CleanupStack::PopAndDestroy(2); 
    referenceFile.Close();
    srcFile.Close();
    
   	return nResult;
}
TInt CWSTester::TestFacets()
    {
	 _LIT8(KFacet1, "urn:liberty:id-sis-pp");
	 _LIT8(KFacet2, "urn:liberty:id-sis-pp:cn");
	 _LIT8(KFacet3, "urn:liberty:id-sis-pp:can");
	 _LIT8(KFacet4, "urn:liberty:id-sis-pp:can:cn");
 	 _LIT8(KFacet5, "urn:should:not:exists");


	TInt retValue = KErrNone;
	TBool hasFacet = TRUE;
	retValue = iSenServiceConnection->HasFacetL(KFacet1, hasFacet);
	if (retValue != KErrNone || hasFacet == FALSE )
		{
		return KErrNotFound;
		}
	retValue = iSenServiceConnection->HasFacetL(KFacet2, hasFacet);
	if (retValue != KErrNone || hasFacet == FALSE )
		{
		return KErrNotFound;
		}
	retValue = iSenServiceConnection->HasFacetL(KFacet3, hasFacet);
	if (retValue != KErrNone || hasFacet == FALSE )
		{
		return KErrNotFound;
		}
	retValue = iSenServiceConnection->HasFacetL(KFacet4, hasFacet);
	if (retValue != KErrNone || hasFacet == FALSE )
		{
		return KErrNotFound;
		}
	retValue = iSenServiceConnection->HasFacetL(KFacet5, hasFacet);
	if (retValue != KErrNone || hasFacet == TRUE )
		{
		return KErrNotFound;
		}
	return KErrNone;
}

void CWSTester::LogResultL(const TDesC8& aDesc8){
    /*HBufC8* writeBuf=HBufC8::New(aDesc8.Length()+2);
    CleanupStack::PushL(writeBuf);
	TPtr8 ptr = writeBuf->Des();
	ptr.Copy(aDesc8);
    ptr.Append(_L8("\r\n"));
	User::LeaveIfError(iResultFile.Write(ptr));
	User::LeaveIfError(iResultFile.Flush());
    CleanupStack::PopAndDestroy(1);*/
#ifndef _PERFORMANCE
    iWriter.WriteL(aDesc8);
	iWriter.WriteL(_L8("\r\n"));
	iWriter.CommitL();
#endif
}
void CWSTester::LogResultL(const TDesC16& aDesc16)
	{
#ifndef _PERFORMANCE
	iWriter.WriteL(aDesc16);
	iWriter.WriteL(_L16("\r\n"));
	iWriter.CommitL();
#endif
	//writer.Close();


    /*HBufC8* writeBuf=HBufC8::New(aDesc16.Length()+2);//13,10 new Line
    CleanupStack::PushL(writeBuf);
	TPtr8 ptr = writeBuf->Des();

	//ptr.Copy(aDesc16);
    ptr.Append(_L8("\r\n"));

	//User::LeaveIfError(iResultFile.Seek(ESeekEnd, pos));
	User::LeaveIfError(iResultFile.Write(ptr));//(pos, ptr));
	User::LeaveIfError(iResultFile.Flush());
    CleanupStack::PopAndDestroy(1);*/
	}



void CWSTester::PopAndDestroyArgc(){
    CleanupStack::PopAndDestroy(iArgc);
    iArgc = 0;
}
TPtrC8 CWSTester::GetParC(CStifItemParser& aItem){
    TPtrC string;
    TBool flag = FALSE;
    TBool secure = FALSE;
    TInt offset = 0;
    if (!(aItem.GetNextString ( string ))){
        if (string.Compare(_L16("http:")) == 0){
            flag = TRUE;
            offset = 7;
        }else if (string.Compare(_L16("https:")) == 0){
            flag = TRUE;
            secure = TRUE;
            offset = 8;
        }else if(string.Compare(_L16("NULL")) == 0){
	        //HBufC8* writeBuf=HBufC8::New(0);
	        //TPtr8 ptr = writeBuf->Des();
	        //ptr.Copy(KNullDesC8);
	        //return ptr;
            //return KNullDesC8;
            return TPtrC8(KNullDesC8);
        }
        iArgc++;
        if (flag){
            aItem.GetNextString ( string );
        }
        HBufC8* writeBuf=HBufC8::New(string.Length() + offset);
        CleanupStack::PushL(writeBuf);
	    TPtr8 ptr = writeBuf->Des();
        if (flag){
            if (secure){
	            ptr.Copy(KFixedHTTPSPrefix);
            }else{
                ptr.Copy(KFixedHTTPPrefix);
            }
            ptr.Append(string);
        }else{
            ptr.Copy(string);
        }
        iLog->Log(ptr);
        return ptr;
    }else{
        //HBufC8* writeBuf=HBufC8::New(0);
        //TPtr8 ptr = writeBuf->Des();
        //ptr.Copy(KNullDesC8);
        //return ptr;
        //return KNullDesC8;
        return TPtrC8(KNullDesC8);
    }
}
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
