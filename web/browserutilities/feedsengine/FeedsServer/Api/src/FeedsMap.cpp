/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Map to store attributes and its values
*
*/


#include "FeedsMap.h"
// -----------------------------------------------------------------------------
// CFeedsMap::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
 EXPORT_C CFeedsMap* CFeedsMap::NewL()
    {
    CFeedsMap * self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
	
// -----------------------------------------------------------------------------
// CFeedsMap::NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsMap* CFeedsMap::NewLC()
    {
    CFeedsMap * self = new (ELeave) CFeedsMap;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsMap::CFeedsMap
//
// C++ default constructor can NOT contain any code, that
// might leave.  
// -----------------------------------------------------------------------------
//
CFeedsMap::CFeedsMap():iValueString(NULL)
    {	
    }

// -----------------------------------------------------------------------------
// CFeedsMap::~CFeedsMap
//
// Deconstructor.  
// -----------------------------------------------------------------------------
//
 EXPORT_C CFeedsMap::~CFeedsMap()
    {
    delete iValueString;	
    iEnums.Reset();
    iStart.Reset();
    iSize.Reset();
    }
	
// -----------------------------------------------------------------------------
// CFeedsMap::ConstructL
//
// Symbian 2nd phase constructor can leave.  
// -----------------------------------------------------------------------------
//
void CFeedsMap::ConstructL()
    {
    iEnums.Reset();
    iStart.Reset();
    iSize.Reset();
    if(iValueString != NULL)
       {
       delete iValueString;
       iValueString = NULL; 
       }
    iValueString = HBufC::NewL(32);
    }	

// -----------------------------------------------------------------------------
// CFeedsMap::ResetL
//
// Resets the Map
// -----------------------------------------------------------------------------
//
 EXPORT_C void CFeedsMap::ResetL()
    {
    ConstructL();
    }

// -----------------------------------------------------------------------------
// CFeedsMap::SetStringAttribute
//
// Sets string attribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::SetStringAttribute(TUint anAttribute, const TDesC& aValue)
    {
    TInt pos=0;
    HBufC* tempPtr = NULL;
    TInt length = iValueString->Des().Length();
    TInt nLength = aValue.Length();

    if( ( pos = iEnums.Find( anAttribute ) ) != KErrNotFound )
    {
        if(nLength > iSize[pos])
        {
        	tempPtr = iValueString->ReAlloc(length + nLength-iSize[pos]);
        	if(tempPtr == NULL)
        		return KErrNoMemory;
        	iValueString = tempPtr;
        }
        iValueString->Des().Delete(iStart[pos],iSize[pos]);
        for(TInt i = 0;i<iStart.Count();i++)
        {
        	if(iStart[i] >= iStart[pos])
        		iStart[i] = iStart[i] - iSize[pos];
        }
        iStart.Remove(pos);
        iEnums.Remove(pos);
        iSize.Remove(pos);
        iEnums.Append(anAttribute);
        iStart.Append(iValueString->Des().Length());
        iSize.Append(nLength);
        iValueString->Des().Append(aValue);
        return KErrNone;
    }
    else if(length+nLength >iValueString->Des().MaxLength())
    {
        tempPtr = iValueString->ReAlloc(length + nLength);
        if(tempPtr == NULL)
        	return KErrNoMemory;
        iValueString = tempPtr;
    }
    iValueString->Des().Append(aValue);

    iEnums.Append(anAttribute);
    iStart.Append(length);
    iSize.Append(nLength);

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFeedsMap::SetIntegerAttribute
//
// Sets integer attribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::SetIntegerAttribute(TUint anAttribute, TInt aValue)
    {
    TBuf16<25>  buf;
    buf.AppendNum(aValue);
    TInt pos=0;
    HBufC* tempPtr = NULL;
    TInt length = iValueString->Des().Length();
    TInt nLength = buf.Length();

    if( ( pos = iEnums.Find( anAttribute ) ) != KErrNotFound )
        {
        if(nLength > iSize[pos])
        {
        	tempPtr = iValueString->ReAlloc(length + nLength-iSize[pos]);
        	if(tempPtr == NULL)
        		return KErrNoMemory;
        	iValueString = tempPtr;
        }
        iValueString->Des().Delete(iStart[pos],iSize[pos]);
        for(TInt i = 0;i<iStart.Count();i++)
        {
        	if(iStart[i] >= iStart[pos])
        		iStart[i] = iStart[i] - iSize[pos];
        }
        iStart.Remove(pos);
        iEnums.Remove(pos);
        iSize.Remove(pos);
        iEnums.Append(anAttribute);
        iStart.Append(iValueString->Des().Length());
        iSize.Append(nLength);
        iValueString->Des().Append(buf);
        return KErrNone;
        }
    else if(length+nLength >iValueString->Des().MaxLength())
        {
        tempPtr = iValueString->ReAlloc(length + nLength);
        if(tempPtr == NULL)
        	return KErrNoMemory;
        iValueString = tempPtr;
        }
    iValueString->Des().Append(buf);

    iEnums.Append(anAttribute);
    iStart.Append(length);
    iSize.Append(nLength);

    return KErrNone;	
    }

// -----------------------------------------------------------------------------
// CFeedsMap::SetTimeAttribute
//
// Sets time attribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::SetTimeAttribute(TUint anAttribute, TTime aValue)
    {
    TBuf16<25>  buf;
    buf.Format(_L("%Ld"), aValue.Int64());
    TInt pos=0;
    HBufC* tempPtr = NULL;
    TInt length = iValueString->Des().Length();
    TInt nLength = buf.Length();

    if( ( pos = iEnums.Find( anAttribute ) ) != KErrNotFound )
    {
        if(nLength > iSize[pos])
        {
        	tempPtr = iValueString->ReAlloc(length + nLength-iSize[pos]);
        	if(tempPtr == NULL)
        		return KErrNoMemory;
        	iValueString = tempPtr;
        }
        iValueString->Des().Delete(iStart[pos],iSize[pos]);
        for(TInt i = 0;i<iStart.Count();i++)
        {
        	if(iStart[i] >= iStart[pos])
        		iStart[i] = iStart[i] - iSize[pos];
        }
        iStart.Remove(pos);
        iEnums.Remove(pos);
        iSize.Remove(pos);
        iEnums.Append(anAttribute);
        iStart.Append(iValueString->Des().Length());
        iSize.Append(nLength);
        iValueString->Des().Append(buf);
        return KErrNone;
    }
    else if(length+nLength >iValueString->Des().MaxLength())
    {
        tempPtr = iValueString->ReAlloc(length + nLength);
        if(tempPtr == NULL)
        	return KErrNoMemory;
        iValueString = tempPtr;
    }
    iValueString->Des().Append(buf);

    iEnums.Append(anAttribute);
    iStart.Append(length);
    iSize.Append(nLength);

    return KErrNone;	
    }

// -----------------------------------------------------------------------------
// CFeedsMap::GetStringAttribute
//
// Gets string attribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::GetStringAttribute(TUint anAttribute, TPtrC& aValue) const
    {
    TInt pos=0;
    if((pos = iEnums.Find(anAttribute)) != KErrNotFound)
        {
        TInt start = iStart[pos];
        TInt end= iSize[pos];
        aValue.Set(iValueString->Mid(iStart[pos],iSize[pos]));
        return KErrNone;
        }
    else 
        {
        return KErrNotFound;	
        }		
    }

// -----------------------------------------------------------------------------
// CFeedsMap::GetIntegerAttribute
//
// Gets integer attribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::GetIntegerAttribute(TUint anAttribute, TInt& aValue) const
    {
    TInt pos=0;
    TPtrC value;
    if((pos = iEnums.Find(anAttribute)) != KErrNotFound)
        {
        value.Set(iValueString->Mid(iStart[pos],iSize[pos]));
        TLex16  lex(value);
        lex.Val(aValue);	
        return KErrNone;	
        }
    else 
        {
        return KErrNotFound;
        }		
    }
	
// -----------------------------------------------------------------------------
// CFeedsMap::GetTimeAttribute
//
// Gets time aatribute
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::GetTimeAttribute(TUint anAttribute, TTime& aValue) const
    {
    TInt pos=0;
    TPtrC value;
    if((pos = iEnums.Find(anAttribute)) != KErrNotFound)
        {
        value.Set(iValueString->Mid(iStart[pos],iSize[pos]));
        TInt64 ts;
        TLex16  lex(value);
        lex.Val(ts);	
        aValue = ts;
        return KErrNone;
        }
    else 
        {
        return KErrNotFound;
        }		
    }

// -----------------------------------------------------------------------------
// CFeedsMap::IsExist
//
// Returns ETrue if specified attribute exists in the map
// -----------------------------------------------------------------------------
//
 EXPORT_C TBool CFeedsMap::IsExist(TUint aEnum) const
    {
    if(iEnums.Find(aEnum) != KErrNotFound)
        {
        return ETrue;
        }
    else
        {
        return EFalse;	
        }		
    }

// -----------------------------------------------------------------------------
// CFeedsMap::Count()
//
// Counts number of entries
// -----------------------------------------------------------------------------
//
 EXPORT_C TInt CFeedsMap::Count() const
    {
    return iEnums.Count();
    }

// -----------------------------------------------------------------------------
// CFeedsMap::operator[]
//
// Return the aatribute at the specified index
// -----------------------------------------------------------------------------
//
 EXPORT_C TUint CFeedsMap::operator[](TInt index) const
    {
    return iEnums[index];
    }

