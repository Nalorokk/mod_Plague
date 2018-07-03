
#include "plague.h"

void listReplacement() {
    FILE *file = fopen("sdmc:/_plague_table", "r+b");
    if(!file) {
        return;
    }
    struct ReplacedTitle a;

    while(fread(&a, sizeof(struct ReplacedTitle), 1, file)) {
        printf("%016llx => %s\n", a.title_id, a.path);
    }
    fclose(file);
    return;
}
void removeReplacement(u64 title_id) {
    FILE *file = fopen("sdmc:/_plague_table", "r+b");
    if(!file) {
        return;
    }

    struct ReplacedTitle a;
    int i = 0;

    int replace = -1;

    while(fread(&a, sizeof(struct ReplacedTitle), 1, file)) {
        if(a.title_id == title_id) {
            replace = i;
            break;
        }
        i++;
    }

    
    struct ReplacedTitle x;
    x.title_id = 0xDEADBEEF;
    memset(x.path, 0, 50);

    if(replace < 0) {
        return;
    } else {
        fseek(file, sizeof(struct ReplacedTitle)*replace, SEEK_SET);
    }

    fwrite(&x, sizeof(struct ReplacedTitle), 1, file);
    fclose(file);
}
bool getReplacement(u64 title_id, char* path) {
    FILE *file = fopen("sdmc:/_plague_table", "r+b");
    if(!file) {
        return false;
    }
    struct ReplacedTitle a;
    int i = 0;


    while(fread(&a, sizeof(struct ReplacedTitle), 1, file)) {
        if(a.title_id == title_id) {
            snprintf(path, 50, "backups/%s", a.path);
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

void addReplacement(u64 title_id, char* path) {
    FILE *file = fopen("sdmc:/_plague_table", "r+b");
    if(!file) {
        file = fopen("sdmc:/_plague_table", "wb");
    }

    struct ReplacedTitle a;
    int i = 0;

    int replace = -1;

    while(fread(&a, sizeof(struct ReplacedTitle), 1, file)) {
        if(a.title_id == title_id) {
            replace = i;
            break;
        }
        i++;
    }

    if(replace < 0) {
        int i = 0;
        fseek(file, 0, SEEK_SET);
        while(fread(&a, sizeof(struct ReplacedTitle), 1, file)) {
            if(a.title_id == 0xDEADBEEF) {
                replace = i;
                break;
            }
            i++;
        }
    }

    
    struct ReplacedTitle x;
    x.title_id = title_id;
    memset(x.path, 0, 50);
    strcpy(x.path, path);

    if(replace < 0) {
        fseek(file, 0, SEEK_END);
    } else {
        fseek(file, sizeof(struct ReplacedTitle)*replace, SEEK_SET);
    }

    fwrite(&x, sizeof(struct ReplacedTitle), 1, file);
    fclose(file);
}