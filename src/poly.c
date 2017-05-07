#include "poly.h"
#include <assert.h>
#include <stdio.h>


static inline Poly PolyFromCoeff(poly_coeff_t c)
{
    return (Poly){ .c = c, .monos = Lists.new() };
}

static inline Poly PolyZero()
{
    return PolyFromCoeff(0);
}

static inline Mono MonoFromCoeff(poly_coeff_t c, poly_exp_t e)
{
    return (Mono) { .exp = e, .p = PolyFromCoeff(c) };
}

static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e)
{
    return (Mono) { .exp = e, .p = *p };
}

static inline bool PolyIsCoeff(const Poly *p)
{
  assert(p != NULL);
  return Lists.empty(p->monos);
}

static inline bool PolyIsZero(const Poly *p)
{
  assert(p!=NULL);
  if(!PolyIsCoeff(p)) return false;
  return p->c;
}

static inline poly_coeff_t PolyGetConstTerm(const Poly* p)
{
  return p->c;
}

void PolyDestroy(Poly *p)
{
  if(p==NULL) return;
  loop_list(p->monos, i) {
    Mono* m = (Mono*) Lists.getValue(i);
    MonoDestroy(m);
  }
  Lists.free(p->monos);
  p->monos = NULL;
  free(p);
}

static inline void MonoDestroy(Mono *m)
{
  if(m==NULL) return;
  PolyDestroy(&(m->p));
  free(m);
}

void* polyCopier(void* data)
{
  Mono* mono = (Mono*) data;
  Mono* monoNew = (Mono*) malloc(sizeof(Mono));
  *monoNew = MonoClone(mono);
  return (void*) monoNew;
}

Poly PolyClone(const Poly *p)
{
  assert(p!=NULL);
  return (Poly) { .c = p->c, .monos = Lists.deepCopy(p->monos, polyCopier) };
}

static inline Mono MonoClone(const Mono *m)
{
  assert(m!=NULL);
  return (Mono) {.exp = m->exp, .p = PolyClone(&(m->p))};
}

static inline Mono* MonoClonePtr(const Mono *m)
{
  Mono* mono = malloc(sizeof(Mono));
  *mono = MonoClone(m);
  return mono;
}

/*
Mono MonoAdd(const Mono *p, const Mono *q)
{
   assert(p!=NULL);
   assert(q!=NULL);
   if(p->isCoeffSingle && !q->isCoeffSingle) {
     Mono m = MonoClone(q);
     listIterator imq = Lists.begin(m.coeffPoly);
     while(imq != NULL) {
       if(((Mono*)Lists.getValue(imq))->exp > 0) break;
       imq = Lists.next(imq);
     }
     Lists.insertElementAt(m.coeffPoly, imq, MonoClonePtr(p));
     return m;
   } else if(!p->isCoeffSingle && q->isCoeffSingle) {
     return MonoAdd(q, p);
   } else {
     Poly* polySum = malloc(sizeof(Poly));
     *polySum = PolyAdd(&(p->coeffPoly), &(q->coeffPoly));
     return MonoFromPoly( polySum, p->exp );
   }
}
*/

Poly PolyAdd(const Poly *p, const Poly *q)
{
  assert(p!=NULL);
  assert(q!=NULL);
  list result = Lists.new();

  listIterator iq = Lists.begin(q->monos);
  listIterator ip = Lists.begin(p->monos);
  while(ip != NULL || iq != NULL) {
    Mono* mp = (Mono*) Lists.getValue(ip);
    Mono* mq = (Mono*) Lists.getValue(iq);
    if(mp == NULL) {
      Lists.pushBack(result, MonoClonePtr(mq));
      iq = Lists.next(iq);
    } else if(mq == NULL) {
      Lists.pushBack(result, MonoClonePtr(mp));
      ip = Lists.next(ip);
    } else if(mp->exp > mq->exp) {
      Lists.pushBack(result, MonoClonePtr(mq));
      iq = Lists.next(iq);
    } else if(mp->exp < mq->exp) {
      Lists.pushBack(result, MonoClonePtr(mp));
      ip = Lists.next(ip);
    } else {
      Mono* partialResult = MonoClonePtr(mp);
      *partialResult = (Mono) { .exp = mp->exp, .p = PolyAdd(&(mp->p), &(mq->p)) };
      Lists.pushBack(result, partialResult);
      ip = Lists.next(ip);
      iq = Lists.next(iq);
    }
  }
  return (Poly) { .c = p->c + q->c, .monos = result };
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{

}

Poly PolyMul(const Poly *p, const Poly *q)
{

}

Poly PolyNeg(const Poly *p)
{

}

Poly PolySub(const Poly *p, const Poly *q)
{

}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{

}

poly_exp_t PolyDeg(const Poly *p)
{

}

bool PolyIsEq(const Poly *p, const Poly *q)
{

}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{

}

void PolyPrintSingleExp(char** wordAccumulator, int varid, poly_exp_t exp)
{
  if(exp == 0) {
    //DO NOTHING
  } else if(exp == 1) {
    *wordAccumulator += sprintf(*wordAccumulator, "[%d]", varid);
  } else {
    *wordAccumulator += sprintf(*wordAccumulator, "[%d]^%d", varid, (int)exp);
  }
}

void PolyPrintSingleWord(char** wordAccumulator, int varid, poly_exp_t exp, poly_coeff_t coeffAccumulator)
{

  if(coeffAccumulator == 0) {
    // DO NOTHING
  } else if(coeffAccumulator == 1) {
    PolyPrintSingleExp(wordAccumulator, varid, exp);
  } else if(coeffAccumulator < 0) {
    *wordAccumulator += sprintf(*wordAccumulator, "-%d", -coeffAccumulator);
    PolyPrintSingleExp(wordAccumulator, varid, exp);
  } else {
    *wordAccumulator += sprintf(*wordAccumulator, "+%d", coeffAccumulator);
    PolyPrintSingleExp(wordAccumulator, varid, exp);
  }
}

void PolyPrintRec(char** accumulator, char** wordAccumulatorBeg, char** wordAccumulator, poly_coeff_t coeffAccumulator, const Poly *p, int varid)
{
  if(p==NULL) return;
  if(PolyIsCoeff(p)) {
    PolyPrintSingleWord( wordAccumulator, varid, 0, coeffAccumulator*PolyGetConstTerm(p) );
    *accumulator += sprintf(*accumulator, "%s", *wordAccumulatorBeg);
  } else {
    loop_list(p->monos, i) {
      const Mono* m = (Mono*) Lists.getValue(i);
      char* wordAccumulatorCp = (char*) malloc(100*sizeof(char));
      char* wordAccumulatorCpBegin = wordAccumulatorCp;
      for(int i=0;i<100;++i) wordAccumulatorCp[i] = '\0';
      PolyPrintSingleExp(&wordAccumulatorCp, varid, m->exp);
      PolyPrintRec(accumulator, &wordAccumulatorCpBegin, &wordAccumulatorCp, coeffAccumulator, &(m->p), varid+1);
      free(wordAccumulatorCpBegin);
    }
  }
}

 char* PolyToString(const Poly *p)
 {
   char* wordAccumulator = (char*) malloc(100*sizeof(char));
   char* wordAccumulatorBegin = wordAccumulator;
   char* accumulator = (char*) malloc(100*sizeof(char));
   char* accumulatorBegin = accumulator;
   for(int i=0;i<100;++i) accumulator[i] = '\0';
   for(int i=0;i<100;++i) wordAccumulator[i] = '\0';
   PolyPrintRec(&accumulator, &wordAccumulatorBegin, &wordAccumulator, (poly_coeff_t)1, p, 0);
   free(wordAccumulatorBegin);
   return accumulatorBegin;
 }

void PolyPrint(const Poly* p)
{
  char* str = PolyToString(p);
  printf("Poly> %s\n", str);
  free(str);
}
