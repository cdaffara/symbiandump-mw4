/* 
 * Summary: string dictionnary
 * Description: dictionary of reusable strings, just used to avoid allocation
 *         and freeing operations.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_DICT_H__
#define __XML_DICT_H__

//#include "Libxml2_xmlversion.h"
#include "Libxml2_xmlstring.h"
//#include "Libxml2_tree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xmlDictStrings xmlDictStrings;
typedef xmlDictStrings* xmlDictStringsPtr;

struct _xmlDictStrings {
    xmlDictStringsPtr next;
    xmlChar* free;
    xmlChar* end;
    int size;
    int nbStrings;
    xmlChar array[1];
};

/*
 * An entry in the dictionnary
 */
typedef struct _xmlDictEntry xmlDictEntry;
typedef xmlDictEntry* xmlDictEntryPtr;
struct _xmlDictEntry {
    struct _xmlDictEntry* next;
    const xmlChar* name;
    int len;
    int valid;
};
/*
 * The dictionnary.
 */
typedef struct _xmlDict xmlDict;
typedef xmlDict* xmlDictPtr;

/*
 * The entire dictionnary
 */
struct _xmlDict {
    int ref_counter;

    struct _xmlDictEntry *dict;
    int size;
    int nbElems;
    xmlDictStringsPtr strings;

    struct _xmlDict *subdict;
};

/*
 * Constructor and destructor.
 */
XMLPUBFUN xmlDictPtr XMLCALL
			xmlDictCreate	(void);
			
XMLPUBFUN xmlDictPtr XMLCALL xmlDictCreateSub(xmlDictPtr sub);

XMLPUBFUN int XMLCALL
			xmlDictReference(xmlDictPtr dict);
XMLPUBFUN void XMLCALL			
			xmlDictFree	(xmlDictPtr dict);

/*
 * Lookup of entry in the dictionnary.
 */
XMLPUBFUN const xmlChar* XMLCALL		
			xmlDictLookup	(xmlDictPtr dict,
		                         const xmlChar *name,
		                         int len);
		                         
//needed by libxslt         
XMLPUBFUN const xmlChar * XMLCALL		
			xmlDictQLookup	(xmlDictPtr dict,
		                         const xmlChar *prefix,
		                         const xmlChar *name);
		                                              		                      
#ifndef XMLENGINE_EXCLUDE_UNUSED		                         
XMLPUBFUN int XMLCALL			
			xmlDictSize	(xmlDictPtr dict);
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */
		               
XMLPUBFUN int XMLCALL
			xmlDictOwns	(xmlDictPtr dict,
					 const xmlChar *str);
#ifdef __cplusplus
}
#endif
#endif /* ! __XML_DICT_H__ */

