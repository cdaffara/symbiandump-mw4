/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Resolves entities.
*
*/



#include <escapeutils.h>
#include <libxml2_xmlmemory.h>
#include <libxml2_globals.h>

#include "CleanupLibXml2.h"
#include "LeakTracker.h"
#include "XmlEntity.h"


// Private consts.
// -------------------------------------------------------------------------
// Note:		This array must be sorted by entity name.  The lookup function
//				does a binary search. */

// IMPORTANT:	When you update this table make sure the constant 
//				NW_XHTML_Num_CaseInsensitive_Entries which is the count of 
//				caseinsensitive entries is updated correctly.

// Note:        See http://kellyjones.netfirms.com/webtools/ascii_utf8_table.html
//              for utf8 entity mappings.


#define KNumCaseInsensitiveEntries 126

static const CXmlEntity::EntityEntry  sSpaceEntity = { " ",      32 };

static const CXmlEntity::EntityEntry sEntityMappings[] = 
    {
// CaseSensitive entries
    { "AElig",      198 },
    { "Aacute",     193 },
    { "Acirc",      194 },
    { "Agrave",     192 },
    { "Alpha",      913 },
    { "Aring",      197 },
    { "Atilde",     195 },
    { "Auml",       196 },
    { "Beta",       914 },
    { "Ccedil",     199 },
    { "Chi",        935 },
    { "Dagger",     8225 },
    { "Delta",      916 },
    { "ETH",        208 },
    { "Eacute",     201 },
    { "Ecirc",      202 },
    { "Egrave",     200 },
    { "Epsilon",    917 },
    { "Eta",        919 },
    { "Euml",       203 },
    { "Gamma",      915 },
    { "Iacute",     205 },
    { "Icirc",      206 },
    { "Igrave",     204 },
    { "Iota",       921 },
    { "Iuml",       207 },
    { "Kappa",      922 },
    { "Lambda",     923 },
    { "Mu",         924 },
    { "Ntilde",     209 },
    { "Nu",         925 },
    { "OElig",      338 },
    { "Oacute",     211 },
    { "Ocirc",      212 },
    { "Ograve",     210 },
    { "Omega",      937 },
    { "Omicron",    927 },
    { "Oslash",     216 },
    { "Otilde",     213 },
    { "Ouml",       214 },
    { "Phi",        934 },
    { "Pi",         928 },
    { "Prime",      8243 },
    { "Psi",        936 },
    { "Rho",        929 },
    { "Scaron",     352 },
    { "Sigma",      931 },
    { "THORN",      222 },
    { "Tau",        932 },
    { "Theta",      920 },
    { "Uacute",     218 },
    { "Ucirc",      219 },
    { "Ugrave",     217 },
    { "Upsilon",    933 },
    { "Uuml",       220 },
    { "Xi",         926 },
    { "Yacute",     221 },
    { "Yuml",       376 },
    { "Zeta",       918 },
    { "aacute",     225 },
    { "acirc",      226 },
    { "acute",      180 },
    { "aelig",      230 },
    { "agrave",     224 },
    { "alpha",      945 },
    { "atilde",     227 },
    { "auml",       228 },
    { "beta",       946 },
    { "ccedil",     231 },
    { "chi",        967 },
    { "dArr",       8659 },
    { "dagger",     8224 },
    { "darr",       8595 },
    { "delta",      948 },
    { "eacute",     233 },
    { "ecirc",      234 },
    { "egrave",     232 },
    { "epsilon",    949 },
    { "eta",        951 },
    { "euml",       235 },
    { "gamma",      947 },
    { "hArr",       8660 },
    { "harr",       8596 },
    { "iacute",     237 },
    { "icirc",      238 },
    { "igrave",     236 },
    { "iota",       953 },
    { "iuml",       239 },
    { "kappa",      954 },
    { "lArr",       8656 },
    { "lambda",     955 },
    { "larr",       8592 },
    { "mu",         956 },
    { "ntilde",     241 },
    { "nu",         957 },
    { "oacute",     243 },
    { "ocirc",      244 },
    { "oelig",      339 },
    { "ograve",     242 },
    { "omega",      969 },
    { "omicron",    959 },
    { "oslash",     248 },
    { "otilde",     245 },
    { "otimes",     8855 },
    { "ouml",       246 },
    { "phi",        966 },
    { "pi",         960 },
    { "psi",        968 },
    { "rArr",       8658 },
    { "rarr",       8594 },
    { "rho",        961 },
    { "scaron",     353 },
    { "sigma",      963 },
    { "tau",        964 },
    { "theta",      952 },
    { "thorn",      254 },
    { "uArr",       8657 },
    { "uacute",     250 },
    { "uarr",       8593 },
    { "ucirc",      251 },
    { "ugrave",     249 },
    { "upsilon",    965 },
    { "uuml",       252 },
    { "xi",         958 },
    { "yacute",     253 },
    { "yuml",       255 },
    { "zeta",       950 },
    { "zwj",        8205 },
    { "zwnj",       8204 },
// Case Insensitive entries
    { "alefsym",    8501 },
    { "amp",        38 },
    { "and",        8743 },
    { "ang",        8736 },
    { "apos",       39 },
    { "aring",      229 },
    { "asymp",      8776 },
    { "bdquo",      8222 },
    { "brvbar",     166 },
    { "bull",       8226 },
    { "cap",        8745 },
    { "cedil",      184 },
    { "cent",       162 },
    { "circ",       710 },
    { "clubs",      9827 },
    { "cong",       8773 },
    { "copy",       169 },
    { "crarr",      8629 },
    { "cup",        8746 },
    { "curren",     164 },
    { "deg",        176 },
    { "diams",      9830 },
    { "divide",     247 },
    { "empty",      8709 },
    { "emsp",       8195 },
    { "ensp",       8194 },
    { "equiv",      8801 },
    { "eth",        240 },
    { "euro",       8364 },
    { "exist",      8707 },
    { "fnof",       402 },
    { "forall",     8704 },
    { "frac12",     189 },
    { "frac14",     188 },
    { "frac34",     190 },
    { "frasl",      8260 },
    { "ge",         8805 },
    { "gt",         62 },
    { "hearts",     9829 },
    { "hellip",     8230 },
    { "iexcl",      161 },
    { "image",      8465 },
    { "infin",      8734 },
    { "int",        8747 },
    { "iquest",     191 },
    { "isin",       8712 },
    { "lang",       9001 },
    { "laquo",      171 },
    { "lceil",      8968 },
    { "ldquo",      8220 },
    { "le",         8804 },
    { "lfloor",     8970 },
    { "lowast",     8727 },
    { "loz",        9674 },
    { "lrm",        8206 },
    { "lsaquo",     8249 },
    { "lsquo",      8216 },
    { "lt",         60 },
    { "macr",       175 },
    { "mdash",      8212 },
    { "micro",      181 },
    { "middot",     183 },
    { "minus",      8722 },
    { "nabla",      8711 },
    { "nbsp",       160 },
    { "ndash",      8211 },
    { "ne",         8800 },
    { "ni",         8715 },
    { "not",        172 },
    { "notin",      8713 },
    { "nsub",       8836 },
    { "oline",      8254 },
    { "oplus",      8853 },
    { "or",         8744 },
    { "ordf",       170 },
    { "ordm",       186 },
    { "para",       182 },
    { "part",       8706 },
    { "permil",     8240 },
    { "perp",       8869 },
    { "piv",        982 },
    { "plusmn",     177 },
    { "pound",      163 },
    { "prime",      8242 },
    { "prod",       8719 },
    { "prop",       8733 },
    { "quot",       34 },
    { "radic",      8730 },
    { "rang",       9002 },
    { "raquo",      187 },
    { "rceil",      8969 },
    { "rdquo",      8221 },
    { "real",       8476 },
    { "reg",        174 },
    { "rfloor",     8971 },
    { "rlm",        8207 },
    { "rsaquo",     8250 },
    { "rsquo",      8217 },
    { "sbquo",      8218 },
    { "sdot",       8901 },
    { "sect",       167 },
    { "shy",        173 },
    { "sigmaf",     962 },
    { "sim",        8764 },
    { "spades",     9824 },
    { "sub",        8834 },
    { "sube",       8838 },
    { "sum",        8721 },
    { "sup",        8835 },
    { "sup1",       185 },
    { "sup2",       178 },
    { "sup3",       179 },
    { "supe",       8839 },
    { "szlig",      223 },
    { "there4",     8756 },
    { "thetasym",   977 },
    { "thinsp",     8201 },
    { "tilde",      732 },
    { "times",      215 },
    { "trade",      8482 },
    { "uml",        168 },
    { "upsih",      978 },
    { "weierp",     8472 },
    { "yen",        165 },
    { "zwj",        8205 },
    { "zwnj",       8204 },
    };


// -----------------------------------------------------------------------------
// CXmlEntity::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXmlEntity* CXmlEntity::NewL()
    {
    CXmlEntity* self = new (ELeave) CXmlEntity();
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CXmlEntity::CXmlEntity
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXmlEntity::CXmlEntity():
        iLeakTracker(CLeakTracker::EXmlEntity), iEntityMappings(15)
    {
    }
        

// -----------------------------------------------------------------------------
// CXmlEntity::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXmlEntity::ConstructL()
    {
    }
        

// -----------------------------------------------------------------------------
// CXmlEntity::~CXmlEntity
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CXmlEntity::~CXmlEntity()
    {
    // Delete the cached encoding-map.
    for (TInt i = 0; i < iEntityMappings.Count(); i++)
        {
        xmlFree(iEntityMappings[i].orig);
        }

    iEntityMappings.Close();
    }


// -----------------------------------------------------------------------------
// CXmlEntity::ResolveL
//
// Resolves the named entity into its char-value.  Can handle numeric entities.
// -----------------------------------------------------------------------------
//
void CXmlEntity::ResolveL(const TDesC& aName, TUint16& aUcs2Value)
    {
    HBufC8*   utf8 = NULL;
    xmlChar*  xmlStr = NULL;

    if (aName.Length() < 1)
        {
        User::Leave(KErrCorrupt);
        }

    // Resolve numeric entities...
    if (ResolveNumericL(aName, aUcs2Value))
        {
        return;
        }

    // Convert the name to utf8 -- its strdup'ed to zero-terminate it.
    utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aName);
    CleanupStack::PushL(utf8);
    
    xmlStr = xmlStrndup(utf8->Ptr(), utf8->Size());
    User::LeaveIfNull(xmlStr);
    CleanupLibXml2::PushL(xmlStr);
    
    // Look up the entity in the static entity table.
    const CXmlEntity::EntityEntry& entity = LookupUsc2Value(xmlStr);
    aUcs2Value = entity.ucs2Value;

    // Clean up.
    CleanupStack::PopAndDestroy(/*xmlStr*/);
    CleanupStack::PopAndDestroy(utf8);
    }


// -----------------------------------------------------------------------------
// CXmlEntity::ResolveL
//
// Resolves the named entity into its EntityEntry.  Can NOT handle numeric entities.
// -----------------------------------------------------------------------------
//
const xmlEntity* CXmlEntity::ResolveL(const xmlChar *aName)
    {
    TInt       index;
    xmlEntity  entity;
    
    // Init the entity.
    memset(&entity, 0x00, sizeof(xmlEntity));

    // Check if the entity has already been resolved.
    entity.name = aName;
    index = iEntityMappings.FindInOrder(entity, LinearOrder);
    
    if (index != KErrNotFound)
        {
        return &iEntityMappings[index];
        }

    // Also check if the entity has already been resolved in a case insensitive way.
    index = iEntityMappings.FindInOrder(entity, LinearCaseOrder);
    
    if (index != KErrNotFound)
        {
        return &iEntityMappings[index];
        }

    // Otherwise look it up in the static table, create a new entry and return it.
    TBuf<2>   ucs2;
    HBufC8*   utf8 = NULL;
    xmlChar*  utf8Value = NULL;
    
    // Get the entity as a ucs2 value from the static table.        
    const EntityEntry& entityEntry = LookupUsc2Value(aName);
            
    // Convert the value to utf8.
    ucs2.Append(entityEntry.ucs2Value);
    
    utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(ucs2);
    CleanupStack::PushL(utf8);
    
    utf8Value = xmlStrndup(utf8->Ptr(), utf8->Size());
    User::LeaveIfNull(utf8Value);
    CleanupLibXml2::PushL(utf8Value);
    
    // Populate the entity.
    entity.type = XML_ENTITY_DECL;
    entity.name = BAD_CAST(entityEntry.entityName);
    entity.orig = const_cast<xmlChar*>(utf8Value);
    entity.content = const_cast<xmlChar*>(utf8Value);
    entity.length = xmlStrlen(utf8Value);
    entity.etype = XML_INTERNAL_PREDEFINED_ENTITY;

    // Add the new entry.
    User::LeaveIfError(iEntityMappings.InsertInOrder(entity, LinearOrder));
    CleanupStack::Pop(/*utf8Value*/);
    CleanupStack::PopAndDestroy(utf8);
    
    // Return the newly added entry.
    index = iEntityMappings.FindInOrder(entity, LinearOrder);

    if (index == KErrNotFound)
        {
        User::Leave(KErrCorrupt);
        }

    return &iEntityMappings[index];
    }


// -----------------------------------------------------------------------------
// CXmlEntity::LookupUsc2Value
//
// Looks up the named entity in the static table.  Can NOT handle numeric entities.
// -----------------------------------------------------------------------------
//
const CXmlEntity::EntityEntry& CXmlEntity::LookupUsc2Value(const xmlChar *aName)
    {
    TInt                index;
    const EntityEntry*  entry;
    TInt                low;
    TInt                high;
    TInt                res;
    TInt                numEntries;

    numEntries = (sizeof(sEntityMappings) / sizeof(EntityEntry));

    // First do a binary search search in the case sensitive part of the array.
    low = 0;
    high = numEntries - KNumCaseInsensitiveEntries - 1;
    res = 0;

    while (low <= high ) 
        {
        index = (high + low) / 2;
        entry = &(sEntityMappings[index]);

        // Do a case sensitive string comparison.
        res = xmlStrcmp(aName, BAD_CAST(entry->entityName));

        if (res > 0) 
            {
            /* name is ahead of this slot.  Increase low bound. */
            low = index + 1;
            } 
            
        else if (res < 0) 
            {
            /* name is behind this slot.  Decrease high bound. */
            high = index - 1;
            } 
        else 
            {
            /* Found the entity name.  Return its value. */
            return *entry;
            }
        }

    // If no match was found search in the case insensitive part of the table.
    low = numEntries - KNumCaseInsensitiveEntries;
    high = numEntries - 1;
    res = 0;

    while (low <= high ) 
        {
        index = (high + low) / 2;
        entry = &(sEntityMappings[index]);

        // Do a case insensitive string comparison.
        res = xmlStrcasecmp(aName, BAD_CAST(entry->entityName));

        if (res > 0) 
            {
            /* name is ahead of this slot.  Increase low bound. */
            low = index + 1;
            } 
            
        else if (res < 0) 
            {
            /* name is behind this slot.  Decrease high bound. */
            high = index - 1;
            } 
        else 
            {
            /* Found the entity name.  Return its value. */
            return *entry;
            }
        }

    // If no match were found return the space.
    return sSpaceEntity;
    }


// -----------------------------------------------------------------------------
// CXmlEntity::ResolveNumeric
//
// Resolves the numeric entity into it's value.
// -----------------------------------------------------------------------------
//
TBool CXmlEntity::ResolveNumericL(const TDesC& aName, TUint16& aUcs2Value)
    {
    _LIT(KHash, "#");
    _LIT(KHex, "x");
    _LIT(KHEX, "X");

    TBool   found = EFalse;

    if (aName.Length() < 2)
        {
        return EFalse;
        }

	if (aName.Left(1) == KHash)
		{
        TRadix   aRadix = EDecimal;
        TPtrC    numeric;

        // Entity of the form, #x123
		if ((aName.Mid(1, 1) == KHex) || (aName.Mid(1, 1) == KHEX))
			{
			numeric.Set(aName.Mid(2, aName.Length() - 2));
            aRadix = EHex;
			}

        // Entity of the form, #123
		else
			{
			numeric.Set(aName.Mid(1, aName.Length() - 1));
			}

        // Convert the text into a ucs2 value.
        if (numeric.Length() > 0)
            {
	        TLex  temp(numeric);

	        temp.Val(aUcs2Value, aRadix);
		    found = ETrue;
            }
		}

    return found;
    }


// -----------------------------------------------------------------------------
// CXmlEntity::LinearOrder
//
// Comparison method for iEntityMappings.
// -----------------------------------------------------------------------------
//
TInt CXmlEntity::LinearOrder(const xmlEntity& aFirst, const xmlEntity& aSecond)
    {
    return xmlStrcmp(aFirst.name, aSecond.name);
    }


// -----------------------------------------------------------------------------
// CXmlEntity::LinearCaseOrder
//
// Comparison method for iEntityMappings.
// -----------------------------------------------------------------------------
//
TInt CXmlEntity::LinearCaseOrder(const xmlEntity& aFirst, const xmlEntity& aSecond)
    {
    return xmlStrcasecmp(aFirst.name, aSecond.name);
    }

