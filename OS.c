#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

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
	    printf("OTHER\n\n");
	}
    }

    printf("---- MENU ----\n");
    printf("n: name\n");
    printf("m: last modif\n");
    printf("a: access\n");
    printf("Please enter your options!\n\n");

    options(filename);
}

void options(char filename[50])
{
    int stop = 0;

    struct stat *buffer;
    buffer = malloc(sizeof(struct stat));

    if(lstat(filename, buffer) < 0)
    {
	printf("Error: lstat\n");
	exit(2);
    }

    printf("Input the desired options: ");
    char option[10];
    scanf("%9s", option);
    printf("\n");

    for(int i = 1; i < strlen(option) && stop == 0; i++)
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
		printf("Time of last modification: %s", ctime(&buffer->st_mtime));
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
		}
		break;
	    }

	    case 't':
	    {
		if(S_ISLNK(buffer->st_mode))
		{
		    struct stat tmp;
		    if(stat(filename, &tmp) < 0)
		    {
			printf("Error: stat!\n");
			exit(5);
		    }
		    printf("Size of target file: %lld\n", (long long) tmp.st_size);
		}
		else
		{
		    printf("Not available!\n");
		}
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