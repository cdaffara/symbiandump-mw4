/*
 * Summary: the XML document serializer
 * Description: API to save document or subtree of document
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_XMLSAVE_H__
#define __XML_XMLSAVE_H__

//#include "Libxml2_xmlversion.h"
//#include "Libxml2_tree.h"
//#include "Libxml2_encoding.h"
#include "Libxml2_xmlIO.h"

#ifdef LIBXML_OUTPUT_ENABLED
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INDENT 60

#ifdef XMLENGINE_NODEFILTER
// TODO: Create separate header file for node filter definitions
/**
Pointer to implementation of interface NodeFilter (as introduced in DOM Level 2)

@param aNode   Node to test
@returns
    1  =  KFilterAccept  - use node
    2  =  KFilterReject  - do not use node but proceed with chilren nodes
    3  =  KFilterSkip    - skip node and its subtree
    4  =  KFilterSkipElementContents - [non-standard option] 
*/
typedef int (XMLCALL *xmlNodeFilterFunc)(xmlNodePtr aNode);
typedef int (XMLCALL *xmlNodeFilterProxyFunc)(void* fn, xmlNodePtr aNode);

const int KFilterAccept = 1;
const int KFilterReject = 2;
const int KFilterSkip   = 3;
// Note: this is non-standard option:
const int KFilterSkipElementContents = 4;

typedef struct _xmlNodeFilterData xmlNodeFilterData;
struct _xmlNodeFilterData{
      void* fn;
      xmlNodeFilterProxyFunc proxyFn;
};
#endif

typedef struct _xmlSaveCtxt xmlSaveCtxt;
typedef xmlSaveCtxt *xmlSaveCtxtPtr;
struct _xmlSaveCtxt {
    void *_private;
    int type;
    int fd;
    const xmlChar *filename;
    const xmlChar *encoding;
    xmlCharEncodingHandlerPtr handler;
    xmlOutputBufferPtr buf;
    xmlDocPtr doc;
    int options;
    int level;
    int format;
    // TODO: Make indent strings be allocated somewhere else (not on the stack with xmlSaveCtxt vars)
    char indent[MAX_INDENT + 1];    /* array for indenting output */
    int indent_nr;
    int indent_size;
    xmlCharEncodingOutputFunc escape;   /* used for element content */
    xmlCharEncodingOutputFunc escapeAttr;/* used for attribute content */
#ifdef XMLENGINE_NODEFILTER
    xmlNodeFilterData* nodeFilter;
#endif
};

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToFd(
                    int fd,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToFilename(
                    const char *filename,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToBuffer(
                    xmlBufferPtr buffer,
                    const char *encoding,
                    int options);

XMLPUBFUN xmlSaveCtxtPtr XMLCALL
        xmlSaveToIO(
                    xmlOutputWriteCallback iowrite,
                    xmlOutputCloseCallback ioclose,
                    void *ioctx,
                    const char *encoding,
                    int options);

XMLPUBFUN long XMLCALL
        xmlSaveDoc(
                    xmlSaveCtxtPtr ctxt,
                    xmlDocPtr doc);

XMLPUBFUN long XMLCALL
        xmlSaveTree(
                    xmlSaveCtxtPtr ctxt,
                    xmlNodePtr node);
// XMLENGINE: was made exported
XMLPUBFUN void
    xmlNodeDumpOutputInternal(
                    xmlSaveCtxtPtr ctxt,
                    xmlNodePtr cur);
XMLPUBFUN int XMLCALL
        xmlSaveFlush(
                    xmlSaveCtxtPtr ctxt);

XMLPUBFUN int XMLCALL
        xmlSaveClose        (xmlSaveCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
        xmlSaveSetEscape    (xmlSaveCtxtPtr ctxt,
                     xmlCharEncodingOutputFunc escape);
XMLPUBFUN int XMLCALL
        xmlSaveSetAttrEscape    (xmlSaveCtxtPtr ctxt,
                     xmlCharEncodingOutputFunc escape);

// KO: added to allow use them externally
XMLPUBFUN void XMLCALL
xmlDocContentDumpOutput(xmlSaveCtxtPtr ctxt, xmlDocPtr cur);
XMLPUBFUN void XMLCALL
xmlSaveCtxtInit(xmlSaveCtxtPtr ctxt);

#ifdef __cplusplus
}
#endif
#endif /* LIBXML_OUTPUT_ENABLED */
#endif /* __XML_XMLSAVE_H__ */


