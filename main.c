#include<stdio.h>
#include<stdlib.h>
#include "xml.h"
const char *source = "doc.xml";

int main()
{
    XMLDocument doc;
    if(XMLDocument_load(&doc, source))
    {
        XMLnode *main = XMLNode_at(doc.root, 0) ;
       
        if( main != NULL)
        {

            //printf("tag name = \"%s\"\n", main->tag);
            //printf("tag name = \"%s\"\ninner text = \"%s\"\n", main->tag, main->inner_text );
            // //Key value pairs
            // for(int i = 0; i < doc.root->attributes.size; i++)
            // {
            //     printf("%s = \"%s\" \n", doc.root->attributes.data[i].key, doc.root->attributes.data[i].value);
            // }

            for(int i = 0; i < main->children.size; i++)
            {
                printf("Node %d: %s=\"%s\"\n", i+1, main->children.data[i]->tag, main->children.data[i]->inner_text);
            }

        }else
            printf("Root has no children.\n");
        
        //inner text
       // printf("Inner text: \"%s\"\n", doc.root->inner_text);
    }
    else{
        printf("Fail to load doc.xml\n");
    }
}
    