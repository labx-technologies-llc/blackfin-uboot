/* README

The executable called "poweroff_board" is an open source program based on the
open source program called parapin -- a Parallel Port Pin Programming
Library for Linux which can be found at
http://www.circlemud.org/~jelson/software/parapin/

Here's the source code.  I simply took the example outputtest.c and
modified it to the code below.

*/


/*
 *
 * Copyright (c) 2003 The Regents of the University of California.  All 
 * rights reserved.
 *
 * Copied and modified from outputtest.c by Bruce Giese July 14, 2004.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Neither the name of the University nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parapin.h"

int main(int argc, char *argv[])
{
  int pin_num;

  if (argc != 2) {
      printf("USAGE: <command-name> <pinnumber>\n");
      exit(1);
  }
  pin_num = atoi(argv[1]);

  if (pin_init_user(LPT1) < 0)
    exit(1);

  pin_output_mode(LP_DATA_PINS | LP_SWITCHABLE_PINS);

  printf("powering off board attached to printer port pin %d\n", pin_num);
  clear_pin(LP_PIN[pin_num]);
  exit(0);
}
