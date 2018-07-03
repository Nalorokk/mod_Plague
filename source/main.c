#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <switch.h>
#include <inttypes.h>

#include "menu.h"
#include "install.h"
#include "apprec.h"
#include "utils.h"
#include "types.h"
#include "npdm.cpp"
#include "plague.h"

void pressAtoContinue() {
    printf("\n\x1b[%dmPress A to continue\x1b[0m\n", GREEN);

    while(1) {
            u64 kDown = waitForKey();
            if (kDown & KEY_A)
            {
                printf("continue...\n");
                consoleClear();
                break;
            }
            if (kDown & KEY_PLUS) //Plus to exit into hbmenu
            {
            	nsExit();
			    gfxExit();
			    fsExit();
                exit(0);
                while(1) {
                }
            }

        }
}

int main(int argc, char **argv) {
    
    gfxInitDefault();
    consoleInit(NULL);
    nsInitialize();
    fsInitialize();
    
    while(appletMainLoop()) {
        //DrawMenu(&menu_top);

        u64 currentTitle = optionalTitle();

        char path[50] = "";
        optionalPath(path);

            consoleClear();
            printf("\x1b[%dmAtmosphere mod Plague backup selector by NermaN\x1b[0m\n\n", RED);

            if(currentTitle > 0) {
                printf("Current donor title: %016lx\n", currentTitle);
            } else {
                printf("Current donor title: none\n");
            }

            printf("Current path: '%s'\n", path);

            npdm_info(path);


            pressAtoContinue();
        


        u64 selectedApp = DrawAppList();

        printf("selected app: %016lx\n", selectedApp);
        pressAtoContinue();

        char folder[100] = "";

        DrawBackupList(folder);

         patch_npdm(folder, selectedApp);
         write_lfs(folder, selectedApp);



        pressAtoContinue();

        
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }
    nsExit();
    gfxExit();
    fsExit();
    return 0;
}

