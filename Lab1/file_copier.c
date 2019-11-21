//
//  Created by Behnam Dezfouli
//  CSEN, Santa Clara University
//
//
// This program offers two options to the user:
// -- Option 1: enables the user to copy a file to another file. 
// The user needs to enter the name of the source and destination files.
// -- Option 2: enables the user to measure the performance of file copy. 
// In addition to the name of source and destination files, the user needs to enter the maximum source file size as well as the step size to increment the source file size.
//
//
// double copier(FILE *s_file, FILE *d_file)
// Precondition: s_file is a pointer to a file opened in read mode, d_file is a file pointer opened in write mode
// Postcondition: Copies the contents of the file pointed to by s_file to the file pointed to by d_file
//


#include <stdio.h>
#include <stdlib.h> // For exit()
#include <time.h>

#define SIZE 1
#define DUM_CHAR 'A'


double copier(FILE *s_file, FILE *d_file)
{
    char buffer[100];
    while ( fread(buffer, 1,1, s_file) == 1){
	fwrite (buffer,1,1,d_file);
    }   
    return 0;
       
}

int main()
{
    char user_option[100];
    int option;

    printf("Type an option in:\n");
    scanf("%s",user_option);
    option = atoi(user_option);
       
    if ( option == 1 ) // File copy
    {
        char str_source[100];
        char str_destination[100];
        
        printf("Type in the source file\n");
        scanf( "%s", str_source);
        
        printf("Type in the destination file\n");
        scanf("%s", str_destination);
        
        FILE *fp_source;
        fp_source = fopen(str_source, "r");
        FILE *fp_destination;
        fp_destination = fopen(str_destination, "w");
        
        copier(fp_source, fp_destination);
        fclose(fp_source);
        fclose(fp_destination);
    }
    
    else if ( option == 2 ) // File copy with performance measurement
    {
        clock_t start,end;
        double cpu_time_used;
        char buffer[100];
        char *dummy = "a";

        char source_file[100];
        printf("Type in the source file\n");
        scanf("%s", source_file);

        char user_max_file_size[100];
        printf("Type in the maximum file size (in bytes)\n");
        scanf("%s", user_max_file_size);
        int max_file_size = atoi(user_max_file_size);

        char user_step_size[100];
        printf("Type in the step_size\n");
        scanf("%s", user_step_size);
        int step_size = atoi(user_step_size);
        
        int current_size = 0;
        FILE *fp;
        FILE *fp2;
        
        while (current_size < max_file_size){
            fp2 = fopen("destination.txt", "w");
            fp = fopen(source_file,"w+");
            start = clock();
            current_size = current_size + step_size;
            int i;
            for (i = 0; i < current_size; ++i){
                fwrite(dummy,1,1,fp);
            }
            fclose(fp);
            fp = fopen(source_file, "r");
            copier(fp, fp2);
            end = clock();
            cpu_time_used = ((double) (end-start))/CLOCKS_PER_SEC;
            printf("CPU time: %lf for file size: %d\n", cpu_time_used, current_size);
            fclose(fp);
            fclose(fp2);
        }

    }
    
    else
    {
        printf("Invalid option!");
        return 0;
    }
    
    return 0;
}

