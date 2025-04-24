#include "xml.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

XMLnode* XMLNode_at( XMLnode *root, int index )
{
    XMLnode *node = NULL;
    int size = root->children.size;
    if( index >= size)
        return NULL;
    
    node = root->children.data[index];
    return node;
}
bool isSpace(char *buff)
{
    return (*buff == ' ');
}
void trim_whitespaces(char *str)
{
  int start= 0, end = strlen(str) - 1; 
  while(str[start] == ' ' ||  str[start] == '\n' || str[start] == '\t')
    start++;
    
  while(end > start && str[end] == ' ' || str[end] == '\n'|| str[end] == '\t')
    end--;
  
    int len = end-start+1;
 
    memmove(str, str + start, len);
  str[len] = '\0';
}
// 
//
// Definations
// 
enum _Tag_type
{
    TAG__START,
    TAG_INLINE
};
typedef enum _Tag_type  Tag_type;

static Tag_type parse_attr(char *buff, char *lex, int *buff_cursor, int *lex_cursor, XMLnode *curr_node){
           
           XMLAttributes curr_attr= {NULL,NULL};
           while( buff[*buff_cursor] != '>')
           {
              lex[(*lex_cursor)++] = buff[(*buff_cursor)++];
              if( buff[*buff_cursor] == ' ' && curr_node->tag == NULL)
              {
                lex[*lex_cursor] = '\0';
                curr_node->tag = strdup( lex );
                *lex_cursor = 0;
                continue;
              }
            
              //Key 
              if(buff[*buff_cursor] == '=')
              {
                lex[*lex_cursor] = '\0';
                trim_whitespaces(lex);
                curr_attr.key = strdup( lex);
                *lex_cursor = 0;
                continue;
              }
              // Value
              if(buff[*buff_cursor] == '"')
              {
                *lex_cursor = 0;
                (*buff_cursor)++;  //consuem '""
                while( buff[*buff_cursor] != '"')
                    lex[(*lex_cursor)++] = buff[(*buff_cursor)++];
                lex[*lex_cursor] = '\0';
                trim_whitespaces(lex);
                curr_attr.value = strdup( lex );
                XMLAttributesList_add(&curr_node->attributes, &curr_attr);
                curr_attr.key = NULL;
                curr_attr.value = NULL;
                *lex_cursor = 0;
                (*buff_cursor)++;
                continue;
              }
              // Inline Tag
              if(buff[*buff_cursor -1] == '/' &&  buff[*buff_cursor] == '>')
              {
                if(curr_node->tag == NULL)
                {
                    lex[*lex_cursor-1] = '\0';
                    curr_node->tag = strdup( lex );        
                }
                *lex_cursor = 0;
                return TAG_INLINE;
              }
           }

           return TAG__START;
}
void XMLNodeList_init(XMLNodeList *list)
{
    if(list != NULL)
    {
        list->heap_size = 1;
        list->size = 0;
        list->data = (XMLnode **) malloc( sizeof(XMLnode*) * list->heap_size ); 
    }
}
void XMLNodeList_add(XMLNodeList *list , XMLnode *node)
{
    while( list->size >= list->heap_size)
    {
        list->heap_size *=2;
        list->data = (XMLnode**) realloc(list->data, sizeof(XMLnode*) * list->heap_size);
    }

    list->data[list->size++] = node;
}
void XMLNodeList_free(XMLNodeList *list)
{
    if( list == NULL ) return;

    for(int i = 0; i < list->size; i++)  
    {
        XMLnode_free(list->data[i]); // recursively free each child node
    }

    free(list->data); 
}
bool ends_with(const char *haystack, const char *needle)
{
    size_t h_len = strlen(haystack);
    size_t n_len = strlen(needle);

    if( h_len < n_len)
        return false; // haystack  is shorter than needle 

    for(int i = 0; i < n_len; i++)
    {
        if(haystack[h_len-n_len + i] != needle[i])
            return false; // haystack & needle  mismatch 
    }

    return true;
}
bool starts_with(char *haystack, char *needle)
{
    size_t hlen =strlen(haystack);
    size_t nlen = strlen(needle);
    if( hlen < nlen)
        return false;

    int i = 0;
    while( i < nlen)
    {
        if( haystack[i] != needle[i])
            return false;
        i++;            
    }
    return true;
}
void XMLAttributesList_add(XMLAttributesList *list, XMLAttributes *attr)
{
  // resize the list if current capacity exceeds the allowable capacity
  if(list->size >= list->heap_size) 
  {
    list->heap_size *= 2;
    list->data = (XMLAttributes*) (realloc(list->data, sizeof(XMLAttributes)* list->heap_size));
  }
   //todo: check for memory allocation fail before use
  list->data[list->size++] = *attr;
}
void XMLAttributesList_init(XMLAttributesList *list)
{
    
    list->heap_size =2;
    list->size = 0;
    list->data = (XMLAttributes*) malloc(sizeof(XMLAttributes) * list->heap_size);
   
}
void XMLAttributesList_free( XMLAttributesList *list)
{
    if( list == NULL ) return;
    for(int i = 0; i < list->size; i++)
    {
        XMLAttributes_free(&list->data[i]);
    }

    free(list->data);
}
void XMLAttributes_free(XMLAttributes *attr)
{
    free(attr->key);
    free(attr->value);
}
XMLnode *XMLnode_new(XMLnode *parent_node)
{
    XMLnode *new_node = (XMLnode*) malloc(sizeof(XMLnode));
    if( new_node == NULL )
    {
        fprintf(stderr, "Could not allocate memory: %s\n",__func__);
        return NULL;
    }
    new_node->parent = parent_node; // store reference to parent node in new node
    new_node->tag = NULL;
    new_node->inner_text = NULL;
    XMLAttributesList_init(&new_node->attributes);
    XMLNodeList_init(&new_node->children);
    // if we have a parent node , add ourselves into parent->children list 
    if( parent_node != NULL)
        XMLNodeList_add(&parent_node->children, new_node);
    return new_node;
}
void XMLnode_free(XMLnode *node)
{
        if(node->tag != NULL ) free(node->tag);
        if(node->inner_text != NULL) free(node->inner_text);
        for(int i=0; i < node->attributes.size; i++)
        //  Delete XMLAttributes 
        XMLAttributesList_free(&node->attributes);

        // TO DO: DELETE XMLNode LIST
        free(node);
}

void XMLDocument_init(XMLDocument *doc)
{
    doc->root = (XMLnode *) malloc( sizeof(XMLnode));
}
void XMLDocument_free(XMLDocument *doc)
{
    free(doc->root);
}
bool XMLDocument_load(XMLDocument *doc, const char *path)
{
    FILE *file = fopen(path, "r");
    if(!file)
    {
        fprintf(stderr,"could not load file from %s\n", path);
        return false;
    }

    fseek(file, 0 , SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *buff = (char*) malloc( sizeof(char) * size +1 );
    if(buff == NULL){
        fprintf(stderr, "Fail to alocate memory\n");
    }

    fread(buff, 1, size, file);
    fclose(file);
    buff[size] ='\0';

    char lex[256];
    int lex_cursor = 0;
    int buff_cursor = 0;

    doc->root = XMLnode_new( NULL );
    XMLnode *curr_node = doc->root;
    
    while(buff[buff_cursor] != '\0')
    {
        
       if(buff[buff_cursor] == '<')
       {   
           
           //Inner text.
           if(lex_cursor > 0)
           {
                lex[lex_cursor] = '\0';
                if(curr_node->tag == NULL)
                {
                    fprintf(stderr, "Text outside document. (%s). \n", lex);
                    return false;
                }
                if(curr_node->inner_text != NULL)
                {   // If we evere get inside here, then we are trying to ovewrite current node inner text
                    fprintf(stderr,"Text outside document. (%s)", lex);
                    return false;
                }
                trim_whitespaces(lex);
                curr_node->inner_text = strdup( lex );
                lex_cursor = 0 ;
                continue;
           }
           // Special nodes
           //1.COMMENTS
           if(buff[buff_cursor+1] == '!' && buff[buff_cursor] =='<')
           {
                while( buff[buff_cursor] && (buff[buff_cursor-1] != '-' || buff[buff_cursor] != '>' ) )
                    lex[lex_cursor++] = buff[buff_cursor++];
                
                lex[lex_cursor++] = buff[buff_cursor]; // read ">"
                lex[lex_cursor] = '\0';
                trim_whitespaces( lex);

                if(!(starts_with(lex, "<!--") && ends_with(lex, "-->")) )
                {
                    fprintf(stderr,"Illegal formated comment. (%s)", lex);
                    return false;
                }
                buff_cursor++;
                lex_cursor=0;
                continue;
           }

           // Node End
           if( buff[buff_cursor+1] == '/' && buff[buff_cursor] == '<')
           {
            buff_cursor += 2;
            lex_cursor = 0;
            while ( buff[buff_cursor] != '>')
                lex[lex_cursor++] = buff[buff_cursor++];
            
            lex[lex_cursor] = '\0';
            if( strcmp(curr_node->tag, lex) != 0)
            {
                fprintf(stderr,"Tag name mismatch. (%s != %s)", curr_node->tag, lex);
                return false;
            }

            curr_node = curr_node->parent;
            buff_cursor++;
            lex_cursor= 0 ;
            continue;      
           }
           
           // Tag Start 
           curr_node = XMLnode_new( curr_node);
           buff_cursor++; //skip '<'
           if( parse_attr(buff, lex, &buff_cursor, &lex_cursor, curr_node) == TAG_INLINE)
           {
                curr_node = curr_node->parent;
                buff_cursor++;
                lex_cursor = 0;
                continue;
           }

           // Set tag Name if none 
           lex[lex_cursor] = '\0';
           if(curr_node->tag == NULL)
            curr_node->tag = strdup( lex );

            lex_cursor=0;
            buff_cursor++; 
          continue;
       }else{
            //Inner text
            lex[lex_cursor++] = buff[buff_cursor++];
            if(lex_cursor-1 == 0 && (lex[lex_cursor-1] ==' ' || lex[lex_cursor-1] =='\n') ) 
                lex_cursor--;
            continue;
       }
       
    }

    return true;
}
