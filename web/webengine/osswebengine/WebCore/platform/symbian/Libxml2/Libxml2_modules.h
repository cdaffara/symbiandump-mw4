// All new configuration directives are in:
#include "XmlEngconfig.h"

#ifdef XMLENGINE_EXCLUDE_EMBED_MSG
extern const char * const __embedded_errtxt_replacement;
#define  EMBED_ERRTXT(str) NULL
//#define  EMBED_ERRTXT(str) __embedded_errtxt_replacement
#else
#define  EMBED_ERRTXT(str) str
#endif

#define EXTRA_OOM_DETECTION_CODE
#ifdef  EXTRA_OOM_DETECTION_CODE
// TODO: Later we may use global flag for OOM error and check it in these macroses
//		 instead of examining returned value for NULLness
// NOTE how we define macroses to make them function-like
#	define	CHECK_OOM(allocRes)  if(allocRes) {} else return NULL
#   define  CHECK_OOM_FREENODE(allocRes,nodeToFree)		    \
				do{											\
				if(!(allocRes)){							\
					xmlFreeNode((xmlNodePtr)(nodeToFree));	\
					nodeToFree = NULL;						\
					return NULL;							\
				}} while(0)
// TODO: OPTIMIZE: Use call to some global function in the IF (reduce block size)
#   define  CHECK_OOM_FREENODE2(allocRes,nodeToFree1,nodeToFree2) \
				do{											\
				if(!(allocRes)){							\
					xmlFreeNode((xmlNodePtr)(nodeToFree1));	\
					nodeToFree1 = NULL;						\
					xmlFreeNode((xmlNodePtr)(nodeToFree2));	\
					nodeToFree2 = NULL;						\
					return NULL;							\
				}} while(0)
// DO NOT USE "OOM_FLAG" in C++ API wrapper's code!!!
#   define  OOM_FLAG        (xmlOOM)
#   define  SET_OOM_FLAG    xmlSetOOM()
#   define  RESET_OOM_FLAG  xmlResetOOM()
#else 
#	define  CHECK_OOM(allocRes)
#	define	CHECK_OOM_FREENODE(allocRes,nodeToFree)
#	define	CHECK_OOM_FREENODE2(allocRes,nodeToFree1,nodeToFree2)
#   define  OOM_FLAG    
#   define  SET_OOM_FLAG  
#   define  RESET_OOM_FLAG  
#endif

/*
*  _xmlDoc struct has field 'cachedGs'
*  where result of xmlGetGlobalState() is stored
*  
*  (cachedGs field is not in the part that is common with _xmlNode and _xmlAttr)
*/
#define LIBXML_ENABLE_GS_CACHING_IN_DOC

/* 
*	struct members 
*		_xmlNode.psvi
*		_xmlAttr.psvi
*	are not in use in the current configuration,
*	so they are not compiled in
*/
#ifdef XMLENGINE_XSLT 
#define LIBXML_ENABLE_NODE_PSVI
#endif

/* 
*	struct member 
*		_xmlNode.line
*	and all related functions/code 
*	are excluded from the current configuration
*/
//#define LIBXML_ENABLE_NODE_LINEINFO


/**
 * LIBXML_CATALOG_ENABLED:
 *
 * Whether the Catalog support is configured in
 */
//#define LIBXML_CATALOG_ENABLED

/**
 * LIBXML_THREAD_ENABLED:
 *
 * Whether the thread support is configured in
 */
//#define LIBXML_THREAD_ENABLED

/**
 * LIBXML_TREE_ENABLED:
 *
 * Whether the DOM like tree manipulation API support is configured in
 */
#define LIBXML_TREE_ENABLED

/**
 * LIBXML_OUTPUT_ENABLED:
 *
 * Whether the serialization/saving support is configured in
 */
#define LIBXML_OUTPUT_ENABLED

/**
 * LIBXML_PUSH_ENABLED:
 *
 * Whether the push parsing interfaces are configured in
 */
#define LIBXML_PUSH_ENABLED

/**
 * LIBXML_READER_ENABLED:
 *
 * Whether the xmlReader parsing interface is configured in
 */
//#define LIBXML_READER_ENABLED

/**
 * LIBXML_PATTERN_ENABLED:
 *
 * Whether the xmlPattern node selection interface is configured in
 */
//#define LIBXML_PATTERN_ENABLED

/**
 * LIBXML_WRITER_ENABLED:
 *
 * Whether the xmlWriter saving interface is configured in
 */
//#define LIBXML_WRITER_ENABLED

/**
 * LIBXML_SAX1_ENABLED:
 *
 * Whether the older SAX1 interface is configured in
 */
//#ifdef XMLENGINE_DOM
// for xmlParseDoc(...)
#define LIBXML_SAX1_ENABLED
//#endif

/**
 * LIBXML_VALID_ENABLED:
 *
 * Whether the DTD validation support is configured in
 */
//#define LIBXML_VALID_ENABLED

/**
 * LIBXML_HTML_ENABLED:
 *
 * Whether the HTML support is configured in
 */
#ifdef XMLENGINE_XSLT
#define LIBXML_HTML_ENABLED
#endif

/**
 * LIBXML_LEGACY_ENABLED:
 *
 * Whether the deprecated APIs are compiled in for compatibility
 */
//#define LIBXML_LEGACY_ENABLED

/**
 * LIBXML_C14N_ENABLED:
 *
 * Whether the Canonicalization support is configured in
 */
//#define LIBXML_C14N_ENABLED

/**
 * LIBXML_XPATH_ENABLED:
 *
 * Whether XPath is configured in
 */
#define LIBXML_XPATH_ENABLED

/**
 * LIBXML_XPTR_ENABLED:
 *
 * Whether XPointer is configured in
 */
//#define LIBXML_XPTR_ENABLED

/**
 * LIBXML_XINCLUDE_ENABLED:
 *
 * Whether XInclude is configured in
 */
//#define LIBXML_XINCLUDE_ENABLED

/**
 * LIBXML_DEBUG_ENABLED:
 *
 * Whether Debugging module is configured in
 */
//#define LIBXML_DEBUG_ENABLED

/**
 * DEBUG_MEMORY_LOCATION:
 *
 * Whether the memory debugging is configured in
 */
//#define DEBUG_MEMORY_LOCATION

/**
 * LIBXML_UNICODE_ENABLED:
 *
 * Whether the Unicode related interfaces are compiled in
 */
//#define LIBXML_UNICODE_ENABLED

/**
 * LIBXML_REGEXP_ENABLED:
 *
 * Whether the regular expressions interfaces are compiled in
 */
//#define LIBXML_REGEXP_ENABLED

/**
 * LIBXML_AUTOMATA_ENABLED:
 *
 * Whether the automata interfaces are compiled in
 */
//#define LIBXML_AUTOMATA_ENABLED

/**
 * LIBXML_SCHEMAS_ENABLED:
 *
 * Whether the Schemas validation interfaces are compiled in
 */
//#define LIBXML_SCHEMAS_ENABLED



