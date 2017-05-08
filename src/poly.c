#include "poly.h"
#include <assert.h>
#include <stdio.h>

void PolyPrint(const Poly* p);
char* PolyToString(const Poly *p);


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

int PolyExtractConstTermsRec(Poly* p)
{
  if(Lists.empty(p->monos)) {
    const int result = p->c;
    p->c = 0;
    return result;
  }
  Mono* fst = (Mono*) Lists.first(p->monos);
  if(fst->exp == 0) {
    const int result = p->c;
    p->c = 0;
    return result + PolyExtractConstTermsRec(&(fst->p));
  }
  return 0;
}

void PolyNormalizeConstTerms(Poly* p) {
  const int topTerm = PolyExtractConstTermsRec(p);
  p->c = topTerm;
}


void PolyInsertMono(Poly* p, Mono* newMono)
{
  if(newMono->exp == 0) {
    p->c += PolyExtractConstTermsRec(&(newMono->p));
    Lists.pushFront(p->monos, newMono);
    return;
  }
  if(Lists.empty(p->monos)) {
    Lists.pushBack(p->monos, newMono);
    return;
  }
  loop_list(p->monos, i) {
    listIterator next = Lists.next(i);
    Mono* m = (Mono*) Lists.getValue(i);
    if(m->exp == newMono->exp) {
      m->p = PolyAdd(&(m->p), &(newMono->p));
      return;
    } else if(newMono->exp > m->exp) {
      if(next == NULL) {
        Lists.pushBack(p->monos, newMono);
        return;
      } else if(newMono->exp < ((Mono*)Lists.getValue(next))->exp) {
        Lists.insertElementAt(p->monos, next, newMono);
        return;
      }
    }
  }
  Lists.pushFront(p->monos, newMono);
}

void PolyInsertMonoValue(Poly* p, Mono newMono)
{
  Mono* newMonoPtr = (Mono*) malloc(sizeof(Mono));
  *newMonoPtr = newMono;
  PolyInsertMono(p, newMonoPtr);
}


Poly PolyAddMonos(unsigned count, const Mono monos[])
{
  Poly p = PolyFromCoeff(0);
  for(unsigned int i=0;i<count;++i) {
    PolyInsertMono(&p, (Mono*)(&monos[i]));
  };
  return p;
}

void PolyScaleConst(Poly *p, const poly_coeff_t c)
{
  assert(p!=NULL);
  (p->c) *= c;
  loop_list(p->monos, iter) {
    Mono* m = (Mono*) Lists.getValue(iter);
    PolyScaleConst(&(m->p), c);
  }
}

Poly PolyMul(const Poly *p, const Poly *q)
{

  assert(p!=NULL);
  assert(q!=NULL);

  Poly result = PolyFromCoeff(0);

  if(q->c != 0) {
    loop_list(p->monos, pIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)Lists.getValue(pIter) );
      PolyScaleConst(&(partialResult->p), q->c);
      PolyInsertMono(&result, partialResult);
    }
  }

  if(p->c != 0) {
    loop_list(q->monos, qIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)Lists.getValue(qIter) );
      PolyScaleConst(&(partialResult->p), p->c);
      PolyInsertMono(&result, partialResult);
    }
  }

  loop_list(p->monos, pIter) {
    loop_list(q->monos, qIter) {
      Mono mp = *((Mono*) Lists.getValue(pIter));
      Mono mq = *((Mono*) Lists.getValue(qIter));
      Poly* factPartialResult = malloc(sizeof(Poly));
      *factPartialResult = PolyMul(&(mp.p), &(mq.p));
      Mono partialResult = MonoFromPoly(factPartialResult, mp.exp + mq.exp);
      PolyInsertMonoValue(&result, partialResult);
    }
  }

  result.c += q->c * p->c;

  return result;
}

void PolyNegRec(Poly *p)
{
  p->c *= -1;
  loop_list(p->monos, i) {
    Mono* m = (Mono*) Lists.getValue(i);
    PolyNegRec(&(m->p));
  }
}

Poly PolyNeg(const Poly *p)
{
  Poly ret = PolyClone(p);
  PolyNegRec(&ret);
  return ret;
}

Poly PolySub(const Poly *p, const Poly *q)
{
  Poly qNeg = PolyNeg(q);
  return PolyAdd(p, &qNeg);
}

poly_exp_t PolyDegByRec(const Poly *p, unsigned var_idcur, unsigned var_idx)
{
  poly_exp_t ret = -1;
  if(p->c != 0) ret = 0;
  loop_list(p->monos, i) {
    Mono* m = (Mono*) Lists.getValue(i);
    poly_exp_t temp = PolyDegByRec(&(m->p), var_idcur+1, var_idx);
    if(var_idcur == var_idx) {
      temp += m->exp;
    }
    if(temp > ret) {
      ret = temp;
    }
  }
  return ret;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
  return PolyDegByRec(p, 0, var_idx);
}

poly_exp_t PolyDeg(const Poly *p)
{
  return PolyDegByRec(p, 0, 0);
}

bool PolyIsEqRec(const Poly *p, const Poly *q)
{
  if(p->c != q->c) return false;
  listIterator iq = Lists.begin(q->monos);
  loop_list(p->monos, ip) {
    Mono* mp = (Mono*) Lists.getValue(ip);
    Mono* mq = (Mono*) Lists.getValue(iq);
    if(mp->exp != mq->exp) return false;
    if(!PolyIsEqRec(&(mp->p), &(mq->p))) {
      return false;
    }
    iq = Lists.next(iq);
  }
  return true;
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
  return PolyIsEqRec(p, q);
}

poly_coeff_t expCoeff(poly_coeff_t coeff, poly_exp_t exp)
{
  poly_coeff_t result = 1;
  for(poly_exp_t i = 0; i < exp; ++i) {
    result *= coeff;
  }
  return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{

  assert(p!=NULL);

  Poly result = PolyFromCoeff(0);
  result.c += p->c;

  loop_list(p->monos, i) {
    Mono* m = (Mono*) Lists.getValue(i);
    poly_coeff_t factValue = expCoeff(x, m->exp);
    Poly partialResult = PolyClone(&(m->p));
    PolyScaleConst(&partialResult, factValue);
    result.c += partialResult.c;
    loop_list(partialResult.monos, j) {
      Mono* submono = (Mono*) Lists.getValue(j);
      Mono cln = MonoClone(submono);
      PolyInsertMonoValue(&result, cln);
    }
    PolyDestroy(&partialResult);
  }

  return result;
}

char* PolyTranslateVarID(int varid)
{
  char* str = malloc(sizeof(char)*3);
  for(int i=0;i<3;++i) str[i]='\0';
  for(int i=0;i<3;++i) {
    str[i] = (varid % ('z'-'a')) + 'a';
    varid /= ('z'-'a');
    if(varid == 0) break;
  }
  return str;
}

void PolyPrintSingleExp(char** wordAccumulatorBeg, char** wordAccumulator, int varid, poly_exp_t exp)
{
  char* varname = PolyTranslateVarID(varid);
  if(exp == 0) {
    //DO NOTHING
  } else if(exp == 1) {
    if(*wordAccumulatorBeg != *wordAccumulator) {
      *wordAccumulator += sprintf(*wordAccumulator, "*");
    }
    *wordAccumulator += sprintf(*wordAccumulator, "%s", varname);
  } else {
    if(*wordAccumulatorBeg != *wordAccumulator) {
      *wordAccumulator += sprintf(*wordAccumulator, "*");
    }
    *wordAccumulator += sprintf(*wordAccumulator, "%s^%d", varname, (int)exp);
  }
  free(varname);
}

void PolyPrintSingleWord(char** wordAccumulatorBeg, char** wordAccumulator, int varid, poly_exp_t exp, poly_coeff_t coeffAccumulator)
{

  if(coeffAccumulator == 0) {
    // DO NOTHING
  } else if(coeffAccumulator == 1) {
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  } else if(coeffAccumulator < 0) {
    if(*wordAccumulatorBeg != *wordAccumulator) {
      *wordAccumulator += sprintf(*wordAccumulator, " - ");
    } else {
      *wordAccumulator += sprintf(*wordAccumulator, "-");
    }
    *wordAccumulator += sprintf(*wordAccumulator, "%d", -coeffAccumulator);
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  } else {
    if(*wordAccumulatorBeg != *wordAccumulator) {
      *wordAccumulator += sprintf(*wordAccumulator, " + ");
    }
    *wordAccumulator += sprintf(*wordAccumulator, "%d", coeffAccumulator);
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  }
}

#define POLY_TO_STRING_BUF_SIZE 700

void PolyPrintRec(char** accumulatorBeg, char** accumulator, char** wordAccumulatorBeg, char** wordAccumulator, poly_coeff_t coeffAccumulator, const Poly *p, int varid)
{
  if(p==NULL) return;

  //printf(" / coeef = %d\n", (PolyGetConstTerm(p)));
  //printf("|  ?coe  = %d\n", (PolyIsCoeff(p)));
  //printf("|  word  = %s\n", *wordAccumulatorBeg);
  //printf(" \\ acc   = %s\n\n", *accumulatorBeg);
  if(PolyGetConstTerm(p) != 0) {
    PolyPrintSingleWord( accumulatorBeg, accumulator, varid, 0, PolyGetConstTerm(p) );
    *accumulator += sprintf(*accumulator, "%s", *wordAccumulatorBeg);
  }

  loop_list(p->monos, i) {
    const Mono* m = (Mono*) Lists.getValue(i);
    char* wordAccumulatorCp = (char*) malloc(POLY_TO_STRING_BUF_SIZE*sizeof(char));
    char* wordAccumulatorCpBegin = wordAccumulatorCp;
    for(int i=0;i<POLY_TO_STRING_BUF_SIZE;++i) wordAccumulatorCpBegin[i] = (*wordAccumulatorBeg)[i];
    wordAccumulatorCp += (*wordAccumulator-*wordAccumulatorBeg);
    PolyPrintSingleExp(&wordAccumulatorCpBegin, &wordAccumulatorCp, varid, m->exp);
    PolyPrintRec(accumulatorBeg, accumulator, &wordAccumulatorCpBegin, &wordAccumulatorCp, coeffAccumulator, &(m->p), varid+1);
    free(wordAccumulatorCpBegin);
  }
}

 char* PolyToString(const Poly *p)
 {
   char* wordAccumulator = (char*) malloc(POLY_TO_STRING_BUF_SIZE*sizeof(char));
   char* wordAccumulatorBegin = wordAccumulator;
   char* accumulator = (char*) malloc(POLY_TO_STRING_BUF_SIZE*sizeof(char));
   char* accumulatorBegin = accumulator;
   for(int i=0;i<POLY_TO_STRING_BUF_SIZE;++i) accumulator[i] = '\0';
   for(int i=0;i<POLY_TO_STRING_BUF_SIZE;++i) wordAccumulator[i] = '\0';
   PolyPrintRec(&accumulatorBegin, &accumulator, &wordAccumulatorBegin, &wordAccumulator, (poly_coeff_t)1, p, 0);
   if(accumulatorBegin == accumulator) {
     sprintf(accumulatorBegin, "0");
   }
   free(wordAccumulatorBegin);
   return accumulatorBegin;
 }

void PolyPrintln(const Poly* p)
{
  char* str = PolyToString(p);
  printf("%s\n", str);
  free(str);
}
