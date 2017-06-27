/*********************************************************************

Copyright (c) 2007, Anthony P. Russo

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Russolutions, Inc. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*********************************************************************/

#ifndef __SUPERGETOPT
#define __SUPERGETOPT

#include <stdarg.h>

/* The function prototypes you need */

#ifdef __cplusplus
extern "C" {
#endif

// for parsing commandline args
int superGetOpt( int argc, char **argv, int *lastArg, ... );

// for parsing args in a file, for instance, where argv[0] isn't ignored
int superParseOpt( int argc, char **argv, int *lastArg, ... );

#ifdef __cplusplus
}
#endif

/* control SuperGetOpt API and behaviour */
#define SG_ENABLE_HELPSTRING 1 // if enabled, each flag requires a helpString parameter from the caller
#define SG_GROUP_UNACC_ARGS 1 /* group all unaccounted for arguments so it is easy to process them */
#define SG_DEBUG 1


#define SG_ERROR_PRINT_USAGE -1
#define SG_ERROR_TOO_MANY_OPTIONS -2
#define SG_ERROR_BAD_FORMAT -3
#define SG_ERROR_BAD_FORMAT_TYPE -4
#define SG_ERROR_BAD_ARGTYPE -5
#define SG_ERROR_BAD_VARARGTYPE -6
#define SG_ERROR_INCORRECT_ARG -7
#define SG_ERROR_MISSING_ARG -8
#define SG_ERROR_NO_FORMATS -9
#define SG_ERROR_MIXED_TYPES_IN_VAR -10
#define SG_ERROR_ZERO_LEN_OPTION -11
#define SG_ERROR_TOO_MANY_ARGS -12

#endif
