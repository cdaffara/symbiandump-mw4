/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#include "SVGMemoryManager.h"
#include "SVGPaintCssValueImpl.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGIntCssValueImpl.h"
#include "SVGStrCssValueImpl.h"
#include "SVGVectorCssValueImpl.h"
#include "SVGClrCssValueImpl.h"
#include "SVGDocumentImpl.h"

class CSvgElementImpl;

const TUint CSvgMemoryManager::KCssPaintBlockSize  = 20;
const TUint CSvgMemoryManager::KCssFloatBlockSize  = 20;
const TUint CSvgMemoryManager::KCssIntBlockSize    = 20;
const TUint CSvgMemoryManager::KCssClrBlockSize    = 20;
const TUint CSvgMemoryManager::KCssStrBlockSize    = 20;
const TUint CSvgMemoryManager::KCssVectorBlockSize = 4;

// ==========================================================================
// Need method description
// ==========================================================================
CSvgMemoryManager* CSvgMemoryManager::NewL()
    {
    CSvgMemoryManager*   self    = new ( ELeave ) CSvgMemoryManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgMemoryManager* CSvgMemoryManager::NewLC()
    {
    CSvgMemoryManager*   self    = new ( ELeave ) CSvgMemoryManager;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgMemoryManager::ConstructL()
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgMemoryManager::CSvgMemoryManager() :
    iCPaintCssValueImplArrays( 2 ),
    iCFloatCssValueImplArrays( 2 ),
    iCClrCssValueImplArrays( 2 ),
    iCIntCssValueImplArrays( 2 ),
    iCStrCssValueImplArrays( 2 ),
    iCVectorCssValueImplArrays( 1 )
    {
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgMemoryManager::~CSvgMemoryManager()
    {
    //delete all of the paint blocks
    TInt cPaintCSSValueImplArraysCnt = iCPaintCssValueImplArrays.Count();
    for (TInt i=0; i < cPaintCSSValueImplArraysCnt; i++)
        {
        delete [] iCPaintCssValueImplArrays[i];
        }
    iCPaintCssValueImplArrays.Close();


    //delete all of the float blocks
    TInt cFloatValueImplArraysCnt = iCFloatCssValueImplArrays.Count();
    for (TInt i=0; i < cFloatValueImplArraysCnt; i++)
        {
        delete [] iCFloatCssValueImplArrays[i];
        }
    iCFloatCssValueImplArrays.Close();

    //delete all of the clr blocks
    TInt cClrCssValueImplArraysCnt = iCClrCssValueImplArrays.Count();
    for (TInt i=0; i < cClrCssValueImplArraysCnt; i++)
        {
        delete [] iCClrCssValueImplArrays[i];
        }
    iCClrCssValueImplArrays.Close();


    //delete all of the int blocks
    TInt cIntCssValueImplArraysCnt = iCIntCssValueImplArrays.Count();
    for (TInt i=0; i < cIntCssValueImplArraysCnt; i++)
        {
        delete [] iCIntCssValueImplArrays[i];
        }
    iCIntCssValueImplArrays.Close();

    //delete all of the str blocks
    TInt cStrCssValueImplArraysCnt = iCStrCssValueImplArrays.Count();
    for (TInt i=0; i < cStrCssValueImplArraysCnt; i++)
        {
        delete [] iCStrCssValueImplArrays[i];
        }
    iCStrCssValueImplArrays.Close();

    //delete all of the vector blocks
    TInt cVectorCssValueImplArraysCnt = iCVectorCssValueImplArrays.Count();
    for (TInt i=0; i < cVectorCssValueImplArraysCnt; i++)
        {
        delete [] iCVectorCssValueImplArrays[i];
        }
    iCVectorCssValueImplArrays.Close();

    }

// ==========================================================================
// Need method description
// ==========================================================================
CCssValue* CSvgMemoryManager::CloneCssValueL(CCssValue* aCssValue )
{
    CCssValue* lCssValueCloned = NULL;

    //see if the value is in a paint block
    TInt cPaintCssValueImplArraysCnt = iCPaintCssValueImplArrays.Count();
    for (TInt i=0; i < cPaintCssValueImplArraysCnt; i++)
    {
        CPaintCssValueImpl* aPaintBlock = iCPaintCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aPaintBlock[0] && (TInt)aCssValue <= (TInt)&aPaintBlock[KCssPaintBlockSize])
        {
            //aValue is a Paint clone it as such
            if ( ((CPaintCssValueImpl*)aCssValue)->iUrlId )
            {
                lCssValueCloned = GetCssPaintObjectL(((CPaintCssValueImpl*)aCssValue)->iUrlId->Des(),
                    ((CPaintCssValueImpl*)aCssValue)->iSvgElementImpl);
   			}
   			else
   			{
   				lCssValueCloned = GetCssPaintObjectL( ((CPaintCssValueImpl*)aCssValue)->iSvgElementImpl );
   			}

   			MGfxPaint* value = ((CPaintCssValueImpl*)aCssValue)->Value();

   			if (value)
   			{
   				((CPaintCssValueImpl*)lCssValueCloned)->CloneRGBValueL( value->GetColor() );
   			}

            return lCssValueCloned;
        }
    }

    //see if the value is in a float block
    TInt cFloatCssValueImplArraysCnt = iCFloatCssValueImplArrays.Count();
    for (TInt i=0; i < cFloatCssValueImplArraysCnt; i++)
    {
        CFloatCssValueImpl* aFloatBlock = iCFloatCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aFloatBlock[0] && (TInt)aCssValue <= (TInt)&aFloatBlock[KCssFloatBlockSize])
        {
            //aValue is a Float clone it as such
            lCssValueCloned = GetCssFloatObjectL(((CFloatCssValueImpl*)aCssValue)->Value());
            return lCssValueCloned;
        }
    }

    //see if the value is in a str block
    TInt cClrCssValueImplArraysCnt = iCClrCssValueImplArrays.Count();
    for (TInt i=0; i < cClrCssValueImplArraysCnt; i++)
    {
        CClrCssValueImpl* aClrBlock = iCClrCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aClrBlock[0] && (TInt)aCssValue <= (TInt)&aClrBlock[KCssClrBlockSize])
        {
            //aValue is a Clr clone it as such
            lCssValueCloned = GetCssClrObjectL();
            ((CClrCssValueImpl*)lCssValueCloned)->SetL((CClrCssValueImpl*)aCssValue);

            return lCssValueCloned;
        }
    }

    //see if the value is in an int block
    TInt cIntCssValueImplArraysCnt = iCIntCssValueImplArrays.Count();
    for (TInt i=0; i < cIntCssValueImplArraysCnt; i++)
    {
        CIntCssValueImpl* aIntBlock = iCIntCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aIntBlock[0] && (TInt)aCssValue <= (TInt)&aIntBlock[KCssIntBlockSize])
        {
            //aValue is an Int clone it as such
            lCssValueCloned = GetCssIntObjectL(((CIntCssValueImpl*)aCssValue)->Value());
            return lCssValueCloned;
        }
    }

    //see if the value is in a str block
    TInt cStrCssValueImplArraysCnt = iCStrCssValueImplArrays.Count();
    for (TInt i=0; i < cStrCssValueImplArraysCnt; i++)
    {
        CStrCssValueImpl* aStrBlock = iCStrCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aStrBlock[0] && (TInt)aCssValue <= (TInt)&aStrBlock[KCssStrBlockSize])
        {
            //aValue is a string clone it as such
            //makes a copy of the string
            lCssValueCloned = GetCssStrObjectL(((CStrCssValueImpl*)aCssValue)->Value());
            return lCssValueCloned;
        }
    }

    //see if the value is in a vector block
    TInt cVectorCssValueImplArraysCnt = iCVectorCssValueImplArrays.Count();
    for (TInt i=0; i < cVectorCssValueImplArraysCnt; i++)
    {
        CVectorCssValueImpl* aVectorBlock = iCVectorCssValueImplArrays[i];
        if ((TInt)aCssValue >= (TInt)&aVectorBlock[0] && (TInt)aCssValue <= (TInt)&aVectorBlock[KCssVectorBlockSize])
        {
            //aValue is a vector clone it as such
            //makes a copy of the vector
            lCssValueCloned = GetCssVectorObjectL(((CVectorCssValueImpl*)aCssValue)->Value());
            return lCssValueCloned;
        }
   	}

   	return NULL;
}

// ==========================================================================
// BEGIN CIntCssValue Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssIntBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssIntBlockL( )
{
    //setup the new current block
    iCurrentIntObjectIndex = 0;

    iCIntCssValueImplArray = new ( ELeave ) CIntCssValueImpl[KCssIntBlockSize];

    //put the new block into the larger tracking array.
    iCIntCssValueImplArrays.AppendL(iCIntCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssIntObject
// MAIN allocation block
// ==========================================================================
CIntCssValueImpl* CSvgMemoryManager::GetCssIntObjectL( TInt aInitValue )
{
        if ( !iCIntCssValueImplArray || iCurrentIntObjectIndex >= KCssIntBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssIntBlockL();
        }
        CIntCssValueImpl* lIntObject = &iCIntCssValueImplArray[iCurrentIntObjectIndex++];
		lIntObject->SetValueL(aInitValue);
		return ( lIntObject );
}

// ==========================================================================
// CSvgMemoryManager::GetCssIntObject
// ==========================================================================
CIntCssValueImpl* CSvgMemoryManager::GetCssIntObjectL( CIntCssValueImpl* aIntValue )
{
	return GetCssIntObjectL( aIntValue->Value() );
}

// ==========================================================================
// CSvgMemoryManager::GetCssIntObject
// ==========================================================================
CIntCssValueImpl* CSvgMemoryManager::GetCssIntObjectL( const TDesC& aValueString )
{
	CIntCssValueImpl* intObject = GetCssIntObjectL();

	intObject->SetValueL(aValueString);

	return intObject;

}
// ==========================================================================
// END OF CIntCssValue Memory Management
// ==========================================================================

// ==========================================================================
// BEGIN CPaintCssValueImpl Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssPaintBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssPaintBlockL()
{
    //setup the new current block
    iCurrentPaintObjectIndex = 0;

    iCPaintCssValueImplArray = new ( ELeave ) CPaintCssValueImpl[KCssPaintBlockSize];

	//may not need this as long as we always pass in initial values for paint
	//Mem::FillZ( iCPaintCssValueImplArray, sizeof(CPaintCssValueImpl)*KCssPaintBlockSize);
	
    //put the new block into the larger tracking array.
    iCPaintCssValueImplArrays.AppendL(iCPaintCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssPaintObject
// ==========================================================================
CPaintCssValueImpl* CSvgMemoryManager::GetCssPaintObjectL( CSvgElementImpl* aElementImpl )
    {
        if ( !iCPaintCssValueImplArray || iCurrentPaintObjectIndex >= KCssPaintBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssPaintBlockL();
        }

		CPaintCssValueImpl* lPaintObject = &iCPaintCssValueImplArray[iCurrentPaintObjectIndex++];
		lPaintObject->SetElement(aElementImpl);
		return ( lPaintObject );
	}

// ==========================================================================
// CSvgMemoryManager::GetCssPaintObject
// ==========================================================================
CPaintCssValueImpl* CSvgMemoryManager::GetCssPaintObjectL( const TDesC& aValue, CSvgElementImpl* aElementImpl)
{
	CPaintCssValueImpl* lPaintObject  = GetCssPaintObjectL(aElementImpl);

    CSvgElementImpl* lReferencedElement = NULL;
   	// iUrlFlag is true means this paint is gradient.

   	CSvgDocumentImpl *lDoc = (CSvgDocumentImpl *)aElementImpl->OwnerDocument();

   	if (lDoc)
   		{
       	lReferencedElement = ( CSvgElementImpl*)lDoc->GetElementById( aValue );
   		}

	if(lReferencedElement)
		{
		lPaintObject->SetUrlflag();
		lPaintObject->SetUrlIdL(aValue);
		lPaintObject->iValue = lPaintObject->Value();
		}
	else
		{
		lPaintObject->SetValueL(aValue);
		}
	return lPaintObject;
}
// ==========================================================================
// END OF CPaintCssValueImpl Memory Management
// ==========================================================================

// ==========================================================================
// BEGIN CFloatCssValueImpl Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssFloatBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssFloatBlockL( )
{
    //setup the new current block
    iCurrentFloatObjectIndex = 0;

    iCFloatCssValueImplArray = new ( ELeave ) CFloatCssValueImpl[KCssFloatBlockSize];

    //put the new block into the larger tracking array.
    iCFloatCssValueImplArrays.AppendL(iCFloatCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssFloatObject
// MAIN allocation block
// ==========================================================================
CFloatCssValueImpl* CSvgMemoryManager::GetCssFloatObjectL( float aFloat )
    {
        if ( !iCFloatCssValueImplArray || iCurrentFloatObjectIndex >= KCssFloatBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssFloatBlockL();
        }

		CFloatCssValueImpl* lFloatObject = &iCFloatCssValueImplArray[iCurrentFloatObjectIndex++];
		lFloatObject->SetValueL( aFloat );
		return ( lFloatObject );
	}

// ==========================================================================
// CSvgMemoryManager::GetCssIntObject
// ==========================================================================
CFloatCssValueImpl* CSvgMemoryManager::GetCssFloatObjectL( CFloatCssValueImpl* aFloatValue )
{
	return GetCssFloatObjectL( aFloatValue->Value() );
}

// ==========================================================================
// CSvgMemoryManager::GetCssFloatObject
// ==========================================================================
CFloatCssValueImpl* CSvgMemoryManager::GetCssFloatObjectL( const TDesC& aValueString )
{
	CFloatCssValueImpl* floatObject = GetCssFloatObjectL();

	floatObject->SetValueL(aValueString);

	return floatObject;

}
// ==========================================================================
// END OF CFloatCssValueImpl Memory Management
// ==========================================================================

// ==========================================================================
// BEGIN CClrCssValueImpl Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssClrBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssClrBlockL( )
{
    //setup the new current block
    iCurrentClrObjectIndex = 0;

    iCClrCssValueImplArray = new ( ELeave ) CClrCssValueImpl[KCssClrBlockSize];

    //put the new block into the larger tracking array.
    iCClrCssValueImplArrays.AppendL(iCClrCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssClrObject
// MAIN allocation block
// ==========================================================================
CClrCssValueImpl* CSvgMemoryManager::GetCssClrObjectL( )
    {
        if ( !iCClrCssValueImplArray || iCurrentClrObjectIndex >= KCssClrBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssClrBlockL();
        }

		CClrCssValueImpl* lClrObject = &iCClrCssValueImplArray[iCurrentClrObjectIndex++];
		return ( lClrObject );
	}

// ==========================================================================
// CSvgMemoryManager::GetCssClrObject
// ==========================================================================
CClrCssValueImpl* CSvgMemoryManager::GetCssClrObjectL( const TDesC& aValueString )
{
	CClrCssValueImpl* clrObject = GetCssClrObjectL();

	clrObject->SetValueL(aValueString);

	return clrObject;

}
// ==========================================================================
// END OF CClrCssValueImpl Memory Management
// ==========================================================================

// ==========================================================================
// BEGIN CStrCssValueImpl Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssStrBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssStrBlockL( )
{
    //setup the new current block
    iCurrentStrObjectIndex = 0;

    iCStrCssValueImplArray = new ( ELeave ) CStrCssValueImpl[KCssStrBlockSize];

	//may not need this as long as we always pass in initial values for string
    Mem::FillZ( iCStrCssValueImplArray, sizeof(CStrCssValueImpl)*KCssStrBlockSize);

    //put the new block into the larger tracking array.
    iCStrCssValueImplArrays.AppendL(iCStrCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssStrObject
// MAIN allocation block
// ==========================================================================
CStrCssValueImpl* CSvgMemoryManager::GetCssStrObjectL( const TDesC& aValue )
    {
        if ( !iCStrCssValueImplArray || iCurrentStrObjectIndex >= KCssStrBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssStrBlockL();
        }

		CStrCssValueImpl* lStrObject = &iCStrCssValueImplArray[iCurrentStrObjectIndex++];
		lStrObject->SetValueL(aValue);
		return ( lStrObject );
	}
// ==========================================================================
// END OF CStrCssValueImpl Memory Management
// ==========================================================================

// ==========================================================================
// BEGIN CVectorCssValueImpl Memory Management
// ==========================================================================
// ==========================================================================
// CSvgMemoryManager::AllocateCssVectorBlockL
// ==========================================================================
void CSvgMemoryManager::AllocateCssVectorBlockL( )
{
    //setup the new current block
    iCurrentVectorObjectIndex = 0;

    iCVectorCssValueImplArray = new ( ELeave ) CVectorCssValueImpl[KCssVectorBlockSize];

    Mem::FillZ( iCVectorCssValueImplArray, sizeof(CVectorCssValueImpl)*KCssVectorBlockSize);

    //put the new block into the larger tracking array.
    iCVectorCssValueImplArrays.AppendL(iCVectorCssValueImplArray);

}

// ==========================================================================
// CSvgMemoryManager::GetCssVectorObject
// MAIN allocation block
// ==========================================================================
CVectorCssValueImpl* CSvgMemoryManager::GetCssVectorObjectL( const TDesC& aValue )
    {
        if ( !iCVectorCssValueImplArray || iCurrentVectorObjectIndex >= KCssVectorBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssVectorBlockL();
        }

		CVectorCssValueImpl* lVectorObject = &iCVectorCssValueImplArray[iCurrentVectorObjectIndex++];
		lVectorObject->SetValueL(aValue);
		return ( lVectorObject );
	}

// ==========================================================================
// CSvgMemoryManager::GetCssVectorObject
// MAIN allocation block
// ==========================================================================
CVectorCssValueImpl* CSvgMemoryManager::GetCssVectorObjectL( CArrayFix<TFloatFixPt>* aValue )
    {
        if ( !iCVectorCssValueImplArray || iCurrentVectorObjectIndex >= KCssVectorBlockSize )
        {
          //we have used all of the objects in the current array
            //and need to allocate another one
            AllocateCssVectorBlockL();
        }

		CVectorCssValueImpl* lVectorObject = &iCVectorCssValueImplArray[iCurrentVectorObjectIndex++];
		lVectorObject->CloneValueL(aValue);
		return ( lVectorObject );
	}

// ==========================================================================
// END OF CVectorCssValueImpl Memory Management
// ==========================================================================

void CSvgMemoryManager::Print()
{
    #ifdef _DEBUG
        RDebug::Printf("CSvgMemoryManager:: iCPaintCssValueImplArrays objects: %d",
        (iCPaintCssValueImplArrays.Count() - 1) * KCssPaintBlockSize + iCurrentPaintObjectIndex);
        RDebug::Printf("CSvgMemoryManager:: iCFloatCssValueImplArrays objects: %d",
        (iCFloatCssValueImplArrays.Count() - 1) * KCssFloatBlockSize + iCurrentFloatObjectIndex);
        RDebug::Printf("CSvgMemoryManager:: iCClrCssValueImplArrays objects: %d",
        (iCClrCssValueImplArrays.Count() - 1) * KCssClrBlockSize + iCurrentClrObjectIndex);
        RDebug::Printf("CSvgMemoryManager:: iCIntCssValueImplArrays objects: %d",
        (iCIntCssValueImplArrays.Count() - 1) * KCssIntBlockSize + iCurrentIntObjectIndex);
        RDebug::Printf("CSvgMemoryManager:: iCStrCssValueImplArrays objects: %d",
        (iCStrCssValueImplArrays.Count() - 1) * KCssStrBlockSize + iCurrentStrObjectIndex);
        RDebug::Printf("CSvgMemoryManager:: iCVectorCssValueImplArrays objects: %d",
        (iCVectorCssValueImplArrays.Count() - 1) * KCssVectorBlockSize + iCurrentVectorObjectIndex);
    #endif
}


