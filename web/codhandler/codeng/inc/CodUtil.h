/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CodUtil.   
*      
*
*/


#ifndef COD_UTIL_H
#define COD_UTIL_H

// INCLUDE FILES

#include <e32base.h>
#include <AiwGenericParam.h>
#include <AiwGenericParam.hrh>

// FORWARD DECLARATION

class RFs;
class RFile;
class CBodyPart;
class CAiwGenericParamList;

// CLASS DECLARATION

/**
* Static utility functions. Wrapped in a struct to keep the global namespace
* clean.
*/
NONSHARABLE_STRUCT( CodUtil )
    {
    public:

        /**
        * Check if file can be opened (unprotected, or rights present).
        * @param aFname File name. Nonempty ASSERT-ed.
        * @return ETrue if file can be opened.
        */
        IMPORT_C static TBool CanOpenL( const TDesC& aFname );
        
        /**
        * Check if file can be opened (unprotected, or rights present).
        * @param aFile File.
        * @return ETrue if file can be opened.
        */
        IMPORT_C static TBool CanOpenL( RFile& aFile );
        
        /**
        * Get integer parameter (if exists).
        * @param aParam Parameter to set. Not modified if not set.
        * @param aSemanticId. Semantic id.
        * @param aParams Parameter list.
        */
        IMPORT_C static void GetIntParam
            (
            TInt& aParam,
            TGenericParamId aSemanticId,
            const CAiwGenericParamList& aParams
            );
        
        /**
        * Get TUint32  parameter (if exists).
        * @param aParam Parameter to set. Not modified if not set.
        * @param aSemanticId. Semantic id.
        * @param aParams Parameter list.
        */
        IMPORT_C static void GetUint32Param
            (
            TUint32& aParam,
            TGenericParamId aSemanticId,
            const CAiwGenericParamList& aParams
            );
        
        /**
        * Get bool parameter (if exists).
        * @param aParam Parameter to set. Not modified if not set.
        * @param aSemanticId. Semantic id.
        * @param aParams Parameter list.
        */
        IMPORT_C static void GetBoolParam
            (
            TBool& aParam,
            TGenericParamId aSemanticId,
            const CAiwGenericParamList& aParams
            );

        /**
        * Get descriptor parameter (if exists).
        * Both 8 and 16-bit variants are checked.
        * Leaves a temp buffer (or NULL) on the cleanup stack.
        * @param aParam Parameter to set. After returning:
        * - Set to point into aParams (8-bit param), or
        * - Set to point into the pushed temp buffer (16-bit param), or
        * - Remains unchaged (no param).
        * @param aSemanticId. Semantic id.
        * @param aParams Parameter list.
        */
        IMPORT_C static void GetDesParamLC
            (
            TPtrC8& aParam,
            TGenericParamId aSemanticId,
            const CAiwGenericParamList& aParams
            );

        /**
        * Get 16-bit descriptor parameter (if exists).
        * @param aParam Parameter to set. After returning:
        * - Set to point into aParams, or
        * - Remains unchaged (no param).
        * @param aSemanticId. Semantic id.
        * @param aParams Parameter list.
        */
        IMPORT_C static void GetDesParam
            (
            TPtrC& aParam,
            TGenericParamId aSemanticId,
            const CAiwGenericParamList& aParams
            );

        /**
        * Check if URL has 'cid' scheme.
        * @param aUrl URL.
        * @return ETrue if URL has 'cid' scheme.
        */
        static TBool IsCidSchemeL( const TDesC8& aUrl );

        /**
        * Lookup body part by CID, or NULL if not found. CID scheme asserted!
        * @param aParts Body parts.
        * @param aUrl URL.
        * @return The body part or NULL.
        */
        static CBodyPart* PartByCidL
            ( const RPointerArray<CBodyPart>& aParts, const TDesC8& aUrl );

        /**
        * Extract file name part of an URL.
        * @param aUrl URL.
        * @return File name part. This may be zero length.
        */
        static TPtrC8 FileNameFromUrl( const TDesC8& aUrl );

        /**
        * Allocate and return an absolute URL:
        * - If aUrl is already absolute, it is returned and aBase is ignored.
        * - If aUrl is a relative URL, resolve it against aBase. In
        *   this case, aBase need to be an absolute URL.
        * URL-s need to be valid URL-s or this function leaves.
        * @param aBase Base URL, used only if aUrl is a relative URL.
        * @param aUrl URL.
        * @return Absolute URL. Ownership passed to caller.
        */
        IMPORT_C static HBufC8* AbsoluteUrlLC
            ( const TDesC8& aBase, const TDesC8& aUrl );

        /**
        * Convert to UCS-2.
        * @param aSrc Source string.
        * @param aEncoding Encoding or empty string.
        * @param aFs File Server Session to be used.
        * @return Converted string. Owner is the caller.
        */
        static HBufC16* ConvertToUcs2L
            ( const TDesC8& aSrc, const TDesC8& aEncoding, RFs& aFs );

        /**
        * Convert UCS-2 to UTF-8.
        * @param aUcs2Src Source string.
        * @return Converted string. Owner is the caller.
        */
        static HBufC8* ConvertUcs2ToUtf8L( const TDesC& aUcs2Src );

        /**
        * Convert 16-bit descriptor to 8 bit.
        * @param aDes Descriptor to convert.
        * @return Converted descriptor. Owner is the caller. Pushed on stack.
        */

        IMPORT_C static HBufC8* ConvertLC( const TDesC& aDes );

        /**
        * Convert 8-bit descriptor to 16 bit.
        * @param aDes Descriptor to convert.
        * @return Converted descriptor. Owner is the caller. Pushed on stack.
        */
        IMPORT_C static HBufC* ConvertLC( const TDesC8& aDes );

        /**
        * Convert 16-bit descriptor to 8 bit.
        * @param aDes Descriptor to convert.
        * @return Converted descriptor. Owner is the caller. Pushed on stack.
        */

        IMPORT_C static HBufC8* ConvertL( const TDesC& aDes );

        /**
        * Convert 8-bit descriptor to 16 bit.
        * @param aDes Descriptor to convert.
        * @return Converted descriptor. Owner is the caller. Pushed on stack.
        */
        IMPORT_C static HBufC* ConvertL( const TDesC8& aDes );

        /**
        * Count Access Points.
        * @return Number of Access Points.
        */
        static TUint32 ApCountL();

        /**
        * Get WAP gateway address (or NULL) for an Access Point.
        * @param aIap Access Point id.
        * @return Gateway address for the Access Point, or NULL if not a
        * WAP AP. Owner is the caller.
        */
        static HBufC8* WapGatewayL( TUint32 aIap );

    };

#endif /* def COD_UTIL_H */
