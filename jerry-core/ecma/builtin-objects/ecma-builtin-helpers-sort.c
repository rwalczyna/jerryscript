/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecma-builtin-helpers.h"
#include "ecma-globals.h"

/**
 * Function used to merge two arrays for merge sort.
 * Arrays are stored as below:
 * First  -> array_a_p [iLeft : iRight - 1]
 * Second -> array_a_p [iRight : iEnd - 1]
 * Output -> array_b_p
 *
 * @return ecma value
 *         Returned value must be freed with ecma_free_value.
 */
static ecma_value_t
ecma_builtin_helper_array_merge_sort_bottom_up (ecma_value_t *array_a_p, /**< arrays to merge */
                                                uint32_t iLeft, /**< first array begin */
                                                uint32_t iRight, /**< first array end */
                                                uint32_t iEnd, /**< second array end */
                                                ecma_value_t *array_b_p, /**< output array */
                                                ecma_value_t compare_func, /**< compare function */
                                                const ecma_builtin_helper_sort_compare_fn_t sort_cb) /**< sorting cb */
{
  ecma_value_t ret_value = ECMA_VALUE_EMPTY;
  uint32_t i = iLeft, j = iRight;

  for (uint32_t k = iLeft; k < iEnd; k++)
  {
    ecma_value_t compare_value = ecma_make_number_value (ECMA_NUMBER_ZERO);

    if (j < iEnd)
    {
      compare_value = sort_cb (array_a_p[i], array_a_p[j], compare_func);
      if (ECMA_IS_VALUE_ERROR (compare_value))
      {
        ret_value = ECMA_VALUE_ERROR;
        break;
      }
    }

    if (i < iRight && ecma_get_number_from_value (compare_value) <= ECMA_NUMBER_ZERO)
    {
      array_b_p[k] = array_a_p[i];
      i = i + 1;
    }
    else
    {
      array_b_p[k] = array_a_p[j];
      j = j + 1;
    }
    ecma_free_value (compare_value);
  }

  return ret_value;
} /* ecma_builtin_helper_array_merge_sort_bottom_up */

/**
 * Mergesort function
 *
 * @return ecma value
 *         Returned value must be freed with ecma_free_value.
 */
ecma_value_t
ecma_builtin_helper_array_merge_sort_helper (ecma_value_t *array_p, /**< array to sort */
                                             uint32_t length, /**< length */
                                             ecma_value_t compare_func, /**< compare function */
                                             const ecma_builtin_helper_sort_compare_fn_t sort_cb) /**< sorting cb */
{
  // create array copy
  ecma_value_t ret_value = ECMA_VALUE_EMPTY;
  JMEM_DEFINE_LOCAL_ARRAY (array_copy_p, length, ecma_value_t);

  uint32_t index = 0;

  while (index < length)
  {
    array_copy_p[index] = array_p[index];
    index++;
  }

  JERRY_ASSERT (index == length);

  ecma_value_t *temp_p;
  ecma_value_t *base_array_p = array_p;
  uint32_t r, e;
  for (uint32_t w = 1; w < length; w = 2 * w)
  {
    for (uint32_t i = 0; i < length; i = i + 2 * w)
    {
      // End of first array
      r = i + w;
      if (r > length)
      {
        r = length;
      }

      // End of second array
      e = i + 2 * w;
      if (e > length)
      {
        e = length;
      }

      // Merge two arrays
      ret_value = ecma_builtin_helper_array_merge_sort_bottom_up (
        array_p, i, r, e, array_copy_p, compare_func, sort_cb);
      if (ECMA_IS_VALUE_ERROR (ret_value))
      {
        w = length;
        break;
      }
    }

    // Swap arrays
    temp_p = array_copy_p;
    array_copy_p = array_p;
    array_p = temp_p;
  }

  // Sorted array is in array_copy_p memory
  if (array_copy_p == base_array_p)
  {
    index = 0;
    temp_p = array_copy_p;
    array_copy_p = array_p;
    array_p = temp_p;

    while (index < length)
    {
      array_p[index] = array_copy_p[index];
      index++;
    }
    JERRY_ASSERT (index == length);
  }

  JMEM_FINALIZE_LOCAL_ARRAY (array_copy_p);

  return ret_value;
} /* ecma_builtin_helper_array_merge_sort_helper */
