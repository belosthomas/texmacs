
/******************************************************************************
* MODULE     : Fast memory allocation
* DESCRIPTION: Fast allocations is realized by using a linked list
*              of allocations for each fixed size divisible by
*              a word legth up to MAX_FAST. Otherwise,
*              usual memory allocation is used.
* ASSUMPTIONS: The word size of the computer is 4.
*              Otherwise, change WORD_LENGTH.
* COPYRIGHT  : (C) 1999  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include "fast_alloc.hpp"

/******************************************************************************
* Statistics
******************************************************************************/

int
mem_used () {
    return 0;
}

void
mem_info () {
    // todo
}


