/*
*   Interface of polynomials
*
*   @author Piotr Styczyński <piotrsty1@gmail.com>
*   @copyright MIT
*   @date 2017-05-13
*/

#include <assert.h>
#include <stdio.h>
#include "memalloc.h"
#include "list.h"
#include "poly.h"

/*
* Creates const polynomial from given coefficient
*/
Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly){ .c = c, .monos = LISTS.new() };
}

/*
* Creates const polynomial equal to zero
*/
Poly PolyZero() {
    return PolyFromCoeff(0);
}

/*
* Creates monomial from given exponent and coefficient
*/
Mono MonoFromCoeff(poly_coeff_t c, poly_exp_t e) {
    return (Mono) { .exp = e, .p = PolyFromCoeff(c) };
}

/*
* Creates monomial from given exponent and coefficient
*/
Mono MonoFromPoly(const Poly *p, poly_exp_t e) {
    return (Mono) { .exp = e, .p = *p };
}

/*
* Checks if polynomial is const
*/
bool PolyIsCoeff(const Poly *p) {
  assert(p != NULL);
  return LISTS.empty(&(p->monos));
}

/*
* Checks if polynomial is equal to zero
*/
bool PolyIsZero(const Poly *p) {
  assert(p!=NULL);
  if(!PolyIsCoeff(p)) return false;
  return p->c == 0;
}

/*
* Extracts polynomial const (MFREE) term
*/
poly_coeff_t PolyGetConstTerm(const Poly* p) {
  return p->c;
}

/*
* Deallocated memory taken by polynomial
*/
void PolyDestroy(Poly *p) {
  if(p==NULL) return;
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) LISTS.getValue(i);
    MonoDestroy(m);
  }
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) LISTS.getValue(i);
    MFREE(m);
  }
  LISTS.free(&(p->monos));
}

/*
* Deallocated memory taken by monomial
*/
void MonoDestroy(Mono *m) {
  if(m==NULL) return;
  PolyDestroy(&(m->p));
}

/*
* Helper function for deep-copying list of monomials
* Deep-copies given monomial pointed by ListData pointer.
* Then returns pointer to the newly copied monomial.
*/
ListData polyCopier(ListData data) {
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
  return (Poly) { .c = p->c, .monos = LISTS.deepCopy(&(p->monos), polyCopier) };
}

/*
* Deep-copies monomial
*/
Mono MonoClone(const Mono *m) {
  assert(m!=NULL);
  return (Mono) {.exp = m->exp, .p = PolyClone(&(m->p))};
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
* Adding two polynomials
*/
Poly PolyAdd(const Poly *p, const Poly *q) {
  assert(p!=NULL);
  assert(q!=NULL);
  List result = LISTS.new();

  ListIterator iq = LISTS.begin(&(q->monos));
  ListIterator ip = LISTS.begin(&(p->monos));
  while(ip != NULL || iq != NULL) {
    Mono* mp = (Mono*) LISTS.getValue(ip);
    Mono* mq = (Mono*) LISTS.getValue(iq);
    if(mp == NULL) {
      LISTS.pushBack(&result, MonoClonePtr(mq));
      iq = LISTS.next(iq);
    } else if(mq == NULL) {
      LISTS.pushBack(&result, MonoClonePtr(mp));
      ip = LISTS.next(ip);
    } else if(mp->exp > mq->exp) {
      LISTS.pushBack(&result, MonoClonePtr(mq));
      iq = LISTS.next(iq);
    } else if(mp->exp < mq->exp) {
      LISTS.pushBack(&result, MonoClonePtr(mp));
      ip = LISTS.next(ip);
    } else {
      Poly polyAddResult = PolyAdd(&(mp->p), &(mq->p));

      if(PolyIsCoeff(&polyAddResult) && PolyGetConstTerm(&polyAddResult) == 0) {
        PolyDestroy(&polyAddResult);
      } else {
        Mono* partial_result = MALLOCATE(Mono);
        *partial_result = (Mono) { .exp = mp->exp, .p = polyAddResult };
        LISTS.pushBack(&result, partial_result);
      }
      ip = LISTS.next(ip);
      iq = LISTS.next(iq);
    }
  }
  return (Poly) { .c = p->c + q->c, .monos = result };
}

/*
* Getting const term of polynomial with setting all unnormalized
* terms c*x^0 to 0.
*/
int PolyExtractConstTermsRec(Poly* p) {
  if(LISTS.empty(&(p->monos))) {
    const int result = p->c;
    p->c = 0;
    return result;
  }
  Mono* fst = (Mono*) LISTS.first(&(p->monos));
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
int PolyInsertMono(Poly* p, Mono* new_mono) {

  if(new_mono->exp == 0) {

    const poly_coeff_t term = PolyExtractConstTermsRec(&(new_mono->p));
    p->c += term;
    new_mono->p.c = 0;
    if(PolyIsCoeff(&(new_mono->p))) {
      return 0;
    }
  }

  if(LISTS.empty(&(p->monos))) {
    LISTS.pushBack(&(p->monos), new_mono);
    return 1;
  }
  if(((Mono*)LISTS.last(&(p->monos)))->exp < new_mono->exp) {
    LISTS.pushBack(&(p->monos), new_mono);
    return 1;
  }
  if(((Mono*)LISTS.first(&(p->monos)))->exp > new_mono->exp) {
    LISTS.pushFront(&(p->monos), new_mono);
    return 1;
  }

  LOOP_LIST(&(p->monos), i) {
    ListIterator next = LISTS.next(i);
    Mono* m = (Mono*) LISTS.getValue(i);
    if(m->exp == new_mono->exp) {
      Poly pom = PolyAdd(&(m->p), &(new_mono->p));
      if(PolyIsCoeff(&pom) && PolyGetConstTerm(&pom)==0) {
        LISTS.detachElement(&(p->monos), i);
        PolyDestroy(&pom);
        MonoDestroy(m);
        MFREE(m);
        return 0;
      }
      PolyDestroy(&(m->p));
      m->p = pom;

      return 0;
    } else if(new_mono->exp > m->exp) {
      if(next == NULL) {
        LISTS.pushBack(&(p->monos), new_mono);
        return 1;
      } else if(new_mono->exp < ((Mono*)LISTS.getValue(next))->exp) {
        LISTS.insertElementAt(&(p->monos), next, new_mono);
        return 1;
      }
    }
  }
  LISTS.pushFront(&(p->monos), new_mono);
  return 1;
}

/*
* Allocates memory for monomial then tries to insert it
* using PolyInsertMono if the function fails
* then memory is automatically MFREEd.
*/
void PolyInsertMonoValue(Poly* p, Mono new_mono) {
  Mono* new_mono_ptr = MALLOCATE(Mono);

  *new_mono_ptr = new_mono;
  if(!PolyInsertMono(p, new_mono_ptr)) {
    MonoDestroy(new_mono_ptr);
    MFREE(new_mono_ptr);
  }
}

/*
* Allocates memory for monomial then tries to insert it
* using PolyInsertMono if the function fails
* then memory is automatically MFREEd.
*/
void PolyInsertMonoPtr(Poly* p, Mono* new_mono_ptr) {
  if(!PolyInsertMono(p, new_mono_ptr)) {
    MonoDestroy(new_mono_ptr);
    MFREE(new_mono_ptr);
  }
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
* Multiplies all coefficients in polynomial by const factor c
*/
void PolyScaleConst(Poly *p, const poly_coeff_t c) {
  assert(p!=NULL);
  if(c == 1) return;
  (p->c) *= c;
  LOOP_LIST(&(p->monos), iter) {
    Mono* m = (Mono*) LISTS.getValue(iter);
    PolyScaleConst(&(m->p), c);
  }
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
      Mono* partialResult = MonoClonePtr( (Mono*)LISTS.getValue(pIter) );
      PolyScaleConst(&(partialResult->p), q->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  if(p->c != 0) {
    LOOP_LIST(&(q->monos), qIter) {
      Mono* partialResult = MonoClonePtr( (Mono*)LISTS.getValue(qIter) );
      PolyScaleConst(&(partialResult->p), p->c);
      PolyInsertMonoPtr(&result, partialResult);
    }
  }

  LOOP_LIST(&(p->monos), pIter) {
    LOOP_LIST(&(q->monos), qIter) {
      Mono* mp = (Mono*) LISTS.getValue(pIter);
      Mono* mq = (Mono*) LISTS.getValue(qIter);
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
    Mono* m = (Mono*) LISTS.getValue(i);
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
  Poly qNeg = PolyNeg(q);
  Poly ret = PolyAdd(p, &qNeg);
  PolyDestroy(&qNeg);
  return ret;
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
    Mono* m = (Mono*) LISTS.getValue(i);
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
  if(p->c != q->c) {
    return false;
  }
  if(p==q) return true;
  ListIterator iq = LISTS.begin(&(q->monos));
  LOOP_LIST(&(p->monos), ip) {
    Mono* mp = (Mono*) LISTS.getValue(ip);
    Mono* mq = (Mono*) LISTS.getValue(iq);
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
    iq = LISTS.next(iq);
  }
  return true;
}

/*
* Equality test for polynomials
*/
bool PolyIsEq(const Poly *p, const Poly *q) {
  return PolyIsEqRec(p, q);
}

/*
* Calculate precisely coeff ^ exp
*/
poly_coeff_t expCoeff(poly_coeff_t coeff, poly_exp_t exp) {
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

/*
* Evaluate polynomial at a given point
*/
Poly PolyAt(const Poly *p, poly_coeff_t x) {

  assert(p!=NULL);

  Poly result = PolyFromCoeff(0);
  result.c += p->c;

  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) LISTS.getValue(i);
    poly_coeff_t factValue = expCoeff(x, m->exp);

    Poly partialResult = PolyClone(&(m->p));
    PolyScaleConst(&partialResult, factValue);
    result.c += partialResult.c;
    partialResult.c = 0;
    LOOP_LIST(&(partialResult.monos), j) {
      Mono* submono = (Mono*) LISTS.getValue(j);
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
  MFREE(varname);
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
    const Mono* m = (Mono*) LISTS.getValue(i);
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

    MFREE(wordAccumulatorCpBegin);
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
   MFREE(wordAccumulatorBegin);
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
  MFREE(str);
}
