#include "poly.h"
#include <assert.h>
#include <stdio.h>

#define POLY_TO_STRING_BUF_SIZE 700

void PolyPrintCard(const Poly *p);



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
  return Lists.empty(&(p->monos));
}

static inline bool PolyIsZero(const Poly *p)
{
  assert(p!=NULL);
  if(!PolyIsCoeff(p)) return false;
  return p->c == 0;
}

static inline poly_coeff_t PolyGetConstTerm(const Poly* p)
{
  return p->c;
}

void PolyDestroy(Poly *p)
{
  if(p==NULL) return;
  loop_list(&(p->monos), i) {
    Mono* m = (Mono*) Lists.getValue(i);
    MonoDestroy(m);
  }
  loop_list(&(p->monos), i) {
    Mono* m = (Mono*) Lists.getValue(i);
    DBG {printf("FREE PolyDestroy %p\n", m);fflush(stdout);}
    free(m);
  }
  Lists.free(&(p->monos));
}

static inline void MonoDestroy(Mono *m)
{
  if(m==NULL) return;
  PolyDestroy(&(m->p));
}

void* polyCopier(void* data)
{
  Mono* mono = (Mono*) data;
  Mono* monoNew = (Mono*) malloc(sizeof(Mono));
  DBG {printf("MALLOC PolyCopier %p\n", monoNew);fflush(stdout);}
  *monoNew = MonoClone(mono);
  return (void*) monoNew;
}

Poly PolyClone(const Poly *p)
{
  assert(p!=NULL);
  return (Poly) { .c = p->c, .monos = Lists.deepCopy(&(p->monos), polyCopier) };
}

static inline Mono MonoClone(const Mono *m)
{
  assert(m!=NULL);
  return (Mono) {.exp = m->exp, .p = PolyClone(&(m->p))};
}

static inline Mono* MonoClonePtr(const Mono *m)
{
  Mono* mono = malloc(sizeof(Mono));
  DBG {printf("MALLOC MonoClonePtr %p\n", mono);fflush(stdout);}
  *mono = MonoClone(m);
  return mono;
}


Poly PolyAdd(const Poly *p, const Poly *q)
{
  assert(p!=NULL);
  assert(q!=NULL);
  list result = Lists.new();

  listIterator iq = Lists.begin(&(q->monos));
  listIterator ip = Lists.begin(&(p->monos));
  while(ip != NULL || iq != NULL) {
    Mono* mp = (Mono*) Lists.getValue(ip);
    Mono* mq = (Mono*) Lists.getValue(iq);
    if(mp == NULL) {
      Lists.pushBack(&result, MonoClonePtr(mq));
      iq = Lists.next(iq);
    } else if(mq == NULL) {
      Lists.pushBack(&result, MonoClonePtr(mp));
      ip = Lists.next(ip);
    } else if(mp->exp > mq->exp) {
      Lists.pushBack(&result, MonoClonePtr(mq));
      iq = Lists.next(iq);
    } else if(mp->exp < mq->exp) {
      Lists.pushBack(&result, MonoClonePtr(mp));
      ip = Lists.next(ip);
    } else {
      Poly polyAddResult = PolyAdd(&(mp->p), &(mq->p));

      if(PolyIsCoeff(&polyAddResult) && PolyGetConstTerm(&polyAddResult) == 0) {
        PolyDestroy(&polyAddResult);
      } else {
        Mono* partialResult = (Mono*) malloc(sizeof(Mono));
        *partialResult = (Mono) { .exp = mp->exp, .p = polyAddResult };
        Lists.pushBack(&result, partialResult);
      }
      ip = Lists.next(ip);
      iq = Lists.next(iq);
    }
  }
  return (Poly) { .c = p->c + q->c, .monos = result };
}

int PolyExtractConstTermsRec(Poly* p)
{
  if(Lists.empty(&(p->monos))) {
    const int result = p->c;
    p->c = 0;
    return result;
  }
  Mono* fst = (Mono*) Lists.first(&(p->monos));
  if(fst->exp == 0) {
    const int result = p->c;
    p->c = 0;
    return result + PolyExtractConstTermsRec(&(fst->p));
  }
  const int result = p->c;
  p->c = 0;
  return result;
}

void PolyNormalizeConstTerms(Poly* p) {
  const int topTerm = PolyExtractConstTermsRec(p);
  p->c = topTerm;
}


int PolyInsertMono(Poly* p, Mono* newMono)
{

  if(newMono->exp == 0) {

    const poly_coeff_t term = PolyExtractConstTermsRec(&(newMono->p));
    p->c += term;
    newMono->p.c = 0;
    if(PolyIsCoeff(&(newMono->p))) {
      return 0;
    }
  }


  if(Lists.empty(&(p->monos))) {
    Lists.pushBack(&(p->monos), newMono);
    return 1;
  }
  if(((Mono*)Lists.last(&(p->monos)))->exp < newMono->exp) {
    Lists.pushBack(&(p->monos), newMono);
    return 1;
  }
  if(((Mono*)Lists.first(&(p->monos)))->exp > newMono->exp) {
    Lists.pushFront(&(p->monos), newMono);
    return 1;
  }

  loop_list(&(p->monos), i) {
    listIterator next = Lists.next(i);
    Mono* m = (Mono*) Lists.getValue(i);
    if(m->exp == newMono->exp) {
      Poly* toDel = &(m->p);
      Poly pom = PolyAdd(&(m->p), &(newMono->p));
      if(PolyIsCoeff(&pom) && PolyGetConstTerm(&pom)==0) {
        Lists.detachElement(&(p->monos), i);
        PolyDestroy(&pom);
        MonoDestroy(m);
        free(m);
        return 0;
      }
      PolyDestroy(&(m->p));
      m->p = pom;

      return 0;
    } else if(newMono->exp > m->exp) {
      if(next == NULL) {
        Lists.pushBack(&(p->monos), newMono);
        return 1;
      } else if(newMono->exp < ((Mono*)Lists.getValue(next))->exp) {
        Lists.insertElementAt(&(p->monos), next, newMono);
        return 1;
      }
    }
  }
  Lists.pushFront(&(p->monos), newMono);
  return 1;
}

void PolyInsertMonoValue(Poly* p, Mono newMono)
{
  Mono* newMonoPtr = (Mono*) malloc(sizeof(Mono));

  *newMonoPtr = newMono;
  if(!PolyInsertMono(p, newMonoPtr)) {
    MonoDestroy(newMonoPtr);
    free(newMonoPtr);
  }
}

void PolyInsertMonoPtr(Poly* p, Mono* newMonoPtr)
{
  if(!PolyInsertMono(p, newMonoPtr)) {
    MonoDestroy(newMonoPtr);
    free(newMonoPtr);
  }
}


Poly PolyAddMonos(unsigned count, const Mono monos[])
{
  Poly p = PolyFromCoeff(0);
  for(unsigned int i=0;i<count;++i) {
    PolyInsertMonoValue(&p, monos[i]);
  };
  return p;
}

void PolyScaleConst(Poly *p, const poly_coeff_t c)
{
  assert(p!=NULL);
  if(c == 1) return;
  (p->c) *= c;
  loop_list(&(p->monos), iter) {
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
    loop_list(&(p->monos), pIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)Lists.getValue(pIter) );
      PolyScaleConst(&(partialResult->p), q->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  if(p->c != 0) {
    loop_list(&(q->monos), qIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)Lists.getValue(qIter) );
      PolyScaleConst(&(partialResult->p), p->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  loop_list(&(p->monos), pIter) {
    loop_list(&(q->monos), qIter) {
      Mono* mp = (Mono*) Lists.getValue(pIter);
      Mono* mq = (Mono*) Lists.getValue(qIter);
      Poly factPartialResult = PolyMul(&(mp->p), &(mq->p));
      Mono partialResult = MonoFromPoly(&factPartialResult, mp->exp + mq->exp);
      PolyInsertMonoValue(&result, partialResult);
    }
  }

  result.c += q->c * p->c;
  return result;
}

void PolyNegRec(Poly *p)
{
  p->c *= -1;
  loop_list(&(p->monos), i) {
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
  Poly ret = PolyAdd(p, &qNeg);
  PolyDestroy(&qNeg);
  return ret;
}

poly_exp_t PolyDegByRec(const Poly *p, unsigned var_idcur, unsigned var_idx, int sum_all)
{
  poly_exp_t ret = -1;
  if(p->c != 0) ret = 0;
  loop_list(&(p->monos), i) {
    Mono* m = (Mono*) Lists.getValue(i);
    poly_exp_t temp = PolyDegByRec(&(m->p), var_idcur+1, var_idx, sum_all);
    if(var_idcur == var_idx || sum_all) {
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
  return PolyDegByRec(p, 0, var_idx, 0);
}

poly_exp_t PolyDeg(const Poly *p)
{
  return PolyDegByRec(p, 0, 0, 1);
}

bool PolyIsEqRec(const Poly *p, const Poly *q)
{
  if(p->c != q->c) {
    return false;
  }
  if(p==q) return true;
  listIterator iq = Lists.begin(&(q->monos));
  loop_list(&(p->monos), ip) {
    Mono* mp = (Mono*) Lists.getValue(ip);
    Mono* mq = (Mono*) Lists.getValue(iq);
    if(mp!=mq) {
      if((ip == NULL && iq != NULL) || (ip != NULL && iq == NULL)) {
        return false;
      }
      if(mp->exp != mq->exp) {
        return false;
      }
      if(!PolyIsEqRec(&(mp->p), &(mq->p))) {
        return false;
      }
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
  if(exp == 0) return 1;
  if(exp == 1) return coeff;
  if(coeff == 0) return 0;
  if(coeff == 1) return 1;
  if(coeff == -1) {
    if(exp % 2 == 0) return 1;
    return -1;
  }
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

  loop_list(&(p->monos), i) {
    Mono* m = (Mono*) Lists.getValue(i);
    poly_coeff_t factValue = expCoeff(x, m->exp);

    Poly partialResult = PolyClone(&(m->p));
    PolyScaleConst(&partialResult, factValue);
    result.c += partialResult.c;
    partialResult.c = 0;
    loop_list(&(partialResult.monos), j) {
      Mono* submono = (Mono*) Lists.getValue(j);
      Poly pcln = PolyClone(&(submono->p));
      Mono cln = MonoFromPoly(&pcln, submono->exp);

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

  int wordBufferEmpty = (*wordAccumulatorBeg == *wordAccumulator);

  if(coeffAccumulator == 0) {
    // DO NOTHING
  } else if(2 == 3) {
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  } else if(coeffAccumulator < 0) {
    if(!wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, " - ");
    } else {
      *wordAccumulator += sprintf(*wordAccumulator, "-");
    }
    if(coeffAccumulator != -1) {
      *wordAccumulator += sprintf(*wordAccumulator, "%d", -coeffAccumulator);
    } else if(wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, "1");
    }
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  } else {

    if(!wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, " + ");
    }
    if(coeffAccumulator != 1) {
      *wordAccumulator += sprintf(*wordAccumulator, "%d", coeffAccumulator);
    } else if(wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, "1");
    }
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  }
}

void PolyPrintRec(char** accumulatorBeg, char** accumulator, char** wordAccumulatorBeg, char** wordAccumulator, poly_coeff_t coeffAccumulator, const Poly *p, int varid)
{
  if(p==NULL) return;

  if(PolyGetConstTerm(p) != 0) {
    PolyPrintSingleWord( accumulatorBeg, accumulator, varid, 0, PolyGetConstTerm(p) );
    *accumulator += sprintf(*accumulator, "%s", *wordAccumulatorBeg);
  }

  loop_list(&(p->monos), i) {
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

void PolySprintf(char* dest, const Poly *p)
{
   char* wordAccumulator = (char*) malloc(POLY_TO_STRING_BUF_SIZE*sizeof(char));
   char* wordAccumulatorBegin = wordAccumulator;

   char* accumulator = dest;
   char* accumulatorBegin = dest;

   accumulator[0] = '\0';
   wordAccumulator[0] = '\0';

   PolyPrintRec(&accumulatorBegin, &accumulator, &wordAccumulatorBegin, &wordAccumulator, (poly_coeff_t)1, p, 0);
   if(accumulatorBegin == accumulator) {
     sprintf(accumulatorBegin, "0");
   }
   free(wordAccumulatorBegin);
}

char* PolyToString(const Poly* p)
{
  char* str = (char*) malloc(POLY_TO_STRING_BUF_SIZE*sizeof(char));
  PolySprintf(str, p);
  return str;
}

void PolyPrintCardRec(const Poly *p)
{
  if(!Lists.empty(&(p->monos))) printf("P(");
  int wsth = 0;
  if(p->c != 0) {
    wsth = 1;
    printf("C(%d)", p->c);
    if(!Lists.empty(&(p->monos))) printf(", 0");
  } else if(Lists.empty(&(p->monos))) {
    printf("C(0)");
  }
  loop_list(&(p->monos), i) {
    Mono* m = (Mono*) Lists.getValue(i);
    if(wsth) { printf(", "); }
    PolyPrintCardRec(&(m->p));
    printf(", %d", m->exp);
    wsth = 1;
  }
  if(!Lists.empty(&(p->monos))) printf(")");
}

void PolyPrintCard(const Poly *p)
{
  PolyPrintCardRec(p);
  printf("\n");
}

void PolyPrint(const Poly* p)
{
  char* str = PolyToString(p);
  printf("%s", str);
  free(str);
}
