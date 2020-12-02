/* play_again0.c
 *	purpose: ask if user wants another transaction
 *	 method: ask a question, wait for yes/no answer
 *	returns: 0=>yes, 1=>no
 *	 better: eliminate need to press return 
 */

#include<stdio.h>


#define	QUESTION	"Do you want another transaction"
int get_response(char* );

int main() {

    int response = get_response(QUESTION);
    printf("response is %d\n", response);
    return response;
}

/*
 * purpose: ask a question and wait for a y/n answer
 *  method: use getchar and ignore non y/n answers
 * returns: 0=>yes, 1=>no
 */
int get_response(char* question) {
    printf("%s (y/n)?", question);
    while(1) {
        switch(getchar()) {
            case 'y':
            case 'Y':
                return 0;
            case 'n':
            case 'N':
            case EOF:
                return 1;
            
        }
    }
}