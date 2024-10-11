#include "pparser.h"

int extract_file_or_directory(char *path, char *name, int index)
{
    int i = index;
    int j = 0;
    while (*(path + i) != '/' && *(path + i) != '\0')
    {
        name[j++] = *(path + i);
        i++;
    }
    name[j++] = '\0';
    return j;
}

int path_parser_validate_format(char **path)
{
    int res = -EBADPATH;
    if (str_len(*path) >= PATH_LIMIT)
    {
        goto out;
    }
    if ((isdigit(*path[0])))
    {
        goto out;
    }
    if ((str_n_cmp(*path+1, ":/", 2)) < 0)
    {
        goto out;
    }
    res = 0;

out:
    return res;
}

int path_parser_create_root(char c, struct root_path *root)
{
    int res = 0;
    root = kzalloc(sizeof(struct root_path));
    if (!root)
    {
        res = -ENOMEM;
        goto out;
    }
    root->disk_id = (int)c;
    root->part = 0;

out:
    return res;
}

int path_parser_parse_path_part(struct root_path *root, char *path)
{
    int path_length = str_len(path + 3);
    struct path_part *next = 0;
    int index = 3;
    for (int i = 0; i < PATH_LIMIT && index < path_length; i++)
    {
        char name[PATH_LIMIT];
        struct path_part *part = kzalloc(sizeof(struct path_part));
        int length = extract_file_or_directory(path, name, index);
        index += length;
        str_n_cpy(part->name, name, length);
        part->next = next;
        next = part;
        if (i == 0)
        {
            root->part = part;
        }
    }
    return 0;
}

struct root_path *path_parser(char *file_path)
{
    int res = 0;
    struct root_path *root=kzalloc(sizeof(struct root_path));
    
    res = path_parser_validate_format(&file_path);
    if (res < 0)
    {
        goto out;
    }
    res = path_parser_create_root(file_path[0], root);
    if (res < 0)
    {
        goto out;
    }

    res=path_parser_parse_path_part(root, file_path);

out:
    if (res < 0)
    {
        return 0;
    }
    return root;
}