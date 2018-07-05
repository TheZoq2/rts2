#ifndef H_UTIL
#define H_UTIL

#define validate_fifo(fifo, name) if(fifo->validate() != 0) {printf("Fifo %s invalid\n", name);}

#endif
