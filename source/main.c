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



void mainMenu() {
	int idx = 0, cnt = 0, dead = 0;
	u64 redirections[100];

	while(1) {
		printf("\x1b[%dm\x1b[%d;%dH[%s]\n\n\x1b[0m", CYAN, 3, 0, "Current redirections: ");
		cnt = 0;
		dead = 0;

		FILE *f_table = fopen("sdmc:/_plague_table", "r+b");
	    if(f_table) {
	    	struct ReplacedTitle a;
	    	char titleName[100];

		    while(fread(&a, sizeof(struct ReplacedTitle), 1, f_table)) {
		        if(a.title_id != 0xDEADBEEF) {

		        	appInfo(a.title_id, titleName);

		        	printf("\x1b[%dm %s (%016llx) => %s \x1b[0m\n", cnt == idx ? GREEN : WHITE,titleName,a.title_id ,a.path);
		        	redirections[cnt] = a.title_id;
		        	cnt++;
		    	} else {
		    		dead++;
		    	}
		    }



	   		fclose(f_table); 
		} else {
			printf("No redirections\n");
		}

		printf("\nDead entries: %d\n", dead);


	    printf("\nPress X to add new redirection\n");
	    printf("Press Y to remove redirection\n");
	    printf("Press A to change current selected redirection\n");

	    printf("\n");

		u64 kDown = waitForKey();

		if (kDown & KEY_DDOWN && idx < (cnt - 1)) {
	    	idx++;
	    } else if (kDown & KEY_DDOWN && idx == (cnt - 1)) {
	    	idx = 0;
	    }
	    if (kDown & KEY_DUP && idx > 0) {
	        idx--;
	    } else if (kDown & KEY_DUP && idx == 0) {
	        idx = cnt - 1;
	    }
		if(kDown & KEY_X) {
			u64 selectedApp = DrawAppList();
		    printf("selected app: %016lx\n", selectedApp);
		    pressAtoContinue();


	        char folder[100] = "";
	        DrawBackupList(folder);

	        printf("selected path: %s\n", folder);
	        pressAtoContinue();

	        addReplacement(selectedApp, folder);

	       	//npdm_info(path);
         	patch_npdm(folder, selectedApp);
         	pressAtoContinue();

	        return;
		}
		if(kDown & KEY_A) {
			u64 selectedApp = redirections[idx];
			if(cnt == 0) {
				printf("You has no active redirections, please add one first\n");
		    	pressAtoContinue();
		    	return;
			}

	        char folder[100] = "";
	        DrawBackupList(folder);

	        addReplacement(selectedApp, folder);

	        //npdm_info(path);
         	patch_npdm(folder, selectedApp);
         	pressAtoContinue();

	        return;
		}
		if(kDown & KEY_Y) {
			u64 toRemove = redirections[idx];
			printf("You are about to remove redirection: %016llx\n", toRemove);
			pressAtoContinue();
	        removeReplacement(toRemove);
	        return;
		}
		if(kDown & KEY_B) {
			u64 check = redirections[idx];

			char subpath[50] = "";
    		if(getReplacement(check, subpath)) {
    			printf("Got replacement: %s\n", subpath);
    		} else {
    			printf("There is problem here\n");
    		}   
			
	        return;
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
            consoleClear();
            printf("\x1b[%dmAtmosphere mod Plague backup selector v0.3 by NermaN\x1b[0m\n\n", YELLOW);

            mainMenu();

        
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }
    nsExit();
    gfxExit();
    fsExit();
    return 0;
}

