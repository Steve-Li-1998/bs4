#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bestfit.h"
#include <errno.h>


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

	size_t neededChunks = size_to_chunks(size);

    unsigned bestFitIndex;
    size_t bestFitGapSize = MEM_POOL_SIZE / CHUNK_SIZE + 1;
    size_t currentGapSize = 0;
    int bestFitGapDetected = 0;
    // Falls nicht 0, ist mindestens eine passende luecke gefunden
    for (unsigned i = 0; i < MEM_POOL_SIZE / CHUNK_SIZE; ++i) {
        if (!bit_is_set(free_list, i)){
            currentGapSize++;
        } else {
            if (currentGapSize >= neededChunks){
                bestFitGapDetected++;
                if (currentGapSize < bestFitGapSize){
                    bestFitGapSize = currentGapSize;
                    bestFitIndex = i - currentGapSize;
                }
            }
            currentGapSize = 0;
        }
    }
    if(currentGapSize >= neededChunks){
        bestFitGapDetected++;
       if (currentGapSize < bestFitGapSize){
           bestFitIndex = MEM_POOL_SIZE / CHUNK_SIZE - currentGapSize;
           bestFitGapDetected++;
       }
    }
    if (bestFitGapDetected){
        for (unsigned i = 0; i < neededChunks; ++i) {
            set_bit(free_list, bestFitIndex + i);
        }
    } else{
        errno = ENOMEM;
        return NULL;
    }

    dump_free_mem();
    return mem_pool + bestFitIndex * CHUNK_SIZE;



}










void
bf_free(void *ptr, size_t size)
{
    unsigned target = ((char*)ptr - mem_pool) / CHUNK_SIZE;
    size_t freeSize = size_to_chunks(size);
    for (unsigned i = 0; i < freeSize; ++i) {
        clear_bit(free_list, target + i);
    }

    dump_free_mem();
}
