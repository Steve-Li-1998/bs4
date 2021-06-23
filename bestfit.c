#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bestfit.h"


/* Im Header sind MEM_POOL_SIZE und CHUNK_SIZE definiert*/

/* der Speicherpool */
static char mem_pool[MEM_POOL_SIZE];

/* die Bitliste, die anzeigt, welche Bloecke frei sind: 1 Bit fuer jew. CHUNK_SIZE Bytes */
static unsigned char free_list[MEM_POOL_SIZE / CHUNK_SIZE / 8];

/* Anzahl der Bits in free_list */
#define FREELIST_BIT_COUNT (sizeof(free_list) * 8)




/* ----------------------------- Hilfsfunktionen ----------------------------- */
/* Funktion zum Debuggen, die die Freispeicher-Bitliste ausgibt */
#ifdef DEBUG
void
dump_free_mem(void)
{
	int i;

	fprintf(stderr, "Freispeicherliste: ");
	for (i = 0; i < sizeof(free_list); ++i) {
		if ((i % 32) == 0) {
			fprintf(stderr, "\n%03X: ", i);
		}
		fprintf(stderr, "%02X ", free_list[i]);
	}
	fprintf(stderr, "\n");
}
#else
#  define dump_free_mem()
#endif




/* rechnet aus, wie viele Chunks fuer 'size' Bytes belegt werden muessen
   (rundet automatisch auf) */
size_t
size_to_chunks(size_t size)
{
	if ((size % CHUNK_SIZE) != 0) {
		return size / CHUNK_SIZE + 1;
	} else {
		return size / CHUNK_SIZE;
	}
}




/* testet, ob im Bitfeld bitfield das Bit chunknr gesetzt ist:
 * liefert 0, wenn es nicht gesetzt ist, sonst eine Zahl > 0 */
int
bit_is_set(const unsigned char *bitfield, unsigned chunknr)
{
	int shift = (7 - (chunknr % 8));
	char bitMask = 1 << shift;
	int arrayPosition = chunknr/8;		
	return (bitfield[arrayPosition] & bitMask);
}


/* setzt im Bitfeld bitfield das Bit mit der Nummer chunknr */
void
set_bit(unsigned char *bitfield, unsigned chunknr)
{
	int shift = (7 - (chunknr % 8));
	char bitMask = 1 << shift;
	int arrayPosition = chunknr/8;	
	bitfield[arrayPosition] |= bitMask;
}


/* loescht im Bitfeld bitfield das Bit mit der Nummer chunknr */
void
clear_bit(unsigned char *bitfield, unsigned chunknr)
{
	int shift = (7 - (chunknr % 8));
	char bitMask = ~(1 << shift);
	int arrayPosition = chunknr/8;
	bitfield[arrayPosition] &= bitMask;
}








/* ----------------------------- Speicherverwaltung ----------------------------- */

void *
bf_alloc(size_t size)
{
    //wenn size gleich 0, return NULL
	if(size == 0){
		return NULL;
	}

	size_t needChunks = size_to_chunks(size);

    /*size_t closest = 0; size_t closestIndex = 0;
    size_t i = 0;
    for (; i < CHUNK_SIZE*8; ++i) {

        size_t thisChunkSize = 0;
        if(bit_is_set(free_list, i)==0){    //wenn auf Position i frei ist
            for (size_t j = i; (j < CHUNK_SIZE*8) && (bit_is_set(free_list, j)==0); ++j) {      //rechnen dann die Laenge des freien Platz
                ++thisChunkSize;
            }

            //get absolutewert
            size_t a = 0; size_t b = 0;
            if(thisChunkSize > needChunks){
                a = thisChunkSize - needChunks;
            }
            else{
                continue;
                //a = needChunks - thisChunkSize;
            }
            if(needChunks > closest){
                b = needChunks - closest;
            }
            else{
                b = closest - needChunks;
            }

            //jetzte size(thisChunkSize) - brauchte size kleiner als brauchte size - closest size: jetzte size ist besser
            if( closest == 0 || a < b ){
                closest = thisChunkSize;
                closestIndex = i;
            }
        }
    }

    for (int j = 0; j < closest-1; ++j) {
        set_bit(free_list, closestIndex);
    }
    //printf("%zu\n", closestIndex);
    //printf("%zu\n", needChunks);
    dump_free_mem();
    return mem_pool + closestIndex * CHUNK_SIZE;*/




    size_t closest = 0; size_t closestIndex = 0;

    size_t i = 0;
    for (; i < CHUNK_SIZE*8; ++i) {

        size_t thisSize = 0;

        if(bit_is_set(free_list, i)==0) {    //wenn auf Position i frei ist
            for (size_t j = i; (j < CHUNK_SIZE*8) && (bit_is_set(free_list, j)==0); ++j) {      //rechnen dann die Laenge des freien Platz
                ++thisSize;
            }

            if(needChunks > thisSize){  //kein genug Platz
                continue;
            }
            else if(needChunks == thisSize){    //Bestcase: fit it
                closest = thisSize;
                closestIndex = i;
                break;
            }
            else{   //needChunks < thisSize
                if(closest == 0 || thisSize - needChunks > abs(thisSize - closest)){
                    closest = thisSize;
                    closestIndex = i;
                }
            }
        }
    }

    for (int j = 0; j < closest; ++j) {
        set_bit(free_list, closestIndex);
    }

    printf("%zu\n", closestIndex);
    printf("%zu\n", closest);
    printf("%zu\n", needChunks);
    dump_free_mem();
    return mem_pool + closestIndex * CHUNK_SIZE;



}










void
bf_free(void *ptr, size_t size)
{
    size_t chunkIndex = ((char*)ptr - mem_pool) / CHUNK_SIZE;


    for (int i = 0; i < size_to_chunks(size); ++i) {
        if(bit_is_set(free_list, chunkIndex)==1){
            clear_bit(free_list, chunkIndex);
        }
        ++chunkIndex;
    }
    dump_free_mem();
}
