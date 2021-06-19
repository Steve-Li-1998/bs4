#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "bestfit.h"

#define DEFAULT_NUM_AREAS	50
#define MIN_AREA_SIZE		(2*sizeof(unsigned))
#define MAX_AREA_SIZE		100

typedef struct {
	void *ptr;
	unsigned size;
} SAllocInfo;

static void checked_bfalloc(SAllocInfo *pai /*in-out*/)
{
	assert(pai != NULL && "TEST FEHLGESCHLAGEN! Ungueltiges Argument (null-ptr)!");
	assert(pai->size >= MIN_AREA_SIZE && "TEST FEHLGESCHLAGEN! Ungueltige Speicheranfrage (< MIN_AREA_SIZE)!");
	
	printf("\nAufruf von bf_alloc(%u)\n", pai->size);
	pai->ptr = bf_alloc(pai->size);
	if (!pai->ptr) {
		fprintf(stderr, "TEST FEHLGESCHLAGEN! NULL-Pointer %p wurde zurueckgegeben von bf_alloc(%u)!\n", pai->ptr, pai->size);
		exit(1);
	}
	printf("\nPointer %p bekommen von bf_alloc(%u)\n", pai->ptr, pai->size);
	
	/* Größe an den Anfang und Größe*2 ans Ende */
	*((unsigned *) pai->ptr) = pai->size;
	*(((unsigned *) pai->ptr) + (pai->size - sizeof(unsigned)) / sizeof(unsigned)) = pai->size * 2;
}

static void checked_bffree(SAllocInfo *ai /*in*/)
{
	unsigned sizeStored, doubledSize;

	assert(ai->ptr != NULL && "TEST FEHLGESCHLAGEN! Ungueltiges Argument (null-ptr)!");

	printf("\nPruefe Speicherinhalt an Pointer %p mit der Laenge %u...", ai->ptr, ai->size);

	sizeStored = *((unsigned *)ai->ptr);

	if (sizeStored != ai->size) {
		fprintf(stderr, "\nTEST FEHLGESCHLAGEN! Speicherinhalt beschaedigt (%p: %u)!\nVermutlich liegt ein Fehler in der Berechnung der Speicheradresse vor oder der Speicher wurde zweimal vergeben.\n", ai->ptr, ai->size);
		abort();
	}
	doubledSize = *(((unsigned *) ai->ptr) + (ai->size - sizeof(unsigned)) / sizeof(unsigned));
	if (doubledSize != ai->size * 2) {
		fprintf(stderr, "\nTEST FEHLGESCHLAGEN! Speicherinhalt beschaedigt (%p: %u)!\nVermutlich liegt ein Fehler in der Berechnung der Speicheradresse vor oder der Speicher wurde zweimal vergeben.\n", ai->ptr, ai->size);
		abort();
	}
	printf(" OK\n");
	printf("\nAufruf von bf_free(%p, %u)\n", ai->ptr, ai->size);
	bf_free(ai->ptr, ai->size);
}

int main(int argc, char **argv)
{
	SAllocInfo ai[] = {
		{ NULL, 0.75 * MEM_POOL_SIZE },
		{ NULL, CHUNK_SIZE },
		{ NULL, 0.75 * MEM_POOL_SIZE - 0.125 * MEM_POOL_SIZE },
		{ NULL, CHUNK_SIZE * 2 },
		{ NULL, 0.125 * MEM_POOL_SIZE + 1 }
	};
	SAllocInfo ai2[] = {
		{ NULL, 0.5 * MEM_POOL_SIZE },
		{ NULL, CHUNK_SIZE },
		{ NULL, 0.4 * MEM_POOL_SIZE }
	};
	SAllocInfo *pData;
	void *ptr;
	unsigned num_areas, allocated_areas;
	unsigned i, repetitions;

	/* Parameter vorhanden? */
	if (argc < 2) {
		/* nein -> Defaultwert */
		num_areas = DEFAULT_NUM_AREAS;
	} else {
		/* ja -> nach unsigned long konvertieren */
		char *endptr;
		num_areas = strtoul(argv[1], &endptr, 0);
		if (endptr == argv[1] || *endptr) {
			fprintf(stderr, "ungueltiger Parameter\n");
			return 1;
		}
	}

	printf("Anzahl Speicherbereiche: %u\n", num_areas);

	/* Speicher fuer Zeigerarray besorgen */
	pData = malloc(num_areas * sizeof(SAllocInfo));

	if (!pData) {
		fprintf(stderr, "Speicher fuer Zeigerarray konnte nicht alloziert werden (malloc).\n");
		return 1;
	}

	srand(time(NULL));

	/* Testet die Funktionen bf_alloc/bf_free bzw. das BestFit-Verfahren. */
	for (repetitions = 0; repetitions < 2; repetitions++) { /* => alles 2x */
		for (i = 0; i < num_areas; i++) {
			pData[i].ptr = NULL;
			pData[i].size = MIN_AREA_SIZE + rand() % (MAX_AREA_SIZE-MIN_AREA_SIZE+1);
			printf("Zufallsanfrage: ");
			checked_bfalloc(pData + i);
		}

		allocated_areas = i;

		/* alle durch 3 teilbaren freigeben */
		for (i = 0; i < allocated_areas; i += 3) {
			checked_bffree(&pData[i]);
		}
		/* die restlichen freigeben */
		for (i = 0; i < allocated_areas; ++i) {
			if ((i % 3) == 0) continue;
			checked_bffree(&pData[i]);
		}

		/* Die Einrückung kennzeichnet die paarweisen Aufrufe von alloc/free. */
		printf("Anfrage 0.75 * MEM_POOL_SIZE");
		checked_bfalloc(&ai[0]);
		  printf("Anfrage 1 CHUNK");
		  checked_bfalloc(&ai[1]);
		printf("Freigabe 0.75 * MEM_POOL_SIZE");
		checked_bffree(&ai[0]);
		    printf("Anfrage 0.75 * MEM_POOL_SIZE - 0.125 * MEM_POOL_SIZE");
		    checked_bfalloc(&ai[2]);
			  printf("Anfrage 2 CHUNKs");
		      checked_bfalloc(&ai[3]);
			printf("Freigabe 0.75 * MEM_POOL_SIZE - 0.125 * MEM_POOL_SIZE");
		    checked_bffree(&ai[2]);
			    printf("Anfrage 0.125 * MEM_POOL_SIZE + 1");
		        checked_bfalloc(&ai[4]);
		  printf("Freigabe 1 CHUNK");
		  checked_bffree(&ai[1]);
		      printf("Freigabe 2 CHUNKs");
		      checked_bffree(&ai[3]);
			    printf("Freigabe 0.125 * MEM_POOL_SIZE + 1");
		        checked_bffree(&ai[4]);
	}
	free(pData);


    /*
     * Ein weiterer Test für die Funktionen bf_alloc/bf_free bzw. das BestFit-Verfahren.
     * Die Einrückung kennzeichnet die paarweisen Aufrufe von alloc/free.
     */
    printf("Anfrage 0.5 * MEM_POOL_SIZE");
    checked_bfalloc(&ai2[0]);
        printf("Anfrage 1 CHUNK");
        checked_bfalloc(&ai2[1]);
    printf("Freigabe 0.5 * MEM_POOL_SIZE");
    checked_bffree(&ai2[0]);
            printf("Anfrage 0.4 * MEM_POOL_SIZE");
            checked_bfalloc(&ai2[2]);
                printf("Anfrage 0.5 * MEM_POOL_SIZE");
                checked_bfalloc(&ai2[0]);
        printf("Freigabe 1 CHUNK");
        checked_bffree(&ai2[1]);
            printf("Freigabe 0.4 * MEM_POOL_SIZE");
            checked_bffree(&ai2[2]);
                printf("Freigabe 0.5 * MEM_POOL_SIZE");
                checked_bffree(&ai2[0]);


	/* Wir sind sehr Speicher-hungrig ... (Maximalanforderung)! */
	printf("\nAufruf von bf_alloc(%u) (Maximalanforderung)\n", MEM_POOL_SIZE);
	ptr = bf_alloc(MEM_POOL_SIZE);
	if (ptr == NULL) {
		fprintf(stderr, "TEST FEHLGESCHLAGEN! Da ging wohl was schief (Anforderung mit: %u bytes)!", MEM_POOL_SIZE);
		abort();
		return 1;
	}
	printf("\nPointer %p bekommen von bf_alloc(%u)\n", ptr, MEM_POOL_SIZE);
	
	printf("\nAufruf von bf_free(%p, %u)\n", ptr, MEM_POOL_SIZE);
	bf_free(ptr, MEM_POOL_SIZE);
	
	printf("\nAufruf von bf_alloc(%u) (Testweise mehr Speicher anfragen, als zur Verfuegung steht)\n", MEM_POOL_SIZE+1);
	if (bf_alloc(MEM_POOL_SIZE+1) != NULL) {
		fprintf(stderr, "TEST FEHLGESCHLAGEN! Speicherbereich (Memory-Pool) verlassen!\nEs wurde von bf_alloc mehr Speicher zugeteilt, als eigentlich zur Verfuegung steht.");
		abort();
		return 1;
	}
	printf("\nAufruf von bf_alloc(%u) wurde erfolgreich abgewiesen.\n", MEM_POOL_SIZE+1);
	
/*
 * Diesen Code koennt ihr zum Testen des finalen Falles "Speicher voll"
 * einschalten (indem ihr #if 0 durch #if 1 ersetzt):
 */
#if 0
	printf("\nSpeicher voll Test\n");
	while (1) {
		void *ptr;
		printf("\nAufruf von bf_alloc(%u)\n", MAX_AREA_SIZE);
		if ((ptr = bf_alloc(MAX_AREA_SIZE))) {
			memset(ptr, 42, MAX_AREA_SIZE);
			continue;
		}
		fprintf(stderr, "\nSpeicher voll Test erfolgreich\n");
		break;
	}
#endif
	printf("\nTEST ERFOLGREICH BEENDET!\n\n");
	return 0;
}
