/** @file
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  UPnP Device Description Store
*
*/


#include <upnpdominterface.h>

#include <upnpdevicedescriptionstore.h>
#include <upnpcommonupnplits.h>
#include "upnpfileutils.h"
#include "upnpicon.h"
#include "upnpdevice.h"
#include "upnpcontenthandlerscontroller.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::NewL()
// Two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceDescriptionStore *CUpnpDeviceDescriptionStore::NewL(
    const TDesC& aFilename )
    {
    CUpnpDeviceDescriptionStore* self = CUpnpDeviceDescriptionStore::NewLC( aFilename );    
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::NewLC()
// Two phased constructor, leaves the object onto the CleanupStack
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceDescriptionStore* CUpnpDeviceDescriptionStore::NewLC(
    const TDesC& aFilename )
    {
    CUpnpDeviceDescriptionStore* self = new (ELeave) CUpnpDeviceDescriptionStore();
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::~CUpnpDeviceDescriptionStore()
// Destructor of CUpnpDeviceDescriptionStore class
// ---------------------------------------------------------------------------
//
CUpnpDeviceDescriptionStore::~CUpnpDeviceDescriptionStore()
    {
    iFileName.Close();
    iDescription.Close();
    iParser.Close();
    iDOMImpl.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::DescriptionFile()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUpnpDeviceDescriptionStore::DescriptionFile()
    {
    return iFileName;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::SetPropertyL()
// Sets some property value in an emmbedded device.
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::SetPropertyL( const TDesC8& aName,
    const TDesC8& aValue, const TDesC8& aUuid )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement local;
    TXmlEngElement property;
    RArray<TXmlEngElement> elementList;

    TRAPD( error, UpnpDomInterface::GetElementListL( root, elementList, KUdn ) );

    for ( TInt i( 0); i < elementList.Count() ; i++ )
        {
        if ( !elementList[ i ].Value().Compare( aUuid ) )
            {
            elementList.Close();

            TRAP( error, UpnpDomInterface::GetElementListL( root,
                elementList, aName ) );

            if ( elementList.Count() && i < elementList.Count() )
                {
                property = elementList[ i ];
                elementList.Close();
                if ( !error )
                    {
                    property.SetValueL( aValue );
                    return;
                    }
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            }
        }

    elementList.Close();
    User::Leave( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::RemoveTagL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::RemoveTagL( const TDesC8& aTagName )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement element;
    TBool elementFound = EFalse;

    elementFound = UpnpDomInterface::GetElementL( root, element, aTagName );
    if ( elementFound )
        {
        UpnpDomInterface::DeleteElement( element );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::AddTagL
// -----------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement CUpnpDeviceDescriptionStore::AddTagL( const TDesC8& aTagName )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement level1;
    TXmlEngElement level2;
    TBool elementFound = EFalse;

    elementFound = UpnpDomInterface::GetElementL( root, level1, KDevice );
    if ( elementFound )
        {
        elementFound = UpnpDomInterface::GetElementL( level1, level2,
            aTagName );
        if ( !elementFound )
            {
            level2 = level1.AddNewElementL( aTagName );
            }
        }
    return level2;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::AddIconL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::AddIconL( const CUpnpIcon& aIcon )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement propertyref;
    TXmlEngElement property;
    TXmlEngElement propertyiconlist;
    TXmlEngElement url;
    RArray<TXmlEngElement> IconList;

    TRAPD( error, UpnpDomInterface::GetElementL( root, property, KDevice ) );
    if ( !error && property.NotNull() )
        {
        TRAPD( error, UpnpDomInterface::GetElementL( property,
            propertyiconlist, KIconList ) );
        if ( !error && propertyiconlist.NotNull() )
            {
            TRAPD( error, UpnpDomInterface::GetElementListL(
                propertyiconlist, IconList, KIcon ) );
            if ( !error && IconList.Count() != 0 )
                {
                AddIconElementL( propertyiconlist, aIcon.MimeType(),
                    aIcon.WidthTPtrC8(), aIcon.HeightTPtrC8(),
                    aIcon.DepthTPtrC8(), aIcon.Url() );
                }
            User::LeaveIfError( error );
            }
        else
            {
            propertyref = property.AddNewElementL( KIconList );
            AddIconElementL( propertyref, aIcon.MimeType(),
                aIcon.WidthTPtrC8(), aIcon.HeightTPtrC8(),
                aIcon.DepthTPtrC8(), aIcon.Url() );
            }
        User::LeaveIfError( error );
        }
    User::LeaveIfError( error );

    IconList.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::UpdateIconL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::UpdateIconL( const CUpnpIcon& aIcon )
    {
    TXmlEngElement propertyiconlist = GetIconlistElementL();
    TXmlEngElement url;
    RArray<TXmlEngElement> IconList;

    TRAPD( error, UpnpDomInterface::GetElementListL( propertyiconlist,
        IconList, KIcon ) );
    if ( !error && IconList.Count() != 0 )
        {
        for ( TInt i=0; i<IconList.Count(); i++ )
            {
            TRAPD( error, UpnpDomInterface::GetElementL( IconList[ i ], url,
                KUrl ) );
            if ( !error && url.NotNull() )
                {
                if ( !url.Value().Compare( aIcon.Url() ) )
                    {
                    IconList[i].Remove();
                    AddIconElementL( propertyiconlist, aIcon.MimeType(),
                        aIcon.WidthTPtrC8(), aIcon.HeightTPtrC8(),
                        aIcon.DepthTPtrC8(), aIcon.Url() );
                    }
                }
            }
        User::LeaveIfError( error );
        }
    IconList.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::RemoveIconL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::RemoveIconL( const TDesC8& aUrl )
    {
    TXmlEngElement propertyiconlist = GetIconlistElementL();
    TXmlEngElement url;
    RArray<TXmlEngElement> IconList;

    TRAPD( error, UpnpDomInterface::GetElementListL( propertyiconlist,
        IconList, KIcon ) );
    if ( !error && IconList.Count()!=0 )
        {
        for ( TInt i=0; i<IconList.Count(); i++ )
            {
            TRAPD( error, UpnpDomInterface::GetElementL( IconList[ i ], url,
                KUrl ) );
            if ( !error && url.NotNull() )
                {
                if ( !url.Value().Compare( aUrl ) )
                    {
                    if ( IconList.Count() == 1 )
                        {
                        propertyiconlist.Remove();
                        }
                    else
                        {
                        IconList[i].Remove();
                        }
                    }
                }
            }
        User::LeaveIfError( error );
        }
    IconList.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::RemoveIconsL()
// ---------------------------------------------------------------------------
//  
EXPORT_C void CUpnpDeviceDescriptionStore::RemoveIconsL()
    {
    TXmlEngElement propertyiconlist = GetIconlistElementL();
    if ( propertyiconlist.NotNull() )
        {
        propertyiconlist.Remove();
        }
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::GetIconlistElementL()
// Gets iconlist element
// ---------------------------------------------------------------------------
//    
EXPORT_C TXmlEngElement CUpnpDeviceDescriptionStore::GetIconlistElementL()
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement property;
    TXmlEngElement propertyiconlist;

    TRAPD( error, UpnpDomInterface::GetElementL( root, property, KDevice ) );
    if ( !error && property.NotNull() )
        {
        TRAPD( error, UpnpDomInterface::GetElementL( property,
            propertyiconlist, KIconList ) );
        if ( !error && propertyiconlist.NotNull() )
            {
            return propertyiconlist;
            }
        User::LeaveIfError( error );
        }
    User::LeaveIfError( error );

    return NULL;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::Property()
// Gets some property value from an embedded device.
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpDeviceDescriptionStore::Property( const TDesC8& aName,
    const TDesC8& aUuid )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement local;
    TXmlEngElement property;
    RArray<TXmlEngElement> elementList;
    TPtrC8 urn = KNullDesC8();

    TRAPD( error, UpnpDomInterface::GetElementListL( root, elementList, KUdn ) );

    for ( TInt i( 0); i < elementList.Count() ; i++ )
        {
        if ( !elementList[ i ].Value().Compare( aUuid ) )
            {
            elementList.Close();

            TRAP( error, UpnpDomInterface::GetElementListL( root,
                elementList, aName ) );

            if ( elementList.Count() )
                {
                property = elementList[ i ];

                if ( !error && property.Value().Length() )
                    {
                    urn.Set( property.Value() );
                    break;
                    }
                }
            }
        }
    elementList.Close();
    return urn;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::SetPropertyL()
// Sets some property value in a root device.
// ---------------------------------------------------------------------------
//   
EXPORT_C void CUpnpDeviceDescriptionStore::SetPropertyL( const TDesC8& aName,
    const TDesC8& aValue )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement level1;

    UpnpDomInterface::GetElementL( root, level1, aName );

    if ( level1.NotNull() )
        {
        level1.SetValueL( aValue );
        }

    else if ( !level1.NotNull() )
        {
        level1 = AddTagL( aName );

        if ( level1.NotNull() )
            {
            level1.SetValueL( aValue );
            }
        }
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::Property()
// Gets some property value from a root device.
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpDeviceDescriptionStore::Property( const TDesC8& aName )
    {
    TXmlEngElement root = iDescription.DocumentElement();
    TXmlEngElement property;
    TPtrC8 urn = KNullDesC8();

    TRAPD( error, UpnpDomInterface::GetElementL( root, property, aName ) );

    if ( !error && property.NotNull() && property.Value().Length() )
        {
        urn.Set( property.Value() );
        }

    return urn;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::SaveL()
// Saves device description file path
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceDescriptionStore::SaveL()
    {
    if ( UpnpFileUtil::CheckDiskSpaceShortL( EDriveC, KMinSpaceToWriteDescription ) )
        {
        User::Leave( KErrDiskFull );
        }
    iDescription.SaveL( iFileName );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::IconListL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray< CUpnpIcon > CUpnpDeviceDescriptionStore::IconListL()
    {
    RPointerArray< CUpnpIcon > iconList;

    HBufC8* descr = UpnpFileUtil::ReadFileL( iFileName );
    CleanupStack::PushL( descr );
    
    CUpnpContentHandlersController* controller = CUpnpContentHandlersController::NewLC();
    CUpnpDevice* device = controller->ParseDeviceL( *descr );
    CleanupStack::PopAndDestroy( controller );
    CleanupStack::PushL( device );

    for ( TInt i = 0; i < device->Icons().Count(); i++ )
        {
        iconList.AppendL( device->Icons()[ i ] );
        }
    device->Icons().Reset();
    
    CleanupStack::PopAndDestroy( device );
    CleanupStack::PopAndDestroy( descr );
    
    return iconList;
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::CUpnpDeviceDescriptionStore()
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CUpnpDeviceDescriptionStore::CUpnpDeviceDescriptionStore()
    {
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::ConstructL()
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CUpnpDeviceDescriptionStore::ConstructL( const TDesC& aFileName )
    {
    iFileName.CreateL( aFileName );

    HBufC8* descr = UpnpFileUtil::ReadFileL( aFileName );
    CleanupStack::PushL( descr );

    iDOMImpl.OpenL();
    User::LeaveIfError( iParser.Open( iDOMImpl ) );
    iDescription = iParser.ParseL( *descr );
    CleanupStack::PopAndDestroy( descr );
    }

// ---------------------------------------------------------------------------
// CUpnpDeviceDescriptionStore::AddIconElementL()
// Adds icon element
// ---------------------------------------------------------------------------
//    
void CUpnpDeviceDescriptionStore::AddIconElementL(
    TXmlEngElement& aPropertyIcon, const TDesC8& aMimeType,
    const TDesC8& aWidth, const TDesC8& aHeight, const TDesC8& aDepth,
    const TDesC8& aUrl )
    {
    TXmlEngElement propertyIcon = aPropertyIcon.AddNewElementL( KIcon );
    TXmlEngElement propertyref = propertyIcon.AddNewElementL( KMimeType );
    propertyref.SetTextL( aMimeType );
    propertyref = propertyIcon.AddNewElementL( KWidth );
    propertyref.SetTextL( aWidth );
    propertyref = propertyIcon.AddNewElementL( KHeight );
    propertyref.SetTextL( aHeight );
    propertyref = propertyIcon.AddNewElementL( KDepth );
    propertyref.SetTextL( aDepth );
    propertyref = propertyIcon.AddNewElementL( KUrl );
    propertyref.SetTextL( aUrl );
    }

//  End of File
