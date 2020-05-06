
#include <string.h>

void tty_raw_mode(void);

int main(int argc, char ** argv) 
{
  tty_raw_mode();

  while (1) {
    char ch;
    read(0, &ch, 1);
    printf("%d\n", ch);
  }
}

