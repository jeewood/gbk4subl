#include <stdio.h>
#include <stdlib.h>

//内在查找函数
long memstr(char* buf,char *sr,long bufsize,int srsize)
{
	char *pb = buf;
	long j = 0,i;
	for(i=0;i<bufsize;i++)
	{
		if (buf[i]==sr[0])
		{
			for(j=1;j<srsize;j++)
			{
				if (buf[j+i]!=sr[j]) 
				{
					j=-1;
					break;
				}
			}
			if (j==srsize) 
				return j+i-1;
		}
	}
	return -1;
}

char fexists(const char *filename)
{
	FILE* fp = fopen(filename,"rb");
	if (0==fp) 
	{
		return 0;
	}
	fclose(fp);
	return 1;
}

char* catmem(const char* bs,const char* es,int bsize,int esize)
{
	char* ret = (char*)malloc((bsize+esize+1)*sizeof(char));
	char* p;
	int i;
	p = ret;
	for(i=0;i<bsize;i++)
	{
		*p++ = *bs++;
	}
	for(i=0;i<esize;i++)
	{
		*p++ = *es++;
	}
	*p = 0;
	return ret;
}

char PatchFile(const char *filename,char* sr,int srsize)
{
	long size,pos;
	FILE *fp;
	char *buf;
	//打开文件
	fp = fopen(filename,"rb");
	if (0==fp) return 2;
    fseek (fp, 0, SEEK_END);   // non-portable
    size=ftell (fp);
    rewind(fp);
    buf = (char*)malloc(size * sizeof(char));
    if (0==buf) return 3;
 	if (fread (buf,1,size,fp)!=size)
 	{
 		free(buf);
 		return 4;
 	}
    fclose (fp);
    fp = 0;

    char* bakfilename = catmem(filename,".bak",strlen(filename),4);
	if (!fexists(bakfilename))
	{
		fp = fopen(bakfilename,"wb");
		if (0==fp) return 2;
	    if (fwrite(buf,sizeof(char),size,fp)!=size) return 13;
	    fclose(fp);
	    fp = 0;
	}
	free(bakfilename);
    
    pos = memstr(buf,sr,size,7);
    if (pos==-1) return 5;

    buf[pos] = 0x90;

    fp = fopen(filename,"wb");
    if (0==fp) return 12;

    if (fwrite(buf,sizeof(char),size,fp)!=size) return 13;
    return 0;
}

int main()
{
    char *sr = "\x85\xC0\x74\x08\x33\xDB\x43";
	char result = PatchFile("sublime_text.exe",sr,7);
	if (result==0)
	{
		printf("Patch successfully\n");
		return 0;
	}

	printf("Patch failed!\n");
	return result;
}