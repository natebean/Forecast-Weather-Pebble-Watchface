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
  int i = 0;
  int start_ptr = 0;


    if (str[0] == '0' || str[0] == ' '){
      start_ptr = 1;
    }
    //Iterate through all characters of input string and update
    // result
    for (i = start_ptr; str[i] != '\0'; ++i)
      res = res*10 + str[i] - '0';
   // return result.
   return res;
}



