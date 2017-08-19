/*
*  Interface of polynomials
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include "memalloc.h"
#include "dynamic_lists.h"
#include "generics.h"
#include "poly.h"
#include "math_utils.h"

/*
* Creates monomial from given exponent and coefficient
*/
static inline Mono MonoFromCoeff(poly_coeff_t c, poly_exp_t e) {
    return (Mono) { .exp = e, .p = PolyFromCoeff(c) };
}

/*
* Deallocated memory taken by polynomial
*/
void PolyDestroy(Poly *p) {
  if(p==NULL) return;
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
    MonoDestroy(m);
  }
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
    free(m);
  }
  ListDestroy(&(p->monos));
}


/*
* Helper function for deep-copying list of monomials
* Deep-copies given monomial pointed by ListData pointer.
* Then returns pointer to the newly copied monomial.
*/
static inline ListData polyCopier(ListData data) {
  Mono* mono = (Mono*) data;
  Mono* mono_new = MALLOCATE(Mono);
  *mono_new = MonoClone(mono);
  return (ListData) mono_new;
}

/*
* Deep-copies polynomial
*/
Poly PolyClone(const Poly *p) {
  assert(p!=NULL);
  return (Poly) { .c = p->c, .monos = ListDeepCopy(&(p->monos), polyCopier) };
}


/*
* Deep-copies monomial and then allocates memory for it
* and returns pointer to that memory.
*/
Mono* MonoClonePtr(const Mono *m) {
  Mono* mono = MALLOCATE(Mono);
  *mono = MonoClone(m);
  return mono;
}


/*
* Multiplies all coefficients in polynomial by const factor c
*/
void PolyScaleConst(Poly *p, const poly_coeff_t c) {
  assert(p!=NULL);
  if(c == 1) return;
  (p->c) *= c;
  LOOP_LIST(&(p->monos), iter) {
    Mono* m = (Mono*) ListGetValue(iter);
    PolyScaleConst(&(m->p), c);
  }
}

Poly PolyAddScaled(const Poly *p, const Poly *q, const poly_coeff_t c);

/*
* Adding two polynomials with optional scaling of second parameter
*/
void PolyAddScaledInPlace(Poly *p, const Poly *q, const poly_coeff_t c) {
  assert(p!=NULL);
  assert(q!=NULL);
  List result = ListNew();

  ListIterator iq = ListBegin(&(q->monos));
  ListIterator ip = ListBegin(&(p->monos));
  while(ip != NULL || iq != NULL) {
    Mono* mp = (Mono*) ListGetValue(ip);
    Mono* mq = (Mono*) ListGetValue(iq);
    if(mp == NULL) {
      Mono* new_mono = MonoClonePtr(mq);
      PolyScaleConst(&(new_mono->p), c);
      ListPushBack(&result, new_mono);
      iq = ListNext(iq);
    } else if(mq == NULL) {
      //Skip clone
      Mono* new_mono = mp;
      ListPushBack(&result, new_mono);
      ip = ListNext(ip);
    } else if(mp->exp > mq->exp) {
      Mono* new_mono = MonoClonePtr(mq);
      PolyScaleConst(&(new_mono->p), c);
      ListPushBack(&result, new_mono);
      iq = ListNext(iq);
    } else if(mp->exp < mq->exp) {
      //Skip clone
      Mono* new_mono = mp;
      ListPushBack(&result, new_mono);
      ip = ListNext(ip);
    } else {
      PolyAddScaledInPlace(&(mp->p), &(mq->p), c);

      if(PolyIsCoeff(&(mp->p)) && PolyGetConstTerm(&(mp->p)) == 0) {
        MonoDestroy(mp);
        free(mp);
      } else {
        ListPushBack(&result, mp);
      }

      /*PolyAddScaledInPlace(&(mp->p), &(mq->p), c);

      if(PolyIsCoeff(&(mp->p)) && PolyGetConstTerm(&(mp->p)) == 0) {
        PolyDestroy(&(mp->p));
      } else {
        Mono* partial_result = MALLOCATE(Mono);
        *partial_result = (Mono) { .exp = mp->exp, .p = mp->p };
        ListPushBack(&result, partial_result);
      }*/
      ip = ListNext(ip);
      iq = ListNext(iq);
    }
  }
  ListDestroy(&(p->monos));
  *p = (Poly) { .c = p->c + (q->c)*c, .monos = result };
}

/*
* Adding two polynomials with optional scaling of second parameter
*/
Poly PolyAddScaled(const Poly *p, const Poly *q, const poly_coeff_t c) {
  assert(p!=NULL);
  assert(q!=NULL);
  List result = ListNew();

  ListIterator iq = ListBegin(&(q->monos));
  ListIterator ip = ListBegin(&(p->monos));
  while(ip != NULL || iq != NULL) {
    Mono* mp = (Mono*) ListGetValue(ip);
    Mono* mq = (Mono*) ListGetValue(iq);
    if(mp == NULL) {
      Mono* new_mono = MonoClonePtr(mq);
      PolyScaleConst(&(new_mono->p), c);
      ListPushBack(&result, new_mono);
      iq = ListNext(iq);
    } else if(mq == NULL) {
      Mono* new_mono = MonoClonePtr(mp);
      ListPushBack(&result, new_mono);
      ip = ListNext(ip);
    } else if(mp->exp > mq->exp) {
      Mono* new_mono = MonoClonePtr(mq);
      PolyScaleConst(&(new_mono->p), c);
      ListPushBack(&result, new_mono);
      iq = ListNext(iq);
    } else if(mp->exp < mq->exp) {
      Mono* new_mono = MonoClonePtr(mp);
      ListPushBack(&result, new_mono);
      ip = ListNext(ip);
    } else {
      Poly polyAddResult = PolyAddScaled(&(mp->p), &(mq->p), c);

      if(PolyIsCoeff(&polyAddResult) && PolyGetConstTerm(&polyAddResult) == 0) {
        PolyDestroy(&polyAddResult);
      } else {
        Mono* partial_result = MALLOCATE(Mono);
        *partial_result = (Mono) { .exp = mp->exp, .p = polyAddResult };
        ListPushBack(&result, partial_result);
      }
      ip = ListNext(ip);
      iq = ListNext(iq);
    }
  }
  return (Poly) { .c = p->c + (q->c)*c, .monos = result };
}

/*
* Adding two polynomials
*/
Poly PolyAdd(const Poly *p, const Poly *q) {
  return PolyAddScaled(p, q, 1);
}

/*
* Getting const term of polynomial with setting all unnormalized
* terms c*x^0 to 0.
*/
int PolyExtractConstTermsRec(Poly* p) {
  if(ListEmpty(&(p->monos))) {
    const int result = p->c;
    p->c = 0;
    return result;
  }
  Mono* fst = (Mono*) ListFirst(&(p->monos));
  if(fst->exp == 0) {
    const int result = p->c;
    p->c = 0;
    return result + PolyExtractConstTermsRec(&(fst->p));
  }
  const int result = p->c;
  p->c = 0;
  return result;
}

/*
* Normalize const terms of polynomial
*/
void PolyNormalizeConstTerms(Poly* p) {
  const int top_term = PolyExtractConstTermsRec(p);
  p->c = top_term;
}

/*
* Inserts given monomial to the given polynomial performing
* all normalizing opertions (simplifying terms etc.).
* If the function returns true then the monomial was inserted
* succesfully, if false then from various reasons like:
*
* - simplifying (the monomial coefficient was set to zero after addition)
*      when it would not exist in resulting polynomial
* - the coefficient of monomial is already zero so there's no reason
*      to insert it anywhere
*
* the monomial is not used and SHOULD BE DESTROYED.
*/
static inline int PolyTryInsertMono(Poly* p, Mono* new_mono) {

  if(PolyIsCoeff(&(new_mono->p)) && PolyGetConstTerm(&(new_mono->p))==0) {
    return 0;
  }


  if(new_mono->exp == 0) {

    //const poly_coeff_t term = PolyExtractConstTermsRec(&(new_mono->p));
    p->c += new_mono->p.c;
    new_mono->p.c = 0;
    if(PolyIsCoeff(&(new_mono->p))) {
      return 0;
    }
  }

  if(ListEmpty(&(p->monos))) {
    ListPushBack(&(p->monos), new_mono);
    return 1;
  }
  if(((Mono*)ListLast(&(p->monos)))->exp < new_mono->exp) {
    ListPushBack(&(p->monos), new_mono);
    return 1;
  }
  if(((Mono*)ListFirst(&(p->monos)))->exp > new_mono->exp) {
    ListPushFront(&(p->monos), new_mono);
    return 1;
  }

  LOOP_LIST(&(p->monos), i) {
    ListIterator next = ListNext(i);
    Mono* m = (Mono*) ListGetValue(i);
    if(m->exp == new_mono->exp) {

      //printf("@");fflush(stdout);



      //TODO: Potential performance issue
      /*Poly pom = PolyAdd(&(m->p), &(new_mono->p));
      if(PolyIsCoeff(&pom) && PolyGetConstTerm(&pom)==0) {
        ListDetachElement(&(p->monos), i);
        PolyDestroy(&pom);
        MonoDestroy(m);
        free(m);
        return 0;
      }
      //Here: destroy poly
      PolyDestroy(&(m->p));
      m->p = pom;*/



      //printf("#"); fflush(stdout);
      PolyAddScaledInPlace(&(m->p), &(new_mono->p), 1);
      if(PolyIsCoeff(&(m->p)) && PolyGetConstTerm(&(m->p))==0) {
        ListDetachElement(&(p->monos), i);
        MonoDestroy(m);
        free(m);
        return 0;
      }

      return 0;
    } else if(new_mono->exp > m->exp) {
      if(next == NULL) {
        ListPushBack(&(p->monos), new_mono);
        return 1;
      } else if(new_mono->exp < ((Mono*)ListGetValue(next))->exp) {
        ListInsertElementAt(&(p->monos), next, new_mono);
        return 1;
      }
    }
  }
  ListPushFront(&(p->monos), new_mono);
  return 1;
}

/*
* Allocates memory for monomial then tries to insert it
* using PolyInsertMono if the function fails
* then memory is automatically freed.
*/
static inline void PolyInsertMonoValue(Poly* p, Mono new_mono) {
  Mono* new_mono_ptr = MALLOCATE(Mono);
  *new_mono_ptr = new_mono;
  if(!PolyTryInsertMono(p, new_mono_ptr)) {
    MonoDestroy(new_mono_ptr);
    free(new_mono_ptr);
  }

}

/*
* Allocates memory for monomial then tries to insert it
* using PolyInsertMono if the function fails
* then memory is automatically freed.
*/
static inline void PolyInsertMonoPtr(Poly* p, Mono* new_mono_ptr) {
  if(!PolyTryInsertMono(p, new_mono_ptr)) {
    MonoDestroy(new_mono_ptr);
    free(new_mono_ptr);
  }
}

void PolyInsertMono(Poly* p, Mono mono) {
  PolyInsertMonoValue(p, mono);
}

int MonoHelperSorter(const void * p1, const void * p2) {
    Mono e1 = *((Mono*)p1);
    Mono e2 = *((Mono*)p2);
    if (e1.exp > e2.exp) return  1;
    if (e1.exp < e2.exp) return -1;
    return 0;
}

/*
* Adds monos to create new polynomial.
*/
Poly PolyAddMonos(unsigned count, const Mono monos[]) {
  Poly p = PolyFromCoeff(0);
  for(unsigned int i=0;i<count;++i) {
    PolyInsertMonoValue(&p, monos[i]);
  };
  return p;
}

/*
* Multiply two polynomials
*/
Poly PolyMul(const Poly *p, const Poly *q) {
  assert(p!=NULL);
  assert(q!=NULL);

  Poly result = PolyFromCoeff(0);

  if(q->c != 0) {
    LOOP_LIST(&(p->monos), pIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)ListGetValue(pIter) );
      PolyScaleConst(&(partialResult->p), q->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  if(p->c != 0) {
    LOOP_LIST(&(q->monos), qIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)ListGetValue(qIter) );
      PolyScaleConst(&(partialResult->p), p->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  LOOP_LIST(&(p->monos), pIter) {
    LOOP_LIST(&(q->monos), qIter) {
      Mono* mp = (Mono*) ListGetValue(pIter);
      Mono* mq = (Mono*) ListGetValue(qIter);
      Poly factPartialResult = PolyMul(&(mp->p), &(mq->p));
      Mono partialResult = MonoFromPoly(&factPartialResult, mp->exp + mq->exp);
      PolyInsertMonoValue(&result, partialResult);
    }
  }

  result.c += q->c * p->c;
  return result;
}

/*
* Recursively negate polynomial
*/
void PolyNegRec(Poly *p) {
  p->c *= -1;
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
    PolyNegRec(&(m->p));
  }
}

/*
* Negate polynomial
*/
Poly PolyNeg(const Poly *p) {
  Poly ret = PolyClone(p);
  PolyNegRec(&ret);
  return ret;
}

/*
* Substract two polynomials
*/
Poly PolySub(const Poly *p, const Poly *q) {
  return PolyAddScaled(p, q, -1);
}

/*
* Find degree of polynomial with respect to the given variable index.
* Recursive helper.
*/
poly_exp_t PolyDegByRec(const Poly *p, unsigned var_idcur, unsigned var_idx,
  int sum_all) {

  poly_exp_t ret = -1;
  if(p->c != 0) ret = 0;
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
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

/*
* Find degree of polynomial with respect to the given variable index.
*/
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
  return PolyDegByRec(p, 0, var_idx, 0);
}

/*
* Find degree of polynomial assuming that all variables are equal.
*/
poly_exp_t PolyDeg(const Poly *p) {
  return PolyDegByRec(p, 0, 0, 1);
}

/*
* Recursive equality test for polynomials
*/
bool PolyIsEqRec(const Poly *p, const Poly *q) {
  if(p == q) return true;
  if(p->c != q->c) {
    return false;
  }

  ListIterator iq = ListBegin(&(q->monos));
  LOOP_LIST(&(p->monos), ip) {
    Mono* mp = (Mono*) ListGetValue(ip);
    Mono* mq = (Mono*) ListGetValue(iq);
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
    iq = ListNext(iq);
  }
  return true;
}

/*
* Equality test for polynomials
*/
bool PolyIsEq(const Poly *p, const Poly *q) {
  return PolyIsEqRec(p, q);
}



Poly PolyPow(const Poly* p, poly_exp_t exp) {
  
  if(exp == 0) return PolyFromCoeff(1);
  if(exp == 1) return PolyClone(p);

  if(PolyIsCoeff(p)) {
    if(p->c == 0) return PolyZero();
    if(p->c == 1) return PolyFromCoeff(1);
    if(p->c == -1) {
      if(exp % 2 == 0) return PolyFromCoeff(1);
      return PolyFromCoeff(-1);
    }
  }

  Poly result = PolyFromCoeff(1);
  Poly base = PolyClone(p);

  while (exp) {
    if (exp & 1) {
      PolyReplace(&result, PolyMul(&result, &base));
    }
    exp >>= 1;
    PolyReplace(&base, PolyMul(&base, &base));
  }
  PolyDestroy(&base);

  return result;
}

/*
* Evaluate polynomial at a given point
*/
Poly PolyAt(const Poly *p, poly_coeff_t x) {

  assert(p!=NULL);

  Poly result = PolyFromCoeff(0);
  result.c += p->c;

  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
    poly_coeff_t factValue = MathFastPowLong(x, m->exp);

    Poly partialResult = PolyClone(&(m->p));
    PolyScaleConst(&partialResult, factValue);
    result.c += partialResult.c;
    partialResult.c = 0;
    LOOP_LIST(&(partialResult.monos), j) {
      Mono* submono = (Mono*) ListGetValue(j);
      Poly pcln = PolyClone(&(submono->p));
      Mono cln = MonoFromPoly(&pcln, submono->exp);

      PolyInsertMonoValue(&result, cln);
    }
    PolyDestroy(&partialResult);

  }

  return result;
}

/*
* Translate variable index to its human readable form.
* Helper function for PolyPrint function family.
*/
char* PolyTranslateVarID(int varid) {
  char* str = MALLOCATE_ARRAY(char, 3);
  for(int i=0;i<3;++i) str[i]='\0';
  for(int i=0;i<3;++i) {
    str[i] = (varid % ('z'-'a')) + 'a';
    varid /= ('z'-'a');
    if(varid == 0) break;
  }
  return str;
}

/*
* Prints variable ^ exp
* to the given accumulators.
* Helper function for PolyPrint function family.
*/
void PolyPrintSingleExp(char** wordAccumulatorBeg, char** wordAccumulator,
  int varid, poly_exp_t exp) {
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

/*
* Prints c * variable ^ exp
* to the given accumulators.
* Helper function for PolyPrint function family.
*/
void PolyPrintSingleWord(char** wordAccumulatorBeg, char** wordAccumulator,
  int varid, poly_exp_t exp, poly_coeff_t coeffAccumulator) {

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
      *wordAccumulator += sprintf(*wordAccumulator, "%ld", -coeffAccumulator);
    } else if(wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, "1");
    }
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  } else {

    if(!wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, " + ");
    }
    if(coeffAccumulator != 1) {
      *wordAccumulator += sprintf(*wordAccumulator, "%ld", coeffAccumulator);
    } else if(wordBufferEmpty) {
      *wordAccumulator += sprintf(*wordAccumulator, "1");
    }
    PolyPrintSingleExp(wordAccumulatorBeg, wordAccumulator, varid, exp);
  }
}

/*
* Recursively prints polynomial human-readable representation to the given accumulator.
* Helper function for PolyPrint function family.
*/
void PolyPrintRec(char** accumulatorBeg, char** accumulator,
  char** wordAccumulatorBeg, char** wordAccumulator,
  poly_coeff_t coeffAccumulator, const Poly *p, int varid) {

  if(p==NULL) return;

  if(PolyGetConstTerm(p) != 0) {
    PolyPrintSingleWord( accumulatorBeg, accumulator,
      varid, 0, PolyGetConstTerm(p) );
    *accumulator += sprintf(*accumulator, "%s", *wordAccumulatorBeg);
  }

  LOOP_LIST(&(p->monos), i) {
    const Mono* m = (Mono*) ListGetValue(i);
    char* wordAccumulatorCp = MALLOCATE_ARRAY(char, POLY_TO_STRING_BUF_SIZE);
    char* wordAccumulatorCpBegin = wordAccumulatorCp;
    for(int i=0;i<POLY_TO_STRING_BUF_SIZE;++i) {
      wordAccumulatorCpBegin[i] = (*wordAccumulatorBeg)[i];
    }

    wordAccumulatorCp += (*wordAccumulator-*wordAccumulatorBeg);
    PolyPrintSingleExp(&wordAccumulatorCpBegin,
      &wordAccumulatorCp, varid, m->exp);

    PolyPrintRec(accumulatorBeg, accumulator, &wordAccumulatorCpBegin,
      &wordAccumulatorCp, coeffAccumulator, &(m->p), varid+1);

    free(wordAccumulatorCpBegin);
  }
}

/*
* Prints polynomial human-readable representation to the given buffer.
*/
void PolySprintf(char* dest, const Poly *p) {
   char* wordAccumulator = MALLOCATE_ARRAY(char, POLY_TO_STRING_BUF_SIZE);
   char* wordAccumulatorBegin = wordAccumulator;

   char* accumulator = dest;
   char* accumulatorBegin = dest;

   accumulator[0] = '\0';
   wordAccumulator[0] = '\0';

   PolyPrintRec(&accumulatorBegin, &accumulator, &wordAccumulatorBegin,
     &wordAccumulator, (poly_coeff_t)1, p, 0);
   if(accumulatorBegin == accumulator) {
     sprintf(accumulatorBegin, "0");
   }
   free(wordAccumulatorBegin);
}

/*
* Prints polynomial human-readable representation to the buffer.
* Then returns pointer to that newly created buffer.
*/
char* PolyToString(const Poly* p) {
  char* str = MALLOCATE_ARRAY(char, POLY_TO_STRING_BUF_SIZE);
  PolySprintf(str, p);
  return str;
}

/*
* Prints polynomial human-readable representation to the stdout.
*/
void PolyPrint(const Poly* p) {
  char* str = PolyToString(p);
  printf("%s", str);
  free(str);
}

/*
* Recursively composes polynomial.
* Accepts one more paramter compared to the default compose function - index.
* The index is number of currently substituted variable
* (index in list x which is considered)
* as the list is parsed from left to right (indexing from 0).
*/
Poly PolyComposeRec(const Poly *p, unsigned count, unsigned index, const Poly x[]) {

  if(PolyIsCoeff(p)) return PolyClone(p);
  if(count == 0) return PolyClone(p);
  if(index>=count) return PolyZero();

  Poly result = PolyZero();

  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);

    Poly partial_result = PolyComposeRec(&(m->p), count, index+1, x);
    Poly pow_result = PolyPow(&x[index], m->exp);
    PolyReplace(&partial_result, PolyMul(&partial_result, &pow_result));
    PolyDestroy(&pow_result);

    PolyReplace(&result, PolyAdd(&result, &partial_result));
    PolyDestroy(&partial_result);
  }

  result.c += p->c;
  return result;
}


/*
* Compose given polynomials to one polynomial.
*/
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]) {
  return PolyComposeRec(p, count, 0, x);
}
