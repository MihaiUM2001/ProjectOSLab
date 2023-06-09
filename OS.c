#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

int fd[2];
char buffer[4096];

int fileType(char filename[50])
{
    printf("%s - ", filename);
    struct stat st;
    lstat(filename, &st);

    if(S_ISLNK(st.st_mode))//LNK
    {
	printf("SYMBOLIC LINK\n\n");
	return 0;
    }
    else
    {
	if(S_ISREG(st.st_mode))//REG
	{
	    printf("REGULAR FILE\n\n");
	    return 1;
	}
	else
	{
	    if(S_ISDIR(st.st_mode))//DIR
	    {
		printf("DIRECTORY\n\n");
		return 2;
	    }
	    else//OTHER
	    {
		printf("OTHER\n\n");
	    }
	}
    }
    return 3;
}

int checkOptions(char option[10], int type)
{
    int n = (strlen(option) - 1);

    if(option[0] != '-')
    {
	return 0;
    }

    //-acdhlmnt
    if(type == 0)//LNK --- adlnt
    {
	for(int i = 1; i < n; i++)
	{
	    if(strchr("adlnt", option[i]) == NULL)
	    {
		return 0;
	    }
	}
    }
    else
    {
	if(type == 1)//REG --- adhlmn
	{
	    for(int i = 1; i < n; i++)
	    {
		if(strchr("adhlmn", option[i]) == NULL)
		{
		    return 0;
		}
	    }
	}
	else
	{
	    if(type == 2)//DIR --- acdn
	    {
		for(int i = 1; i < n; i++)
		if(strchr("acdn", option[i]) == NULL)
		{
		    return 0;
		}
	    }
	    else
	    {
		printf("Invalid type!\n\n");
		return 0;
	    }
	}
    }
    return 1;
}

void permissions(unsigned short mode);
void createSymbolicLink(char filename[50]);
void deleteSymbolicLink(char filename[50]);

void optionsREG(char filename[50])
{
    printf("---- MENU ----\n");
    printf("n: name\n");
    printf("d: size\n");
    printf("h: hard link counter\n");
    printf("m: time of last modification\n");
    printf("a: access rights\n");
    printf("l: create symbolic link\n\n");

    char option[10];
    printf("Please input the desired options: ");
    fgets(option, 10, stdin);
    while(checkOptions(option, 1) == 0)
    {
	printf("Incorrect input! Try again: ");
	fgets(option, 10, stdin);
    }
    int n = strlen(option);

    struct stat *buffer;
    buffer = malloc(sizeof(struct stat));
    if(lstat(filename, buffer) < 0)
    {
	printf("Error: lstat\n\n");
	exit(1);
    }

    for(int i = 1; i < n; i++)
    {
	switch(option[i])
	{
	    case 'n':
	    {
		printf("Name: %s\n\n", filename);
		break;
	    }

	    case 'd':
	    {
		printf("Size: %lld\n\n", (long long) buffer->st_size);
		break;
	    }

	    case 'h':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Hard link count: %ld\n\n", (long) buffer->st_nlink);
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }

	    case 'm':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Time of last modification: %s\n\n", ctime(&buffer->st_mtime));
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }

	    case 'a':
	    {
		printf("Access rights:\n\n");
		permissions(buffer->st_mode);
		printf("\n");
		break;
	    }

	    case 'l':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Create symbolic link:\n\n");
		    createSymbolicLink(filename);
		    printf("\n");
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }
	}
    }
}

void optionsLNK(char filename[50])
{
    printf("---- MENU ----\n");
    printf("n: name\n");
    printf("d: size\n");
    printf("a: access rights\n");
    printf("l: delete symbolic link\n");
    printf("t: size of target file\n\n");

    char option[10];
    printf("Please input the desired options: ");
    fgets(option, 10, stdin);
    while(checkOptions(option, 0) == 0)
    {
	printf("Incorrect input! Try again: ");
	fgets(option, 10, stdin);
    }
    int n = strlen(option);

    struct stat *buffer;
    buffer = malloc(sizeof(struct stat));
    if(lstat(filename, buffer) < 0)
    {
	printf("Error: lstat\n\n");
	exit(1);
    }

    int stop = 0;
    for(int i = 1; i < n && stop == 0; i++)
    {
	switch(option[i])
	{
	    case 'n':
	    {
		printf("Name: %s\n\n", filename);
		break;
	    }

	    case 'd':
	    {
		printf("Size: %lld\n\n", (long long) buffer->st_size);
		break;
	    }

	    case 'a':
	    {
		printf("Access rights:\n\n");
		permissions(buffer->st_mode);
		printf("\n");
		break;
	    }

	    case 'l':
	    {
		if(S_ISLNK(buffer->st_mode))
		{
		    printf("Delete symbolic link:\n\n");
		    deleteSymbolicLink(filename);
		    printf("\n");
		    stop = 1;
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }

	    case 't':
	    {
		if(S_ISLNK(buffer->st_mode))
		{
		    struct stat tmp1;
		    if(stat(filename, &tmp1) < 0)
		    {
			printf("Error: stat\n\n");
			exit(1);
		    }
		    printf("Size of target file: %lld\n\n", (long long) tmp1.st_size);
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }
	}
    }
}

void optionsDIR(char filename[50])
{
    printf("---- MENU ----\n");
    printf("n: name\n");
    printf("d: size\n");
    printf("a: access rights\n");
    printf("c: total number of files with the '.c' extension\n\n");

    char option[10];
    printf("Please input the desired options: ");
    fgets(option, 10, stdin);
    while(checkOptions(option, 2) == 0)
    {
	printf("Incorrect input! Try again: ");
	fgets(option, 10, stdin);
    }
    int n = strlen(option);

    struct stat *buffer;
    buffer = malloc(sizeof(struct stat));
    if(lstat(filename, buffer) < 0)
    {
	printf("Error: lstat\n\n");
	exit(1);
    }

    for(int i = 1; i < n; i++)
    {
	switch(option[i])
	{
	    case 'n':
	    {
		printf("Name: %s\n\n", filename);
		break;
	    }

	    case 'd':
	    {
		printf("Size: %lld\n\n", (long long) buffer->st_size);
		break;
	    }

	    case 'a':
	    {
		printf("Access rights:\n\n");
		permissions(buffer->st_mode);
		printf("\n");
		break;
	    }

	    case 'c':
	    {
		if(S_ISDIR(buffer->st_mode))
		{
		    DIR *tmp2 = opendir(filename);
		    if(tmp2 == NULL)
		    {
			printf("Error: opendir\n\n");
			exit(1);
		    }

		    int contr = 0;
		    char *c;
		    struct dirent *parse;
		    parse = readdir(tmp2);
		    while(parse != NULL)
		    {
			if(parse->d_type == DT_REG)
			{
			    c = strrchr(parse->d_name, '.');
			    if((c != NULL) && (strcmp(c, ".c") == 0))
			    {
				contr++;
			    }
			}
			parse = readdir(tmp2);
		    }

		    if(closedir(tmp2) == -1)
		    {
			printf("Error: closedir\n\n");
			exit(1);
		    }

		    printf("Total number of files with the '.c' extension: %d\n\n", contr);
		}
		else
		{
		    printf("Invalid type!\n\n");
		}
		break;
	    }
	}
    }
}

void secondREG(char filename[50])
{
    char *c = strrchr(filename, '.');
    if((c != NULL) && (strcmp(c, ".c") == 0))//with the '.c' extenstion
    {
	printf("The regular file '%s' has the '.c' extension!\n\n", filename);

	close(fd[0]);
	dup2(fd[1], 1);

	if(execlp("./script.sh", "./script.sh", filename, NULL) == -1)
	{
	    printf("Error execlp - secondREG\n\n");
	    exit(1);
	}
    }
    else//doesn't have the '.c' extension
    {
	printf("The regular file '%s' doesn't have the '.c' extension!\n\n", filename);

	/*
	if(execl("/bin/wc", "wc", "-l", filename, NULL) == -1)
	{
	    printf("Error: execl - wc");
	    exit(1);
	}
	*/

	FILE *finREG = fopen(filename, "r");
	if(finREG == NULL)
	{
	    printf("Error: finREG - fopen\n\n");
	    exit(1);
	}

	char c;
	int contr = 0;

	while(!feof(finREG))
	{
	    c = fgetc(finREG);
	    if((c == '\n') || (c == EOF))
	    {
		contr++;
	    }
	}

	printf("The regular file '%s' has %d lines!", filename, contr);

	if(fclose(finREG))
	{
	    printf("Error: finREG - fclose\n\n");
	    exit(1);
	}
    }
}

void secondLNK(char filename[50])
{
    printf("The permissions for the target file of the symbolic link '%s' will be changed!\n\n", filename);
    //7 = rwx - User
    //6 = rw  - Group
    //0       - Other
    //if(execl("/bin/chmod", "chmod", "-v", "u+rwx,g+rw,g-x,o-rwx",filename, NULL) == -1)
    if(execl("/bin/chmod", "chmod", "-v", "760", filename, NULL) == -1)
    {
	printf("Error: execl - chmod - secondLNK\n\n");
	exit(1);
    }
}

void secondDIR(char filename[50])
{
    char filenameDIR[100];
    strcpy(filenameDIR, filename);
    strcat(filenameDIR, "_file.txt");

    FILE *finDIR = fopen(filenameDIR, "w");
    if(finDIR == NULL)
    {
	printf("Error: finDIR - fopen\n\n");
	exit(1);
    }

    printf("The file '%s' was created!\n\n", filenameDIR);

    if(fclose(finDIR))
    {
	printf("Error: finDIR - fclose\n\n");
	exit(1);
    }
}

void permissions(unsigned short mode)
{
    printf("User:\n");
    printf((mode & S_IRUSR) ? "Read - yes\n" : "Read - no\n");
    printf((mode & S_IWUSR) ? "Write - yes\n" : "Write - no\n");
    printf((mode & S_IXUSR) ? "Exec - yes\n\n" : "Exec - no\n\n");

    printf("Group:\n");
    printf((mode & S_IRGRP) ? "Read - yes\n" : "Read - no\n");
    printf((mode & S_IWGRP) ? "Write - yes\n" : "Write - no\n");
    printf((mode & S_IXGRP) ? "Exec - yes\n\n" : "Exec - no\n\n");

    printf("Others:\n");
    printf((mode & S_IROTH) ? "Read - yes\n" : "Read - no\n");
    printf((mode & S_IWOTH) ? "Write - yes\n" : "Write - no\n");
    printf((mode & S_IXOTH) ? "Exec - yes\n" : "Exec - no\n");
}

void createSymbolicLink(char filename[50])
{
    printf("Input the name of the link: ");
    char nameSL[50];
    scanf("%49s", nameSL);
    printf("\n");

    if(symlink(filename, nameSL) < 0)
    {
	printf("Error: symlink\n\n");
	exit(1);
    }

    printf("The symbolic link '%s -> %s' was created!\n", nameSL, filename);
}

void deleteSymbolicLink(char filename[50])
{
    if(unlink(filename) < 0)
    {
	printf("Error: unlink\n\n");
	exit(1);
    }

    printf("The symbolic link was deleted!\n");
}

//First Process
void FP(char filename[50], int ft)
{
    if(ft == 0)//LNK
    {
	optionsLNK(filename);
    }
    else
    {
	if(ft == 1)//REG
	{
	    optionsREG(filename);
	}
	else
	{
	    if(ft == 2)//DIR
	    {
		optionsDIR(filename);
	    }
	    else//OTHER
	    {
		printf("The file '%s' is not a Symbolic Link/Regular File/Directory!\n\n", filename);
	    }
	}
    }
}

//Second Process
void SP(char filename[50], int ft)
{
    if(ft == 1)//REG
    {
	secondREG(filename);
    }
    else
    {
	if(ft == 2)//DIR
	{
	    secondDIR(filename);
	}
	else
	{
	    if(ft == 0)//LNK
	    {
		secondLNK(filename);
	    }
	    else//OTHER
	    {
		printf("The file '%s' is not a Symbolic Link/Regular File/Directory!\n\n", filename);
	    }
	}
    }
}

int main(int argc, char **argv)
{
    int w1status;
    pid_t pid1;
    pid_t w1;

    int w2status;
    pid_t pid2;
    pid_t w2;

    if(pipe(fd) < 0)
    {
	printf("Error: pipe\n\n");
	exit(1);
    }

    if(argc < 2)
    {
	printf("Error: argc\n\n");
	exit(1);
    }
    else
    {
	for(int i = 1; i < argc; i++)
	{
	    int ft = fileType(argv[i]);

	    pid1 = fork();
	    if(pid1 == 0)
	    {
		FP(argv[i], ft);
		exit(0);
	    }

	    pid2 = fork();
	    if(pid2 == 0)
	    {
		SP(argv[i], ft);
		exit(0);
	    }

	    w1 = waitpid(pid1, &w1status, 0);
	    if(WIFEXITED(w1status))
	    {
		printf("Child process exited; pid = %d; status = %d\n\n", w1, WEXITSTATUS(w1status));
	    }

	    w2 = waitpid(pid2, &w2status, 0);
	    if(WIFEXITED(w2status))
	    {
		//REG --- with the '.c' extension
		if(ft == 1)
		{
		    char *c = strrchr(argv[i], '.');
		    if((c != NULL) && (strcmp(c, ".c") == 0))
		    {
			int ReadBinary;
			int contrW;//Warining Counter
			int contrE;//Error Counter
			char fn[50];//File Name
			char so[4096];//Script Output

			close(fd[1]);
			ReadBinary = read(fd[0], buffer, sizeof(buffer));
			if(ReadBinary < 0)
			{
			    printf("Error: read\n\n");
			    exit(1);
			}
			strcpy(so, buffer);

			int index = 1;
			char *t = strtok(so, ",");
			while(t)
			{
			    switch(index)
			    {
				case 1:
				{
				    contrW = atoi(t);
				    break;
				}

				case 2:
				{
				    contrE = atoi(t);
				    break;
				}

				case 3:
				{
				    strcpy(fn, t);
				    break;
				}

				default:
				{
				    t = NULL;
				}
			    }
			    index++;
			    t = strtok(NULL, ",");
			}

			printf("Warnings: %d\n\n", contrW);
			printf("Errors: %d\n\n", contrE);
			printf("Filename: %s\n\n", fn);

			int score = 0;
			if((contrW == 0) && (contrE == 0))
			{
			    score = 10;
			}
			else
			{
			    if(contrE >= 1)
			    {
				score = 1;
			    }
			    else
			    {
				if((contrE == 0) && (contrW > 10))
				{
				    score = 2;
				}
				else
				{
				    if((contrE == 0) && (contrW <= 10))
				    {
					score = 2 + 8 * (10 - contrW) / 10;
				    }
				}
			    }
			}

			FILE *fgrade = fopen("grades.txt", "w");
			if(fgrade == NULL)
			{
			    printf("Error: fgrade - fopen\n\n");
			    exit(1);
			}

			fprintf(fgrade, "%s %d\n", fn, score);

			if(fclose(fgrade))
			{
			    printf("Error: fgrade - fclose\n\n");
			    exit(1);
			}
		    }
		}

		printf("Child process exited; pid = %d; status = %d\n\n", w2, WEXITSTATUS(w2status));
	    }
	}
    }

    return 0;
}