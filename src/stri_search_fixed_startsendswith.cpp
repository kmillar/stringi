/* This file is part of the 'stringi' package for R.
 * Copyright (c) 2013-2014, Marek Gagolewski and Bartek Tartanus
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "stri_stringi.h"
#include "stri_container_utf8_indexable.h"
#include "stri_container_integer.h"


/**
 * Detect if a string starts with a pattern match
 *
 * @param str character vector
 * @param pattern character vector
 * @param from integer vector
 * @return logical vector
 *
 * @version 0.3-1 (Marek Gagolewski, 2014-06-03)
 */
SEXP stri_startswith_fixed(SEXP str, SEXP pattern, SEXP from)
{
   str = stri_prepare_arg_string(str, "str");
   pattern = stri_prepare_arg_string(pattern, "pattern");
   from = stri_prepare_arg_integer(from, "from");

   STRI__ERROR_HANDLER_BEGIN
   int vectorize_length = stri__recycling_rule(true, 3,
      LENGTH(str), LENGTH(pattern), LENGTH(from));
   StriContainerUTF8_indexable str_cont(str, vectorize_length);
   StriContainerUTF8 pattern_cont(pattern, vectorize_length);
   StriContainerInteger from_cont(from, vectorize_length);

   SEXP ret;
   STRI__PROTECT(ret = Rf_allocVector(LGLSXP, vectorize_length));
   int* ret_tab = LOGICAL(ret);

   for (R_len_t i = pattern_cont.vectorize_init();
         i != pattern_cont.vectorize_end();
         i = pattern_cont.vectorize_next(i))
   {
      STRI__CONTINUE_ON_EMPTY_OR_NA_STR_PATTERN(str_cont, pattern_cont,
         ret_tab[i] = NA_LOGICAL,
         ret_tab[i] = FALSE)
         
      if (from_cont.isNA(i)) {
         ret_tab[i] = NA_LOGICAL;
         continue;
      }
      
      R_len_t from_cur = from_cont.get(i);
      if (from_cur >= 0)
         from_cur = str_cont.UChar32_to_UTF8_index_fwd(i, from_cur-1);                                                           
      else
         from_cur = str_cont.UChar32_to_UTF8_index_back(i, -from_cur); 


      const char* str_cur_s = str_cont.get(i).c_str();
      R_len_t     str_cur_n = str_cont.get(i).length();
      const char* pattern_cur_s = pattern_cont.get(i).c_str();
      R_len_t     pattern_cur_n = pattern_cont.get(i).length();
      
      if (str_cur_n-from_cur < pattern_cur_n)
         ret_tab[i] = FALSE;
      else {
         ret_tab[i] = TRUE;
         for (R_len_t j=0; j<pattern_cur_n; ++j) {
            if (str_cur_s[j+from_cur] != pattern_cur_s[j]) {
               ret_tab[i] = FALSE;
               break;
            }
         }
      }
   }

   STRI__UNPROTECT_ALL
   return ret;
   STRI__ERROR_HANDLER_END( ;/* do nothing special on error */ )
}


/**
 * Detect if a string ends with a pattern match
 *
 * @param str character vector
 * @param pattern character vector
 * @param to integer vector
 * @return logical vector
 *
 * @version 0.3-1 (Marek Gagolewski, 2014-06-03)
 */
SEXP stri_endswith_fixed(SEXP str, SEXP pattern, SEXP to)
{
   str = stri_prepare_arg_string(str, "str");
   pattern = stri_prepare_arg_string(pattern, "pattern");
   to = stri_prepare_arg_integer(to, "to");

   STRI__ERROR_HANDLER_BEGIN
   int vectorize_length = stri__recycling_rule(true, 3,
      LENGTH(str), LENGTH(pattern), LENGTH(to));
   StriContainerUTF8_indexable str_cont(str, vectorize_length);
   StriContainerUTF8 pattern_cont(pattern, vectorize_length);
   StriContainerInteger to_cont(to, vectorize_length);

   SEXP ret;
   STRI__PROTECT(ret = Rf_allocVector(LGLSXP, vectorize_length));
   int* ret_tab = LOGICAL(ret);

   for (R_len_t i = pattern_cont.vectorize_init();
         i != pattern_cont.vectorize_end();
         i = pattern_cont.vectorize_next(i))
   {
      STRI__CONTINUE_ON_EMPTY_OR_NA_STR_PATTERN(str_cont, pattern_cont,
         ret_tab[i] = NA_LOGICAL,
         ret_tab[i] = FALSE)
         
      if (to_cont.isNA(i)) {
         ret_tab[i] = NA_LOGICAL;
         continue;
      }
      
      R_len_t to_cur = to_cont.get(i);
      if (to_cur >= 0)
         to_cur = str_cont.UChar32_to_UTF8_index_fwd(i, to_cur);                                                           
      else
         to_cur = str_cont.UChar32_to_UTF8_index_back(i, -to_cur-1); 


      const char* str_cur_s = str_cont.get(i).c_str();
//      R_len_t     str_cur_n = str_cont.get(i).length();
      const char* pattern_cur_s = pattern_cont.get(i).c_str();
      R_len_t     pattern_cur_n = pattern_cont.get(i).length();
      
      if (to_cur - pattern_cur_n < 0)
         ret_tab[i] = FALSE;
      else {
         ret_tab[i] = TRUE;
         for (R_len_t j=0; j<pattern_cur_n; ++j) {
            if (str_cur_s[to_cur-pattern_cur_n+j] != pattern_cur_s[j]) {
               ret_tab[i] = FALSE;
               break;
            }
         }
      }
   }

   STRI__UNPROTECT_ALL
   return ret;
   STRI__ERROR_HANDLER_END( ;/* do nothing special on error */ )
}