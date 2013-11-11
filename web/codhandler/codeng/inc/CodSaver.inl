/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Inline method definitions of class CCodSaver.   
*      
*
*/


#ifndef COD_SAVER_INL
#define COD_SAVER_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodSaver::CCodSaver()
// ---------------------------------------------------------
//
CCodSaver::CCodSaver( const TDesC8& aType )
: iType( aType ),
  iSize( 0 ),
  iMaxSize( 0 ),
  iHandler( KNullUid ),
  iState( EInit )
    {
    }

// ---------------------------------------------------------
// CCodSaver::DataType()
// ---------------------------------------------------------
//
const TDataType& CCodSaver::DataType() const
    {
    return iType;
    }

// ---------------------------------------------------------
// CCodSaver::DataSize()
// ---------------------------------------------------------
//
TInt CCodSaver::DataSize() const
    {
    return iSize;
    }

// ---------------------------------------------------------
// CCodSaver::SetSourceUriL()
// ---------------------------------------------------------
//
void CCodSaver::SetSourceUriL( const TDesC8& aSourceUri )
    {
    HBufC8* buf = aSourceUri.AllocL();
    delete iSourceUri;
    iSourceUri = buf;
    }

// ---------------------------------------------------------
// CCodSaver::SetParams()
// ---------------------------------------------------------
//
void CCodSaver::SetParams( const CAiwGenericParamList* aParams )
    {
    iParams = aParams;
    }

#endif /* def COD_SAVER_INL */
