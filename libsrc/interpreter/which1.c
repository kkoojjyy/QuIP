#include "quip_config.h"

#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "quip_prot.h"
#include "query_stack.h"
#include "query_prot.h"
#include "warn.h"
#include "history.h"

#ifdef HAVE_HISTORY
static void check_preload(QSP_ARG_DECL  const char *prompt, int n, const char **choices)
{
	const char *formatted_prompt;

	if( ! IS_COMPLETING(THIS_QSP) ) return;
	if( ! intractive(SINGLE_QSP_ARG) ) return;
	if( *prompt == 0 ) return;

	formatted_prompt = format_prompt(QSP_ARG  prompt);

	preload_history_list(QSP_ARG  formatted_prompt,n,choices);
}
#endif /* HAVE_HISTORY */

static int _one_of(QSP_ARG_DECL  const char *prompt, int n, const char** choices)
{
	int i;
	int nmatches=0;
	int lastmatch=(-1);	/* init to elim warning */
	const char *user_response;

#ifdef HAVE_HISTORY
	check_preload(QSP_ARG  prompt, n, choices);
#endif /* HAVE_HISTORY */

	user_response = NAMEOF(prompt);

	for(i=0;i<n;i++){
		assert(choices[i]!=NULL);
		if( !strcmp( user_response, choices[i] ) ){
			return(i);
		}
	}
	
	/* if no exact match check for substring match */
	for(i=0;i<n;i++)
		if( is_a_substring( user_response, choices[i] ) ){
			lastmatch=i;
			nmatches++;
		}
	if( nmatches==1 ){
		sprintf(ERROR_STRING,"Unambiguous substring match of \"%s\" to \"%s\"",
			user_response,choices[lastmatch]);
		//advise(ERROR_STRING);
		WARN(ERROR_STRING);
		return(lastmatch);
	}
	else if( nmatches > 1 ){
		sprintf(ERROR_STRING,"ambiguous choice \"%s\"",user_response);
		WARN(ERROR_STRING);
		return(-1);
	}

	sprintf(ERROR_STRING,"invalid choice \"%s\"",user_response);
	WARN(ERROR_STRING);
	sprintf(ERROR_STRING,"valid selections for %s are:",prompt);
	advise(ERROR_STRING);
	for(i=0;i<n;i++){
		sprintf(ERROR_STRING,"\t%s",choices[i]);
		advise(ERROR_STRING);
	}
#ifdef HAVE_HISTORY
	if( intractive(SINGLE_QSP_ARG) ) rem_def(QSP_ARG  prompt,user_response) ;
#endif /* HAVE_HISTORY */

	return(-1);
}

/* sometimes we would like to repetetively prompt the user
 * for a word from the list, and other times not!?
 */

int which_one(QSP_ARG_DECL  const char *prompt,int n,const char** choices)
{
	return _one_of(QSP_ARG prompt,n,choices);
}


int which_one2(QSP_ARG_DECL  const char* s,int n,const char** choices)
{
	inhibit_next_prompt_format(SINGLE_QSP_ARG);
	return _one_of(QSP_ARG s,n,choices);
}


