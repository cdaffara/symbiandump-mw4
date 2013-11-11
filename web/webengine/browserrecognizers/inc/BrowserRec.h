/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Recognizer for the bowser supported MIME types  
*
*/

#ifndef BROWSEREC_H
#define BROWSEREC_H

// includes
#include <apmrec.h>  // For CApaDataRecognizerType
#include <Browser_platform_variant.hrh>

// constants
const TUid KUidMimeBrowserRecognizer = { 0x1005A02 };

const TInt KBrowserRecognizerImplUIDValue = 0x101F880E;
const TUid KUidBrowserRecognizerImpl = { KBrowserRecognizerImplUIDValue };

#ifndef BRDO_WML_DISABLED_FF
const TInt KSupportedMimetypes(13);
#else 
const TInt KSupportedMimetypes(9);
#endif 

// Exported mimetypes
#ifndef BRDO_WML_DISABLED_FF
_LIT8( KWMLCMimeType,        "application/vnd.wap.wmlc"       );
_LIT8( KWMLMimeType,         "text/vnd.wap.wml"               );
_LIT8( KWMLSCMimeType,       "application/vnd.wap.wmlscriptc" );
_LIT8( KWBXMLMimeType,       "application/vnd.wap.wbxml1"     );
#endif 
_LIT8( KHTMLMimeType,        "text/html"                      );
_LIT8( KXHTMLMimeType1,      "application/vnd.wap.xhtml+xml"   );
_LIT8( KXHTMLMimeType2,      "application/xhtml+xml"          );
_LIT8( KCssMimeType,         "text/css"                       );
_LIT8( KJavaSCMimeType,      "application/x-javascript"       );
_LIT8( KJavaEcmaMimeType,    "text/ecmascript"                );
_LIT8( KMultiMixedMimeType,  "multipart/mixed"                );
_LIT8( KOPMLMimeType,        "text/x-opml"                    );

// File extensions
#ifndef BRDO_WML_DISABLED_FF
_LIT( KDotWMLC,             ".wmlc"      );
_LIT( KDotWMLSC,            ".wmlsc"     );
_LIT( KDotWBXML,            ".wbxml"     );
#endif 
_LIT( KDotHTML,             ".html"      );
_LIT( KDotHTM,              ".htm"       );
_LIT( KDotXHTML,            ".xhtml"     );
_LIT( KDotXHTML2,           ".xht"       );
_LIT( KDotCss,              ".css"       );
_LIT( KDotJavaScript,       ".js"        );
_LIT( KDotEcmaScript,       ".es"        );
_LIT( KDotOPML,             ".opml"      );

//
// CBrowserRecognizer
//
class CBrowserRecognizer : public CApaDataRecognizerType
  {
  public: // from CApaDataRecognizerType
    CBrowserRecognizer();

    static CApaDataRecognizerType* CreateRecognizerL();

    virtual TUint PreferredBufSize();
    virtual TDataType SupportedDataTypeL(TInt aIndex) const;

  private: // from CApaDataRecognizerType
    virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

    // New funtions
  private:

    TBool CheckForWMLC( const TDesC8& aBuffer );
    TBool CheckWbxmlVersion( TUint8 byte );
    TBool CheckWbxmlPubId( TUint8 byte );
    TBool CheckForHtml( const TDesC8& aBuffer );
    TBool CheckForXhtml( const TDesC8& aBuffer );
    TBool CheckForWML( const TDesC8& aBuffer );
    TBool CheckForOPML( const TDesC8& aBuffer );
  };

#endif // BROWSEREC_H

// END FILE
