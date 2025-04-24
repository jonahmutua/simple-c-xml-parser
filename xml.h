#ifndef XML_H_
#define XML_H_

#include<stdbool.h>
#include<string.h>
//
// Declaration  PARSER-XML
//

// xml attributes structure 
/**
 *  @brief a structure to XML attributes
 */
struct _XMLAttributes
{
    char *key;
    char *value;
};
typedef struct _XMLAttributes XMLAttributes;

// xml attributes list  structure
/**
 *  @brief a structure to XML atrributes list 
 */
struct _XMLAttributesList
{
  int heap_size;
  int size;
  XMLAttributes *data;
};
typedef struct _XMLAttributesList  XMLAttributesList;

// xml node list structure .. 
/**
 *  A structure to XML node list 
 */
struct _XMLNodeList
{
    int heap_size;
    int size;
    struct _XMLnode **data;
};
typedef struct _XMLNodeList XMLNodeList;

// xml node structure
/**
 *  @brief a structure to XML node 
 */
struct _XMLnode
{
    char *tag;
    char *inner_text;
    struct _XMLnode *parent;
    XMLAttributesList attributes;
    XMLNodeList children;
};
typedef struct _XMLnode XMLnode;

/**
 *  @brief a structure to XML Document 
 */
struct _XMLDocument
{
    XMLnode *root;
};
typedef struct _XMLDocument XMLDocument;

// INITIALIZING FUNCTIONS 
/**
 *  @brief -Initializes XML node list 
 *  @param list - a pointer to XML node list which to initialise
 */
void XMLNodeList_init(XMLNodeList *list);
/**
 *  @brief appends a XML node into XML node list
 *  @param list - a pointer to XML node list 
 *  @param node - pointer to a XML node which to append to XML node list
 */
void XMLNodeList_add(XMLNodeList *list , XMLnode *node);
/**
 *  @brief - releases memory resources used up by XML node list
 *  @param - list - a pointer to XML node list 
 */
void XMLNodeList_free(XMLNodeList *list);
/**
 *  @brief - loads and parses XML document in memory
 *  @param doc a pointer to XML document 
 *  @param path - a pointer to xml file in local memory
 *  @return  bool - True - success , False - fail to parse 
 */
bool XMLDocument_load(XMLDocument *doc, const char *path);
/**
 *  @brief 
 */
void XMLDocument_free(XMLDocument *doc);
XMLnode *XMLnode_new(XMLnode *parent);
void XMLnode_free(XMLnode *node);
void XMLAttributes_free(XMLAttributes *attr);
void XMLAttributesList_init(XMLAttributesList *list);
void XMLAttributesList_free( XMLAttributesList *list);
void XMLAttributesList_add(XMLAttributesList *list,XMLAttributes *attr);

bool ends_with(const char *haystack, const char *needle);
bool isSpace(char *buff);
void trim_whitespaces(char *str);

// Retreival functions 
/**
 *  @brief retreives Xmlnode at position indicated by index
 *  @param root  -a pointer to a list of XMLnode
 *  @param index - the position in the list 
 *  @return - returns a pointer to xmlnode if found otherwise returns NULL 
 */
XMLnode* XMLNode_at( XMLnode *root, int index );



#endif