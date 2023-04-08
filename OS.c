#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void menu(char filename[50])
{
    printf("%s - ", filename);
    struct stat st;
    lstat(filename, &st);

    if(S_ISLNK(st.st_mode))
    {
	printf("SYMBOLIC LINK\n\n");
    }
    else
    {
	if(S_ISREG(st.st_mode))
	{
	    printf("REGULAR FILE\n\n");
	}
	else
	{
	    if(S_ISDIR(st.st_mode))
	    {
		printf("DIRECTORY\n\n");
	    }
	    else
	    {
		printf("OTHER\n\n");
	    }
	}
    }

    printf("---- MENU ----\n");

    if(S_ISREG(st.st_mode))
    {
	printf("n: name\n");
	printf("d: size\n");
	printf("h: hard link count\n");
	printf("m: time of last modification\n");
	printf("a: access rights\n");
	printf("l: create symbolic link\n");
    }

    if(S_ISLNK(st.st_mode))
    {
	printf("n: name\n");
	printf("d: size of symbolic link\n");
	printf("a: access rights\n");
	printf("l: delete symbolic link\n");
	printf("t: size of target file\n");
    }

    if(S_ISDIR(st.st_mode))
    {
	printf("n: name\n");
	printf("d: size\n");
	printf("a: access rights\n");
	printf("c: total number of files with the .c extension\n");
    }

    options(filename);
}

void options(char filename[50])
{
    printf("Please enter your options!\n\n");

    struct stat *buffer;
    buffer = malloc(sizeof(struct stat));
    if(lstat(filename, buffer) < 0)
    {
	printf("Error: lstat\n");
	exit(2);
    }

    char option[10];
    printf("Input the desired options: ");
    scanf("%9s", option);
    printf("\n");

    //Check if the string contains invalid input and if the first character in the string is '-'
    int i = 0;
    int n = strlen(option);
    for(i = 0; i < n; i++)
    {
	if((strchr("-acdhlmnt", option[i]) == NULL) || (option[0] != '-'))
	{
	    printf("Incorrect input! Try again: ");
	    scanf("%9s", option);
	    printf("\n");
	    i = 0;
	    n = strlen(option);
	}
    }
    printf("Options: %s\n\n", option);

    int stop = 0;
    for(int i = 1; i < n && stop == 0; i++)
    {
	switch(option[i])
	{
	    case 'n':
	    {
		printf("Name: %s\n", filename);
		break;
	    }

	    case 'd':
	    {
		printf("Size: %lld\n", (long long) buffer->st_size);
		break;
	    }

	    case 'h':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Hard link count: %ld\n", (long) buffer->st_nlink);
		}
		else
		{
		    printf("Not available!\n");
		}
		break;
	    }

	    case 'm':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Time of last modification: %s", ctime(&buffer->st_mtime));
		}
		else
		{
		    printf("Not available!\n");
		}
		break;
	    }

	    case 'a':
	    {
		printf("Access rights:\n");
		permissions(buffer->st_mode);
		break;
	    }

	    case 'l':
	    {
		if(S_ISREG(buffer->st_mode))
		{
		    printf("Create symbolic link:\n");
		    createSymbolicLink(filename);
		}
		else
		{
		    if(S_ISLNK(buffer->st_mode))
		    {
			printf("Delete symbolic link:\n");
			deleteSymbolicLink(filename);
			stop = 1;
		    }
		    else
		    {
			printf("Not available!\n");
		    }
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
			printf("Error: stat!\n");
			exit(5);
		    }
		    printf("Size of target file: %lld\n", (long long) tmp1.st_size);
		}
		else
		{
		    printf("Not available!\n");
		}
	    }

	    case 'c':
	    {
		if(S_ISDIR(buffer->st_mode))
		{
		    DIR *tmp2 = opendir(filename);
		    if(tmp2 == NULL)
		    {
			printf("Error: opendir!\n");
			exit(6);
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
			printf("Error: closedir!\n");
			exit(7);
		    }

		    printf("Total number of files with the .c extension: %d\n", contr);
		}
		else
		{
		    printf("Not available!\n");
		}
		break;
	    }

	    default:
	    {
		printf("Invalid input!\n");
	    }
	}
	printf("\n");
    }
}

void permissions(unsigned short mode)
{
    printf("User:\n");
    printf((mode & S_IRUSR) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWUSR) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXUSR) ? "Exec-yes\n" : "Exec-no\n\n");

    printf("Group:\n");
    printf((mode & S_IRGRP) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWGRP) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXGRP) ? "Exec-yes\n" : "Exec-no\n\n");

    printf("Others:\n");
    printf((mode & S_IROTH) ? "Read-yes\n" : "Read-no\n");
    printf((mode & S_IWOTH) ? "Write-yes\n" : "Write-no\n");
    printf((mode & S_IXOTH) ? "Exec-yes\n" : "Exec-no\n");
}

void createSymbolicLink(char filename[50])
{
    printf("Input the name of the link: ");
    char nameSL[50];
    scanf("%49s", nameSL);
    if(symlink(filename, nameSL) < 0)
    {
	printf("Error: symlink!\n");
	exit(3);
    }

    printf("The symbolic link was created!\n%s -> %s\n", nameSL, filename);
}

void deleteSymbolicLink(char filename[50])
{
    if(unlink(filename) < 0)
    {
	printf("Error: unlink!\n");
	exit(4);
    }

    printf("The symbolic link was deleted!\n");
}

int main(int argc, char **argv)
{
    //Check if one or more files have ben given as arguments
    if(argc < 2)
    {
	printf("Error: argc!\n");
	exit(1);
    }
    else
    {
	for(int i = 1; i < argc; i++)
	{
	    menu(argv[i]);
	}
    }
    return 0;
}