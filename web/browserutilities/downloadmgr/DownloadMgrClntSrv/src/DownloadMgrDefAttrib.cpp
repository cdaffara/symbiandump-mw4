/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the declaration of the default attrib of Download Mgr Server.
*
*/



// INCLUDE FILES
#include "DownloadMgrDefAttrib.h"
#include "DownloadMgrLogger.h"

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// TDMgrUtils::TDMgrUtils
// -----------------------------------------------------------------------------
//
TDMgrUtils::TDMgrUtils()
    {
    }

// ---------------------------------------------------------
// TDMgrUtils::NextAttribL
// ---------------------------------------------------------
//
TBool TDMgrUtils::NextAttribL( CDefaultAttrib* aAttrib,
                               const TDesC8& aPackedAttribs )
    {
    CLOG_ENTERFN_NULL( "TDMgrUtils::NextDefaultAttribL" )
    // ||: type;attribute;length;value :||
    if( iCurrent >= aPackedAttribs.Length() )
        {
        return EFalse;
        }

    TInt current( 0 );
    THttpDownloadMgrIntegerBuf intBuf( 0 );

    //CLOG_WRITE( "TDMgrUtils::NextDefaultAttrib - start" );
    // type info length
    TPtrC8 type( iPtr.Mid( current, intBuf.Size() ) );
    //CLOG_WRITE( "type info length" );
    // + (type info length)
    current = current + intBuf.Size();
    //CLOG_WRITE( "+ (type info length)" );
    // attribute info length
    TPtrC8 attribute( iPtr.Mid( current, intBuf.Size() ) );
    //CLOG_WRITE( "attribute info length" );
    //  + (attribute info length)
    current = current + intBuf.Size();
    //CLOG_WRITE( "+ (attribute info length)" );
    // data length info length
    TPtrC8 data_length( iPtr.Mid( current, intBuf.Size() ) );
    //CLOG_WRITE( "data length info length" );
    //  + (data length info length)
    current = current + intBuf.Size();
    intBuf.Copy( data_length );
    TPtrC8 value( iPtr.Mid( current, intBuf() ) );
    //CLOG_WRITE( "+ (data length info length)" );
    //  + (data length)
    current = current + intBuf();
    //CLOG_WRITE( "+ (data length)" );
    // = next item
    iCurrent = iCurrent + current;
    iPtr.Set( aPackedAttribs.Mid( iCurrent, ( aPackedAttribs.Length() - iCurrent ) ) );
    //CLOG_WRITE( "= next item" );
    // set aAttrib
    aAttrib->Reset();
    //CLOG_WRITE( "reset atrib" );
    aAttrib->SetL( type, attribute, value );
    //CLOG_WRITE( "set aAttrib" );

    return ETrue;
    }

// ---------------------------------------------------------
// TDMgrUtils::AllocDefAttribBufLC
// ---------------------------------------------------------
//
HBufC8* TDMgrUtils::AllocDefAttribBufLC( CArrayPtrFlat< CDefaultAttrib >* aAttribs )
    {
    //__ASSERT_DEBUG( ( aAttribs != NULL ), User::Panic( KErrArgument ) );
    
    CLOG_ENTERFN_NULL( "TDMgrUtils::AllocDefAttribBufLC" )
    TInt length( 0 );

    for( TInt i = 0; i < aAttribs->Count(); i++ )
        {
        CDefaultAttrib* attrib = (*aAttribs)[i];
        length = length + attrib->PackedAttribSize();
        }

    //CLOG_WRITE_FORMAT( "length %d", length );
    return HBufC8::NewLC( length );
    }

// ---------------------------------------------------------
// TDMgrUtils::PackedAttributesL
// ---------------------------------------------------------
//
HBufC8* TDMgrUtils::PackedAttributesL( CArrayPtrFlat< CDefaultAttrib >* aAttribs )
    {
    //__ASSERT_DEBUG( aAttribs, User::Panic( KErrArgument ) );
    
    CLOG_WRITE_NULL( "TDMgrUtils::PackedDefaultAttributesLC" )
    HBufC8* buf = AllocDefAttribBufLC( aAttribs );
    TPtr8 ptr( buf->Des() );

    for( TInt i = 0; i < aAttribs->Count(); i++ )
        {
        CDefaultAttrib* attrib = (*aAttribs)[i];
        attrib->PackAttribute( ptr );
        }

    CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------
// TDMgrUtils::PackEventAttribL
// ---------------------------------------------------------
//
HBufC8* TDMgrUtils::PackEventAttribL( TInt32 aUserData, TInt32 aMODownloadedSize,TInt32 aDownloadedSize, TInt32 aMOAttrLength, TInt32 aAttrLength )
    {
    THttpDownloadMgrIntegerBuf intBuf( 0 );
    HBufC8* buf = HBufC8::NewL( 6 * intBuf.Size() );
    TPtr8 package = buf->Des();
    
    //EDlAttrUserData
    intBuf() = aUserData;
    package.Append( intBuf );
    
    //EDlAttrMODownloadedSize
    intBuf() = aMODownloadedSize;
    package.Append( intBuf );

    //EDlAttrDownloadedSize
    intBuf() = aDownloadedSize;
    package.Append( intBuf );
          
    //EDlAttrMOLength
    intBuf() = aMOAttrLength;
    package.Append( intBuf );

    //EDlAttrLength
    intBuf() = aAttrLength;
    package.Append( intBuf );
        
    return buf;
    }

// ---------------------------------------------------------
// TDMgrUtils::UnPackEventAttribL
// ---------------------------------------------------------
//        
void TDMgrUtils::UnPackEventAttribL( const TDesC8& aPack, 
									 TInt32& aUserData, 
									 TInt32& aMODownloadedSize,
									 TInt32& aDownloadedSize, 
									 TInt32& aMOAttrLength,  
									 TInt32& aAttrLength, 
									 TInt32& aMoIndex )
    {
    THttpDownloadMgrIntegerBuf intBuf( 0 );
    TInt current( 0 );
    TInt32 maskedData = 0;
    
    //EDlAttrUserData
    intBuf.Copy( aPack.Mid( current, intBuf.Size() ) );
    maskedData = intBuf(); 
    current = current + intBuf.Size();
       
    //EDlAttrMultipleMODownloadedSize
    intBuf.Copy( aPack.Mid( current, intBuf.Size() ) );
    aMODownloadedSize= intBuf(); 
    current = current + intBuf.Size();

    //EDlAttrDownloadedSize
    intBuf.Copy( aPack.Mid( current, intBuf.Size() ) );
    aDownloadedSize= intBuf(); 
    current = current + intBuf.Size();
              
    //EDlAttrLength
    intBuf.Copy( aPack.Mid( current, intBuf.Size() ) );
    aMOAttrLength= intBuf(); 
    current = current + intBuf.Size();         
    
    //EDlAttrMultipleMOLength
    intBuf.Copy( aPack.Mid( current, intBuf.Size() ) );
    aAttrLength= intBuf(); 
    current = current + intBuf.Size();         
    
    // Unmask maskedData to get UserData and MoIndex
    aUserData = (maskedData & 0x0000FFFF);
    aMoIndex = (maskedData >> 16);
    }
    
// -----------------------------------------------------------------------------
// CDefaultAttrib::CDefaultAttrib
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDefaultAttrib::CDefaultAttrib()
    {
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::CDefaultAttrib
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDefaultAttrib::CDefaultAttrib( THttpDownloadAttrib aAttribute, TAttribType aType )
    {
    iAttribute = aAttribute;
    iType = aType;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::ConstructL( TInt32 aValue )
    {
    iInt32 = aValue;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::ConstructL( TBool aValue )
    {
    iBool = aValue;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::ConstructL( const TDesC8& aValue )
    {
    iBufC8 = aValue.AllocL();
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::ConstructL( const TDesC16& aValue )
    {
    iBufC16 = aValue.AllocL();
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDefaultAttrib* CDefaultAttrib::NewL()
    {
    CDefaultAttrib* self = new( ELeave ) CDefaultAttrib();
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDefaultAttrib* CDefaultAttrib::NewL( THttpDownloadAttrib aAttribute,
                                      TInt32              aValue )
    {
    CDefaultAttrib* self = new( ELeave ) CDefaultAttrib( aAttribute, EInt );
    
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDefaultAttrib* CDefaultAttrib::NewL( THttpDownloadAttrib aAttribute,
                                      TBool               aValue )
    {
    CDefaultAttrib* self = new( ELeave ) CDefaultAttrib( aAttribute, EBool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDefaultAttrib* CDefaultAttrib::NewL( THttpDownloadAttrib aAttribute,
                                      const TDesC8&       aValue )
    {
    CDefaultAttrib* self = new( ELeave ) CDefaultAttrib( aAttribute, EString8 );
    
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDefaultAttrib* CDefaultAttrib::NewL( THttpDownloadAttrib aAttribute,
                                        const TDesC16&      aValue )
    {
    CDefaultAttrib* self = new( ELeave ) CDefaultAttrib( aAttribute, EString16 );
    
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CDefaultAttrib::~CDefaultAttrib()
    {
    delete iBufC16;
    iBufC16 = NULL;

    delete iBufC8;
    iBufC8 = NULL;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::AsTInt32L
// -----------------------------------------------------------------------------
//
TInt32 CDefaultAttrib::AsTInt32L() const
    {
    if( iType == EInt )
        {
        return iInt32;
        }
    else
        {
        User::Leave( KErrNotFound );
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::AsTBoolL
// -----------------------------------------------------------------------------
//
TBool CDefaultAttrib::AsTBoolL() const
    {
    if( iType == EBool )
        {
        return iBool;
        }
    else
        {
        User::Leave( KErrNotFound );
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::AsTPtrC8L
// -----------------------------------------------------------------------------
//
TPtrC8 CDefaultAttrib::AsTPtrC8L() const
    {
    if( iType == EString8 )
        {
        return iBufC8->Des();
        }
    else
        {
        User::Leave( KErrNotFound );
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::AsTPtrC16L
// -----------------------------------------------------------------------------
//
TPtrC16 CDefaultAttrib::AsTPtrC16L() const
    {
    if( iType == EString16 )
        {
        return iBufC16->Des();
        }
    else
        {
        User::Leave( KErrNotFound );
        return KNullDesC16();
        }
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::PackAttribute
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::PackAttribute( TDes8& aPackage )
    {
    THttpDownloadMgrIntegerBuf intBuf( 0 );
    intBuf() = iType;
    aPackage.Append( intBuf );
    intBuf() = (TInt32)iAttribute;
    aPackage.Append( intBuf );

    switch( iType )
        {
        case EString8:
            {
            intBuf() = iBufC8->Length();
            aPackage.Append( intBuf );
            aPackage.Append( iBufC8->Des() );
            }
            break;
        case EString16:
            {
            intBuf() = iBufC16->Size();
            aPackage.Append( intBuf );
            TUint8* ptr8 = &(aPackage[0]) + aPackage.Size();
            // Here we use the padding byte because we can
            // cast to even memory address only
            if(  (TInt)ptr8%2 == 1 )
                {
                ptr8 = ptr8 + 1;
                }
            TPtr16 ptr( (TUint16*)(ptr8), iBufC16->Length() );
            ptr.Copy( *iBufC16 );
            aPackage.SetLength( aPackage.Size() + iBufC16->Size() );
            }
            break;
        case EInt:
            {
            intBuf() = intBuf.Size();
            aPackage.Append( intBuf );
            intBuf() = iInt32;
            aPackage.Append( intBuf );
            }
            break;
        case EBool:
            {
            intBuf() = intBuf.Size();
            aPackage.Append( intBuf );
            intBuf() = iBool;
            aPackage.Append( intBuf );
            }
            break;
        default:
            break;
        }

    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::PackedAttribLength
// -----------------------------------------------------------------------------
//
TInt CDefaultAttrib::PackedAttribSize()
    {
    TInt length( 0 );
            
    THttpDownloadMgrIntegerBuf intBuf( 0 );
    // length of type info
    length = length + intBuf.Size();
    // length of attribute info
    length = length + intBuf.Size();
    // length of size info
    length = length + intBuf.Size();

    switch( iType )
        {
        case EString8:
            {
            // data length
            length = length + iBufC8->Size();
            }
            break;
        case EString16:
            {
            // data length
            // We must add 1 padding byte, in case of odd memory address 
            // when the 16 bits descipror is copied to the 8 bits buffer.
            length = length + iBufC16->Size() + 1;
            }
            break;
        case EInt:
        case EBool:
            {
            // data length
            length = length + intBuf.Size();
            }
            break;
        default:
            break;
        }

    return length;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::Reset
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::Reset()
    {
    delete iBufC16;
    iBufC16 = NULL;

    delete iBufC8;
    iBufC8 = NULL;
    }

// -----------------------------------------------------------------------------
// CDefaultAttrib::SetL
// -----------------------------------------------------------------------------
//
void CDefaultAttrib::SetL( const TDesC8& aType,
                           const TDesC8& aAttribute,
                           const TDesC8& aValue )
    {
    THttpDownloadMgrIntegerBuf intBuf( 0 );
    intBuf.Copy( aType );
    iType = (TAttribType)intBuf();
    intBuf.Copy( aAttribute );
    iAttribute = (THttpDownloadAttrib)intBuf();

    delete iBufC16;
    iBufC16 = NULL;

    delete iBufC8;
    iBufC8 = NULL;

    switch( iType )
        {
        case EString8:
            {
            iBufC8 = aValue.AllocL();
            }
            break;
        case EString16:
            {
            iBufC16 = HBufC16::NewL( aValue.Size() / 2 );
            const TUint8* ptr8 = &(aValue[0]);
            // Here we use the padding byte because we can
            // cast to even memory address only
            if(  (TInt)ptr8%2 == 1 )
                {
                ptr8 = ptr8 + 1;
                }
            TPtrC16 ptr( (TUint16*)ptr8, aValue.Size() / 2 );
            iBufC16->Des().Copy( ptr );
            }
            break;
        case EInt:
            {
            intBuf.Copy( aValue );
            iInt32 = intBuf();
            }
            break;
        case EBool:
            {
            intBuf.Copy( aValue );
            iBool = intBuf();
            }
            break;
        default:
            break;
        }
    }

//  End of File  
