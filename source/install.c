#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plague.h"
#include "install.h"
#include "types.h"

static int m_errorCode = 0;

static Service g_nsAppManSrv, g_nsGetterSrv/*, g_nsvmSrv*/;

void appInfo(u64 titleId, char* output) {
    Result rc=0;

    NsApplicationControlData *buf=NULL;
    size_t outsize=0;

    buf = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
    if (buf==NULL) {
		m_errorCode = 1;
		return;
    }
    memset(buf, 0, sizeof(NsApplicationControlData));

    rc = nsInitialize();
    if (R_FAILED(rc)) {
		m_errorCode = 2;
		return;
    }

    rc = nsGetApplicationControlData(1, titleId, buf, sizeof(NsApplicationControlData), &outsize);
    if (R_FAILED(rc)) {
		m_errorCode = 3;
		return;
    }

    NacpLanguageEntry *langentry = NULL;

    if (R_SUCCEEDED(rc)) {
            rc = nacpGetLanguageEntry(&buf->nacp, &langentry);

            if (R_FAILED(rc) || langentry==NULL) printf("Failed to load LanguageEntry.\n");
        }

        if (R_SUCCEEDED(rc)) {
            memset(output, 0, 100);
            strncpy(output, langentry->name, 100-1);//Don't assume the nacp string is NUL-terminated for safety.

          //printf("Name: %s\n", name);//Note that the print-console doesn't support UTF-8. The name is UTF-8, so this will only display properly if there isn't any non-ASCII characters. To display it properly, a print method which supports UTF-8 should be used instead.

            //You could also load the JPEG icon from buf->nacp.icon. The icon size can be calculated with: iconsize = outsize - sizeof(buf->nacp);
        }
}

Result nssListApplicationRecord(u64 offset, AppRecord* buffer, size_t size, size_t* actual_size) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, buffer, size, 0);

    struct {
        u64 magic;
        u64 cmd_id;
        u64 offset;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 0;
    raw->offset = offset;

    Result rc = serviceIpcDispatch(&g_nsAppManSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u64 actual_size;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc) && actual_size) *actual_size = resp->actual_size;
    }

    return rc;
}

static Result _nsGetInterface(Service* srv_out, u64 cmd_id) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(&g_nsGetterSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(srv_out, r.Handles[0]);
        }
    }

    return rc;
}



u64 DrawAppList() {
	Result rc=0;
	AppRecord *buf=NULL;
	size_t outsize=0;

	buf = (AppRecord*)malloc(sizeof(AppRecord));
	if (buf==NULL) {
		m_errorCode = 1;
		return 0;
	}
	memset(buf, 0, sizeof(AppRecord));

	rc = smGetService(&g_nsGetterSrv, "ns:am2");//TODO: Support the other services?(Only useful when ns:am2 isn't accessible)
	if (R_FAILED(rc)) return rc;

	rc = _nsGetInterface(&g_nsAppManSrv, 7996);
	int actualEntries = 0;
	u64 entries[100];
	char titles[100][100];

	for (int i = 0; i < 50; ++i) {
		rc = nssListApplicationRecord(i, buf, sizeof(AppRecord), &outsize);
		if (R_FAILED(rc)) {
			printf("nsListApplicationRecord: Failed\n");
		} else {

			if (buf->tid == 0) {
				break;
			}
			entries[i] = buf->tid;
			appInfo(buf->tid, titles[i]);
			actualEntries++;
		}
	}

    int idx = 0, cnt = 0;
    // int prev_idx = 0;

    consoleClear();

    while(1){
        // Draw the menu
        printf("\x1b[%dm\x1b[%d;%dH[%s]\n\n\x1b[0m", CYAN, 0, 0, "Installed titles list: ");

        for (cnt = 0; cnt < actualEntries; cnt++) {
            printf("\x1b[%dm %s - %016lx \x1b[0m\n", cnt == idx ? GREEN : WHITE,titles[cnt] , entries[cnt]);
            printf(" \n");
        }

        printf("\n");

        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_DDOWN && idx < (cnt - 1))
            idx++;
        else if (kDown & KEY_DDOWN && idx == (cnt - 1))
            idx = 0;
        if (kDown & KEY_DUP && idx > 0)
            idx--;
        else if (kDown & KEY_DUP && idx == 0)
            idx = cnt - 1;
        if (kDown & KEY_A) {
            //printf("Before return %016lx, idx: %d\n", entries[idx], idx);
            //selectedTid = entries[idx];
            return (u64)entries[idx];
            consoleClear();
        }

        //gfxFlushBuffers();
        //gfxSwapBuffers();
        //gfxWaitForVsync();
    }
}

u64 DrawBackupList(char *folder) {
	// int actualEntries = 0;
	char titles[100][100];

    // int len;
    struct dirent *pDirent;
    DIR *pDir;
    pDir = opendir ("sdmc:/backups");
	if (pDir == NULL) {
		printf ("Cannot open directory\n");
		return 0;
	}

	int i = 0;
	while ((pDirent = readdir(pDir)) != NULL) {
		//printf ("[%s]\n", pDirent->d_name);

		memset(titles[i], 0, 100);
		strncpy(titles[i], pDirent->d_name, 100-1);
		i++;
	}
	closedir (pDir);

    int idx = 0, cnt = 0;
    // int prev_idx = 0;

    consoleClear();

    while(1){
        // Draw the menu
        printf("\x1b[%dm\x1b[%d;%dH[%s]\n\n\x1b[0m", CYAN, 0, 0, "Backups: ");

        for (cnt = 0; cnt < i; cnt++) {
            printf("\x1b[%dm %s \x1b[0m\n", cnt == idx ? GREEN : WHITE,titles[cnt]);
            printf(" \n");
        }

        printf("\n");

        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_DDOWN && idx < (cnt - 1))
            idx++;
        else if (kDown & KEY_DDOWN && idx == (cnt - 1))
            idx = 0;
        if (kDown & KEY_DUP && idx > 0)
            idx--;
        else if (kDown & KEY_DUP && idx == 0)
            idx = cnt - 1;
        if (kDown & KEY_A)
        {
            memset(folder, 0, 100);
            strncpy(folder, titles[idx], 100-1);
            consoleClear();
            return 0;
        }

        //gfxFlushBuffers();
        //gfxSwapBuffers();
        //gfxWaitForVsync();
    }
}
