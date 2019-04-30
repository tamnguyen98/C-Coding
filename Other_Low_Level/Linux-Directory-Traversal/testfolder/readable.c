#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define PATH_LEN 100

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
		if (access(argv[1], R_OK)==0) {
			printDir(realpath(argv[1], path));
		} else {
			fprintf(stderr, "Error %d: %s (Don't have read permission to given directory).\n", errno, strerror(errno));
		}
	}

	return 0;
}


// Print the directory
int printDir(char *path) {
	DIR *dir;
	struct dirent *entry;
	if (access(path, R_OK) != 0)
		return 0;
	if ((dir=opendir(path)) == NULL) {
		fprintf(stderr, "Error %d: %s (Can't open directory).\n", errno, strerror(errno));
		return -1;
	}
	else {
		while ((entry = readdir(dir)) != NULL) {
			// Type values: 4(Folders), 8(Files)
			struct stat buff;
			lstat(entry->d_name, &buff);
			if (!S_ISLNK(buff.st_mode)) { // Check if file is not symbolic link
				//  MAYBE CHANGE?:  ignore the current and parant directory file (. a&..)
				if (strcmp(entry->d_name,"..") && strcmp(entry->d_name,".")) { // So, if it's not . or ..
					printf("%s/%s\n", path, entry->d_name);
					char folderPath[PATH_LEN] = {'\0'}; // Start off with a clear buffer
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