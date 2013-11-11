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
* Description:  Declares the CUpnpDeviceDescriptionStore class
 *
*/


#ifndef C_CUPNPDEVICEDESCRIPTIONSTORE_H
#define C_CUPNPDEVICEDESCRIPTIONSTORE_H

#include <e32base.h> 
#include <xml/dom/xmlengdom.h> 
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengdomparser.h>
#include <upnpicon.h>

class CUpnpDevice;

const TInt KMinSpaceToWriteDescription = 204800;

class CUpnpDeviceDescriptionStore : public CBase
    {
public:

    /**    
     * Two phased constructor
     * @since S60 3.2    
     * @param aDescription device description xml file
     * @return an instance of CUpnpDeviceDescriptionStore class
     */
    IMPORT_C static CUpnpDeviceDescriptionStore* NewL( const TDesC& aDescription );

    /**    
     * Two phased constructor, leaves the object onto the CleanupStack
     * @since S60 3.2    
     * @param aDescription device description xml file
     * @return an instance of CUpnpDeviceDescriptionStore class
     */
    IMPORT_C static CUpnpDeviceDescriptionStore* NewLC( const TDesC& aDescription );

    /**    
     * Destructor
     * @since S60 3.2        
     */
    virtual ~CUpnpDeviceDescriptionStore();

    /**    
     * Returns description file
     * 
     * @since S60 5.1
     * @return file name with path
     */
    IMPORT_C const TDesC& DescriptionFile();
    
    /**     
     * Sets some property value in an emmbedded device.
     * @since S60 3.2
     * @param aName tag name
     * @param aValue tag value
     * @param aUuid device udn
     */
    IMPORT_C void SetPropertyL( const TDesC8& aName, const TDesC8& aValue,
        const TDesC8& aUuid );

    /**
     * Removes the tag of the device description of xml
     * @param aTagName - the tag of device that should be removed
     */
    IMPORT_C void RemoveTagL( const TDesC8& aTagName );

    /**
     * Adds the tag of the device description of xml
     * @param aTagName - the tag of device that should be added
     * @return the element that was added
     */
    IMPORT_C TXmlEngElement AddTagL( const TDesC8& aTagName );

    /**     
     * Removes icon property 
     * @since S60 3.2
     * @param aUrl
     */
    IMPORT_C void RemoveIconL( const TDesC8& aUrl );

    /**     
     * Removes all icons property 
     * @since S60 3.2
     */
    IMPORT_C void RemoveIconsL();

    /**     
     * Adds icon property 
     * @since S60 3.2
     * @param aIcon
     */
    IMPORT_C void AddIconL( const CUpnpIcon& aIcon );

    /*Updates icon property 
     * @since S60 3.2
     * @param aIcon
     */
    IMPORT_C void UpdateIconL( const CUpnpIcon& aIcon );

    /**
     * Gets some property value from an embedded device.
     * @since S60 3.2
     * @param aName tag name
     * @param aUuid device udn
     * @return property value
     */
    IMPORT_C TPtrC8 Property( const TDesC8& aName, const TDesC8& aUuid );

    /**     
     * Sets some property value in a root device.
     * @since S60 3.2
     * @param aName tag name
     * @param aValue tag value  
     */
    IMPORT_C void SetPropertyL( const TDesC8& aName, const TDesC8& aValue );

    /**     
     * Gets some property value from a root device.
     * @since S60 3.2
     * @param aName tag name 
     * @return property value
     */
    IMPORT_C TPtrC8 Property( const TDesC8& aName );

    /**     
     * Saves device description file path
     * @since S60 3.2    
     */
    IMPORT_C void SaveL();

    /**     
     * Get icons from device description xml.
     * Caller is owner of the returned array.
     * 
     * @since S60 5.1
     * @return array with icons
     */
    IMPORT_C RPointerArray< CUpnpIcon > IconListL();

private:

    /**    
     * Default C++ constructor
     * @since S60 3.2        
     */
    CUpnpDeviceDescriptionStore();

    /**    
     * 2nd phase constructor
     * @since S60 3.2    
     * @param aDescription    
     */
    void ConstructL( const TDesC& aDescription );

    /**    
     * Adds icon element
     * @since S60 3.2    
     * @param aPropertyIcon 
     * @param aMimeType icon mime type    
     * @param aWidth icon width
     * @param aHeight icon height
     * @param aDepth icon depth
     * @param aUrl icon url
     */
    void AddIconElementL( TXmlEngElement& aPropertyIcon,
        const TDesC8& aMimeType, const TDesC8& aWidth, const TDesC8& aHeight,
        const TDesC8& aDepth, const TDesC8& aUrl );

    /**    
     * Gets iconlist element
     * @since S60 3.2    
     */
    TXmlEngElement GetIconlistElementL();

private:
    

    RXmlEngDocument iDescription;
    RXmlEngDOMImplementation iDOMImpl;
    RXmlEngDOMParser iParser;

    RBuf iFileName;
    };

#endif // ? C_CUPNPDEVICEDESCRIPTIONSTORE_H
