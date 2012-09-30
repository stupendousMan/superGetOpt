
/*********************************************************************

Copyright (c) 2007-2012, Anthony P. Russo

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

/* Supergetopt: an amazing program to handle your argument needs.
	It verifies options and the format of the arguments to them.
	Handles variable argument lists also. Prints usage info as well.
*/

// suppress MS warnings under windows
#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_DEPRECATE


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "supergetopt.h"

#define DEBUG 1

#define MAXARGS 10		/* no called function can have more than this number of args */
#define MAXOPTS 50		/* only this many options total to superGetOpt() */
#define MAXSTRING 120	/* max of any string passed through */

enum 
{
	CHAR,
	SHORT,
	INT,
	FLOAT,
	DOUBLE,
	STRING,
	NUMTYPES
};

const char typeNames[NUMTYPES][10] = { "char", "short", "int", "float", "double", "string" };

typedef union
{
	char c;
	short h;
	int i;
	float f;
	double d;
	char *string;
} ANYTYPE;

typedef union
{
	char *c;
	short *h;
	int *i;
	float *f;
	double *d;
	char **string;
} PANYTYPE;

struct optionlist_s 
{
	char name[MAXSTRING];
	int numargs;
	int varflag;
	int argtype[MAXARGS];
	PANYTYPE argptr[MAXARGS];
	ANYTYPE argval[MAXARGS];
	int *pNumArgs;
	int numArgsMax;
	char *helpString;
};


static int superParseInternal( int argc, char **argv, int usageCall,  int *lastArg, va_list ap );
static ANYTYPE getval(char *s, int type, int *flag);
static char myread_char(char *s, int *flag);
static short myread_short(char *s, int *flag);
static int myread_int(char *s, int *flag);
static float myread_float(char *s, int *flag);
static double myread_double(char *s, int *flag);
static int parse_string(char *s, struct optionlist_s *option, int *noName);
static int check_if_option(char *s, struct optionlist_s *optionlist, int numopts);
static int parse_format(char *s, int *argtypes);

int superGetOpt( int argc, char **argv, int *lastArg, ... )
{
	va_list ap;
	int n;
	int usageCall = 0;
	
	if( argv != NULL )	argv++;
	else usageCall = 1;
    if( argc > 1 ) argc--;
	else usageCall = 1;

	va_start( ap, lastArg );

	n = superParseInternal( argc, argv, usageCall, lastArg, ap );
	
	va_end( ap );
	
	//printf("n=%d lastErr=%d arc=%d\n", n,*lastArg,argc);
	if( argc == 1 && *lastArg == 1 ) n = SG_ERROR_PRINT_USAGE;
	
	return(n);
}

int superParseOpt( int argc, char **argv, int *lastArg, ... )
{
	va_list ap;
	int n;
	int usageCall = 0;
	
	if( argc == 0 || argv == NULL ) usageCall = 1;
	
	va_start( ap, lastArg );

	n = superParseInternal( argc, argv, usageCall,lastArg, ap );
	
	va_end( ap );
	
	return(n);
}

static int superParseInternal( int argc, char **argv, int usageCall, int *lastArg, va_list ap )
{
	char *optstring[MAXOPTS+1];
	static int optnum;
	int argsleft;
	register int i,j;
	int x;
	int good;
	int found = 0;
	int noName;
	static struct optionlist_s optionlist[MAXOPTS+1]; //static allows easy re-call for usage printout
	int return_val;
	int lastArgProcessed = argc;
	int lastArgProcessedSuccessfully = 1;
	int lastFlag = 0;
	
	*lastArg = 0;
	
	// To do: return correct value (error or last arg processed) in all cases
	//printf("Orig optnum = %d usageCall=%d\n", optnum,usageCall);
	return_val = 0;
	
	//optnum = 0;

	if( argv == NULL ) argc = 0;

	if( argc != 0 ) optnum = 0;

	// parse all passed-in option formats
	while( /*usageCall == 0 &&*/ (optstring[optnum] = (char *) va_arg(ap, char *)) != (char *) NULL )
	{ 
		optionlist[optnum].numargs = parse_string(optstring[optnum],&optionlist[optnum], &noName);
		
		//printf("Num args to option = %d for <%s>\n",optionlist[optnum].numargs,optstring[optnum]);
		for( i = 0 ; i < optionlist[optnum].numargs ; i++ )
		{
			//printf("Looping through numargs=%d at iter=%d var=%d\n", optionlist[optnum].numargs, i, optionlist[optnum].varflag);
			if( optionlist[optnum].varflag != 1 )
			{
				// this only works for fixed arg formats
				switch( optionlist[optnum].argtype[i] )
				{
				case CHAR: 
					optionlist[optnum].argptr[i].c = va_arg(ap, char *);
					//printf("Char: type=%d ret=%d val=0x%x\n",optionlist[optnum].argtype[i],good,optionlist[optnum].argptr[i].c);
					break;
				case SHORT: 
					optionlist[optnum].argptr[i].h = va_arg(ap, short *);
					break;
				case INT: 
					optionlist[optnum].argptr[i].i = va_arg(ap, int *);
					break;
				case FLOAT: 
					optionlist[optnum].argptr[i].f = va_arg(ap, float *);
					break;
				case DOUBLE: 
					optionlist[optnum].argptr[i].d = va_arg(ap, double *);
					break;
				case STRING: 
					optionlist[optnum].argptr[i].string = va_arg(ap, char **);
					break;
				}
			}
			else
			{
				// for vararg formats, just get pointer to array and pointer to numArgs.
				switch( optionlist[optnum].argtype[i] )
				{
				case CHAR: 
					optionlist[optnum].argptr[i].c = va_arg(ap, char *);
					if( optionlist[optnum].argptr[i].c == NULL ) return( SG_ERROR_MISSING_ARG );
					//printf("Char: type=%d ret=%d val=0x%x pc=0x%x\n",optionlist[optnum].argtype[i],good,optionlist[optnum].argptr[i].c,pC);
					break;
				case SHORT: 
					optionlist[optnum].argptr[i].h = va_arg(ap, short *);
					if( optionlist[optnum].argptr[i].h == NULL ) return( SG_ERROR_MISSING_ARG );
					break;
				case INT: 
					optionlist[optnum].argptr[i].i = va_arg(ap, int *);
					if( optionlist[optnum].argptr[i].i == NULL ) return( SG_ERROR_MISSING_ARG );
					break;
				case FLOAT: 
					optionlist[optnum].argptr[i].f = va_arg(ap, float *);
					//printf("App passed in 0x%x pointer to float array (%d,%d)\n", optionlist[optnum].argptr[i].f,optnum,i);
					if( optionlist[optnum].argptr[i].f == NULL ) return( SG_ERROR_MISSING_ARG );
					break;
				case DOUBLE: 
					optionlist[optnum].argptr[i].d = va_arg(ap, double *);
					if( optionlist[optnum].argptr[i].d == NULL ) return( SG_ERROR_MISSING_ARG );
					break;
				case STRING: 
					optionlist[optnum].argptr[i].string = va_arg(ap, char **);
					if( optionlist[optnum].argptr[i].string == NULL ) return( SG_ERROR_MISSING_ARG );
					break;
				}

				// now pop pointer to numArgs
				optionlist[optnum].pNumArgs = va_arg(ap, int *);
				optionlist[optnum].numArgsMax = *optionlist[optnum].pNumArgs;
				//printf("Varargs pNumArgs=0x%x %d\n", optionlist[optnum].pNumArgs, *optionlist[optnum].pNumArgs);
				if( optionlist[optnum].pNumArgs == NULL )
				{
					return( SG_ERROR_MISSING_ARG );
				}
				else
				{
					*optionlist[optnum].pNumArgs = 0; // initialize
				}
			}
		}
		
		// flagless arg (like -help)
		if(optionlist[optnum].numargs == 0 /*&& usageCall == 0*/)
		{
			// need to pop off the 
			//printf("flagless arg <%s>\n", optstring[optnum]);
			optionlist[optnum].argptr[0].i = va_arg(ap, int *);
			*optionlist[optnum].argptr[0].i = 0; // init
		}

#if SG_ENABLE_HELPSTRING
		// get help string
		optionlist[optnum].helpString = va_arg(ap, char *);
#endif
		optnum++;
		//printf("optnum = %d\n", optnum);
		if( optnum > MAXOPTS )
		{
#if DEBUG
			fprintf(stderr, "Too many options in string. More than %d\n",MAXOPTS);
#endif
			lastArg = (int *) va_arg(ap, int *);
			*lastArg = optnum;
			return( SG_ERR_TOO_MANY_OPTIONS );
		}
	}


	// user can tell us to print usage by calling with NULL or argc = 0 or both
    if( argv == NULL || argc == 0 /*|| usageCall == 1*/ )
	{
		argc = 0; // prevent any parsing and just print out 
		if( optnum > 0 ) fprintf(stderr, "***** Usage *****\n");
		// print out usage!!!
		//printf("Help optNum = %d\n", optnum);
		for( i = 0 ; i < optnum ; i++ )
		{
			int t;
			printf("\t %s", optionlist[i].name);
			for( t = 0 ; t < optionlist[i].numargs ; t++ )
			{
				if( optionlist[i].varflag == 0 )
				{
				    printf( " %s", typeNames[optionlist[i].argtype[t]]);
				}
				else
				{
				    printf( " %s [%s, ...]", typeNames[optionlist[i].argtype[t]], typeNames[optionlist[i].argtype[t]]);
				}
			}
#if SG_ENABLE_HELPSTRING
			if( optionlist[i].helpString != NULL )
			{
				printf(" <%s>", optionlist[i].helpString);
			}
#endif
			if( optnum > 0 ) printf("\n");
		}
		
		return(0);
	}
	
	argsleft = argc;
			
	//printf("Have argsleft=%d\n", argsleft);

	// now process cmdline argument list
	while( argsleft > 0 && usageCall == 0 )
	{

	for( i = 0, lastFlag = 0 ; i < optnum && argsleft > 0 ; i++ )
	{
		//printf("Looking for option %d <%s>: argv=<%s> argsleft=%d\n", i, optionlist[i].name,argv[0],argsleft);
		found = 0;
		if( strcmp( argv[0], optionlist[i].name ) == 0 )
		{
			lastFlag = i;
			
			found = 1;
			argsleft--;	
			lastArgProcessedSuccessfully++;		
			if( argsleft > 0 ) argv++;
			//printf("Found option <%s>\n", optionlist[i].name);
			
			if( optionlist[i].numargs == 0 )
			{
				// handle flagless arg like -help
				*optionlist[i].argptr[0].i = 1;
			}
			
			for( j = 0 ; (j < optionlist[i].numargs && optionlist[i].varflag != 1 && argsleft > 0 ) || (optionlist[i].varflag == 1 && argsleft > 0) ; j++, argsleft--, argv++ )
			{
				if( optionlist[i].varflag != 1 )
				{
					optionlist[i].argval[j] = getval(argv[0], optionlist[i].argtype[j], &good);
					switch( optionlist[i].argtype[j] )
					{
						case CHAR: 
							*optionlist[i].argptr[j].c = optionlist[i].argval[j].c;
							//printf("got char: %c\n",optionlist[i].argval[j].c);
							break;
						case SHORT: 
							*optionlist[i].argptr[j].h = optionlist[i].argval[j].h;
							break;
						case INT: 
							*optionlist[i].argptr[j].i = optionlist[i].argval[j].i;
							break;
						case FLOAT: 
							*optionlist[i].argptr[j].f = optionlist[i].argval[j].f;
							break;
						case DOUBLE: 
							*optionlist[i].argptr[j].d = optionlist[i].argval[j].d;
							break;
						case STRING: 
							//printf("For string, argval=0x%x argv=0x%x\n", optionlist[i].argval[j].string,argv[0]);
							*optionlist[i].argptr[j].string = optionlist[i].argval[j].string;
/* 							if( check_if_option(argv[0], optionlist, optnum) >= 0 && optionlist[i].varflag == 1 ) */
/* 								good = -1; */
							break;
						default: 
#if DEBUG
							fprintf(stderr, "Bad argtype %d\n",optionlist[i].argtype[j]); 
#endif
							//*lastArg = lastArgProcessed;
							*lastArg = lastArgProcessedSuccessfully;
							return( SG_ERROR_BAD_ARGTYPE );
					}
					
					if( good == 0 )
					{
						lastArgProcessed = argc - argsleft + 1;
						lastArgProcessedSuccessfully++;		
						//printf("good read: lastArg=%d\n",lastArgProcessed);
					}
					else if( good == -1 )
					{
						x = check_if_option(argv[0], optionlist, optnum);
						if( x < 0 )
						{
#if DEBUG
							fprintf(stderr,"User did not supply correct arguments to option name <%s>\n",optionlist[i].name);
#endif
							//*lastArg = lastArgProcessed;
							*lastArg = lastArgProcessedSuccessfully;
							return(SG_ERROR_INCORRECT_ARG);
						}
						else 
						{
#if DEBUG
							fprintf(stderr,"User did not supply enough arguments to option name <%s>\n",optionlist[i].name);
#endif
							//*lastArg = lastArgProcessed;
							*lastArg = lastArgProcessedSuccessfully;
							return( SG_ERROR_MISSING_ARG );
						}
					}
				}
				else		/* var arg list */
				{
					//optionlist[i].argtype[j] = optionlist[i].argtype[0];
					switch( optionlist[i].argtype[0] )
					{
						case CHAR: 
							optionlist[i].argptr[0].c[j] = myread_char(argv[0],&good); 
							break;
						case SHORT: 
							optionlist[i].argptr[0].h[j] = myread_short(argv[0],&good); 
							break;
						case INT: 
							optionlist[i].argptr[0].i[j] = myread_int(argv[0],&good); 
							break;
						case FLOAT:
							optionlist[i].argptr[0].f[j] = myread_float(argv[0],&good); 
							break;
						case DOUBLE: 
							optionlist[i].argptr[0].d[j] = myread_double(argv[0],&good); 
							break;
						case STRING: 
							x = check_if_option(argv[0], optionlist, optnum);
 							if( x >= 0 ) /* end of var list */
							{
								good = -2;
							}
							else
							{	
								good = 0;
								if( j < optionlist[i].numArgsMax )
								{
									optionlist[i].argptr[0].string[j] = argv[0]; 
								}
								else
								{
									good = -3;
								}
							} 
								break;
						default: 
#if DEBUG
							fprintf(stderr, "Bad varargtype %d\n",optionlist[i].argtype[j]); 
#endif
							//*lastArg = lastArgProcessed;
							*lastArg = lastArgProcessedSuccessfully;
							return( SG_ERROR_BAD_VARARGTYPE );
					}
					
					if( good == 0 ) // good read
					{
						*optionlist[i].pNumArgs = j+1;
						lastArgProcessed = argc - argsleft + 1;
						lastArgProcessedSuccessfully++;		
						//printf("2 good read: lastArg=%d\n",lastArgProcessed);
					}
					
					if( good == -1 )	/* bad data type */
					{
						x = check_if_option(argv[0], optionlist, optnum);
						if( x < 0 )
						{
#if DEBUG
							fprintf(stderr, "Var arg list bad data type for option <%s>\n",optionlist[i].name);
#endif
							//*lastArg = lastArgProcessed;
							*lastArg = lastArgProcessedSuccessfully;
							return( SG_ERROR_INCORRECT_ARG );
						}
						else	/* next option detected -- end of var list -- move 1 arg back */
						{
							optionlist[i].numargs = j;
							break;		/* get out of numargs loop and call function */
						}
					}
					else if( good == -2 )	/* detected end of var list -- move 1 arg back */
					{
						/* Var arg string list terminates at next option */
						optionlist[i].numargs = j;
						break;		/* get out of numargs loop and call function */
					}
					else if( good == -3 ) // too many args
					{
#if DEBUG
						fprintf(stderr, "Warning: too many commandline args supplied for option <%s>. Max=%d\n",optionlist[i].name,optionlist[i].numArgsMax);
#endif
					}
					else if( argsleft == 1 )	/* end of command line ends var arg list */
					{
						optionlist[i].numargs = j+1;
					}
				}
			}

			if( j != optionlist[i].numargs && optionlist[i].varflag != 1 )
			{
#if DEBUG
				fprintf(stderr,"User did not supply enough arguments to option name <%s> Expected %d Got %d\n",optionlist[i].name,optionlist[i].numargs,j);
#endif
				//*lastArg = lastArgProcessed;
				*lastArg = lastArgProcessedSuccessfully;
				return( SG_ERROR_MISSING_ARG );
			}
			i = -1;  /* experimental bug fix? */
		}
		else
		{
			found = 0;
			//printf("last successful arg = %d\n",lastArgProcessedSuccessfully);
		}
	}

	if( found == 0 )
	{
		//fprintf(stderr,"option not found at argv=%s left=%d\n",argv[0],argsleft);
#if DEBUG
		//fprintf(stderr,"User did not supply option name <%s>\n",optionlist[lastFlag].name);
#endif
		//*lastArg = lastArgProcessed+1;
		*lastArg = lastArgProcessedSuccessfully;
		return( SG_ERROR_TOO_MANY_ARGS );
		if( argsleft > 0 ) argv++;
		argsleft--;
	}
	
	}

	*lastArg = lastArgProcessedSuccessfully;
	//return_val = lastArgProcessed;
	
	return( return_val );
}


static int parse_string(char *s, struct optionlist_s *option, int *noName)
{
    size_t len;
	int z;
	int offset = 0;
	char *pN, *pM;

	//printf("Called parse_string: s = <%s>\n", s);
	
	len = strlen( s );

	if( len <= 0 )
		return(0);

	if( len < 2 )
	{
#if DEBUG
		fprintf(stderr, "Parse_string: option has zero length <%s> len=%d\n", s, (int) len);
#endif
		return(SG_ERROR_ZERO_LEN_OPTION);
	}

	pN = strstr( s, "%" );	/* explicit list implied */
	pM = strstr( s, "*" );	/* variable argument list implied */

	if( pM != NULL )		/* vararg list implied */
	{
		option->varflag = 1;
		offset = 1;
		//printf("var found\n");
	}
	else if( pN != NULL )	/* explicit list implied */
	{
		option->varflag = 0;
		offset = 0;
	}
	else
	{
	}
	
	if( pN != NULL )
	{
		strncpy(option->name, s, pN-s);
		option->name[strlen(s)-strlen(pN)-1-offset] = '\0';

		if( pN == s )		/* no name ==> wildcard */
			*noName = 1;
		else
			*noName = 0;

		if( pN-s >= len - 1 )
		{
#if DEBUG
			fprintf(stderr, "Parse_String: No formats given in <%s>\n",s /* was scopy */);
#endif
			return(SG_ERROR_NO_FORMATS);
		}
		else
		{
			if( option->varflag == 0 )
			{
				return( parse_format( pN, option->argtype ) );
			}
			else
			{
				if( (z = parse_format( pN, option->argtype )) == 0 )
				{
#if DEBUG
					fprintf(stderr,"var arg option but no valid var arg list\n");
#endif
					return(0);  /* newly added 9-27-92. bug if gave vararg option but no list */
				}
				else if( z < 0 )
				{
#if DEBUG
					fprintf(stderr,"Incorrect var arg list\n");
#endif
					return(SG_ERROR_MIXED_TYPES_IN_VAR);
				}
				
				return( z );
			}
		}
	}
	else
	{
		// no arguments to this flag. Assume 1 int to be returned.
		//printf("no_arg flag found <%s>\n",s);
		
		option->varflag = 0;
		*noName = 0;
		strcpy( option->name, s );
		return( 0 );
	}

}

static int parse_format(char *s, int *argtypes)
{
	char *scopy, *sp, string[MAXSTRING];
	int len;
	int i, numargs;
	static char *arg[MAXARGS];
	
	//printf("parse_format: s = <%s>\n", s);
	
	sp = strstr(s,"%");	
	if( sp == NULL )
	{
#if DEBUG
		fprintf(stderr, "Bad format in <%s>. No %% sign.\n", s);
#endif
		return(SG_ERROR_BAD_FORMAT);
	}

	len = strlen( sp );
	scopy = malloc( len + 1 );
	strcpy( scopy, sp );
	//scopy = (char *) strdup( sp ); // eliminate strdup call

	for( numargs = 0, arg[numargs] = strtok(scopy, "%");
		(arg[++numargs] = strtok( (char *) NULL, "%")) != NULL && numargs < MAXARGS-1; )
	{
		//printf("strtok iteration: numargs=%d for <%s>\n",numargs, s);
		;
	}
		//printf("strtok iteration: numargs=%d for <%s>\n",numargs, s);

	for( i = 0 ; i < numargs ; i++ )
	{
		strcpy(string,"%");
		strcat(string, arg[i]);

		if( strstr(string, "%f") != NULL )
			argtypes[i] = (int) FLOAT;
		else
		if( strstr(string, "%lf") != NULL )
			argtypes[i] = (int) DOUBLE;
		else
		if( strstr(string, "%c") != NULL )
			argtypes[i] = (int) CHAR;
		else
		if( strstr(string, "%hd") != NULL )
			argtypes[i] = (int) SHORT;
		else
		if( strstr(string, "%d") != NULL )
			argtypes[i] = (int) INT;
		else
		if( strstr(string, "%s") != NULL )
			argtypes[i] = (int) STRING;
		else
		{
#if DEBUG
			fprintf(stderr, "Parse_Format: Bad format <%s> len=%d arg0=%s\n",string,len,arg[0]);
#endif
			
			free(scopy); /* memleak!!!! */
			
			return(SG_ERROR_BAD_FORMAT_TYPE);
		}
	}
	
	free(scopy); /* memleak!!!! */

	//printf("final numargs=%d for <%s>\n",numargs, s);
	return( numargs );
}


static ANYTYPE getval(char *s, int type, int *flag)
{
	ANYTYPE value;
	value.d = 0.0;

	*flag = 0;

	switch( type )
	{
		case CHAR:
			if( sscanf( s, "%c", &value.c ) != 1 )
			{
#if DEBUG
				fprintf(stderr," Getval: Bad argument. Expected character\n");
#endif
				*flag = -1;
				return( value );
			}
			else  return( value ); 	
		case SHORT:
			if( sscanf( s, "%hd", &value.h ) != 1 )
			{
#if DEBUG
				fprintf(stderr," Getval: Bad argument. Expected short\n");
#endif
				*flag = -1;
				return( value );
			}
			else return( value ); 	
		case INT:
			if( sscanf( s, "%d", &value.i ) != 1 )
			{
#if DEBUG
				fprintf(stderr," Getval: Bad argument. Expected integer\n");
#endif
				*flag = -1;
				return( value );
			}
			else return( value ); 	
		case FLOAT:
			if( sscanf( s, "%f", &value.f ) != 1 )
			{
#if DEBUG
				fprintf(stderr," Getval: Bad argument. Expected float\n");
#endif
				*flag = -1;
				return( value );
			}
			else return( value ); 	
		case DOUBLE:
			if( sscanf( s, "%lf", &value.d ) != 1 )
			{
#if DEBUG
				fprintf(stderr," Getval: Bad argument. Expected double\n");
#endif
				*flag = -1;
				return( value );
			}
			else return( value ); 	
		case STRING:
			//strcpy( value.string, s );
			value.string = s;
			return( value );
		default:
#if DEBUG
			fprintf(stderr, "Getval: Bad argument type %d\n",type);
#endif
			*flag = -1;
			return( value );
	} 	
}

static char myread_char(char *s, int *flag)
{
	char x;
	*flag = 0;
	if( sscanf( s, "%c", &x ) != 1 || strlen(s) > 1 )
	{
		*flag = -1;
	}
	return( x );
} 	
static short myread_short(char *s, int *flag)
{
	short x;
	*flag = 0;
	if( sscanf( s, "%hd", &x ) != 1 )
	{
		*flag = -1;
	}
	return( x );
} 	
static int myread_int(char *s, int *flag)
{
	int x;
	*flag = 0;
	if( sscanf( s, "%d", &x ) != 1 )
	{
		*flag = -1;
	}
	return( x );
} 	
static float myread_float(char *s, int *flag)
{
	float x;
	*flag = 0;
	if( sscanf( s, "%f", &x ) != 1 )
	{
		*flag = -1;
	}
	return( x );
} 	
static double myread_double(char *s, int *flag)
{
	double x;
	*flag = 0;
	if( sscanf( s, "%lf", &x ) != 1 )
	{
		*flag = -1;
	}
	return( x );
} 	
 
static int check_if_option(char *s, struct optionlist_s *optionlist, int numopts)
{

	register int i;

	for( i = 0 ; i < numopts ; i++ )
	{
		if( strcmp( s, optionlist[i].name ) == 0 )
			return( i );
	}

	return( -1 );
}
