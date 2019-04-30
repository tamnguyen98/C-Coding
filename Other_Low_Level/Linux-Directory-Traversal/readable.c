#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/*
Name: Tam Nguyen
Assignment 2: List Directory
Description:  program will recursively traverse the directories and their files, starting with the pathname, and list every regular
file that is readable by the current process.
Class: CS360
Professor: Ben McCammish
*/


#define PATH_LEN 1000

int printDir(char *path);

int main(int argc, char **argv) {
	if (argc > 2) {
		fprintf(stderr, "Error: Must give at most one argument.\n");
		return 0;
	}
	char path[PATH_LEN];
	if (argc == 1) { // No argument given
		if (getcwd(path, sizeof(path))) { // If it's not null (path exist)
			printDir(path);
		} else {
			fprintf(stderr, "Error %d: %s (Can't get CWD path).\n", errno, strerror(errno));
			return -1;
		}
	} else { // Got argument
		char *fullPath = realpath(argv[1], path);
		if(fullPath != NULL)
		  printDir(fullPath);
		else 
		  fprintf(stderr, "Error %d: %s (Can't register path %s).\n", errno, strerror(errno), argv[1]);
	}

	return 0;
}


// Print the directory
int printDir(char *path) {
	DIR *dir;
	struct dirent *entry;
	if (access(path, R_OK) != 0) {
		fprintf(stderr, "Error %d: %s (Don't have read permission for %s).\n", errno, strerror(errno),path);
		return 0;
        }
	if ((dir=opendir(path)) == NULL) {
		fprintf(stderr, "Error %d: %s (Can't open directory %s).\n", errno, strerror(errno), path);
		return -1;
	}
	else {
		while ((entry = readdir(dir)) != NULL) {
			struct stat buff;
			lstat(entry->d_name, &buff);
			if (!S_ISLNK(buff.st_mode)) { // Check if file is not symbolic link
				//  MAYBE CHANGE?:  ignore the current and parant directory file (. a&..)
				if (strcmp(entry->d_name,"..") && strcmp(entry->d_name,".")) { // So, if it's not . or ..
			        	printf("%s/%s\n", path, entry->d_name);
					char folderPath[PATH_LEN] = {'\0'}; // Start off with a clear buffer

					// Type values: 4(Folders), 8(Files)
					if(entry->d_type == 4) { // If it's a directory, go into it
						strncat(folderPath, path, PATH_LEN);
						strcat(folderPath,"/");
						strncat(folderPath, entry->d_name, PATH_LEN);
						printDir(folderPath); // Recursion starts here
					}
				}
			}
		}
	}
	closedir(dir);
}
