#ifndef __BESTFIT_H__
#define __BESTFIT_H__

#include <stddef.h>

/* die Groesse des Speicherpools in Bytes */
#define MEM_POOL_SIZE      ((unsigned) (1024*12)) /* 12 Kilobyte */

/* 1 Bit in der Freispeicher-Bitliste repraesentiert 32 Bytes im Speicherpool */
#define CHUNK_SIZE         32

/* Funktionsdeklarationen (Modulschnittstelle): */

void *
bf_alloc(size_t size);

void
bf_free(void *ptr, size_t size);

#endif /* __BESTFIT_H__ */
