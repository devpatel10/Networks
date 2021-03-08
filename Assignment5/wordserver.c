/*
        ##################################################################
        #              Assignment 5 Computer Networks Lab                #
        #                           GROUP NO : 43                        #
        #           Patel Devarshi Chandrakant   | 18CS10040             #
        #               Saransh Patel            | 18CS30039             #
        ##################################################################        
*/

//Compile using gcc wordserver.c -o wordserver
//To run ./wordserver
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* the port users will be connecting to */
#define MYPORT 4950
//Max buffer length of response and receiving query
#define MAXBUFLEN 500

//Reading word from file in form of string
int getword(FILE * ptr, char * buf)
{
    //If the file has reached EOF before encountering word "END" then giving error
    if(feof(ptr))
    {
        printf("End of file reached\nWRONG FORMAT GIVEN\n");
        //exit(1);
        return 0;
    }
    //Reading from file
    fscanf(ptr,"%s",buf);
    return 1;
}
int main(int argc, char *argv[])
{
    int sockfd;
    char *fnf="FILE_NOT_FOUND";
    char *wf="WRONG_FILE_FORMAT";
    /* my address information */
    struct sockaddr_in my_addr;
    /* connector’s address information */
    struct sockaddr_in their_addr;
    int addr_len, numbytes;
    //Maintains strings of response and queries received
    char buf[MAXBUFLEN];
    addr_len = sizeof(struct sockaddr);
    //Initializing socket of type IPv4 (UDP) Datagram Socket (SOCK_DGRAM):
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        //Error Handling
        perror("Server-socket() sockfd error!");
        exit(1);
    }  
    printf("PORT NO : %d Connection established\n",MYPORT);
    /* host byte order */
    my_addr.sin_family = AF_INET;
    /* short, network byte order */
    my_addr.sin_port = htons(MYPORT);
    /* automatically fill with my IP */
    my_addr.sin_addr.s_addr = INADDR_ANY;
    /* zero the rest of the struct */
    memset(&(my_addr.sin_zero), '\0', 8);

    //Binding with the port 
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        //Error Handling
        perror("Server-bind() error!");
        exit(1);
    }

    //Loop to continue multiple client request after a request is finished
    while(1)
    {
        printf("\nWaiting for client filename\n");
        //Receiving the filename from client
        if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            //Error Handling
            perror("Server-recvfrom() error!");
            exit(1);
        }
        //Making proper string
        buf[numbytes] = '\0';
        //File pointer
        FILE * ptr;

        //If the file exist 
        if(ptr=fopen(buf,"r"))
        {
            //Getting the first word from file
            getword(ptr,buf);
            //If the word is not hello the giving WRONG_FORMAT response to client and exiting server process
            if(strcmp(buf,"HELLO")!=0)
            {
                if((numbytes=sendto(sockfd, wf, strlen(wf), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1)
                {
                    //Error Handling
                    perror("Server-file sendto() error!");
                    exit(1);
                }
                printf("Wrong File Format\n");
                exit(1);
            }
            //If the first word is indeed HELLO the getting the words from file
            else{
                //Sending Hello to client so that it knows the file is exists and fine 
                if((numbytes=sendto(sockfd, "HELLO", strlen("HELLO"), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1)
                {
                    //Error Handling
                    perror("Server-file sendto() error!");
                    exit(1);
                }

                //While loop till we reach the last word END
                while(strcmp(buf,"END")!=0)
                {
                    //Receiving word query WORD<NO> from client
                    if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len))==-1)
                    {
                        //Error Handling
                        perror("Server-recvfrom() error!");
                        exit(1);
                    }
                    //Printing the query received from client
                    buf[numbytes]='\0';
                    printf("Word request from client : %s\n",buf);
                    fflush(stdout);
                    //Handling the case if EOF is reached without reading END word 
                    if(!getword(ptr,buf))
                    {
                        if((numbytes =sendto(sockfd, "END", strlen("END"), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1)
                        {
                            //Error Handling
                            perror("Server-file sendto() error!");
                            exit(1);
                        }
                        exit(1);
                    }
                    //Sending the word to client
                    else{
                        if((numbytes =sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1)
                        {
                            //Error Handling
                            perror("Server-file sendto() error!");
                            exit(1);
                        }
                    }   
                    
                }
                //Closing file ptr
                fclose(ptr);
            }
        }

        //Giving response of FILE_NOT_FOUND if file doesnot exist and waiting for another client request
        else{
            sendto(sockfd, fnf, strlen(fnf), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
            printf("File Not Found\n");
        }
    }
    return 0;
}