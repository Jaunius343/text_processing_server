#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h> 

#include <unistd.h> 

#define MYPORT "3490"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define HOST "127.0.0.2"
#define HOST6 "::1"
#define MAXDATASIZE 1000

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void to_upper(char *str)
{
	int i = 0;
	char chr;
	
	while (str[i]) { 
        chr = str[i]; 
        str[i] = toupper(chr);
        ++i; 
    } 
}

int main(int argc, char *argv[])
{
	
	int status;
	struct addrinfo hints, *res;
	int sockfd, new_fd, numbytes;
	char buf [MAXDATASIZE];
	int yes=1;
	
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	
	 if (argc != 3) {
        fprintf(stderr,"usage: server hostname port\n");
        return 1;
    }

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	//hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos

	// ... do everything until you don't need servinfo anymore ....


	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
			perror("socket");
			exit(1);
	}
	//if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != 0) {
		//fprintf(stderr, "bind error: %s\n", gai_strerror(status));
		//exit(1);
	//}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
    }

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
		perror("bind");
		exit(1);
	}
	//if ((bind(sockfd, res->ai_addr, res->ai_addrlen)) ! = 0) {
		//fprintf(stderr, "bind error: %s\n", gai_strerror(status));
		//exit(1);
	//}
	
	if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    

	freeaddrinfo(res); // free the linked-list
	
	printf("server: waiting for connections...\n");
	
	//while(1){
		addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		
		if (new_fd == -1) {
            perror("accept");
            //continue;
        }
		
		char s[INET6_ADDRSTRLEN];
		inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
		
		FILE *fd_out;
		if ((fd_out = fopen("text_out.txt", "w")) == NULL) {
			printf("Error! File cannot be opened.");
			// Program exits if the file pointer returns NULL.
			exit(1);
		}
		
		
		while(1){
			printf("waiting for command\n");
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			printf("command received\n");
			
			buf[numbytes] = '\0';					//reik pasiimti viska is buferio i kita var
													//nuskaityti kiek eina zinuciu is jo
													//pasalinti nuskaityta dali
													//poto nauja bufferi appendinti i ta var
			
			if(strcmp(buf, "%%END") == 0){
				printf("file ended\n");
				fclose(fd_out);
				break;
			} 
			
			if(strcmp(buf, "%%T") == 0){
				//printing to file
				
				char msg[MAXDATASIZE] = "receiving text...";
				int len = strlen(msg), bytes_sent;
				if ((bytes_sent = send(new_fd, msg, len, 0)) == -1) {
					perror("send");
					exit(1);
				}
				
				printf("%s\n", msg);
				if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
				}
				
				buf[numbytes] = '\0';		
				
				
				printf("received from client: '%s'\n",buf);
				//fputs(buf, fd_out);
				
				//sleep(3);
				
				//issivalom visus laukus pries pradedant skaityti
				char line[MAXDATASIZE], sep[MAXDATASIZE], temp[MAXDATASIZE], new_temp[MAXDATASIZE];
				memset(temp, '\0', sizeof(temp));
				memset(line, '\0', sizeof(line));
				memset(sep, '\0', sizeof(sep));
				memset(new_temp, '\0', sizeof(new_temp));
				strcpy(temp, buf);
				//strcat(temp, buf);
				
				while(strlen(temp) > 0){
					int len = strcspn(temp, "\n");		//ieskom naujos eilutes
					int s_len = strlen(temp);			//turimo string ilgis
					
					//printf("\n\noffset to \\n %d %d\n", len, s_len);
					
					//printf("\ntemp is: \n'%s'\n", temp);
					//printf("temp len: %zu\n\n", strlen(temp));
					
					sscanf(temp, "%[^\n]", line);
					len = strlen(line);
					
					//%%T 
					//serveris turi atsakyti klientui
					//klientas siuncia info
					//
					
					//if(strcmp(line, "%%END") == 0){
						//break;
					//} 
					
					//jei per ilga zinute, tiesiog det i faila
					//(per ilga reiskia, kad nerastas skyrimo simbolis \n)
					if(len  == s_len){
						fputs(temp, fd_out);
						memset(temp, '\0', sizeof(temp));
						break;
					}
					
					//kad nereiketu kopijuoti i nauja string, tiesiog valom senojo pradzia
					//t.y. istrinam nuskaityta dali
					//strncpy(new_temp, &temp[len+1], strlen(temp) - len);
					for(int i = len+1, j = 0; i<MAXDATASIZE; ++i, ++j){
						temp[j] = temp[i];
						temp[i] = '\0';
					}
					
					
					
					
					
					//sscanf(new_temp, "%c", sep);
					//strcat(line, sep);
					printf("read: '%s'\n\n", line);
					
					//if(strcmp(line, "%%END") != 0){
						strcat(line, "\n");
						fputs(line, fd_out);
					//}
					
					//printf("\ntemp is: \n'%s'\n", temp);
					//printf("temp len: %zu\n\n", strlen(temp));
				}
				
				//while(sscanf(temp, "%[^\n]", line) == 1 && strcmp(temp, "") != 0) {
					//sscanf(temp, "%c", sep);
					//strcat(line, sep);
					
					//printf("\ngot: %s\nleft in buffer: %zu\n\n", line, strlen(line)); 
				//}
					
			}
			
			
		}
		
		
		
		//printf("converting...\n");
		//to_upper(buf);
		
		//printf("sending: %s\n", buf);
		//int len = strlen(buf), bytes_sent;
		//if ((bytes_sent = send(new_fd, buf, len, 0)) == -1) {
			//perror("send");
			//exit(1);
		//}

	//}
	

}



