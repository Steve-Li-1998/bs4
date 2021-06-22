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



    size_t closest = 0; size_t closestIndex = 0;
    size_t i;
    for (i = 0; i < CHUNK_SIZE; i++) {

        size_t thisChunkSize = 0;
        if(bit_is_set(free_list, i)==0){    //wenn auf Position i frei ist
            for (int j = i; j < CHUNK_SIZE && (bit_is_set(free_list, j)==0); j++) {      //rechnen dann die Laenge des freien Platz
                thisChunkSize++;
            }

            //jetzte size(thisChunkSize) - brauchte size kleiner als brauchte size - closest size: jetzte size ist besser
            if( closest == 0 || abs(needChunks-closest)>abs(thisChunkSize-needChunks) ){
                closest = thisChunkSize;
                closestIndex = i;
            }
        }
    }
    return mem_pool + closestIndex * size;

}




void
bf_free(void *ptr, size_t size)
{
	/* HIER MUESST IHR EUREN CODE EINFUEGEN! */

	dump_free_mem();
}
