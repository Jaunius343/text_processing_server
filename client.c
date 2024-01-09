#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h> 

#define MYPORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 1000

void inform_server(int sockfd);
void wait_for_server(int sockfd);

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
		//printf("bytes left: %d\n", bytesleft);
		printf("sending\n");
		
		inform_server(s);
		
		wait_for_server(s);
		
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 onm failure, 0 on success
} 

int main(int argc, char *argv[])
{
	int status;
	struct addrinfo hints, *res;
	int sockfd, numbytes;
	char buf [MAXDATASIZE];
	
	if (argc != 3) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

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
	
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("client: connect");
            exit(1);
    }
    
    char s[INET6_ADDRSTRLEN];
    inet_ntop(res->ai_family, get_in_addr((struct sockaddr *)res->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

	freeaddrinfo(res); // free the linked-list
	
	//printf("input text for sending: \n");
	//char input[MAXDATASIZE];
	int len, bytes_sent;
	//scanf("%s", input);
	
	//len = strlen(input);
	//if ((bytes_sent = send(sockfd, input, len, 0)) == -1) {
			//perror("send");
			//exit(1);
	//}
	
	
	//if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			//perror("recv");
			//exit(1);
		//}
	//buf[numbytes] = '\0';
    
	//printf("received from server: '%s'\n",buf);
	
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
    
	
	//jei failas tuscias, net nepradeda siusti duomenu
    while (fscanf(fptr, "%[^\n]", line1) == 1){
		fscanf(fptr, "%c", temp);
			//fscanf(fptr, "%c", temp);
		strcat(line1, temp);
		//printf("%s", line1);
		
		//fputs(line1, fd_out);						//servui
		//if ((bytes_sent = send(sockfd, line1, strlen(line1), 0)) == -1) {
			//perror("send");
			//exit(1);
		//}
		
		//inform_server(sockfd);
		
		//wait_for_server(sockfd);
		
		len = strlen(line1);
		if(sendall(sockfd, line1, &len) == -1){
			perror("send");
			exit(1);
		}
		
		if (fscanf(fptr2, "%[^\n]", line2) == 1) {
			fscanf(fptr2, "%c", temp);																
				//fscanf(fptr2, "%c", temp);																
			strcat(line2, temp);
			//printf("%s", line2);	
			
			//fputs(line2, fd_out);					//servui
			//if ((bytes_sent = send(sockfd, line2, strlen(line2), 0)) == -1) {
				//perror("send");
				//exit(1);
			//}
			
			//inform_server(sockfd);
			
			//wait_for_server(sockfd);
			
			len = strlen(line2);
			if(sendall(sockfd, line2, &len) == -1){
				perror("send");
				exit(1);
			}
		}
		

	} if (feof(fptr)){
		while (fscanf(fptr2, "%[^\n]", line2) == 1){
			fscanf(fptr2, "%c", temp);
				//fscanf(fptr2, "%c", temp);
			strcat(line2, temp);
			//printf("%s", line2);
		
			//fputs(line2, fd_out);					//servui
			//if ((bytes_sent = send(sockfd, line2, strlen(line2), 0)) == -1) {
				//perror("send");
				//exit(1);
			//}
			
			//inform_server(sockfd);
			
			//wait_for_server(sockfd);
		
			len = strlen(line2);
			if(sendall(sockfd, line2, &len) == -1){
				perror("send");
				exit(1);
			}
		} if (feof(fptr2)){
			//baigesi visas skaitymas, reikia pranesti serveriui
			char input[MAXDATASIZE] = "%%END";
			len = strlen(input);
			
			if ((bytes_sent = send(sockfd, input, len, 0)) == -1) {
				perror("send");
				exit(1);
			}
			
				
		}
	}
    fclose(fptr);

	
	return 0;
}

	//pranesam, kad siusim duomenis
void inform_server(int sockfd)
{
	char wr[MAXDATASIZE] = "%%T";
	int len = strlen(wr);	

	printf("informing server\n");
	
	if ((send(sockfd, wr, len, 0)) == -1) {
		perror("send");
		exit(1);
	}
}

	//laukiam leidimo
void wait_for_server(int sockfd)
{
	char buf [MAXDATASIZE];
	int numbytes;
	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}

	buf[numbytes] = '\0';
	
	printf("server: %s\n", buf);

}
