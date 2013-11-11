// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// class definition for phone as modem at extension plugin
// 
//

#ifndef PAMPLUGIN_H
#define PAMPLUGIN_H

#include <etel3rdparty.h>
#include <atextpluginbase.h>


class CPamEngine;

class CPamPlugin : public CATExtPluginBase
  	{
public:

	/**
	* Two-phased constructor.
	*/
	static CPamPlugin* NewL();
	
	/**
	* Destructor.
	*/
	~CPamPlugin();

	void NameReqCallback( const TDesC8& aName );

private:
    //From CATExtPluginBase
    TBool IsCommandSupported( const TDesC8& aCmd ) ;

    void HandleCommand( const TDesC8& aCmd, RBuf8& aReply, TBool aReplyNeeded );
    void HandleCommandCancel();
    TInt NextReplyPartLength();
    TInt GetNextPartOfReply( RBuf8& aNextReply );
    void ReceiveUnsolicitedResult();
    void ReceiveUnsolicitedResultCancel();
    void ReportNvramStatusChange( const TDesC8& aNvram );
    void ReportExternalHandleCommandError();
    void ReportConnectionName( const TDesC8& aName );
    
private:    
    /**
    * Read Supported Access points
    */   
    TInt SupportedAccessPoint( RBuf8& aReply );


    /**
    * Read Access points data
    */   
    TInt ReadAccessPoint(TInt aAccessPoint, RBuf8& aReply );


    
private:

    TInt HandleRunError(TInt aErr);
    

private:

    CPamPlugin();
    
    void ConstructL();
       
    
private:
    // Data

    CPamEngine* iPAMEngine;
    RBuf8* iReply;
    };

#endif      // PAMPLUGIN_H
            
// End of File
