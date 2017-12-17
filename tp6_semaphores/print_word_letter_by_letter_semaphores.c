#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "time.h"
#include "string.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/sem.h"

int 	print_word_letter_by_letter(char* word, int delay) 
{
	
	struct 	timespec ts;
	int 	success = 1;
	int 	i;
	size_t 	word_length;

	ts.tv_sec = 0;
	ts.tv_nsec = delay * 1000000L;

	word_length = strlen(word);

	for (i = 0; i < word_length; i++)
	{
		putchar(word[i]);
		fflush(stdout);

		if (nanosleep(&ts, NULL) == -1) 
		{
			perror("nanosleep");
			success = 0;
		}
	}
	putchar(' ');

	return success;
}

int 	main(int argc, char** argv) {
	
	if (argc != 4) 
	{
		printf("Usage : %s <mot a afficher> <délai d'affichage en ms> <numéro du programme>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct sembuf	sem_in = {0, -1 ,0}; 
	struct sembuf	sem_out = {0, 1, 0};;
	key_t			key_in, key_out;
	int				semid_in, semid_out; 
	int 			delay, program_number, print_success;

	delay = atoi(argv[2]);
	if (delay > 1000) 
	{
		printf("le delai ne doit pas exceder 1000ms\n");
		exit(EXIT_FAILURE);
	}

	program_number = atoi(argv[3]);
	if (program_number > 9) 
	{
		printf("le numéro de programme ne doit pas dépasser 9\n");
		exit(EXIT_FAILURE);
	}

	key_in = ftok("/tmp",argv[3][0]);
	key_out = ftok("/tmp",argv[3][0]+'\n');
	
	printf("num = %s = %c\n", argv[3],argv[3][0]);
	printf("num = %s = %c\n", argv[3],argv[3][0]+'\n');

	if (key_in == -1 || key_out == -1)
	{
		perror("ftok");
		exit(errno);
	}

	semid_in = semget(key_in, 1, 0);
	semid_out = semget(key_out, 1, 0);

	if (semid_in == -1 || semid_out == -1)
	{
		perror("semget");
		exit(errno);
	}

	while (1)
	{
		printf("entering while\n");
		if (semop(semid_in, &sem_in, 1) == -1)
		{
			perror("semop");
			exit(errno);
		}

		printf("entering print\n");
		print_success = print_word_letter_by_letter(argv[1],delay);

		if (!print_success)
		{
			exit(errno);
		}

		if (semop(semid_out, &sem_out, 1) == -1)
		{
			perror("semop");
			exit(errno);
		}
	}

	exit(EXIT_SUCCESS);
}
