/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpSvgImageConverter class declaration.
*
*/

#ifndef __UPNPSVGIMAGECONVERTER_H__
#define __UPNPSVGIMAGECONVERTER_H__

// Include Files
#include <SVGEngineInterfaceImpl.h>

/**
 * CUpnpSvgImageConverter class acts as an intermediate between the invoker and the
 * actual converter.Class encapsulates svg to bitmap conversion process.
 * Does all the pre-processing needed before invoking actual conversion API.
 * It takes svg file as an input and provides bmp ( converted ) file as an output.
 * This class is responsible for creating a new thread and allocating resources to it.
 * The icon file conversion happens in the new thread being created.
 */
class CUpnpSvgImageConverter : public CBase                      
    {
public:
    /**
    * Two-phased constructor
    * @param aIconWidth width of the requested icon
    * @param aIconHeight height of the requested icon
    */
    static CUpnpSvgImageConverter* NewL( TInt aIconWidth, TInt aIconHeight );
    /**
     * Prepares the DOM for given SVG file, creates a new thread and returns
     * the converted icon(bmp) file path
     */ 
    void ConvertToBitmapL( const TDesC& aSvgFile, RBuf& aBitmapFile );
    ~CUpnpSvgImageConverter();
    CFbsBitmap& BitmapObject()const;
    //static thread method serving as thread's main function
    static TInt ImageConverter( TAny* aParam );
    //Leaving variant of thread function
    static void ImageConverterL( CUpnpSvgImageConverter& aSvgConverter ); 
  
private: 
    // Constructors
    CUpnpSvgImageConverter( );
    void ConstructL( TInt aIconWidth, TInt aIconHeight );
    // Thread creation method
    void StartThreadL( );
    // "Getter" method to fetch file path
    const TDesC& Filepath( )const;
        
private:
    RFbsSession                  iFbsSession;
    RFs                          iFileSession; 
    CFbsBitmap*                  iMask;
    CSvgEngineInterfaceImpl*     iSvgModule;
    RBuf                         iBitMapFilePath;
    CFbsBitmap*                  iBitmap;
    };

#endif // __UPNPSVGIMAGECONVERTER_H__ 
