 #include <stdio.h>
 #include <inttypes.h>
 #include <switch.h>   


#include "npdm.hpp"


void patch_npdm(char* path, u64 new_tid) {

    FILE *f_npdm;

    NpdmCache npdm = {0};


        char npdm_path[100] = "";
        snprintf(npdm_path, 100, "sdmc:/backups/%s/exefs/main.npdm", path);
        printf("npdm path: %s\n", npdm_path);

        if((f_npdm=fopen(npdm_path, "r+b"))==NULL) {
            printf ("Cannot open file.\n");
        } else {
            fseek(f_npdm, 0, SEEK_END);
            size_t npdm_size = ftell(f_npdm);
            fseek(f_npdm, 0, SEEK_SET);


            if (npdm_size > sizeof(npdm.buffer) || fread(npdm.buffer, 1, npdm_size, f_npdm) != npdm_size) {
                printf("npdm size: %ld buffer size: %ld\n", npdm_size, sizeof(npdm.buffer));
                printf("npdm probably corrupted\n");
            }else {

                fseek(f_npdm, 0, SEEK_SET);
                
                npdm.info.header = (NpdmHeader *)(npdm.buffer);
                NpdmInfo *info = &npdm.info;

                info->aci0 = (NpdmAci0 *)(npdm.buffer + info->header->aci0_offset);


                printf("Current NPDM title id: %016lx\n", info->aci0->title_id);

                if(info->aci0->title_id == 0) {
                    printf("Seems like something wrong, stop\n");
                    return;
                }

                info->aci0->title_id = new_tid;

                printf("Replacing with: %016lx\n", info->aci0->title_id);

                //f_npdm=fopen(npdm_path, "wb");

                fwrite(npdm.buffer, info->header->aci0_offset + info->header->aci0_size, 1, f_npdm);

                fclose(f_npdm);
            }

        }
}


void npdm_info(char* path) {

    FILE *f_npdm;
    NpdmCache npdm = {0};


        char npdm_path[100] = "";
        snprintf(npdm_path, 100, "sdmc:/%s/exefs/main.npdm", path);
        printf("npdm path: %s\n", npdm_path);

        if((f_npdm=fopen(npdm_path, "r"))==NULL) {
            printf ("Cannot open file.\n");
        } else {
            fseek(f_npdm, 0, SEEK_END);
            size_t npdm_size = ftell(f_npdm);
            fseek(f_npdm, 0, SEEK_SET);


            if (npdm_size > sizeof(npdm.buffer) || fread(npdm.buffer, 1, npdm_size, f_npdm) != npdm_size) {
                printf("npdm probably corrupted\n");
                fclose(f_npdm);
            }else {
            
                

                npdm.info.header = (NpdmHeader *)(npdm.buffer);
                NpdmInfo *info = &npdm.info;

                info->aci0 = (NpdmAci0 *)(npdm.buffer + info->header->aci0_offset);


                printf("Current backup NPDM title id: %016lx\n", info->aci0->title_id);

               fclose(f_npdm);

                
            }

        }
}

int write_lfs(char* path, u64 new_tid) {
    FILE *lfs_target = fopen("sdmc:/lfs_target", "wb");

    if(lfs_target == NULL) {
        printf ("Cannot open lfs_target file.\n");
        return 1;
    }

    fwrite(&new_tid, sizeof(u64), 1, lfs_target);
    fclose(lfs_target);

    printf("Updated lfs_target\n");

    FILE *lfs_path = fopen("sdmc:/lfs_path", "wb");

    if(lfs_path == NULL) {
        printf ("Cannot open lfs_path file.\n");
        return 1;
    }

        char bpath[100] = "";
        snprintf(bpath, 100, "backups/%s", path);
        printf("new backup path: %s\n", bpath);

    fwrite(&bpath, sizeof(bpath), 1, lfs_path);
    fclose(lfs_path);

    printf("Updated lfs_path\n");

    return 0;
}