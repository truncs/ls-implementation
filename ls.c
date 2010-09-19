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

