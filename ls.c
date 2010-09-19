/*-
 * Copyright (c) 2010 Aditya Sarawgi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * 
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static const char * lookup[] = {"Jan", "Feb", "Mar", "Apr", "May"\
				"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void
display_contents(char * name){
	struct stat sb;
	struct tm * t;
	char link_read[255];
	ssize_t bytes_read;
	lstat(name, &sb);

	printf("%c", S_ISDIR(sb.st_mode)?'d':\
		     S_ISFIFO(sb.st_mode)?'p':\
		     S_ISLNK(sb.st_mode)?'l':'-');
		     
	
	printf("%c", (S_IRUSR & sb.st_mode) ? 'r' : '-');
	printf("%c", (S_IWUSR & sb.st_mode) ? 'w' : '-');
	printf("%c", (S_IXUSR & sb.st_mode) ? 'x' : '-');
	printf("%c", (S_IRGRP & sb.st_mode) ? 'r' : '-');
	printf("%c", (S_IWGRP & sb.st_mode) ? 'w' : '-');
	printf("%c", (S_IXGRP & sb.st_mode) ? 'x' : '-');
	printf("%c", (S_IROTH & sb.st_mode) ? 'r' : '-');
	printf("%c", (sb.st_mode & S_IWOTH) ? 'w' : '-');
	printf("%c  ", (S_IXOTH & sb.st_mode) ? 'x' : '-');
	printf("%d\t", sb.st_nlink);
	printf("%s\t", user_from_uid(sb.st_uid,0));
	printf("%s\t", group_from_gid(sb.st_gid,0));
	printf("%5.0lu ", sb.st_size);
	t = localtime(&sb.st_ctime);
	printf("%s ", lookup[t->tm_mon]);
	printf("%2.0d %2.0d:%2.0d ", t->tm_mday, t->tm_hour, t->tm_min);
	if(S_ISLNK(sb.st_mode)){
		printf("@\033[35m%s ",name );
		printf("\033[37m");
	        bytes_read = readlink(name, link_read, 254);
		link_read[bytes_read] = '\0';
		printf("-> %s\n", link_read);
	}
	else if((S_IXUSR & sb.st_mode) || (S_IXGRP & sb.st_mode) || (S_IXOTH & sb.st_mode))
		printf("\033[31m%s\033[37m\n", name);
	else if(S_ISFIFO(sb.st_mode))
		printf("\033[33m%s\033[37m\n", name);
	else 
		printf("%s\n", name);


}	
void
get_contents(DIR *d){
	
	struct dirent *entry;
	int i =0;
	while((entry = readdir(d)) != NULL){
		if(i < 2 ){
			i++;
			continue;
		}	
		display_contents(entry->d_name);
	}

}

int main(int argc, char *argv[]){

	DIR *d;
	int i=1;
	struct stat s;
	char buf[255];
	if(argc < 2){
		d = opendir(".");
		get_contents(d);
	}
	else 
		for (; argc > 1; argc--){
			lstat(argv[i], &s);
			if(S_ISDIR(s.st_mode)){
				getwd(buf);
				chdir(argv[i]);
				printf("%s\n", argv[i]);
				d = opendir(".");
				get_contents(d);
				chdir(buf);
			}
			else
				display_contents(argv[i]);
			i++;
		}
		
	return 0;
}	

