
#include "plague.h"

u64 optionalTitle() {
    FILE *file = fopen("sdmc:/lfs_target", "rb");
    if(file == NULL) {
        return 0;
    }
    u64 title_id;

    fread(&title_id, 1, sizeof(title_id), file);
    fclose(file);

    return title_id;
}

void optionalPath(char* path) {
    FILE *file = fopen("sdmc:/lfs_path", "r");
    if(file == NULL) {
        return;
    }

    fread(path, 1, sizeof(char)*50, file);
    fclose(file);
}
 