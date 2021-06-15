#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
  
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE 2097152UL
#define MAP_MASK (MAP_SIZE - 1)

int main(int argc, char **argv) {
    int fd;
    void *map_base, *virt_addr; 
	unsigned long read_result, writeval;
	off_t target = 0x70000000;
	int access_type = 'w';
	
    if(argc < 2)
    {
        printf("USAGE: ./PROGRAM SIZE OUTPUT_NAME\n");
    }

	int size = atol(argv[1]);
    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    printf("/dev/mem opened.\n"); 
    fflush(stdout);
    
    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    fflush(stdout);
    
    virt_addr = map_base + (target & MAP_MASK);
	FILE* pFile;
	pFile = fopen(argv[2],"wb");
    fwrite(virt_addr, size, 1, pFile);
	fclose(pFile);

    printf("Value at address 0x%X (%p): 0x%X\n", target, virt_addr, read_result); 
    fflush(stdout);

	
	
	if(munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
    return 0;
}
