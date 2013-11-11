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
* Description:  Declares CUpnpIcon class
*
*/



// INCLUDE FILES
#include "upnpicon.h"
#include "upnpstring.h"
#include "upnpargument.h"
#include "upnpserviceimplementation.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpIcon::CUpnpIcon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpIcon::CUpnpIcon():iSendBufferPtr(0,0)
{

}

// -----------------------------------------------------------------------------
// CUpnpIcon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpIcon::ConstructL( )
{
    iFilename.Zero();
    
    iSendBuffer = ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
}

// -----------------------------------------------------------------------------
// CUpnpIcon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpIcon::ConstructL(const TDesC16& aPath, const TDesC8& aUrl, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType)
{
    iFilename.Zero();
    SetBitmapFilename(aPath);
    SetUrlL(aUrl);
    SetWidthL(aWidth);
    SetHeightL(aHeight);
    SetDepthL(aDepth);
    SetMimeTypeL(aMimeType);
    
    iSendBuffer = ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
}

// -----------------------------------------------------------------------------
// CUpnpIcon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpIcon::ConstructL(const TDesC16& aPath, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType)
{
    iFilename.Zero();
    SetBitmapFilename(aPath);
    SetWidthL(aWidth);
    SetHeightL(aHeight);
    SetDepthL(aDepth);
    SetMimeTypeL(aMimeType);
    
    iSendBuffer = ToDes8L();
    iSendBufferPtr.Set(iSendBuffer->Des());
}

// -----------------------------------------------------------------------------
// CUpnpIcon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpIcon* CUpnpIcon::NewL(  )
{
    CUpnpIcon* self = new (ELeave) CUpnpIcon();
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpIcon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpIcon* CUpnpIcon::NewL(const TDesC16& aPath, const TDesC8& aUrl, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType)
{
    CUpnpIcon* self = new (ELeave) CUpnpIcon();
    CleanupStack::PushL( self );
    self->ConstructL( aPath, aUrl, aWidth, aHeight, aDepth, aMimeType  );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpIcon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpIcon* CUpnpIcon::NewL(const TDesC16& aPath, const TInt aWidth, const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType)
{
    CUpnpIcon* self = new (ELeave) CUpnpIcon();
    CleanupStack::PushL( self );
    self->ConstructL( aPath, aWidth, aHeight, aDepth, aMimeType  );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpIcon::~CUpnpIcon
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpIcon::~CUpnpIcon()
{
    iProperties.ResetAndDestroy();
    iProperties.Close();
    
    if(iSendBuffer)
    {
        delete iSendBuffer;
    }
    //TRAP_IGNORE( DeleteIconFromFsL());
}

// -----------------------------------------------------------------------------
// CUpnpIcon::Url
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpIcon::Url() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KUrl().Compare( var->Name() ) == KErrNone )
        {
            return var->Value();
        }
        
        index++;
    }
    
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetUrlL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpIcon::SetUrlL(const TDesC8& aUrl)
{
    TInt index = 0;
     
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KUrl().Compare( var->Name() ) == KErrNone )
        {
            var->SetValueL(aUrl);
            return;
        }
        
        index++;
    }
    CUpnpDescriptionProperty* prop = CUpnpDescriptionProperty::NewL(KUrl, aUrl);
    iProperties.AppendL(prop);
}

// -----------------------------------------------------------------------------
// CUpnpIcon::GetIconBufferPtr
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtr8& CUpnpIcon::GetIconBufferPtr() const
{
	return iSendBufferPtr;
}

// -----------------------------------------------------------------------------
// CUpnpIcon::BitmapFilename
//
// -----------------------------------------------------------------------------
//
EXPORT_C TFileName CUpnpIcon::BitmapFilename() const
{
    return iFilename;
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetBitmapFilename
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpIcon::SetBitmapFilename( const TDesC& aName )
{
    iFilename.Zero();
    iFilename.Append( aName );  
}

// -----------------------------------------------------------------------------
// CUpnpIcon::Width
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpIcon::Width() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KWidth().Compare( var->Name() ) == KErrNone )
        {
            TLex8 val(var->Value());
            TInt ret(0);
            TInt err = val.Val( ret );
            if(err == KErrNone)
                return ret;
            else
                return err;
        }
        
        index++;
    }
    
    return KErrNotFound;    
}


// -----------------------------------------------------------------------------
// CUpnpIcon::WidthTPtrC8
//
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpIcon::WidthTPtrC8() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KWidth().Compare( var->Name() ) == KErrNone )
        {
            return var->Value();
        }
        
        index++;
    }
    
     return KNullDesC8();    
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetWidthL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::SetWidthL(TInt aWidth) 
{
    TInt index = 0;
    TBuf8<KMaxNumberPropertySize> nbuf;
    nbuf.Num(aWidth);
     
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KWidth().Compare( var->Name() ) == KErrNone )
        {
            var->SetValueL(nbuf);
            return;
        }
        
        index++;
    }
    CUpnpDescriptionProperty* prop = CUpnpDescriptionProperty::NewL(KWidth, nbuf);
    iProperties.AppendL(prop);
}

// -----------------------------------------------------------------------------
// CUpnpIcon::Depth
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpIcon::Depth() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KDepth().Compare( var->Name() ) == KErrNone )
        {
            TLex8 val(var->Value());
            TInt ret(0);
            TInt err = val.Val( ret );
            if(err == KErrNone)
                return ret;
            else
                return err;
        }
        
        index++;
    }
    
    return KErrNotFound;    
}

// -----------------------------------------------------------------------------
// CUpnpIcon::DepthTPtrC8
//
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpIcon::DepthTPtrC8() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KDepth().Compare( var->Name() ) == KErrNone )
        {
            return var->Value();
        }
        
        index++;
    }
    
     return KNullDesC8();   
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetDepthL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::SetDepthL(TInt aDepth) 
{
    TInt index = 0;
    TBuf8<KMaxNumberPropertySize> nbuf;
    nbuf.Num(aDepth);
     
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KDepth().Compare( var->Name() ) == KErrNone )
        {
            var->SetValueL(nbuf);
            return;
        }
        
        index++;
    }
    CUpnpDescriptionProperty* prop = CUpnpDescriptionProperty::NewL(KDepth, nbuf);
    iProperties.AppendL(prop);
}

// -----------------------------------------------------------------------------
// CUpnpIcon::Height
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpIcon::Height() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KHeight().Compare( var->Name() ) == KErrNone )
        {
            TLex8 val(var->Value());
            TInt ret(0);
            TInt err = val.Val( ret );
            if(err == KErrNone)
                return ret;
            else
                return err;
        }
        
        index++;
    }
    
    return KErrNotFound;    
}

// -----------------------------------------------------------------------------
// CUpnpIcon::HeightTPtrC8
//
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpIcon::HeightTPtrC8() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KHeight().Compare( var->Name() ) == KErrNone )
        {
            return var->Value();
        }
        
        index++;
    }
    
     return KNullDesC8();   
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetHeightL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::SetHeightL(TInt aHeight)
{
    TInt index = 0;
    TBuf8<KMaxNumberPropertySize> nbuf;
    nbuf.Num(aHeight);
     
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KHeight().Compare( var->Name() ) == KErrNone )
        {
            var->SetValueL(nbuf);
            return;
        }
        
        index++;
    }
    CUpnpDescriptionProperty* prop = CUpnpDescriptionProperty::NewL(KHeight, nbuf);
    iProperties.AppendL(prop);
}

// -----------------------------------------------------------------------------
// CUpnpIcon::MimeType
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpIcon::MimeType() const
{
    TInt index = 0;
    
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KMimeType().Compare( var->Name() ) == KErrNone )
        {
            return var->Value();
        }
        
        index++;
    }
    
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
// CUpnpIcon::SetMimeTypeL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::SetMimeTypeL(const TDesC8& aMimeType)
{

    TInt index = 0;
     
    while ( index < iProperties.Count() )
    {
        CUpnpDescriptionProperty* var = iProperties[index];
        
        if ( KMimeType().Compare( var->Name() ) == KErrNone )
        {
            var->SetValueL(aMimeType);
            return;
        }
        
        index++;
    }
    CUpnpDescriptionProperty* prop = CUpnpDescriptionProperty::NewL(KMimeType, aMimeType);
    iProperties.AppendL(prop);
}

// -----------------------------------------------------------------------------
// CUpnpIcon::DeleteIconFromFsL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::DeleteIconFromFsL()
{
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    fs.Delete(iFilename);
    fs.Close();
}

// -----------------------------------------------------------------------------
// CUpnpIcon::AddPropertyL
//
// -----------------------------------------------------------------------------
//
void CUpnpIcon::AddPropertyL( CUpnpDescriptionProperty* aProperty )
    {
    iProperties.AppendL( aProperty);
    }
    
// -----------------------------------------------------------------------------
// CUpnpIcon::ExternalizeL
//
// -----------------------------------------------------------------------------
//    
void CUpnpIcon::ExternalizeL( RWriteStream& aStream ) const  
{	
    aStream << BitmapFilename();
    aStream << Url();
    aStream.WriteInt8L(Width());
    aStream.WriteInt8L(Height());
    aStream.WriteInt8L(Depth());
    aStream << MimeType();
}

// -----------------------------------------------------------------------------
// CUpnpIcon::InternalizeL
//
// -----------------------------------------------------------------------------
//      
EXPORT_C  void CUpnpIcon::InternalizeL( RReadStream& aStream ) 
{
    SetBitmapFilename(HBufC16::NewLC( aStream, KMaxFileName)->Des());
    SetUrlL(HBufC8::NewLC( aStream, KMaxFileName)->Des());
    SetWidthL(aStream.ReadInt8L());
    SetHeightL(aStream.ReadInt8L());
    SetDepthL(aStream.ReadInt8L());
    SetMimeTypeL(HBufC8::NewLC( aStream, KMaxMimeTypeSize)->Des());
    
    CleanupStack::PopAndDestroy(3);
}
   
// -----------------------------------------------------------------------------
// CUpnpIcon::ToDes8L
//
// -----------------------------------------------------------------------------
//     
EXPORT_C HBufC8* CUpnpIcon::ToDes8L() const
{
 // serialize object
    CBufFlat* buf = CBufFlat::NewL(KDefBuffer);
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf); 
    CleanupClosePushL(stream);
    
    stream << *this;
    
    // create heap descriptor
    HBufC8* hbuf = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(hbuf->Des());
    buf->Read(0, ptr, buf->Size());
    
    // clean up
    CleanupStack::Pop(hbuf);
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
    
    return hbuf;
    	
}
// End of File
