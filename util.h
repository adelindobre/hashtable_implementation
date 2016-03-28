#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdlib.h>
#include <stdio.h>

#define DIE(assertion, call_description)	\
	do {	\
		if (assertion) {	\
			fprintf(stderr, "(%s, %d): ",	\
				__FILE__, __LINE__);	\
			perror(call_description);	\
			exit(EXIT_FAILURE);		\
		}	\
	} while(0)

#endif
