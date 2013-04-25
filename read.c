/*
 * This program will drive the function 'getQuestion()', which reads in data from the questionnaire file
 * and returns it to a single structure named question.
 *
 * Note there's a data error in the file on the web site.  The answer for question 6 doesn't match exactly
 * the option string.  Fortunately, the assert( ) routine will fire and stop the program, allowing you to
 * fix the data file.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

//#define DEBUG

#define QUESTION_NUMBER_OF_OPTIONS 6
#define QUESTION_STRING_LENGTH     100
#define MAX_QUESTION_LENGTH        250

typedef struct {
        char question[MAX_QUESTION_LENGTH + 1];
        char options[QUESTION_NUMBER_OF_OPTIONS][QUESTION_STRING_LENGTH + 1];
        int  number_of_options;
        int  answer;

} question;



// function prototypes
void *  getQuestion( const char *filename, question *qu );
int     getAnswer( char *in, question *q );
char *  substring(char *string, int position, int length);
char *  trim( char *s );
void    parseOptions( char *in, question *q );



#ifdef DEBUG
void printstring( char *s )
{
        while ( *s ) {
                printf( "'%c' %d\n" , *s, *s );
                s++;
        }
        printf( "'%c' %d\n" , *s, *s );
}
#endif



int main()
{
        question q;
        int count = 0;
        int i;

        while ( getQuestion( "english.txt", &q ) ) {
                printf( "%2d. Question: %s\n", ++count, q.question );
                printf( "     Options: (%d)\n", q.number_of_options );
                for ( i = 0; i < q.number_of_options; i++ ) {
                        printf( "         [%d] = \"%s\"\n", i, q.options[i] );
#ifdef DEBUG
                        printstring( q.options[i] );
#endif
                }
                printf( "The answer is array element %d, which is \"%s\".\n", 
                        q.answer, q.options[ q.answer ] );
                puts( "\n" );
        }
        printf( "Read in %d questions.\n", count );

        return EXIT_SUCCESS;
}



char *trim( char *s )
{
        static char tmp[ MAX_QUESTION_LENGTH + 1 ];
        int i = 0;
        int previous_was_space = 0;

        // skip any spaces at the start
        while ( isspace( *s ) ) {
                s++;
        }

        // copy data till end, allow one space only between words
        while ( *s ) { 
                if ( isspace( *s ) ) { 
                        if ( ! previous_was_space ) {
                                tmp[ i++ ] = *s;
                                previous_was_space = 1;
                        }
                } else {
                        previous_was_space = 0;
                        tmp[ i++ ] = *s;
                }
                s++;
        }


        i--;    // go back to the last character inserted in string
        while ( isspace( tmp[i] ) ) {
                i--;
        }

        tmp[i+1] = '\0';

        return tmp;
}



void parseOptions( char *in, question *q )
{
        const char *needle = "*) ";
        int i,j;

        assert ( ( in[0] == '*' ) && ( in[1] == ')' ) && ( in[2] == ' ' ) );

        i = j = 0;
        in+= 3; // skip at the start, because we know it's "*) "
        while ( in ) {
                if ( *in == '\n' ) {
                        q->options[i][j] = '\0';
                        i++;
                        break;
                }
                if   ( *in == '*' && *(in+1) == ')' && *(in+2) == ' ' ) {
                        in+=3;                          // skip the needle
                        q->options[i][j] = '\0';        // end the string
                        strcpy( q->options[i], trim( q->options[i] ) );         // remove spaces & replace
                        i++;                            // go to next string
                        assert( i < QUESTION_NUMBER_OF_OPTIONS );
                        j=0;                            // reset character loc
                }

                q->options[i][j++] = *in;
                in++;

                assert( j < QUESTION_STRING_LENGTH );
        }

        q->number_of_options = i;

        return;
}



void * getQuestion( const char *filename, question *qu )
{
        static FILE *IN = NULL;
        char buf[ MAX_QUESTION_LENGTH ];
        void *rc;

        if ( filename == NULL ) {               // reset the file, so we can read again
                IN = NULL;
                rc = NULL;
                if ( IN != NULL )
                        fclose( IN );
        }
        else
        {
                if ( IN == NULL ) {             // we've not been here before, so open the file
                        IN = fopen( filename, "r" );
                        assert ( IN != NULL );
                }

                if ( feof( IN ) ) {     // make sure we're not at the end
                        rc =  NULL;
                } 
                else 
                {

                        rc = IN;                        // default is just to return the file pointer

                        // read in question
                        fgets( buf, MAX_QUESTION_LENGTH, IN );
        #ifdef DEBUG
                        printf( "Just read in '%s'.\n", buf );
        #endif
                        assert( isalnum( buf[0] ) || ispunct( buf[0] ) );
                        strcpy( qu->question, trim(buf) );



                        // read in options
                        fgets( buf, MAX_QUESTION_LENGTH, IN );
        #ifdef DEBUG
                        printf( "Just read in '%s'.\n", buf );
        #endif
                        assert( buf[0] == '*' );
                        parseOptions( buf, qu );



                        // read in answers
                        fgets( buf, MAX_QUESTION_LENGTH, IN );
        #ifdef DEBUG
                        printf( "Just read in '%s'.\n", buf );
        #endif
                        qu->answer = getAnswer( trim(buf), qu );
                        assert( isspace( buf[0] ) );



                        // skip blank line
                        fgets( buf, MAX_QUESTION_LENGTH, IN );
        #ifdef DEBUG
                        printf( "Just read in '%s'.\n", buf );
        #endif
                        assert( isspace( buf[0] ) );

                }
        }
        return rc;
}


int getAnswer( char *in, question *q )
{
        int i = 0;

        while ( strcmp( in, q->options[i] ) != 0 ) {
                i++;
                assert( i < q->number_of_options );
        }
        return i;
}
