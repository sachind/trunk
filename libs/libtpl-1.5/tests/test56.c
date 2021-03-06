#include "tpl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define S1_LEN 6
#define S2_LEN 4

struct has_strings {
    char a;
    char s1[S1_LEN];
    char s2[S2_LEN];
};

int main(int argc,char*argv[]) {
    tpl_node *tn;
    struct has_strings hs,hs2;
    void *img;
    size_t sz;

    strncpy(hs.s1, "draco",S1_LEN);
    strncpy(hs.s2, "po",S2_LEN);

    tn = tpl_map("c#c#", hs.s1, S1_LEN, hs.s2, S2_LEN);
    tpl_pack(tn,0);
    tpl_dump(tn,TPL_MEM,&img,&sz);
    tpl_free(tn);

    /* unpack */

    tn = tpl_map("c#c#", hs2.s1, S1_LEN, hs2.s2, S2_LEN);
    tpl_load(tn,TPL_MEM,img,sz);
    tpl_unpack(tn,0);
    tpl_free(tn);
    free(img);

    printf("hs2.s1 length: %d\n", (int)strlen(hs2.s1));
    printf("hs2.s1: %s\n", hs2.s1);
    printf("hs2.s2 length: %d\n", (int)strlen(hs2.s2));
    printf("hs2.s2: %s\n", hs2.s2);
    return(0);
}
