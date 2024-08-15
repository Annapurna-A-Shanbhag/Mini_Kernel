#include "pparser.h"

int extract_file_or_directory(char *path,char *name,int index){
    int i=index;
    int j=0;
    while(*(path+i)!='/' || *(path+i)!='\n'){
        i++;
        name[j++]=*(path+i);
    }
    name[j++]='\n';
    return j;
    
}

struct root_path * path_parser(struct disk *disk,char* file_path){
    if(str_n_cmp(file_path,(void *)disk->id,1)<0){
        return 0;
    }
    struct  root_path *root=kzalloc(sizeof(struct root_path));
    
    
    root->disk_id=disk->id;
    root->part=0;

    int index=3;
    
    int path_length=str_len(file_path+3);
    
    struct path_part *next=0;
    for(int i=0;i<PATH_LIMIT && index<path_length;i++){
        char name[PATH_LIMIT];
        struct path_part *part=kzalloc(sizeof(struct path_part));
        int length=extract_file_or_directory(file_path,name,index);
        index+=length;
        str_n_cpy(part->name,name,length);
        part->next=next; 
        next=part;
    }
    return root;



}