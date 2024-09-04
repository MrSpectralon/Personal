#include "../header_files/helper_methods.h"


void removeNewline(char* str)
{
  size_t len = strlen(str);
  if (len > 0 && str[len-1] == '\n')
  {
    str[len-1] = '\0';
  };
}


