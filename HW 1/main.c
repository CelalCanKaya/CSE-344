#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif 

int z;

int isdirectory(char *path);
int isregularfile(char *path);
int sizepathfun (char *path);
int postOrderApply (char *path, int pathfun (char *path1));

int isdirectory(char *path) {			/* Verilen path'deki dosyanın bir directory olup olmadığını bulan fonksyiyon */
	struct stat statbuf;
	if (stat(path, &statbuf) == -1){
		return 0;
	}
	else{
		return S_ISDIR(statbuf.st_mode);
	}
} 

int isregularfile(char *path){			/* Verilen path'deki dosyanın bir regular file olup olmadığını kontrol eden fonksiyon */
	struct stat statbuf;
	if (lstat(path, &statbuf) == -1 || !S_ISREG(statbuf.st_mode)){
		return -1;
	}
	return 1;
}

int sizepathfun (char *path) {			/* Verilen path'deki dosyanın size'ını return eden fonksiyon. Hata durumunda -1 return ediyor. */
	struct stat statbuf;
	if (lstat(path, &statbuf) == -1){
		return -1;
	}
	else{
		return (int)statbuf.st_size;
	}
}

 int postOrderApply (char *path, int pathfun (char *path1)){
 	struct dirent *direntp;
	DIR *dirp;
	char *temp = malloc(sizeof(char)*PATH_MAX);			/* Path'in ilk halini tutabilmem için oluşturduğum array */
	int size = 0;
	strcpy(temp, path);		/* Temp = Path */
	if ((dirp = opendir(path)) == NULL){	/* Verilen directory açılıyor. */
		perror ("Failed to open directory!");
		free(temp);	
		return -1;
	}
	while ((direntp = readdir(dirp)) != NULL){		/* O directory içindeki dosyalar bitene kadar .. */
		if(strcmp(direntp->d_name, ".")!=0 && strcmp(direntp->d_name, "..")!=0){	/* . ve .. dosyalarını ignore ediyorum */
			strcat(path, "/");
			strcat(path, direntp->d_name);		/* Okudupum dosyayı pathin sonuna ekleyip path'i düzenliyorum */
			if(isdirectory(path)==0){		/* Eğer directory değilse */
				if(isregularfile(path) == -1){	/* Eğer regular file değilse */
					printf("Special File %s\n", path);
				}
				else{	/* Toplam size'ı hesaplamak için su ana kadar olan size ile okudugum dosyanın size'ını topluyorum */
					size = size + pathfun(path);
				}
			}
			else{
				if(z==1){	/* Eğer okunan dosya bir directory ise ve -z girildiyse ... */
					size = size + postOrderApply(path, pathfun);
				}
				else{
					postOrderApply(path, pathfun);
				}
			}
			strcpy(path, temp);	/* Path'i ilk haline alıyorum */
		}
	}
	printf("%d  %s\n", size/1024, path);	/* Size ve pathi ekrana bastırıyorum */
	free(temp);	/* Mallocla aldıgım yeri geri veriyorum */
	while ((closedir(dirp) == -1) && (errno == EINTR));
	return size;
 }

 int main(int argc, char *argv[]) {	
 	char *mycwd; 
	 if ((argc != 2 && argc != 3) || (argc==3 && strcmp(argv[1], "-z")!=0) || (argc==2 && strcmp(argv[1], "-z")==0)) {	/* Eğer 2 yada 3 arguman girilmediyse hatalı girilmiştir. */
		fprintf(stderr, "Usage: %s -z(Optional) directory_name\n", argv[0]);
		return -1;
	}
	mycwd = malloc(sizeof(char)*PATH_MAX);
 	if(strcmp(argv[1], "-z")==0){
 		z=1;
 		strcpy(mycwd, argv[2]);
 	}
 	else{
 		z=0;
 		strcpy(mycwd, argv[1]);
 	}
 	if(isdirectory(mycwd)==0){		/* Eğer bir directory girmediyse */
		fprintf(stderr, "You Must Enver A Valid Directory!\nUsage: %s -z(Optional) directory_name\n", argv[0]);
		free(mycwd);
 		return -1;
 	}
 	postOrderApply (mycwd, sizepathfun);
 	free(mycwd); /* Mallocla aldıgım yeri geri veriyorum */

	return 0;
 }
