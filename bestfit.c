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
	if(size == 0){
		return NULL;
	}
	
	size_t chunk_index = 0;
	size_t chunks_needed = size_to_chunks(size);
	
	while(chunk_index < CHUNK_SIZE){
		if(free_list[chunk_index].status==0 && size_to_chunks(chunk_index)>=chunks_needed){
            break;
		}
		chunk_index++;
	}
	if(chunk_index >= CHUNK_SIZE){
        return NULL;
	}

	size_t reference_chunk_index = chunk_index;

    while (reference_chunk_index < CHUNK_SIZE){
        if(free_list[reference_chunk_index].status==0
        && size_to_chunks(reference_chunk_index)>=chunks_needed
        && size_to_chunks(reference_chunk_index)< size_to_chunks(chunk_index))
            chunk_index = reference_chunk_index;
            reference_chunk_index ++;
    }

    //size_t oldLength = free_list[chunk_index].length;
    size_t oldLength = size_to_chunks(chunk_index);

    free_list[chunk_index].status = ;
    size_to_chunks(chunk_index) = chunks_needed;

    if(oldLength > chunks_needed){
        free_list[chunk_index + chunks_needed].status = 0;
        size_to_chunks(chunk_index + chunks_needed) = oldLength - chunks_needed;
        return chunk_index * CHUNK_SIZE;
    }
	
}

void
bf_free(void *ptr, size_t size)
{
	/* HIER MUESST IHR EUREN CODE EINFUEGEN! */

	dump_free_mem();
}
