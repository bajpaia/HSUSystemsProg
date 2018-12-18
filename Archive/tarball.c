
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
void tar_compress(char files[][256]);
void tar_add(FILE* tar_file, const char* file);
void tar_list();
void tar_extract();
struct tar_header{
    char name[100];
    char mode[8];
    char owner[8];
    char group[8];
    char size[12];

};
char tar_filename[50];
int num_files;


void fexpand(FILE* f, size_t amount, int value){
    while( amount-- ){
        fputc( value, f );
    }
}


void tar_compress(char files[][256])
{
  FILE *output = fopen(tar_filename,"wb");
  for (int i=0;i<num_files;i++)
  {
    tar_add(output,files[i]);
  }

}


void tar_extract()
{
  FILE *input=fopen(tar_filename,"rb");
  while (!feof(input))
   {
		struct tar_header header;
		memset(&header, 0, sizeof(struct tar_header));

	   	fread(header.name, sizeof(header.name), 1, input);
    	fread(header.mode, sizeof(header.mode), 1, input);
    	fread(header.owner, sizeof(header.owner), 1, input);
    	fread(header.group, sizeof(header.group), 1, input);
    	fread(header.size, sizeof(header.size), 1, input);

    	int size;
    	uid_t id;
    	gid_t gid;
    	sscanf(header.size, "%d", &size);
    	sscanf(header.owner, "%u", &id);
    	sscanf(header.group, "%u", &gid);
    	mode_t mode = strtol(header.mode, NULL, 8);

		char buffer[2000];
		size_t read = fread(buffer, 1, size, input);
		if (read == size)
    {
			FILE *fout = fopen(header.name, "wb");
			fwrite(buffer, 1, read, fout);
			chmod(header.name, mode);
			chown(header.name, id, gid);
			fclose(fout);
		}
    else
    {
    	break;
    }
	}
	fclose(input);


}
void tar_list()
{
  FILE *input = fopen(tar_filename, "r");
while (!feof(input))
{
  int name_size;
  int file_size;
  fscanf(input, "%d", &name_size);
  char file_name[256];
  memset(file_name, 0, sizeof(file_name));
  size_t read = fread(file_name, sizeof(char), name_size, input);
  if (read == name_size)
  {
    fscanf(input, "%d", &file_size);
    printf("%s\n", file_name);
    char buf[file_size];
    memset(buf, 0, file_size);
    fread(buf, 1, file_size, input);
  }
}
fclose(input);
}

void tar_add(FILE* tar_file, const char* file)
{
    //Get current position; round to a multiple of 512 if we aren't there already
    size_t index = ftell( tar_file );
    size_t offset = index % 512;
    if( offset != 0 ){
        fexpand( tar_file, 512 - offset, 0);
    }
    //Store the index for the header to return to later
    index = ftell( tar_file );
    //Write some space for our header
    fexpand( tar_file, sizeof(struct tar_header), 0 );
    //Write the input file to the tar file
    FILE* input = fopen( file, "rb" );
    if( input == NULL ){
        fprintf( stderr, "Failed to open %s for reading\n", file);
        return;
    }
    //Copy the file content to the tar file
    while( !feof(input) )
    {
        char buffer[2000];
        size_t read = fread( buffer, 1, 2000, input );
        fwrite( buffer, 1, read, tar_file);
    }
    //Get the end to calculate the size of the file
    size_t end = ftell( tar_file );
    //Round the file size to a multiple of 512 bytes
    offset = end % 512;
    if( end != 0 ){
        fexpand( tar_file, 512 - offset, 0);
    }
    //Fill out a new tar header
    struct tar_header header;
    struct stat fileinfo;
    memset( &header, 0, sizeof( struct tar_header ) );
    snprintf( header.name, 100, "%s", file  );
    snprintf( header.mode, 8, "%06o ", fileinfo.st_mode ); //You should probably query the input file for this info
    snprintf( header.owner, 8, "%06o ", getpwuid(fileinfo.st_uid) ); //^
    snprintf( header.group, 8, "%06o ", getgrgid(fileinfo.st_gid) ); //^
    snprintf( header.size, 12, "%011o", fileinfo.st_size);

    fwrite(&header, sizeof(struct tar_header), 1, tar_file);
    fclose( input );
}
int main(int argc, char **argv)
{
	if (argc == 1)
   {
		return 1;
	 }

	else if (argc > 2)
   {
		char* arg = argv[1];
		if (arg[0] == '-')
     {
			switch (arg[1])
      {
				case 'c':
					strcpy(tar_filename, argv[2]);
					num_files = argc - 3;
					if (num_files > 0)
           {
						char files[num_files][256];
						for (int j = 3; j-3 < num_files; j ++)
             {
							strcpy(files[j - 3], argv[j]);
						}
						tar_compress(files);
					}
					break;
				case 'x':
					strcpy(tar_filename, argv[2]);
					tar_extract();
					break;
				case 'l':
					strcpy(tar_filename, argv[2]);
					tar_list();
					break;
				default:
					printf("Unknown flag: %s\n", arg);
					return 1;
			}
		}
    else
    {
			printf("Unknown flag: %s\n", arg);
				return 1;
		}
	}
}
