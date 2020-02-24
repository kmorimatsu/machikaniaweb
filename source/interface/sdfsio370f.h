/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20

typedef struct
{
    unsigned long seek;
    unsigned long size;
    unsigned char flags;
    char          name[11];
} FSFILE;
typedef struct
{
    unsigned long filesize;
    unsigned char attributes;   // 0 or ATTR_DIRECTORY(0x10)
    char          filename[13];
} SearchRec;

int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec);
int FindNext (SearchRec * rec);
int FSmkdir (char * path);
char * FSgetcwd (char * path, int numbchars);
int FSchdir (char * path);
size_t FSfwrite(const void *data_to_write, size_t size, size_t n, FSFILE *stream);
int FSremove (const char * fileName);
int FSrename (const char * fileName, FSFILE * fo);
int FSfeof( FSFILE * stream );
long FSftell(FSFILE *fo);
int FSfseek(FSFILE *stream, long offset, int whence);
size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream);
void FSrewind (FSFILE *fo);
int FSfclose(FSFILE *fo);
FSFILE * FSfopen(const char * fileName, const char *mode);
int FSInit(void);
