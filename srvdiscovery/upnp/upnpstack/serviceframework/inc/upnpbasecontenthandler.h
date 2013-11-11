/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares the CUpnpBaseContentHandler class
*
*/


#ifndef __UPNPBASECONTENTHANDLER_H__
#define __UPNPBASECONTENTHANDLER_H__

#include "upnperrors.h"
#include "upnpcommonupnplits.h"

#include <Xml/ContentHandler.h>
#include <Xml/DocumentParameters.h>
#include <f32file.h>
#include <CStack.h>

#include <Xml/Parser.h>
#include <Xml/ParserFeature.h>
#include <Xml/MatchData.h>

// CLASS DECLARATION

/**
*  This class implements the interface MContentHandler required by a client of the xml framework
*  It is designed to be used by xml parser that generates events using callback methods   
*
*  @since Series60 2.6
*/

// for upnpcontenthandlers
_LIT8(KUpnpRoot,                    "root");
_LIT8(KUpnpIconList,                "iconList");
_LIT8(KUpnpUpc,                     "UPC");
_LIT8(KUpnpInmpro3,                 "INMPR03");
_LIT8(KUpnpScpd,                    "scpd");
_LIT8(KUpnpActionList,              "actionList");
_LIT8(KUpnpArgument,                "argument");
_LIT8(KUpnpArgumentList,            "argumentList");
_LIT8(KUpnpServiceStateTable,       "serviceStateTable");
_LIT8(KUpnpName,                    "name");
_LIT8(KUpnpIn,                      "in");
_LIT8(KUpnpDirection,               "direction");
_LIT8(KUpnpRelatedStateVariable,    "relatedStateVariable");
_LIT8(KUpnpDataType,                "dataType");
_LIT8(KUpnpAllowedValue,            "allowedValue");
_LIT8(KUpnpDefaultValue,            "defaultValue");
_LIT8(KUpnpValue,                   "value");
_LIT8(KUpnpMinimum,                 "minimum");
_LIT8(KUpnpMaximum,                 "maximum");
_LIT8(KUpnpStep,                    "step");

using namespace Xml;


class CUpnpBaseContentHandler : public CBase, public MContentHandler
{        
public:
   	/**    
   	* Destructor    
    */	
	virtual ~CUpnpBaseContentHandler();
	
	/**    
   	* Removes all elements from the stack   
    */	
    void ClearResultsL();

public:	// from MContentHandler

    /**
    * This method is a callback to indicate the start of the document.
    * @param				aDocParam Specifies the various parameters of the document.
    * @arg				aDocParam.iCharacterSetName The character encoding of the document.
    * @param				aErrorCode is the error code. 
    * 					If this is not KErrNone then special action may be required.
    */
	virtual void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);

    /**
    * This method is a callback to indicate the end of the document.
    * @param				aErrorCode is the error code. 
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnEndDocumentL(TInt aErrorCode);
       
    /**
    * This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
    * This method is always called before the corresponding OnStartElementL method.
    * @param				aPrefix is the Namespace prefix being declared.
    * @param				aUri is the Namespace URI the prefix is mapped to.
    * @param				aErrorCode is the error code.
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
    								   TInt aErrorCode);
    /**
    * This method is a notification of the end of the scope of a prefix-URI mapping.
    * This method is called after the corresponding DoEndElementL method.
    * @param				aPrefix is the Namespace prefix that was mapped.
    * @param				aErrorCode is the error code.
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);

    /**
    * This method is a notification of ignorable whitespace in element content.
    * @param				aBytes are the ignored bytes from the document being parsed.
    * @param				aErrorCode is the error code.
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);

    /**
    * This method is a notification of a skipped entity. If the parser encounters an 
    * external entity it does not need to expand it - it can return the entity as aName 
    * for the client to deal with.
    * @param				aName is the name of the skipped entity.
    * @param				aErrorCode is the error code.
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);

    /**
    This method is a receive notification of a processing instruction.
    * @param				aTarget is the processing instruction target.
    * @param				aData is the processing instruction data. If empty none was supplied.
    * @param				aErrorCode is the error code.
    * 				If this is not KErrNone then special action may be required.
    */
    virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
    									  TInt aErrorCode);
    /**
    * This method indicates an error has occurred.
    * @param				aError is the error code
    */
    virtual void OnError(TInt aErrorCode);

    /**
    * This method obtains the interface matching the specified uid.
    * @return				0 if no interface matching the uid is found.
    * 				Otherwise, the this pointer cast to that interface.
    * @param				aUid the uid identifying the required interface.
    */
    virtual TAny* GetExtendedInterface(const TInt32 aUid);
    
        /**    
   	* Removes all elements from the stack if parsing failed
    */
	virtual void ClearStack();
	    
protected:
    
    /**    
   	* Default C++ constructor    
    */
    CUpnpBaseContentHandler();    
    
    /**    
   	* 2nd phase constructor dedicated to future, inherited class      
    */
    void BaseConstructL();    
    
    /**    
   	* Gets an object most recently pushed onto the stack   
    * @return    		
    */
    CBase* GetElementL();	
    
    /**
    * This method shows whether parsing of a xml document is finished
    * @since Series60 3.2 
    * @return TBool    
    */	
    virtual TBool IsReady() = 0;        
    
    /**
    * Sets a flag to indicate that parsing of the xml document has thrown an error
    * @since Series60 3.2     
    */	
    virtual void SetError() = 0; 
    
    /**
    * Sets a flag to indicate that parsing of the xml document has finished
    * @since Series60 3.2     
    */	   
    virtual void SetFinish() = 0;
   
   /**
    * Get content value.
    * @since Series60 3.2
    */
    HBufC8* GetContentL();

protected:
			
	CStack<CBase, EFalse> *iStack;
	HBufC8* iContent;	
	CParser* iParser;
	TBuf8<KMaxInfoSize> iInfo;
	HBufC8* iBodyTag1;
	HBufC8* iBodyTag2;
	TBool iChunk;			

};

#endif //__UPNPBASECONTENTHANDLER_H__