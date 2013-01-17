/* This file is part of the 'stringi' library.
 * 
 * Copyright 2013 Marek Gagolewski, Bartek Tartanus, Marcin Bujarski
 * 
 * 'stringi' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * 'stringi' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with 'stringi'. If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "stringi.h"


/** get char types for each code point UTF-8 string
 * @param s UTF-8 encoded string
 * @param n number of bytes
 * @param codes [OUT] the char types/codes; array of length >= n
 * @param ncodes [OUT] number of code points found
 */
void stri__uchar_charType(const char* s, int n, int* codes, int& ncodes)
{
   UChar32 c;
   ncodes=0;
   for (int i=0; i<n; ) {
      U8_NEXT(s, i, n, c);
      codes[ncodes++] =  u_charType(c);
   }
}


/** Get character types
 * @param s character vector
 * @return list of integer vectors
 * @TODO other encodings..................
 */
SEXP stri_chartype(SEXP s)
{
   s = stri_prepare_arg_string(s);
   R_len_t ns = LENGTH(s);
   if (ns <= 0) return R_NilValue;
   
   R_len_t bufsize = stri__numbytes_max(s);
   int* buf = 0;
   if (bufsize > 0) buf = new int[bufsize];
   
   SEXP ret;
   PROTECT(ret = allocVector(VECSXP, ns));
   for (R_len_t i=0; i<ns; ++i) {
      SEXP ns = STRING_ELT(s, i);
      if (ns == NA_STRING)
         SET_VECTOR_ELT(ret, i, ScalarInteger(NA_INTEGER)); // hmmmm..... integer(0)?
      else {
         R_len_t cns = LENGTH(ns);
         int ncodes;
         stri__uchar_charType(CHAR(ns), cns, buf, ncodes);
         SEXP codes;
         PROTECT(codes = allocVector(INTSXP, ncodes));
         int* int_codes = INTEGER(codes);
         for (int j=0; j<ncodes; ++j)
            int_codes[j] = buf[j];
         UNPROTECT(1);
         SET_VECTOR_ELT(ret, i, codes);
      }
   }
   if (buf) delete [] buf;
   UNPROTECT(1);
   return ret;
}


/** Get general character categories
 * Based on ICU4C/uchar.h
 * U_UNASSIGNED (id=0) is omitted
 * @return list of 3 vectors (integer, character, character)
 */
SEXP stri_charcategories()
{
   const R_len_t numcolumns = 3;
   const R_len_t numcategories = U_CHAR_CATEGORY_COUNT-1;
   SEXP vals;
   SEXP names;
   PROTECT(names = allocVector(STRSXP, numcolumns));
   SET_STRING_ELT(names, 0, mkChar("Identifier"));
   SET_STRING_ELT(names, 1, mkChar("CategoryFull"));
   SET_STRING_ELT(names, 2, mkChar("Category"));
   
   SEXP id;
   SEXP cat;
   SEXP cat2;
   PROTECT(id   = allocVector(INTSXP, numcategories));
   PROTECT(cat  = allocVector(STRSXP, numcategories));
   PROTECT(cat2 = allocVector(STRSXP, numcategories));
   
#define STRI_CHARCATEGORIES_CREATE(x, y, z) \
   INTEGER(id)[x-1] = x; \
   SET_STRING_ELT(cat,  x-1, mkChar(y)); \
   SET_STRING_ELT(cat2, x-1, mkChar(z)); \

  
//STRI_CHARCATEGORIES_CREATE( U_UNASSIGNED,             "UNASSIGNED",               "Cn" )
STRI_CHARCATEGORIES_CREATE( U_UPPERCASE_LETTER,       "UPPERCASE_LETTER",         "Lu" )
STRI_CHARCATEGORIES_CREATE( U_LOWERCASE_LETTER,       "LOWERCASE_LETTER",         "Ll" )
STRI_CHARCATEGORIES_CREATE( U_TITLECASE_LETTER,       "TITLECASE_LETTER",         "Lt" )
STRI_CHARCATEGORIES_CREATE( U_MODIFIER_LETTER,        "MODIFIER_LETTER",          "Lm" )
STRI_CHARCATEGORIES_CREATE( U_OTHER_LETTER,           "OTHER_LETTER",             "Lo" )
STRI_CHARCATEGORIES_CREATE( U_NON_SPACING_MARK,       "NON_SPACING_MARK",         "Mn" )        
STRI_CHARCATEGORIES_CREATE( U_ENCLOSING_MARK,         "ENCLOSING_MARK",           "Me" )
STRI_CHARCATEGORIES_CREATE( U_COMBINING_SPACING_MARK, "COMBINING_SPACING_MARK",   "Mc" )
STRI_CHARCATEGORIES_CREATE( U_DECIMAL_DIGIT_NUMBER,   "DECIMAL_DIGIT_NUMBER",     "Nd" )
STRI_CHARCATEGORIES_CREATE( U_LETTER_NUMBER,          "LETTER_NUMBER",            "Nl" )
STRI_CHARCATEGORIES_CREATE( U_OTHER_NUMBER,           "OTHER_NUMBER",             "No" )
STRI_CHARCATEGORIES_CREATE( U_SPACE_SEPARATOR,        "SPACE_SEPARATOR",          "Zs" )
STRI_CHARCATEGORIES_CREATE( U_LINE_SEPARATOR,         "LINE_SEPARATOR",           "Zl" )
STRI_CHARCATEGORIES_CREATE( U_PARAGRAPH_SEPARATOR,    "PARAGRAPH_SEPARATOR",      "Zp" )
STRI_CHARCATEGORIES_CREATE( U_CONTROL_CHAR,           "CONTROL_CHAR",             "Cc" )
STRI_CHARCATEGORIES_CREATE( U_FORMAT_CHAR,            "FORMAT_CHAR",              "Cf" )
STRI_CHARCATEGORIES_CREATE( U_PRIVATE_USE_CHAR,       "PRIVATE_USE_CHAR",         "Co" )
STRI_CHARCATEGORIES_CREATE( U_SURROGATE,              "SURROGATE",                "Cs" )
STRI_CHARCATEGORIES_CREATE( U_DASH_PUNCTUATION,       "DASH_PUNCTUATION",         "Pd" )
STRI_CHARCATEGORIES_CREATE( U_START_PUNCTUATION,      "START_PUNCTUATION",        "Ps" )
STRI_CHARCATEGORIES_CREATE( U_END_PUNCTUATION,        "END_PUNCTUATION",          "Pe" )
STRI_CHARCATEGORIES_CREATE( U_CONNECTOR_PUNCTUATION,  "CONNECTOR_PUNCTUATION",    "Pc" )
STRI_CHARCATEGORIES_CREATE( U_OTHER_PUNCTUATION,      "OTHER_PUNCTUATION",        "Po" )
STRI_CHARCATEGORIES_CREATE( U_MATH_SYMBOL,            "MATH_SYMBOL",              "Sm" )
STRI_CHARCATEGORIES_CREATE( U_CURRENCY_SYMBOL,        "CURRENCY_SYMBOL",          "Sc" )
STRI_CHARCATEGORIES_CREATE( U_MODIFIER_SYMBOL,        "MODIFIER_SYMBOL",          "Sk" )
STRI_CHARCATEGORIES_CREATE( U_OTHER_SYMBOL,           "OTHER_SYMBOL",             "So" )
STRI_CHARCATEGORIES_CREATE( U_INITIAL_PUNCTUATION,    "INITIAL_PUNCTUATION",      "Pi" )
STRI_CHARCATEGORIES_CREATE( U_FINAL_PUNCTUATION,      "FINAL_PUNCTUATION",        "Pf" )

   PROTECT(vals = allocVector(VECSXP, numcolumns));
   SET_VECTOR_ELT(vals, 0, id);
   SET_VECTOR_ELT(vals, 1, cat);
   SET_VECTOR_ELT(vals, 2, cat2);
   
   setAttrib(vals, R_NamesSymbol, names);
   UNPROTECT(5);
   return vals;
}


/** Get Unicode character General Category Mask
 *  @param x One- or two-letter category name
 *  @return integer mask
 */
SEXP stri_char_getcategorymask(SEXP x)
{
   x = stri_prepare_arg_string(x);
   if (LENGTH(x) == 0) error("please provide category name");
   else if (LENGTH(x) > 1) warning("only one name supported. taking first");
   SEXP xf = STRING_ELT(x, 0);
   if (xf == NA_STRING || LENGTH(xf) == 0 || LENGTH(xf) > 2)
      error("incorrect category name");
   
   const char* name = CHAR(xf);
   int id = NA_INTEGER;
   switch(name[0]) {
      case 'C':
         switch (name[1]) {
            case 'n':  id = U_GC_CN_MASK; break;
            case 'c':  id = U_GC_CC_MASK; break;
            case 'f':  id = U_GC_CF_MASK; break;
            case 'o':  id = U_GC_CO_MASK; break;
            case 's':  id = U_GC_CS_MASK; break;
            case '\0': id = U_GC_C_MASK;  break;
         }
         break;
         
      case 'L':
         switch (name[1]) {
            case 'u':  id = U_GC_LU_MASK; break;
            case 'l':  id = U_GC_LL_MASK; break;
            case 't':  id = U_GC_LT_MASK; break;
            case 'm':  id = U_GC_LM_MASK; break;
            case 'o':  id = U_GC_LO_MASK; break;
            case 'c':  id = U_GC_LC_MASK; break;
            case '\0': id = U_GC_L_MASK;  break;
         }
         break;
         
      case 'M':
         switch (name[1]) {
            case 'n':  id = U_GC_MN_MASK; break;
            case 'e':  id = U_GC_ME_MASK; break;
            case 'c':  id = U_GC_MC_MASK; break;
            case '\0': id = U_GC_M_MASK;  break;
         }
         break;
         
      case 'N':
         switch (name[1]) {
            case 'd':  id = U_GC_ND_MASK; break;
            case 'l':  id = U_GC_NL_MASK; break;
            case 'o':  id = U_GC_NO_MASK; break;
            case '\0': id = U_GC_N_MASK;  break;
         }
         break;
         
      case 'P':
         switch (name[1]) {
            case 'd':  id = U_GC_PD_MASK; break;
            case 's':  id = U_GC_PS_MASK; break;
            case 'e':  id = U_GC_PE_MASK; break;
            case 'c':  id = U_GC_PC_MASK; break;
            case 'o':  id = U_GC_PO_MASK; break;
            case 'i':  id = U_GC_PI_MASK; break;
            case 'f':  id = U_GC_PF_MASK; break;
            case '\0': id = U_GC_P_MASK;  break;
         }
         break;
         
      case 'S':
         switch (name[1]) {
            case 'm':  id = U_GC_SM_MASK; break;
            case 'c':  id = U_GC_SC_MASK; break;
            case 'k':  id = U_GC_SK_MASK; break;
            case 'o':  id = U_GC_SO_MASK; break;
            case '\0': id = U_GC_S_MASK;  break;
         }
         break;
         
      case 'Z':
         switch (name[1]) {
            case 's':  id = U_GC_ZS_MASK; break;
            case 'l':  id = U_GC_ZL_MASK; break;
            case 'p':  id = U_GC_ZP_MASK; break;
            case '\0': id = U_GC_Z_MASK;  break;
         }
         break;
   }
   
   if (id == NA_INTEGER) {
      error("incorrect category name");  
   }
   
   return ScalarInteger(id);
}


/**
 *  Get Unicode character binary property identifier
 *  @param ...
 *  @return
 */
SEXP stri_char_getbinaryproperty(SEXP x)
{
   return R_NilValue;
   
}
