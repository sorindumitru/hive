#include <stdio.h>

int main(int argc, const char *argv[])
{
	int i, num_workers = 8;
	
	fprintf(stdout, "Starting the hive simulator\n");

	for (i = 0; i < num_workers; i++) {	
		worker_init(i);
	}
	
	return 0;
}
