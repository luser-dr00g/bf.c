#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

enum { MEMSIZE = 30000 };

//unsigned char *mem;
unsigned char mem[30000];
unsigned char *ptr;
unsigned char *prog;
size_t progsize;

int init(char *progname) {
    int f,r;
    struct stat fs;
    ptr = mem;// = calloc(MEMSIZE, 1);
    f = open(progname, O_RDONLY);
    assert(f != -1);
    r = fstat(f, &fs);
    assert(r == 0);
    prog = mmap(NULL, progsize = fs.st_size, PROT_READ, MAP_PRIVATE, f, 0);
    assert(prog != NULL);
    return 0;
}

int findmatch(int ip, char src){
    char *q, *p="[]";
    int i, defer, dir[]= { 1, -1 };
    i = strchr(p,src)-p;
    ip+=dir[i];
    for (defer=dir[i]; defer!=0; ip+=dir[i]) {
        if (ip<0||ip>=progsize) error("mismatch");
        if (q = strchr(p,prog[ip])) {
            int j = q-p;
            defer+=dir[j];
        }
    }
    return ip - dir[i];
}

int run() {
    int ip;
    for(ip = 0; ip>=0 && ip<progsize; ip++)
        switch(prog[ip]){
        case '>': ++ptr; break;
        case '<': --ptr; break;
        case '+': ++*ptr; break;
        case '-': --*ptr; break;
        case '.': putchar(*ptr); break;
        case ',': *ptr=getchar(); break;
        case '[': /*while(*ptr){*/
                  if (!*ptr)
                      ip=findmatch(ip,'[')-1;
                  break;
        case ']': /*}*/
                  if (*ptr)
                      ip=findmatch(ip,']')-1;
                  break;
        }

    return 0;
}

int cleanup() {
    //free(mem);
    ptr = NULL;
    return 0;
}

int main(int argc, char *argv[]) {
    init(argc > 1? argv[1]: NULL);
    run();
    cleanup();
    return 0;
}

