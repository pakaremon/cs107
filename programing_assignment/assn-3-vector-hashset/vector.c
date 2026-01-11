#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

static const int kInitialAllocation = 4;
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(elemSize > 0);
  assert(initialAllocation >= 0);
  v->elemSize = elemSize;
  v->logLength = 0;
  if (initialAllocation == 0)
    initialAllocation = kInitialAllocation;
  v->allocLength = initialAllocation;
  v->freeFn = freeFn;
  v->initAlloc = initialAllocation;
  v->elems = malloc(initialAllocation * elemSize);
  assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
  if (v->freeFn != NULL) {
    for (int i = 0; i < v->logLength; i++)
      v->freeFn((char *) v->elems + i * v->elemSize);
  }

  free(v->elems);
}

int VectorLength(const vector *v)
{
  return v->logLength;
}

void *VectorNth(const vector *v, int position)
{
  assert(position >= 0 && position < v->logLength);
  return (char *) v->elems + position * v->elemSize;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0 && position < v->logLength);
  void *destAddr = (char *) v->elems + position * v->elemSize;
  if (v->freeFn != NULL)
    v->freeFn(destAddr);
  memcpy(destAddr, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0 && position <= v->logLength);
  if (v->logLength == v->allocLength) {
    v->allocLength += v->initAlloc;
    v->elems = realloc(v->elems, v->allocLength * v->elemSize);
    assert(v->elems != NULL);
  }
  int endSize = (v->logLength - position) * v->elemSize;
  void *destAddr = (char *) v->elems + position * v->elemSize;
  if (endSize != 0)
    memmove((char *) destAddr + v->elemSize, destAddr, endSize);
  memcpy(destAddr, elemAddr, v->elemSize);
  v->logLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
  if (v->logLength == v->allocLength) {
    v->allocLength += v->initAlloc;
    v->elems = realloc(v->elems, v->allocLength * v->elemSize);
    assert(v->elems != NULL);
  }

  void *destAddr = (char *) v->elems + v->logLength * v->elemSize;
  memcpy(destAddr, elemAddr, v->elemSize);
  v->logLength++;
}

void VectorDelete(vector *v, int position)
{
  assert(position >= 0 && position < v->logLength);
  void *destAddr = (char *) v->elems + position * v->elemSize;
  if (v->freeFn != NULL)
    v->freeFn(destAddr);
  int endSize = (v->logLength - position) * v->elemSize;
  if (endSize != 0)
    memmove(destAddr, (char *) destAddr + v->elemSize, endSize);
  v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  assert(compare != NULL);
  qsort(v->elems, v->logLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  assert(mapFn != NULL);
  for (int i = 0; i < v->logLength; i++)
    mapFn((char *) v->elems + i * v->elemSize, auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
  assert(startIndex >= 0 && startIndex <= v->logLength);
  assert(searchFn != NULL);
  assert(key != NULL);

  void *found;
  size_t logLength = (size_t) v->logLength;
  if (isSorted)
    found = bsearch(key, v->elems, logLength, v->elemSize, searchFn);
  else
    found = lfind(key, v->elems, &logLength, v->elemSize, searchFn);

  if (found == NULL)
    return kNotFound;
  return ((char *) found - (char *) v->elems) / v->elemSize;
} 
