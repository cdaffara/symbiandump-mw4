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
*      Declaration of class TCodParser.   
*      
*
*/


#ifndef COD_PARSER_H
#define COD_PARSER_H

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATION
class CMediaObjectData;
class CCodData;

// CLASS DECLARATION


/**
* COD Parser.
*/
NONSHARABLE_CLASS( TCodParser )
    {

    public:     // constructor

        /**
        * Constructor.
        */
        inline TCodParser();

    public:     // new methods

        /**
        * Parse the buffer into aData. Existing data in aData is lost.
        * Leaves on failure; leave code can be KErrCodInvalidDescriptor as well
        * as general errors. The resulting data is not checked semantically
        * (i.e. mandatory attributes may be missing).
        * @param aBuf COD content buffer to be parsed.
        * @param aData COD will be parsed into this.
        */
        void ParseL( const TDesC& aBuf, CCodData& aData );

    private:    // types

        enum TCodAttr               ///< COD Attributes.
            {
            ECodName,               ///< COD-Name.
            ECodVendor,             ///< COD-Vendor.
            ECodDescription,        ///< COD-Description.
            ECodUrl,                ///< COD-URL.
            ECodSize,               ///< COD-Size.
            ECodType,               ///< COD-Type.
            ECodInstallNotify,      ///< COD-Install-Notify.
            ECodNextUrl,            ///< COD-Netx-URL.
            ECodNextUrlAtError,     ///< COD-Next-URLatError.
            ECodInfoUrl,            ///< COD-Info-URL.
            ECodPrice,              ///< COD-Price.
            ECodIcon,               ///< COD-Icon.
            ECodUnknownAttr         ///< Future COD attributes.
            };

    private:    // parsing

        /**
        * Parse one line.
        * @return ETrue if more lines to go, EFalse if done.
        */
        TBool AttrLineL(CMediaObjectData *& aMediaObject);

        /**
        * Parse an attribute name. Empty token sets error.
        * @return Attribute name type.
        */
        TCodAttr AttrName();

        /**
        * Parse an attribute value.
        * @return pointer to the value.
        */
        TPtrC AttrValue();

        /**
        * Skip (optional) white space.
        */
        void SkipWhiteSpace();

        /**
        * Skip past the end of current line (ignoring anything).
        */
        void NextLine();

        /**
        * Parse *WS until ?CR LF. Set error if something else found.
        * (Advance to LF, leaving the LF as lookahead char).
        */
        void EndOfLine();

        /**
        * Parse *WS : *WS. Set error if no colon found.
        * @return ETrue if colon found.
        */
        TBool Colon();

        /**
        * Check if current character is value char.
        * @return ETrue if current character is value char.
        */
        TBool IsValueChar() const;

        /**
        * Check if current character is control char.
        * @return ETrue if current character is control char.
        */
        TBool IsControl() const;

        /**
        * Check if current character is white space.
        * @return ETrue if current character is white space.
        */
        TBool IsWhiteSpace() const;

        /**
        * Check if current character is separator.
        * @return ETrue if current character is separator.
        */
        TBool IsSeparator() const;

        /**
        * Check if current character is CR LF or LF.
        * @return ETrue if current character is CR LF or LF.
        */
        TBool IsEndOfLine() const;

        /**
        * Set error code if not already set.
        * @param aError Error.
        */
        inline void Error( TInt aError );

    private:    // data

        const TDesC* iBuf;      ///< Data buffer. Not owned.
        CCodData* iData;        ///< COD Data. Not owned.
        const TText* iCurP;     ///< Current character position.
        const TText* iEndP;     ///< End pointer (past the buffer);
        TInt iError;            ///< Error code.

    };

#include "CodParser.inl"

#endif /* def COD_PARSER_H */
