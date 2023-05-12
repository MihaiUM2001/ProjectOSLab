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

void writeOptions(char option[10], int type)
{
    printf("Please input the desired options: ");

    fgets(option, 11, stdin);
    int n = strlen(option);
    printf("n = %d\n", n);

    //-acdhlmnt
    int ok = 1;
    if(type == 0)//LNK
    {
	;
    }
    else
    {
	if(type == 1)//REG
	{
	    for(int i = 1; i < n; i++)
	    {
		if((strchr("adhlmn", option[i]) == NULL) || (option[0] != '-'))
		{
		    printf("Incorrect input! Try again: ");
		    fgets(option, 11, stdin);
		    n = strlen(option);
		    printf("n = %d\n", n);
		    i = 0;
		}
	    }
	}
	else
	{
	    if(type == 2)//DIR
	    {
		;
	    }
	    else
	    {
		printf("Invalid type!\n\n");
	    }
	}
    }
}

void optionsREG(char filename[50])
{
    printf("---- MENU ----\n");
    printf("n: name\n");
    printf("d: size\n");
    printf("h: hard link counter\n");
    printf("m: time of last modification\n");
    printf("a: access rights\n");
    printf("l: create symbolic link\n\n");

    /*char option[10];
    writeOptions(option, 1);
    printf("Options selected: %s\n\n", option);
    int n = strlen(option);
    printf("\n");*/

    char option[10] = "-ndhmal";
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
		    printf("Create symbolic link:\n");
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

    char option[10] = "-ndalt";
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
		    printf("Delete symbolic link:\n");
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

    char option[10] = "-ndac";
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

		    printf("Total number of files with the .c extension: %d\n\n", contr);
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

void secondLNK(char filename[50])
{
}

void secondDIR(char filename[50])
{
    char filenameDIR[100];
    strcpy(filenameDIR, filename);
    strcat(filenameDIR, "_file.txt");

    FILE *fin = fopen(filenameDIR, "w");
    
    if(fin == NULL)
    {
	printf("Error: fin - fopen\n\n");
	exit(1);
    }

    printf("The file '%s' was created!\n\n", filenameDIR);

    if(fclose(fin))
    {
	printf("Error: fin - fclose\n\n");
    }
}

void permissions(unsigned short mode)
{
    printf("User:\n");
    printf((mode & S_IRUSR) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWUSR) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXUSR) ? "Exec-yes\n\n" : "Exec-no\n\n");

    printf("Group:\n");
    printf((mode & S_IRGRP) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWGRP) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXGRP) ? "Exec-yes\n\n" : "Exec-no\n\n");

    printf("Others:\n");
    printf((mode & S_IROTH) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWOTH) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXOTH) ? "Exec-yes\n" : "Exec-no\n");
}

void createSymbolicLink(char filename[50])
{
    printf("Input the name of the link:\n");
    char nameSL[50];
    scanf("%49s", nameSL);
    if(symlink(filename, nameSL) < 0)
    {
	printf("Error: symlink\n");
	exit(1);
    }

    printf("The symbolic link was created!\n%s -> %s\n", nameSL, filename);
}

void deleteSymbolicLink(char filename[50])
{
    if(unlink(filename) < 0)
    {
	printf("Error: unlink\n");
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
	;
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

    if(argc < 2)
    {
	printf("Error: argc\n\n");
	exit(0);
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
		printf("Second child!\n");
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
		printf("Child process exited; pid = %d; status = %d\n\n", w2, WEXITSTATUS(w2status));
	    }
	}
    }

    return 0;
}