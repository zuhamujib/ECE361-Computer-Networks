#ifndef DELIVER_H
#define DELIVER_H

extern int NUM_FRAGMENTS = 1000;

struct packet {
    unsigned int total_frag;    // num of fragments in file
    unsigned int frag_no;   // sequence num of fragment
    unsigned int size;  // size of the data ranging 0 - 1000
    char* filename;
    char filedata[1000];
}


#endif