/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        This utility class provides static methods for searching
*                StartTag and EndTag of certain element from XML Document.
*
*/








#ifndef SEN_SAX_UTILS_H
#define SEN_SAX_UTILS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KSenMaxLengthSaxParsingSoapMsg = 10000;

// CLASS DECLARATION

/**
* This utility class provides static methods for searching
* StartTag and EndTag from XML Document.
*  @lib SenUtils.lib
*  @since Series60 4.0
*/
class SenSaxUtils
    {
    public:
        /**
        * Search StartTag from XML Document.
        * Searching starts from the beginning of the given document.
        *
        * @since Series60 4.0
        * @param aDocument      XML Document from which StartTag is
        *                       searched.
        * @param aLocalName     LocalName of StartTag to be searched.
        * @param aStartTagStart Reference to TInt which will contain
        *                       value telling starting position of
        *                       searched StartTag.
        * @param aStartTagEnd   Reference to TInt which will contain
        *                       value telling ending position of
        *                       searched StartTag.
        * @param aPrefix        Reference to TPtrC8 for which pointer
        *                       of the Prefix of StartTag will be
        *                       stored.
        *                       Note: aPrefix points to given
        *                             aDocument descriptor.
        * @return KErrNone      if StartTag were found.
        *         KErrNotFound  if StartTag were not found.
        */
        IMPORT_C static TInt SearchStartTagL(const TDesC8& aDocument,
                                             const TDesC8& aLocalName,
                                             TInt& aStartTagStart,
                                             TInt& aStartTagEnd,
                                             TPtrC8& aPrefix);

        /**
        * Search StartTag from XML Document.
        * Searching starts from the beginning of the given document.
        *
        * @since Series60 4.0
        * @param aDocument      Document from which StartTag is searched
        * @param aPrefix        Prefix of StartTag to be searched.
        * @param aLocalName     LocalName of StartTag to be searched.
        * @param aStartTagStart Reference to TInt which will contain
        *                       value telling starting position of
        *                       searched StartTag.
        * @param aStartTagEnd   Reference to TInt which will contain
        *                       value telling ending position of
        *                       searched StartTag.
        * @return KErrNone      if StartTag were found.
        *         KErrNotFound  if StartTag were not found.
        */
        IMPORT_C static TInt SearchStartTagL(const TDesC8& aDocument,
                                             const TDesC8& aPrefix,
                                             const TDesC8& aLocalName,
                                             TInt& aStartTagStart,
                                             TInt& aStartTagEnd);

        /**
        * Search EndTag from XML Document.
        * Searching starts from the end of the given document.
        *
        * @since Series60 4.0
        * @param aDocument      XML Document from which EndTag is
        *                       searched.
        * @param aLocalName     LocalName of EndTag to be searched.
        * @param aEndTagStart   Reference to TInt which will contain
        *                       value telling ending position of
        *                       searched EndTag.
        * @param aEndTagEnd     Reference to TInt which will contain
        *                       value telling ending position of
        *                       searched EndTag.
        * @param aPrefix        Reference to TPtrC8 for which pointer
        *                       of the Prefix of EndTag will be
        *                       stored.
        *                       Note: aPrefix points to given
        *                             aDocument descriptor.
        * @return KErrNone      if EndTag were found.
        *         KErrNotFound  if EndTag were not found.
        */
        IMPORT_C static TInt SearchEndTagL(const TDesC8& aDocument,
                                           const TDesC8& aLocalName,
                                           TInt& aEndTagStart,
                                           TInt& aEndTagEnd,
                                           TPtrC8& aPrefix);

        /**
        * Search EndTag from XML Document.
        * Searching starts from the end of the given document.
        *
        * @since Series60 4.0
        * @param aDocument      Document from which EndTag is searched
        * @param aPrefix        Prefix of EndTag to be searched.
        * @param aLocalName     LocalName of EndTag to be searched.
        * @param aEndTagStart   Reference to TInt which will contain
        *                       value telling starting position of
        *                       searched EndTag.
        * @param aEndTagEnd     Reference to TInt which will contain
        *                       value telling ending position of
        *                       searched EndTag.
        * @return KErrNone      if EndTag were found.
        *         KErrNotFound  if EndTag were not found.
        */
        IMPORT_C static TInt SearchEndTagL(const TDesC8& aDocument,
                                           const TDesC8& aPrefix,
                                           const TDesC8& aLocalName,
                                           TInt& aEndTagStart,
                                           TInt& aEndTagEnd);


    private:
        static TInt SearchStartTagL(const TDesC8& aDocument,
                                    const TDesC8& aLocalName,
                                    TInt& aStartTagStart,
                                    TInt& aStartTagEnd,
                                    TPtrC8& aPrefix,
                                    TBool aSearchPrefix);
                                    
        static TInt SearchEndTagL(const TDesC8& aDocument,
                                  const TDesC8& aLocalName,
                                  TInt& aEndTagStart,
                                  TInt& aEndTagEnd,
                                  TPtrC8& aPrefix,
                                  TBool aSearchPrefix);
                                  
        static TInt SearchMarkStartingFromPlus(const TDesC8& aDocument,
                                               const TDesC8& aMark,
                                               TInt position,
                                               TInt size);
                                           
        static TInt SearchMarkStartingFromMinus(const TDesC8& aDocument,
                                                const TDesC8& aMark,
                                                TInt position);
                                           
        static TInt SearchMatchingGreaterThanStartingFrom(const TDesC8& aDocument,
                                                          TInt position,
                                                          TInt size);
                                           
        static TInt SearchEndOfCommentStartingFrom(const TDesC8& aDocument,
                                                   TInt position,
                                                   TInt size);
                                                   
        static TInt SearchStartOfCommentStartingFrom(const TDesC8& aDocument,
                                                     TInt position);

        /**
        * Hide default C++ constructor.
        */
        SenSaxUtils() { }

        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        SenSaxUtils( const SenSaxUtils& );

        /**
        * Prohibit assignment operator if not deriving from CBase.
        */
        SenSaxUtils& operator=( const SenSaxUtils& );
    };

#endif // SEN_SAX_UTILS_H

// End of File
