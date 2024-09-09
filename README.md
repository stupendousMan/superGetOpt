superGetOpt
===========

SuperGetOpt is a printf-like getopt replacement for command line argument handling.

The supergetopt library makes it very easy to handle all kinds of command-line arguments in a printf()-style way. 
All types of command-line arguments are supported and all user-supplied values are checked for type validity.
 
If you have been searching for an easy-to-use, lightweight, flexible, portable command-line and general use parser, this is it!

A Simple Example:
=================


    int main( int argc, char *argv[] )
    {
        int n, argPos;
        float yourFloat;
        int yourInt;
        char *yourString;

        n = superGetOpt( argc, argv, &argPos, "-YourFlag %f%s%d", &yourFloat, yourString, &yourInt, "Help Message", NULL );

        return(n);
    }


It's that easy. No header files to create, no argument order to concern yourself with.
Please read the testSuperGetOpt.c example and you'll fully understand.

Function usage:
===============

The superGetOpt() function definition:


This function has the following syntax for the three main types of command line argument handling: fixed, variable, and argless:


    int superGetOpt( int argc, char *argv[], int *argPos,
                     char *fixedFormat, *arg1, *arg2,..., "Help string fixed",
                     char *varFormat, argArray[], int *numInArray, "Help string var",
                     char *argLessFormat, int *wasThisFlagOnCmdLine, "Help string flags with no args",
                     // arbitrarily long list of format/argPtr/helpString triplets
                     NULL); // NULL is used to terminate the arg list


where "fixedFormat" is an arbitrary keyword like "--help" or "+debug" or "display" plus
a set of printf()-like % formats. “varFormat” is denoted by using a '*'  before a single %, implying that an arbitrarily long list of values of the specified format are expected (see below). 

Allowable formats are: 
======================

    %d   integer
    %hd short int
    %c   character
    %f   float
    %lf  double
    %s  char * (string)


The superGetOpt() function usage:


    int superGetOpt( argc, argv, &argPos,
                     "fixedKeyword %X %Y %Z", &argX, &argY, &argZ, "Help message fixed",
                     "varKeyword  * %X", argArrayX, &numInArrayX, " Help message var",
                     "argLessKeyword", &wasThisFlagOnCmdLine, "Help message flag with no arguments",
                     // arbitrarily long list of format/argPtr/HelpString triplets
                     NULL); // NULL is used to terminate the arg list


where %X, %Y and %Z are any of the above allowable format options.

Fixed number of arguments to a flag:

If '*' is not specified, the number arguments to a given flag is the number of %’s in the format (fixed). There must be a matching number of argument pointers passed to superGetOpt() directly after the format.

Variable number of arguments to a given flag:

If '*' is specified prior to the ‘%’, then only one '%' may appear in the format. In this case, argArray is a pointer to an array large enough to hold numInArray values, which must be set prior to the superGetOpt() call. When superGetOpt returns, numInArray will be set to the number of user supplied arguments, and argArray will be filled with those user-supplied values.

Flags without any arguments:

If the ‘%’ format is not specified, then no arguments are expected to your flag (e.g. "--help"), 
and wasThisFlagOnCmdLine will be False unless the user supplied the flag on the command line.


Return values:
==============

These are defined in supergetopt.h as follows:

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
    
In the case of an error, *argPos will be the argument number where the problem occurred. If there is not 
any error then argPos will be 0. If extra arguments are present that are not related to flag usage, then the return value will be the number of extra arguments, and argPos will be where the first one occurs.


The superParse() function:
==========================

This function is exactly the same as superGetOpt() except that argv[0] is not ignored. This makes it useful for parsing config files where each line can be turned into argv, argc format, such as with xargs. E.g.  cat configFile | xargs testSuperParse


Portability
===========

This library conforms to ANSI C specs and should be completely portable. It has been tested on Unix systems (Linux, SunOS, Solaris) and Windows XP.

History
=======

I've been using it for years!

    SuperGetOpt-1.0: Release February 3, 1999.
    SuperGetOpt-2.0: Release November 7, 2007.
    SuperGetOpt-2.1: Release Oct 1, 2012.
    SuperGetOpt-2.2: Release Oct 1, 2014.
    SuperGetOpt-2.3: Release Oct 1, 2015.
    SuperGetOpt-2.4: Release July 1, 2016.


Known Bugs
==========

Version 2.4 and greater group all flagless args and set the arg position correctly. However, for all older versions, if extra arguments are present that both precede the first flag and come after the last flag, there is no way to easily find them. You'll know how many there are, and where the first one is, but that is all.

No other bugs known. Please report bugs to the author.
