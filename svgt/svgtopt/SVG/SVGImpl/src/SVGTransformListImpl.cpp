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


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "GfxAffineTransform.h"
#include "SVGTransformListImpl.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTransformListImpl* CSvgTransformListImpl::NewL()
    {
    CSvgTransformListImpl* self = new ( ELeave ) CSvgTransformListImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTransformListImpl* CSvgTransformListImpl::NewLC()
    {
    CSvgTransformListImpl* self = new ( ELeave ) CSvgTransformListImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTransformListImpl::~CSvgTransformListImpl()
    {
    iTransformations.Close();
    iTransformAdditive.Close();
    iTransformTypes.Close();
    }

// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTransformListImpl::AppendItemL( TGfxAffineTransform& aTransform )
    {
    iTransformations.AppendL( aTransform );
    iTransformAdditive.AppendL( 2 );  // do nothing if not set Default: additive
    iTransformTypes.AppendL( 0 );  // Default: not animMotion
    iAvoidConsolidation = EFalse;
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTransformListImpl::AppendItemL( TGfxAffineTransform& aTransform,
                                          TInt32 aAdditive,
                                          TInt32 aSrcAnimMotion )
    {
    iTransformations.AppendL( aTransform );
    iTransformAdditive.AppendL( aAdditive );
    iTransformTypes.AppendL( aSrcAnimMotion );
    iAvoidConsolidation = EFalse;
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTransformListImpl::Clear()
    {
    iTransformations.Reset();
    iAvoidConsolidation = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TGfxAffineTransform CSvgTransformListImpl::Consolidate(TBool aOverwriteTrans)
    {
    if(!iAvoidConsolidation)
    {
    TInt numTransforms = iTransformations.Count();
    TInt i;
    TGfxAffineTransform tr,tr2,tr3;
    TGfxAffineTransform tmpAnimMotion;
    TInt32 tmpAdditive;
    TInt32 tmpAnimType;

    for ( i = 0; i < numTransforms; i++ )
        {
        tmpAdditive = iTransformAdditive[i];
        tmpAnimType = iTransformTypes[i];
        if ( tmpAnimType == 0 )
            {
             tr.Concatenate( iTransformations[i]);
            }
        }

    // first find the animate Motion

    for ( i = numTransforms-1 ; i > -1 ; i-- )
        {
        tmpAdditive = iTransformAdditive[i];
        tmpAnimType = iTransformTypes[i];
        if ( tmpAnimType == 1 && tmpAdditive == 0 )
            {
                {
                // overwrite the previous animTrans transforms,
                // but not the previous animMotion transforms
                //tr = iTransformations[i];
                tr2.Concatenate( iTransformations[i] );
                   //i--;
				break;
                }
            }
        }
        i++;
        for ( ;i < numTransforms; i++ )
        {
            if ( 1 == iTransformTypes[i] )
            {
                /*tmpAnimMotion = iTransformations[i];*/
                tr.Concatenate(tmpAnimMotion);
                tr2.Concatenate( iTransformations[i] );
            }
        }



    for ( i = 0; i < numTransforms; i++ )
        {
        tmpAdditive = iTransformAdditive[i];
        tmpAnimType = iTransformTypes[i];

        if ( tmpAnimType == 2 )
            {
            // if matrix is additive, continue calculation
            if ( !aOverwriteTrans)
                {
                tr3.Concatenate( iTransformations[i] );
                }
            else
                {
                // overwrite the previous animTrans transforms,
                // but not the previous animMotion transforms
                //tr = tmpAnimMotion;
                tr = tmpAnimMotion;
                tr3 = iTransformations[i];
                }
            }

		}


     tr.Concatenate(tr2);
	 tr.Concatenate(tr3);
		 /*Saving Conslidated Matrix for Later use*/
		iConsolidatedMatrix.iM00 = tr.iM00;
		iConsolidatedMatrix.iM01 = tr.iM01;
		iConsolidatedMatrix.iM02 = tr.iM02;
		iConsolidatedMatrix.iM10 = tr.iM10;
		iConsolidatedMatrix.iM11 = tr.iM11;
		iConsolidatedMatrix.iM12 = tr.iM12;
		iConsolidatedMatrix.iTransType = tr.iTransType;

		iAvoidConsolidation = ETrue;
		}
    return iConsolidatedMatrix;

	}




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TGfxAffineTransform CSvgTransformListImpl::GetItem( TInt aIndex )
    {
    if(aIndex < iTransformations.Count())
        {
        return iTransformations[aIndex];
        }
    else
        {
    		// what does this means.
    	TGfxAffineTransform a;
    		return a;

    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTransformListImpl::NumberOfItems()
    {
    return iTransformations.Count();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TGfxAffineTransform CSvgTransformListImpl::InitializeL( TGfxAffineTransform& aNewItem )
    {
    iTransformations.Reset();
    iTransformations.AppendL( aNewItem );
    iAvoidConsolidation = EFalse;
    return aNewItem;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TGfxAffineTransform CSvgTransformListImpl::ReplaceItem( TGfxAffineTransform newItem,
                                                        int aIndex )
    {
    if(aIndex < iTransformations.Count())
        {
        iTransformations[ aIndex ] = newItem;
        }
    iAvoidConsolidation = EFalse;
    return newItem;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTransformListImpl::SetAdditive( TInt32 additive, TInt32 index )
    {
    iTransformAdditive[ index ] = additive;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTransformListImpl* CSvgTransformListImpl::CloneL()
    {
    CSvgTransformListImpl* retValue = CSvgTransformListImpl::NewL();

    CleanupStack::PushL(retValue);
    TInt i;
    // copy iTransformations
    retValue->iTransformations.Reset();
    TInt transformationCnt = this->iTransformations.Count();
    for (i=0; i<transformationCnt; i++)
        retValue->iTransformations.AppendL(this->iTransformations[i]);
        {
        }

    // copy iTransformAdditive
    retValue->iTransformAdditive.Reset();
    TInt transformAdditiveCnt = this->iTransformAdditive.Count();
    for (i=0; i<transformAdditiveCnt; i++)
        {
        retValue->iTransformAdditive.AppendL(this->iTransformAdditive[i]);
        }

    // copy iTransformTypes
    retValue->iTransformTypes.Reset();
    TInt transformTypesCnt = this->iTransformTypes.Count();
    for (i=0; i<transformTypesCnt; i++)
        {
        retValue->iTransformTypes.AppendL(this->iTransformTypes[i]);
        }
    CleanupStack::Pop();
    return retValue;
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTransformListImpl::ConstructL()
    {
    TGfxAffineTransform tr;
    iTransformations.AppendL( tr );

    iTransformAdditive.AppendL( 2 ); // pass

    iTransformTypes.AppendL( 0 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTransformListImpl::CSvgTransformListImpl() : iTransformations( 1 ),
                                                 iTransformAdditive( 1 ),
                                                 iTransformTypes( 1 )

    {
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void	CSvgTransformListImpl::SetAvoidConsolidation(TBool aAvoidConsolidation)
	{
		iAvoidConsolidation = aAvoidConsolidation;
	}
