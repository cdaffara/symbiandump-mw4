/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef SENUSER_H
#define SENUSER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "senbaseidentity.h"

// CLASS DECLARATION

_LIT(KSenUser, "SenUser");

/**
*  CSenUser
* 
*/
class CSenUser : public CBase
{
public: // Constructors and destructor

	/**
		* Destructor.
		*/
	~CSenUser();

		/**
		* Two-phased constructor.
		*/
	static CSenUser* NewL(const TDesC8& aUserName, const TDesC8& aPIN, TBool aActive = EFalse);

		/**
		* Two-phased constructor.
		*/
	static CSenUser* NewLC(const TDesC8& aUserName, const TDesC8& aPIN, TBool aActive = EFalse);
    
     IMPORT_C inline  TDesC8& UserName() const;
	 IMPORT_C inline  TDesC8& PIN() const;
     IMPORT_C inline  TBool Active() const;
     IMPORT_C inline  RPointerArray<CSenBaseIdentity>& Identites();

    

private:

	/**
		* Constructor for performing 1st stage construction
		*/
	CSenUser(TBool aActive);

	/**
		* EPOC default constructor for performing 2nd stage construction
		*/
	void ConstructL(const TDesC8& aUserName, const TDesC8& aPIN);


    RBuf8 iUserName;
    RBuf8 iPIN;
    TBool iActive;
    RPointerArray<CSenBaseIdentity> iBaseIdentities;
    
};

#include "senuser.inl"

#endif // SENUSER_H

