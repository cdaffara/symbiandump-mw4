/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles VPN command file parsing.
*
*/




#ifndef C_VPNCOMMANDFILEPARSER_H
#define C_VPNCOMMANDFILEPARSER_H

#include <e32base.h>

/**
 * Parser utility class for VPN command file (.vpn).
 * Public interface offers functions for parsing
 * and querying the parsed values. 
 * Internal implementation parses XML tree and extracts
 * found parameters to local container.
 *
 */
class CCmdFileParser : public CBase 
    {
// C'tors
public:
    /**
     * No need for two-phased construction
     */
    CCmdFileParser();
    ~CCmdFileParser();

// Public functions
public:
    /**
     * Parse an XML file and store parsed variables to local
     * structures.
     *
     * @param aInputData XML file in 8bit desc buffer.
     *
     * @leave Leaves with KErrCorrupt, if the data is not in 
     *        expected format.
     */
    void    ParseL(const TDesC8& aInputData);

    /**
     * Returns the a parsed string value. Control of the instantiated
     * object is transferred to the caller. If no value is found,
     * returns a NULL object.
     *
     * @param aUri The "full uri" of the requested value
     *
     * @ret HBufC* A pointer to a newly allocated 16bit descriptor containing
     *      the parsed value, or NULL if none found.
     *
     * @leave Leaves in case 
     */
    HBufC*  GetParsedValueStrL(const TDesC8& aUri);
    
    /**
     * Translates a parsed value to TInt and returns it.
     * Leaves if the operation cannot be accomplished.
     *
     * @param aUri The "full uri" of the requested value
     * @param aVal Used for TInt return value (if any)
     *
     * @leave Leaves if the operation fails.
     */
    void    GetParsedValueIntL(const TDesC8& aUri, TInt& aVal);

    /**
     * Translates a parsed value to TBool and returns it.
     * Leaves if the operation cannot be accomplished.
     *
     * @param aUri The "full uri" of the requested value
     * @param aVal Used for TBool return value (if any)
     *
     * @leave Leaves if the operation fails.
     */
    void    GetParsedValueBoolL(const TDesC8& aUri, TBool& aVal);

private:
    void    ReleaseResources();
    
    /**
     * Parse the input XML
     *
     */
    void    ParseXmlL(const TDesC8& aInputData);

    /**
     * Checks whether the encountered "closing" XML node is indeed 
     * the most recently opened.
     *
     * @param aName The name to check
     *
     * @return ETrue, iff the given name was found on the top 
     *         of the iParserStack
     *
     */
    TBool       IsTopmostNode(const TDesC8& aName) const;

    /**
     * Pushes an "opening" XML node on the parser stack. 
     * Instantiates a new stack object based on the given name.
     *
     * @param aName The name of the object to push
     *
     * @leave Leaves if OOM.
     */
    void        PushNodeL(const TDesC8& aName);

    /**
     * Pops the topmost node from the parser stack. 
     * No questions asked.
     *
     */
    void        PopNode();

    /**
     * Constructs a new string, containing the full "URI"
     * for the given text. It is assumed that the text is
     * located inside the node that is currently open (i.e.
     * on the top of the parser stack).
     * URI is of form "root/branch1/branch6/aText".
     *
     * @aText The text in the leafnode -- the last segment 
     *        of the uri
     *
     * @return Returns a new string containing the full uri
     *         of the leaf (text) node.
     *
     * @leave Leaves if OOM
     *
     */
    HBufC8*     ParseTreeUriL(const TDesC8& aText) const;
    
    /**
     * Checks whether the text segment in XML is valid. 
     * If it begins with CR/LF, it's not.
     *
     * @param aText The text to verify.
     *
     * @return ETrue only if the text is valid in that
     *         it's not a string beginning with CR/LF
     *
     */
    TBool       IsValidText(const TDesC8& aText) const;

    /**
     * Iterates through parsed values array and checks whether
     * the given URI is found. If it is, returns the leaf
     * value of the uri.
     * URI here must end with slash ("/"), so it must not contain
     * the related leaf node itself (representing the parameter's 
     * textual value).
     * Example: FindUri("root/node1/node2/") will return "666"
     * if there exists a URI "root/node1/node2/666" in the parsed
     * values array.
     *
     * @return A pointer to the found value or KNullDesC8 if none
     *         was found. 
     * 
     */
    TPtrC8      FindUri(const TDesC8& aUri) const;


// Private variables
private:
    HBufC*                          iFileName;
    HBufC*                          iFileContents;
    HBufC*                          iPKCS12Pwd;
    TInt                            iDevLockPolicy;
    TBool                           iDevLockRequired;
    TBool                           iParseReady;

    // Parser stack consisting of parse tree nodes
    CArrayFixFlat<HBufC8*>*         iParserStack;

    // Parsed stack, consisting of leaf values and their full
    // tree paths (root/branch1/branch6/leaf_value)
    CArrayFixFlat<HBufC8*>*         iParsedValuesArray;
    };

#endif // C_VPNPOLICYINSTALLER_H

