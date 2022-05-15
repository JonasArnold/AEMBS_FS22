#include <stdio.h>
#include "main.h"
#include "blinky.h"
#include "delay.h"

int main(int argc, char *argv[]) {
	int count;

	printf ("This program was called with \"%s\".\n",argv[0]);
	if (argc > 1) {
	  for (count = 1; count < argc; count++) {
		  printf("argv[%d] = %s\n", count, argv[count]);
	  }
	} else {
	  printf("The command had no other arguments.\n");
	}

	BLINKY_Init();

	printf("Hello World with Blinky!\n");
	for(int i=0; i<10; i++) {
		BLINKY_Blink();
	}
	for(;;) {
	  BLINKY_CheckButtons();
	  delay_ms(50);
	}
	return 0;
}
