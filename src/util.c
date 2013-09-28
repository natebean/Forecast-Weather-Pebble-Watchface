#include "util.h"
#include "receive_keys.h"
#include "pebble_os.h"
#include "pebble_app.h"

#define INT_DIGITS 5		/* enough for 64 bit integer */

char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

int util_atoi(char *str)
{
   int res = 0; // Initialize result

    // Iterate through all characters of input string and update
    // result
   for (int i = 0; str[i] != '\0'; ++i)
           res = res*10 + str[i] - '0';
               // return result.
      return res;
}



void incoming_params_parser(char terms[][TERM_LEN], char* incoming)
{
  int found[NUM_TERMS]= {0};
  int i, j =0, k=0;
  for (i = 0; i < (int)strlen(incoming); i ++){
       if (incoming[i] == '|'){
         found[j] = i;
         j++;
       }
      }

  for (k=0; k< NUM_TERMS; k++){
    int temp = found[k];
    int left_adj = 0, left_side = 0, right_side=0, len_adj =0;
    if (temp > 0){
       if(k!=0){
         left_side = found[k-1];
         left_adj = 1;
         len_adj = 1;
    }
    right_side = found[k];
    int len = (right_side - left_side);
    memcpy(&terms[k],&incoming[left_side+left_adj],len-len_adj);
    }
  }
}//func
