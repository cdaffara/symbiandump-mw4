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
* Description:  Widget installer info file parsing.
*
*/


#ifndef WIDGETUIINFOPLISTCONFIGHANDLER_H
#define WIDGETUIINFOPLISTCONFIGHANDLER_H

// INCLUDES
#include <libxml2_tree.h>
#include <WidgetRegistryConstants.h>
#include <WidgetPropertyValue.h>

// FORWARD DECLARATION


namespace SwiUI
    {

    // CLASS DECLARATION

    /**
     *  Parse the widget info file for widget entry creation.
     *  @lib WidgetInstallerUI.lib
     *  @since 3.1
     */
    class CWidgetUIConfigHandler: public CBase
        {
    public: // constuctors and destructor

        /**
         * Two-phased constructor.
         */
        static CWidgetUIConfigHandler* NewL();

        /**
         * Destructor.
         */
        virtual ~CWidgetUIConfigHandler();

    public:

        /**
         * Parse the widget info file and create CWidgetInfo and CWidgetData
         * check for required keys and values
         *
         * @param aBuffer The buffer contains widget info file content.
         * @param aPropertyValues output filled with parsed values from buf
         * @since 3.1
         * @return none.
         */
        void ParseValidateBundleMetadataL(
            TPtr8 aBuffer,
            RPointerArray<CWidgetPropertyValue>& aPropertyValues,
            RFs& aFileSession );

        /**
         * Parse the localized buffer from infoPlist.strings in format of
         * CFBundleDisplayName = "localized display name"
         * and update the widgetInfo's displayname
         *
         * @param aBuffer: the buffer holding data from infoPlist.strings file
         * @param aWidgetInfo: the widget info data which contains the widget display name
         * @since 3.1
         */
        void ParseInfoLocL( TPtrC8 aBuffer,
                            RFs& aFileSession,
                            CWidgetPropertyValue& aBundleDisplayName );


    protected:

        /**
         * C++ default constructor.
         */
        CWidgetUIConfigHandler();

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

        /**
         * Traverse to the next Node
         *
         * @param aNode: current node
         * @since 3.1
         * @return next node
         */
        xmlNode* TraverseNextNode( xmlNode* aNode );

        /**
         * Convert a string in some encoding to UCS2.
         *
         * @param aEncoding encoding number
         * @param aUnicodeSizeMultiplier how many input bytes per unicode char
         * @param aInBuf input data
         * @param aOutBuf output data, allocated and returned to caller
         * @param aFileSession
         * @since 3.1
         * @return void
         */
        void ToUnicodeL( TInt aEncoding,
                         TInt aUnicodeSizeMultiplier,
                         TPtrC8 aInBuf, HBufC16** aOutBuf,
                         RFs& aFileSession );

        /**
        * Utility to bundle extraction of XML text content
        *
        * @param aEncoding input buffer encoding
        * @param aUnicodeSizeMultiplier how many bytes of input make one unicode char
        * @param aInBuf input data in encoding
        * @param aOutBuf malloc'ed output buf, caller takes ownership
        * @param aFileSession CCnvCharacterSetConverter requires it
        * @since 3.1
        * @return void
        */
        void GetContentL( RFs& aFileSession,
                          xmlDocPtr aDoc,
                          xmlNode* aNode,
                          HBufC** aContent );


        /**
         * Validate property values.
         *
         * @param aPropertyValues
         * @since 3.1
         * @return void
         */
        void ValidateL( RPointerArray<CWidgetPropertyValue>& aPropertyValues );

    private:

        enum DtdType
            {
            EDtdTypeUnknown,
            EDtdTypeApple,
            EDtdTypeNokia
            };

        // these index iProperties description table, not property
        // VALUES which are indexed by a TWidgetPropertyId
        enum TWidgetPropertyDescriptionId
            {
            EPropertyDescriptionAppleBundleIdentifier,     // required; string; example: com.apple.hello
            EPropertyDescriptionAppleBundleDisplayName,    // required; string; example: Hello World
            EPropertyDescriptionAppleBundleVersion,        // optional; string; example: 1.0
            EPropertyDescriptionAppleAllowFullAccess,      // optional; bool but int 0/1 internally
            // AppleAllowFullAccess is mapped to AllowNetworkAccess if that is missing, otherwise ignored
            EPropertyDescriptionNokiaIdentifier,           // required; string; example: com.nokia.hello
            EPropertyDescriptionNokiaDisplayName,          // required; string; example: Hello World
            EPropertyDescriptionNokiaVersion,              // optional; string; example: Hello World
            EPropertyDescriptionMainHTML,                  // required; string; example: HellowWorld.html
            EPropertyDescriptionAllowNetworkAccess,        // optional; bool but int 0/1 internally
            EPropertyDescriptionHeight,                    // optional; integer; example: 100
            EPropertyDescriptionWidth,                     // optional; integer; example: 100
            EPropertyDescriptionNokiaMiniViewEnable,       //optional; bool but int 0/1 internally
            EPropertyDescriptionNokiaPromptNeeded,       //optional; bool but int 0/1 internally
            EPropertyDescriptionNokiaPreInstalled,         // optional; bool but int 0/1 internally
            // end property description list, begin special values
            EPropertyDescriptionIdCount,     // must be at end of properties
            EPropertyDescriptionIdInvalid    // must be after EPropertyDescriptionIdCount
            };

        TWidgetPropertyId GetPropertyId(
            const TDesC& aKeyName,
            DtdType aDtdType,
            TWidgetPropertyDescriptionId& aSpecialPropertyDescriptionId );

        class TWidgetProperty
            {
        public:
            TWidgetPropertyId id; // the registry value id for this name
            TPtrC name;
            TWidgetPropertyType type;
            TInt flags;
            };

        // Table describing properties from the bundle metadata.  Used by parser.
        TFixedArray<TWidgetProperty, EPropertyDescriptionIdCount> iProperties;

        DtdType iDtdType; // used by lproj processing after manifest parse
        };
    }
#endif

// End of File
