/* Last updated: 2016.10.09 */

/* tee.c

   An implemention of the tee command.

   The program writes a copy of standard input to standard output and
   to the file named in its command-line argument.

   The -a option causes tee to append text to the end of a file if it
   already exits.

   Usage: tee [-a] file

   Example:
	tee -a teefile
*/
#include <ctype.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 500
#define printable(ch) (isprint((unsigned char) ch) ? ch : '?')

int main(int argc, char *argv[])
{
	int opt;
	int fd;
	int append;
	int openFlags;
	mode_t filePerms;
	int numRead;
	char *file;
	char buf[BUF_SIZE];

	if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s [-a] file\n", argv[0]);

	append = 0;
	while ((opt = getopt(argc, argv, "a")) != -1) {
		printf("opt = %3d (%c); optind = %d", opt, printable(opt), optind);
		if (opt == '?' || opt == ':')
			printf("; optopt = %3d (%3c)", optopt, printable(optopt));
		printf("\n");

		switch (opt) {
		case 'a': 
			append = 1;
			printf("get -a option\n");
			break;
		case '?':
		default:
			fatal("Unexpected case in switch");
		}		
	}
	
	file = argv[optind];
	printf("file (argv[%d]) is %s\n", optind, file);

	openFlags = append ? O_WRONLY | O_CREAT | O_APPEND : O_WRONLY | O_CREAT | O_TRUNC;
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
		    S_IROTH | S_IWOTH;		/* rw-rw-rw- */

	fd = open(file, openFlags, filePerms);
	if (fd == -1)
		errExit("open");

	while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
	{
		if (write(STDOUT_FILENO, buf, numRead) != numRead)
			errExit("write error");
		if (write(fd, buf, numRead) != numRead)
			errExit("write error");
	}

	if (numRead == -1)
		errExit("read");

	if (close(fd) == -1)
		errExit("close");

	exit(EXIT_SUCCESS);
}
