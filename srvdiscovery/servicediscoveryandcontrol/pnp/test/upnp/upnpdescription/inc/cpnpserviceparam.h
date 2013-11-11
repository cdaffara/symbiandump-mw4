// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CPNPSERVICEPARAM_H_
#define __CPNPSERVICEPARAM_H_

#include <e32cmn.h>
#include "cupnpproperty.h"

class CUPnPAction;
class CUPnPStateVariable;
class CUPnPArgument;
class CUPnPAllowedValue;

/** 
Container for action and service state objects

@publishedPartner
@prototype */

class CUPnPServiceDescription: public CUPnPProperty
	{

public:
	IMPORT_C static CUPnPServiceDescription* NewL();
	IMPORT_C ~CUPnPServiceDescription();

   
    inline void SetMinorNumberL(const TInt aValue);
    inline const TInt MinorNumber () const;

    inline void SetMajorNumberL(const TInt aValue);
    inline const TInt MajorNumber () const;
    
    inline void AppendToActionList(const CUPnPAction* aValue);
    inline TInt CountOfActionList () const;
    inline const CUPnPAction* AtActionList( TInt aIndex) const ;

    inline void AppendToServiceStateTable(const CUPnPStateVariable* aValue);
    inline TInt CountOfServiceStateTable () const;
    inline const CUPnPStateVariable* AtServiceStateTable(const TInt aIndex) const;

    TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const;
private :

	CUPnPServiceDescription();
	void ConstructL();

private :
	TVersion 							iVersion;
	RPointerArray<CUPnPAction> 			iActionList;
	RPointerArray<CUPnPStateVariable>	iServiceStateTable;
	};


/*
Following class store the service action information.
*/
class CUPnPAction: public CUPnPProperty
	{

public:
	static CUPnPAction* NewL();
	~CUPnPAction();


    inline void AppendL(const CUPnPArgument* aValue);
    inline TInt Count () const;
    inline CUPnPArgument* At(TInt aIndex) const;

	
	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const;

private :

	CUPnPAction();
	void ConstructL();
    
private :
	RPointerArray<CUPnPArgument> 		iArgumentList;		
	};
/* 
Following class store argument object information.
 */

class CUPnPArgument: public CUPnPProperty
	{

public:
	static CUPnPArgument* NewL();
	~CUPnPArgument();
	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const;
	void SetRetVal( TBool aValue );
	TBool RetVal();
private :

	CUPnPArgument();
	void ConstructL();
	TBool iRetVal;
private:
	};
/* 
 Following structure store state varibale information. 
*/
class CUPnPStateVariable: public CUPnPProperty
	{

public:
	static CUPnPStateVariable* NewL();
	~CUPnPStateVariable();

    inline void SetAllowedValuesL( CUPnPAllowedValue *aValue);
    inline const CUPnPAllowedValue* AllowedValues () const;
    inline void SetAllowedValueType ( TBool aStatus );
    inline TBool AllowedValueType ( ) const ;
    TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const;
	
private :
	CUPnPStateVariable();
	void ConstructL();    
    TBool isValue;
	CUPnPAllowedValue*	iAllowedValue;
	};
/* 
 Following structure is the base class for allowed list or range objects. 
*/

class CUPnPAllowedValue: public CUPnPProperty
	{
public:
	enum TAllowedValueType
				{
				EArgValueList,
				EArgValueRange,
				};
		TAllowedValueType iType;

	};
/* 
 Following structure stores information about argument value list. 
*/
class CUPnPArgValueList: public CUPnPAllowedValue
	{

public:
	static CUPnPArgValueList* NewL();
	~CUPnPArgValueList();

    inline void AppendL(const TDesC8& aValue);
    inline  TInt Count () const;
    inline const TDesC8& At(const TInt aIndex) const;
private :

	CUPnPArgValueList();
	void ConstructL();
private :
	RArray<RBuf8>	iAllowedValueList;
	};

/* 
 CUPnPValueRange stores the upnp argument value range. 
*/
class CUPnPValueRange: public CUPnPAllowedValue
	{
public:
	IMPORT_C static CUPnPValueRange* NewL();
	IMPORT_C ~CUPnPValueRange();
  
private :

	CUPnPValueRange();
	void ConstructL();

	};

#include "cpnpserviceparam.inl"	
#endif /*CPNPSERVICEPARAM_H_*/
