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



#ifndef __DOWNLOADMGR_DEFATTRIB_H__
#define __DOWNLOADMGR_DEFATTRIB_H__

//  INCLUDES
#include <e32base.h>
#include <HttpDownloadMgrCommon.h>

// DATA TYPES
typedef TPckgBuf< TInt32 > THttpDownloadMgrIntegerBuf;

// CLASS DECLARATION
class CDefaultAttrib;

/**
*  Class for spack/unpack utils
*
*  @lib -
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( TDMgrUtils )
    {
    public:  // Constructor
    
        TDMgrUtils();
        
    public: // New functions

		/**
		* Get the next default/event attribute from the packed attribute list.
		* @param aAttrib unpacked attribute
        * @param aCount the next attribute to be unpack
        * @param aPackedAttribs packed attribute list
        * @return TBool.
		*/
        TBool NextAttribL( CDefaultAttrib* aAttrib,
                           const TDesC8& aPackedAttribs );

		/**
		* Allocate buffer for default attributes.
		* @param aAttrib unpacked attributes
        * @return HBufC8*.
		*/                          
        static HBufC8* AllocDefAttribBufLC( CArrayPtrFlat< CDefaultAttrib >* aAttribs );
        
		/**
		* Pack attributes.
		* @param aAttrib unpacked attributes
        * @return HBufC8*.
		*/         
        static HBufC8* PackedAttributesL( CArrayPtrFlat< CDefaultAttrib >* aAttribs );

		/**
		* Alloc and pack event attributes.
		* @param aUserData User data
		* @param aDownloadedSize Downloaded size
		* @param aAttrLength Attribute length
        * @return HBufC8*.
		*/
        static HBufC8* PackEventAttribL( TInt32 aUserData = 0,
                                         TInt32 aMODownloadedSize = 0,
                                         TInt32 aDownloadedSize = 0,
                                         TInt32 aMOAttrLength = 0,
                                         TInt32 aAttrLength = 0 );
        
		/**
		* Unpack event attributes.
		* @param aPack Package
		* @param aUserData User data
		* @param aDownloadedSize Downloaded size
		* @param aAttrLength Attribute length
        * @return None.
		*/        
        static void UnPackEventAttribL( const TDesC8& aPack,
                                        TInt32& aUserData,
                                        TInt32& aMODownloadedSize,
                                        TInt32& aDownloadedSize,
                                        TInt32& aMOAttrLength,
                                        TInt32& aAttrLength,
                                        TInt32& aMoIndex );
    
    public: // Members
    
        // utils used by the default/event attributes
        TPtrC8         iPtr;
        TInt           iCurrent;
    };

/**
*  Class for storing default attribute
*
*  @lib -
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CDefaultAttrib ) : public CBase
    {
    public:

        /**
        *  Default attribute type
        *
        *  @lib -
        *  @since Series 60 2.8
        */
        enum TAttribType
            {
            EInt,
            EBool,
            EString8,
            EString16
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructors.
        */
        static CDefaultAttrib* NewL();

        static CDefaultAttrib* NewL( THttpDownloadAttrib aAttribute,
                                     TInt32              aValue );

        static CDefaultAttrib* NewL( THttpDownloadAttrib aAttribute,
                                     TBool               aValue );

        static CDefaultAttrib* NewL( THttpDownloadAttrib aAttribute,
                                     const TDesC8&       aValue );

        static CDefaultAttrib* NewL( THttpDownloadAttrib aAttribute,
                                     const TDesC16&      aValue );

        /**
        * Destructor.
        */
        virtual ~CDefaultAttrib();

    protected:

        /**
        * C++ default constructor.
        */
        CDefaultAttrib( THttpDownloadAttrib aAttribute, TAttribType aType );
        CDefaultAttrib();

        /**
        * By default Symbian 2nd phase constructors are private.
        */
        void ConstructL();
        void ConstructL( TInt32 aValue );
        void ConstructL( TBool aValue );
        void ConstructL( const TDesC8& aValue );
        void ConstructL( const TDesC16& aValue );

    public:

        TInt32 AsTInt32L() const;
        TBool AsTBoolL() const;
        TPtrC8 AsTPtrC8L() const;
        TPtrC16 AsTPtrC16L() const;

        void SetL( const TDesC8& aType, const TDesC8& aAttribute, const TDesC8& aValue );
        void Reset();

        void PackAttribute( TDes8& aPackage );
        TInt PackedAttribSize();

    public:    // Data

        // attribute name
        THttpDownloadAttrib iAttribute;
        // attribute type
        TAttribType         iType;
        // Possible attribute values
	    TInt32   iInt32;  // Storage for integer based data
	    TBool    iBool;   // Storage for TBool
	    HBufC8*  iBufC8;  // Storage for 8 bits buffer based data.
	    HBufC16* iBufC16; // Storage for 16 bits buffer based data.

    };

#endif      // __DOWNLOADMGR_DEFATTRIB_H__   
            
// End of File
