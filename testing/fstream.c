#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char line1[1000];
	char line2[1000];
	char temp[256];
    FILE *fptr, *fptr2;
    if ((fptr = fopen("text.txt", "r")) == NULL) {
        printf("Error! File cannot be opened.");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    if ((fptr2 = fopen("text2.txt", "r")) == NULL) {
        printf("Error! File cannot be opened.");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }

	FILE *fd_out;
	if ((fd_out = fopen("text_out.txt", "w")) == NULL) {
        printf("Error! File cannot be opened.");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    // reads text until newline is encountered
    while (fscanf(fptr, "%[^\n]", line1) == 1){
		fscanf(fptr, "%c", temp);
		printf("%s\n", line1);
		strcat(line1, temp);
		
		fputs(line1, fd_out);
		
		if (fscanf(fptr2, "%[^\n]", line2) == 1) {
			fscanf(fptr2, "%c", temp);						
			printf("%s\n", line2);					
													
			strcat(line2, temp);
			fputs(line2, fd_out);
		}
		

	} if (feof(fptr)){
		while (fscanf(fptr2, "%[^\n]", line2) == 1){
			fscanf(fptr2, "%c", temp);
			printf("%s\n", line2);
			strcat(line2, temp);
			
			fputs(line2, fd_out);
		} if (feof(fptr2)){
			//baigesi visas skaitymas, reikia pranesti serveriui
			printf("test ar veikia, jei antras baigiasi greiciau");
		}
	}
    fclose(fptr);
    fclose(fd_out);

    return 0;
}
