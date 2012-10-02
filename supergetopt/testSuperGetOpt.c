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
/*********************************************************************
    This file is part of SuperGetOpt.

    SuperGetOpt is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SuperGetOpt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with SuperGetOpt.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/

#include <stdio.h>
#include "supergetopt.h"

static void usage();

int main( int argc, char *argv[] )
{
	int n, i;
	int argPos;
	
	// default values
	char c = 'A'; 
	double lf = 3.333; 
	char *s = NULL; 
	int d=0, d1=1, d2=2; 
	short h = 12; 
	float f = 15.0;
	char *ss = NULL;
	
	float farray[20];
	int numf = 20; // must be set to max initially -- will be overwritten with actual number
	char *sarray[10] = {0};
	int nums = 10;
	int helpSet = 0;
	
/* example call to supergetopt. If called with NULL argv, will print usage info */
	
	n = superGetOpt(argc,argv, &argPos,
			"-puffy %c %lf %s %d",&c, &lf, &s, &d, "help message 1",
			"-eminem %hd %f", &h, &f, "help message 2",
			"-e %d %d", &d1, &d2, "help message 3",
			"-vanna *%f", farray, &numf, "help message 4",
			"-stringo *%s", sarray, &nums, "help message 5",
			"-what %s", &ss, "help message 6",
			"-help", &helpSet, "to get this help message",
			(char * ) 0 ); 

	printf("Supergetopt returned %d argPos=%d helpSet=%d\n", n,argPos,helpSet);
	
	
	// Note: n = SG_ERROR_EXTRA_ARGS just means extra args, starting at argv[argPos]
	if( helpSet || (n < 0) ) 
	{
		usage();
		return(1);
	}
	else if( n > 0)
	{
		printf("Have %d extra Args: ", n);
		for( i = argPos ; i < argPos+n ; i++ ) printf("<%s> ", argv[i]);
		printf("\n");
	}
	printf("-puffy has c=%c double=%lf s=%s int=%d eminem has short=%hd fl=%f\n", c,lf,s,d,h,f);
	printf("nums = %d numf=%d\n", nums,numf);
	
	//printf("-stringo array=%s,%s,%s nums=%d\n", sarray[0],sarray[1],sarray[2],nums);
	for( i = 0, printf("stringo: ") ; i < nums ; i++ )
		printf("<%s> ", sarray[i]);
	printf("\n");
	for( i = 0, printf("vanna: ") ; i < numf ; i++ )
		printf("<%f> ", farray[i]);
	printf("\n");

	
	return(0);
}

static void usage()
{
	int argErr;
	
	superGetOpt(0,NULL, &argErr, NULL); // prints help info
}
