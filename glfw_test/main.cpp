#include <stdio.h>
#include <iostream>
#include "application.h"


int main(void)
{
	Application app;
	if (app.run())
		exit(EXIT_FAILURE);
}

