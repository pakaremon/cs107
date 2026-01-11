#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
                HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
  assert(elemSize > 0);
  assert(numBuckets > 0);
  assert(hashfn != NULL);
  assert(comparefn != NULL);
  h->numBuckets = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->buckets = malloc(numBuckets * sizeof(vector));
  h->count = 0;

  // initialize buckets
  for (int i = 0; i < numBuckets; i++)
    VectorNew(&h->buckets[i], elemSize, freefn, 4);
}

void HashSetDispose(hashset *h)
{
  for (int i = 0; i < h->numBuckets; i++)
    VectorDispose(&h->buckets[i]);
  free(h->buckets);
}

int HashSetCount(const hashset *h)
{
  return h->count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
  assert(mapfn != NULL);
  for (int i = 0; i < h->numBuckets; i++)
    VectorMap(&h->buckets[i], mapfn, auxData);
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
  assert(elemAddr != NULL);
  int hashCode = h->hashfn(elemAddr, h->numBuckets);
  assert(hashCode >= 0 && hashCode < h->numBuckets);

  int position = VectorSearch(&h->buckets[hashCode], elemAddr, h->comparefn, 0,
                              false);
  if (position == -1) { // if not found
    VectorAppend(&h->buckets[hashCode], elemAddr);
    h->count++;
  } else {
    VectorReplace(&h->buckets[hashCode], elemAddr, position);
  }
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
  assert(elemAddr != NULL);
  int hashCode = h->hashfn(elemAddr, h->numBuckets);
  assert(hashCode >= 0 && hashCode < h->numBuckets);

  int position = VectorSearch(&h->buckets[hashCode], elemAddr, h->comparefn, 0,
                              false);
  if (position != -1) // if found
    return VectorNth(&h->buckets[hashCode], position);
  return NULL;
}
