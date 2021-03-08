/*
        ##################################################################
        #              Assignment 5 Computer Networks Lab                #
        #                           GROUP NO : 43                        #
        #           Patel Devarshi Chandrakant   | 18CS10040             #
        #               Saransh Patel            | 18CS30039             #
        ##################################################################        
*/

//Compile using gcc wordclient.c -o wordclient
//To run ./wordclient 127.0.0.1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/* the port users will be connecting to */
#define MYPORT 4950
//Max buffer length of response and sending query
#define MAXBUFLEN 500

//Append no to string "WORD"  ---- WORD + 12 == WORD12
void makeword(char *str,int a)
{
    char str1[MAXBUFLEN];
    int count=0;
    while(a>0)
    {
        str1[count]=(a%10)+'0';
        count++;
        a/=10;
    }
    str1[count]='\0';
    for(int i=count-1;i>=0;i--)
    {
        str[4-i-1+count]=str1[i];
    }
    str[4+count]='\0';

}

//Writing to outputfile
void writeword(FILE* ptr,char *buf){
    fprintf(ptr,"%s\n",buf);
    return;
}
int main(int argc, char *argv[ ])
{
    int sockfd;
    char filename[MAXBUFLEN];
    char buf[MAXBUFLEN];
    /* connector’s address information */
    struct sockaddr_in their_addr;
    struct hostent *he;
    int numbytes,addr_len;
    addr_len=sizeof(struct sockaddr);
    //Defining command line format
    if (argc != 2)
    {
        fprintf(stderr, "Client-Usage: %s <hostname> \n", argv[0]);
        exit(1);
    }
    /* get the host info */
    //Getting host by name In our example it is 127.0.0.1
    if ((he = gethostbyname(argv[1])) == NULL)
    {
        //Error Handling
        perror("Client-gethostbyname() error!");
        exit(1);
    }
    //Initializing socket of type IPv4 (UDP) Datagram Socket (SOCK_DGRAM):
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        //Error Handling
        perror("Client-socket() error lol!");
        exit(1);
    }
    /* host byte order (IPv4)*/
    their_addr.sin_family = AF_INET;
    /* short, network byte order */
    printf("Using port: 4950\n");
    //htons called on int datatype to protect from big-small indian problems
    their_addr.sin_port = htons(MYPORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    /* zero the rest of the struct */
    memset(&(their_addr.sin_zero), '\0', 8);
    //Prompting user to enter the filename
    printf("Enter filename : ");
    scanf("%s",filename);
    //Sending the filename to server
    if((numbytes = sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
    {
        //Error Handling
        perror("Client-file sendto() error!");
        exit(1);
    }
    //Receiving response from server
    if((numbytes=recvfrom(sockfd,buf,MAXBUFLEN-1,0,(struct sockaddr*)&their_addr,&addr_len))==-1)
    {
        //Error Handling
        perror("Error!");
        exit(1);
    }
    //making it to proper string format
    buf[numbytes]='\0';
    //If the response from server is FILE_NOT_FOUND then printing error and exiting
    if(strcmp(buf,"FILE_NOT_FOUND")==0){
        printf("File Not Found\n");
        exit(1);
    }
    //If the response from server is WRONG_FILE_FORMAT then printing error and exiting
    else if(strcmp(buf,"WRONG_FILE_FORMAT")==0){
        printf("Wrong File Format\n");
        exit(1);
    }

    //Generating output file by the name out.<filename>
    char output[MAXBUFLEN];
    output[0]='o',output[1]='u',output[2]='t',output[3]='.'; 
    for(int i=0;i<strlen(filename);i++)
    {
        output[i+4]=filename[i];
    }
    output[4+strlen(filename)]='\0';

    //Opening output file in "w" mode
    FILE*ptr=fopen(output,"w");

    //str stores the string of the format WORD<INT> for quering
    char str[MAXBUFLEN];
    str[0]='W';
    str[1]='O';
    str[2]='R';
    str[3]='D';
    str[4]='\0';
    for(int i=1;strcmp(buf,"END")!=0;i++)
    {
        //Making str to proper format
        makeword(str,i);

        //Sending it to server
        if((numbytes = sendto(sockfd, str, strlen(str), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1)
        {
            //Error Handling
            perror("Client-file sendto() error!");
            exit(1);
        }

        //Receiving response from server
        if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0, (struct sockaddr *)&their_addr, &addr_len))==-1)
        {
            //Error Handling
            perror("Client-recvfrom() error!");
            exit(1);
        }
        buf[numbytes]='\0';

        //If Reached "END" the exiting
        if(strcmp(buf,"END")==0)
            break;
        writeword(ptr,buf);
    }

    //Closing file pointer
    fclose(ptr);

    //Closing socket
    if (close(sockfd) != 0)
    printf("Client-sockfd closing is failed!\n");
    else
    printf("Client-sockfd successfully closed!\n"); 
    return 0;
}