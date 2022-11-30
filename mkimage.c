#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>

#include <redconf.h>
#include <redtypes.h>
#include <redmacs.h>
#include <redvolume.h>
#include <redposix.h>

const VOLCONF gaRedVolConf[REDCONF_VOLUME_COUNT] =
{
    { 4096U, 2048U, false, 256U, 0U, "" }
};


typedef int (*actionFunc)(const char *src, const char *dst);

int scanDir(const char *src, const char *dst, actionFunc action)
{
	DIR *currDir;
	struct dirent *file;
	printf("scan source %s\n", src);
	currDir = opendir(src);
	if (!currDir)
	{
		printf("open path %s error %d\n", src, errno);
		return -1;
	}
	printf("make dest dir %s\n", dst);
	red_mkdir(dst);	
	while((file = readdir(currDir)) != NULL)
	{
		char *newSrc;
		char *newDst;
		if (strncmp(file->d_name, ".", 1) == 0)
			continue;
		newSrc = (char*)malloc(strlen(src) + strlen(file->d_name) + 2);
		newDst = (char*)malloc(strlen(dst) + strlen(file->d_name) + 2);
		assert(newSrc != NULL);
		assert(newDst != NULL);

		sprintf(newSrc, "%s/%s", src, file->d_name);
		sprintf(newDst, "%s/%s", dst, file->d_name);
		if (file->d_type == DT_DIR)
		{
			scanDir(newSrc, newDst, action);
		}
		else
		{
			action(newSrc, newDst);
		}

		free(newSrc);
		free(newDst);
	}
	closedir(currDir);
	return 0;
}

int copyfile(const char *src, const char *dst)
{
	FILE *sourceFp;
	int32_t dstFd;
	char buff[4096];
	sourceFp = fopen(src, "rb");
	if (sourceFp != NULL)
	{
		dstFd = red_open(dst, RED_O_CREAT | RED_O_TRUNC | RED_O_WRONLY);
		if (dstFd != -1)
		{
			int ret = 0;
			do{
				ret = fread(buff, 1, sizeof(buff), sourceFp);
				if (ret > 0)
				{
					red_write(dstFd, buff, ret);
				}
				else
				{
					break;
				}
			}while(true);
			red_close(dstFd);
		}
		else
		{
			printf("open dst %s failed\n", dst);
		}
		fclose(sourceFp);
	}
	else
	{
		printf("open %s failed err:%d\n", src, errno);
	}
	printf("create file %s\n", dst);
	return 0;
}

#define volname ""

void initfs()
{
	uint32_t lstate;
	lstate = red_init();
	if (lstate != 0)
	{
		printf("red_init failed\n");
		return;
	}
	lstate = red_format(volname);
	if (lstate != 0)
	{
		printf("red_format failed\n");
		return;
	}
	lstate = red_mount(volname);
	if (lstate != 0)
        {
                printf("red_mount failed\n");
		return;
        }
	printf("init fs ok\n");
}

extern void GetImageInfo(void **imgBuf, uint32_t *imgSize);
int exportImage()
{
	char *imgBuf;
	uint32_t imgSize;
	FILE *imgFp = fopen("./udisk.bin", "wb");
	if (imgFp != NULL){
		printf("generating image...\n");
		fseek(imgFp, 0L, SEEK_SET);
		GetImageInfo((void**)&imgBuf, &imgSize);
		fwrite(imgBuf, 1, imgSize, imgFp);
		fclose(imgFp);
	}

}

int main(int argc, char **argv)
{
	initfs();
	scanDir(argv[1], "", copyfile);
	printf("scanning file over...\n");
	return exportImage();
}
