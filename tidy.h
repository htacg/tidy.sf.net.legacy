#ifndef __HTML_TIDY_H__
#define __HTML_TIDY_H__

/* tidy.h
*/

/**
 * Defines HTML Tidy API implemented by tidy library.
 *
 * HTML Tidy is copyright by the World Wide Web Consortium.
 * For details see http://www.w3.org/People/Raggett/tidy/
 *
 * Very rough initial cut for discussion purposes.
 *
 * Public interface is const-correct and doesn't explicitly depend
 * on any globals.  Thus, thread-safety may be introduced w/out
 * changing the interface.
 *
 * Looking ahead to a C++ wrapper, C functions always pass 
 * this-equivalent as 1st arg.
 *
 * Created 2001-05-20 by Charles Reitzel
 */

typedef enum
{
  tidyOpt_Unknown,
  tidyOpt_1,
  tidyOpt_2,
  /* ... */
  tidyOpt_N
} TidyOption;

/**
 * Opaque data structure.
 * Cast to _tidy_impl struct within lib.
 * This will reduce inter-dependencies/conflicts w/ application code.
 */
struct _tidy_doc
{
  int _opaque;
};
typedef struct _tidy_doc TidyDoc;


/**
 * Message handling interface
 */

typedef enum 
{
  TidyInfo,
  TidyWarning,
  TidyError,
  TidyBadDocument,
  TidyFatal
} TidyReportLevel;


typedef void (TidyReport)( TidyDoc* inst, TidyReportLevel lvl, const char* mssg );


/* Tidy public interface
**
** Most functions return an integer:
**
** 0    -> SUCCESS
** >0   -> WARNING
** <0   -> ERROR
** 
*/

TidyDoc*    tidyCreate();
void        tidyRelease( TidyDoc* inst );

int         tidySetMessageHandler( TidyDoc* inst, TidyReport rptFunc );
int         tidyLoadConfig( TidyDoc* inst, const char* configFile );

/* Let application store a chunk of data w/ each Tidy instance.
** Useful for callbacks.
*/
void        tidySetAppData( TidyDoc* inst, unsigned appData );
unsigned    tidyGetAppData( TidyDoc* inst );


/* All text options are ASCII? - no charset issues? 
** Wrong, filenames may go beyond ascii.  Use LC_TYPE?
*/
const char* tidyGetOption( TidyDoc* inst, TidyOption opt );
int         tidySetOption( TidyDoc* inst, TidyOption opt, const char* val );

int         tidyGetOptionInt( TidyDoc* inst, TidyOption opt );
int         tidySetOptionInt( TidyDoc* inst, TidyOption opt, int val );


/* Library helpers
*/
const char* tidyLookupMessage( int errorNo );
TidyOption  tidyGetOptionId( const char* opt );
const char* tidyGetOptionText( TidyOption opt );


/* Document info */
int         tidyParseStatus( TidyDoc* inst );
const char* tidyHtmlVersion( TidyDoc* inst ); /* FPI for HTML/XHTML version */
const char* tidyCharset( TidyDoc* inst );     /* text of charset MIME parameter */


/* I/O Functions
**
** Initial version supports only whole-file operations.
** Do not expose Tidy StreamIn or Out data structures - yet.
*/

/* Parse/load Functions
**
** HTML/XHTML version determined from input.
** If null passed to charset, will use current LC_TYPE.
*/
int         tidyParseHtmlFile( TidyDoc* inst, const char* filename,
                               const char* charset );

int         tidyParseXhtmlFile( TidyDoc* inst, const char* filename,
                                const char* charset );


/* Print/save Functions
**
** If null passed to (x)html version, will use default.
** If null passed to charset, will use current LC_TYPE.
*/
int         tidySaveAsHtml( TidyDoc* inst, const char* filename, 
                            const char* htmlVersionPubId,
                            const char* charset );

int         tidySaveAsXhtml( TidyDoc* inst, const char* filename, 
                             const char* xhtmlVersionPubId, 
                             const char* charset );


/* Tree traversal functions
**
** The big issue here is the degree to which we should mimic
** a DOM and/or SAX nodes.
** 
** Is it 100% possible (and, if so, how difficult is it) to 
** emit SAX events from this API?  If SAX events are possible,
** is that 100% of data needed to build a DOM?
*/

typedef enum 
{
  tidyNode_Root,
  tidyNode_DocType,
  tidyNode_Comment,
  tidyNode_ProcIns,
  tidyNode_Text,
  tidyNode_Start,
  tidyNode_End,
  tidyNode_StartEnd,
  tidyNode_CDATA,
  tidyNode_Section,
  tidyNode_Asp,
  tidyNode_Jste,
  tidyNode_Php
} TidyNodeType;


/* All opaque structs */
struct _tidy_node
{
  int _opaque;
};

typedef struct _tidy_node TidyNode;

struct _tidy_attr
{
  int _opaque;
};
typedef struct _tidy_attr TidyAttr;

struct _tidy_attr_list
{
  int _opaque;
};
typedef struct _tidy_attr_list TidyAttrList;

TidyNode*   tidyGetRoot( TidyDoc* inst );
TidyNode*   tidyGetHtml( TidyDoc* inst );
TidyNode*   tidyGetHead( TidyDoc* inst );
TidyNode*   tidyGetBody( TidyDoc* inst );

/* parent / child */
TidyNode*   tidyGetParent( TidyDoc* inst, TidyNode* nod );
TidyNode*   tidyGetChild( TidyDoc* inst, TidyNode* nod );

/* siblings */
TidyNode*   tidyGetNext( TidyDoc* inst, TidyNode* nod );
TidyNode*   tidyGetPrev( TidyDoc* inst, TidyNode* nod );

/* Node info */
TidyNodeType  tidyNodeType( TidyNode* nod );
const char*   tidyNodeName( TidyNode* nod );
TidyAttrList* tidyNodeAttrs( TidyNode* nod );

/* Null for non-element nodes and all pure HTML */
const char*   tidyNodeNsLocal( TidyNode* nod );
const char*   tidyNodeNsPrefix( TidyNode* nod );
const char*   tidyNodeNsUri( TidyNode* nod );

/* Iterate over attribute values */
TidyAttr*     tidyAttrFirst( TidyAttrList* attrList );
TidyAttr*     tidyAttrLast( TidyAttrList* attrList );

TidyAttr*     tidyAttrNext( TidyAttrList* attrList, TidyAttr* attr );
TidyAttr*     tidyAttrPrev( TidyAttrList* attrList, TidyAttr* attr );

const char*   tidyAttrName( TidyAttr* attr );
const char*   tidyAttrValue( TidyAttr* attr );

/* Null for pure HTML */
const char*   tidyAttrNsLocal( TidyAttr* attr );
const char*   tidyAttrNsPrefix( TidyAttr* attr );
const char*   tidyAttrNsUri( TidyAttr* attr );


#endif /* __HTML_TIDY_H__ */
