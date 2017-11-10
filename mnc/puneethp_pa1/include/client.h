#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "global.h"
#include "logger.h"
#include <strings.h>
#include <string.h>
#include <unistd.h>
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256
#define MSG_SIZE 256
struct block{
	 char name[30];
	 char port[10];
	 char ipad[40];
	// char *name;
	// char *port;
	// char *ipad;
};

int compare(const void *s1, const void *s2)
					{
					  struct block *e1 = (struct block *)s1;
					  struct block *e2 = (struct block *)s2;
					  return atoi(e1->port) - atoi(e2->port);
					  
					}
int client(char* por)
{
	int connect_to_host(char *, int);
	struct block blist[10];
	int k=0,j=0;
	int server2,selret;
	fd_set wlist,slist;
	FD_SET(0,&wlist);
	loop:while(TRUE){
		 	printf("\n[SHELL]$ ");
			char *command = (char*) malloc(sizeof(char)*CMD_SIZE);
			fgets(command, CMD_SIZE-1, stdin);
			strtok(command,"\n");
			char *cmd;
			cmd=strtok(command," ");
				if(strncmp("AUTHOR",cmd,strlen(cmd))==0){
					char *ubit="puneethp";
					cse4589_print_and_log("[%s:SUCCESS]\n", command);
					cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",ubit);
					
					}
						
				else if(strncmp("IP",cmd,strlen("IP"))==0){
				int fd;
				struct ifreq ifr;
				fd = socket(AF_INET, SOCK_DGRAM, 0);
				ifr.ifr_addr.sa_family = AF_INET;
				strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
						
				ioctl(fd, SIOCGIFADDR, &ifr);
				 close(fd);
      			 cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
				 cse4589_print_and_log("IP:%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
				 
				}
				else if(strncmp("PORT",cmd,strlen("PORT"))==0){
						cse4589_print_and_log("[%s:SUCCESS]\n", command);
						cse4589_print_and_log("PORT:%s\n",por);
						
						}
				else if(strncmp("EXIT",cmd,4)==0){
						cse4589_print_and_log("[%s:SUCCESS]\n", command);
						cse4589_print_and_log("[%s:END]\n", command);
						exit(0);
				}
				else if(strncmp("LOGIN",cmd,strlen(cmd))==0){
					char *ip=strtok(NULL," ");
					char *portt=strtok(NULL,"\n");
					if(portt==NULL){
					cse4589_print_and_log("[%s:ERROR]\n", cmd);
					cse4589_print_and_log("[%s:END]\n", cmd);
					goto loop;
					}
					
					//printf("%s %d",ip,atoi(portt));
					server2 = connect_to_host(ip, atoi(portt));
					int head=server2;					
					if(server2==0) {
					cse4589_print_and_log("[%s:ERROR]\n", cmd);
					cse4589_print_and_log("[%s:END]\n", cmd);
					goto loop;
					}
					else{
						cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
						cse4589_print_and_log("[%s:END]\n", cmd);
					}
					
					FD_SET(head,&wlist);
					char port[7];
					char pt[20]="PORT ";
					sprintf(port,"%d",por);
					
					strcat(pt,por);
					//printf("%s\n",pt);
					send(server2,pt,strlen(pt),0);
					char name[50]="NAME ";
					char hname[30];
					gethostname(hname,sizeof(hname));
					strcat(name,hname);
					send(server2,name,strlen(name),0);
	
				loop2:while(TRUE){
					memcpy(&slist, &wlist, sizeof(wlist));
					printf("\n[PA1-Client@CSE489/589]$ ");
					fflush(stdout);
					
					selret = select(head + 1, &slist, NULL, NULL, NULL);
					//if(selret < 0){perror("select failed.");}
					if(selret>0){
					for(int sock=0;sock<head+1;sock+=1){
					if(FD_ISSET(sock, &slist)){
					if(sock==0){
					char *msg1 = (char*) malloc(sizeof(char)*MSG_SIZE);
				//memset(msg1, '\0', MSG_SIZE);
				//if(fgets(msg1, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
					//exit(-1);
				fgets(msg1, MSG_SIZE-1, stdin);
				char mes[100];
				strcpy(mes,msg1);
				strtok(msg1,"\n");
					char *token=strtok(msg1," ");
					//if(send(server,msg1,strlen(msg1),0)==strlen(msg1)) printf("sent");
					//else printf("send error");
				if(strncmp("AUTHOR",token,strlen(token))==0){
				char *ubit="puneethp";
				cse4589_print_and_log("[%s:SUCCESS]\n", token);
				cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",ubit);
				
				}
						
				else if(strncmp("IP",token,strlen(token))==0){
				int fd;
				struct ifreq ifr;
				fd = socket(AF_INET, SOCK_DGRAM, 0);
				ifr.ifr_addr.sa_family = AF_INET;
				strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
						
				ioctl(fd, SIOCGIFADDR, &ifr);
				 close(fd);
      			 cse4589_print_and_log("[%s:SUCCESS]\n", token);
				 cse4589_print_and_log("IP:%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
				 
				}
				else if(strncmp("PORT",token,strlen(token))==0){
						cse4589_print_and_log("[%s:SUCCESS]\n", token);
						cse4589_print_and_log("PORT:%s\n",por);
						
						}
				else if(strncmp(token,"SEND",4)==0){
					char *msg=strtok(NULL,"\n");
					fflush(stdout);
					//printf("%s",msg);
					send(head, msg, strlen(msg), 0);
				
					fflush(stdout);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
					}
					
				
				
				else if(strncmp("LOGOUT",token,6)==0){
					close(head);
					FD_CLR(head, &wlist);
					
					//send(server2,token,strlen(token),0);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
					cse4589_print_and_log("[%s:END]\n", token);
					goto loop;
					}
				else if(strncmp("LIST",token,strlen(token))==0){
					send(server2,token,strlen(token),0);
					 // printf("\nNo\tName\t\t\tIpaddr\t\tport\n");
					 // printf("%s",blist[0].name);
					// // //qsort(blist,j, sizeof(struct block), compare);	
						// for(int i=0;i<j;i++){
							// printf("%d\t%s\t%s\t%s\n",i+1,blist[i].name,blist[i].ipad,blist[i].port);
						 // }
				    goto loop2;
					//cse4589_print_and_log("[%s:SUCCESS]\n", token);
				}
				else if(strncmp("REFRESH",token,strlen(token))==0){
					send(server2,token,strlen(token),0);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
				}
				else if(strncmp("EXIT",token,4)==0){
					send(server2,token,strlen(token),0);
					close(head);
					FD_CLR(head, &wlist);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
					cse4589_print_and_log("[%s:END]\n", token);
					exit(0);
					}
				else if(strncmp(token,"BLOCK",5)==0){
					send(server2,mes,strlen(mes),0);
					//printf("%s\n",mes);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
				}
				else if(strncmp("BROADCAST",token,9)==0){
					//char *msg=strtok(NULL,"\n");
					send(server2,mes,strlen(mes),0);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
				}
				else if(strncmp("UNBLOCK",token,7)==0){
					//printf("%s\n",msg1);
					send(server2,mes,strlen(mes),0);
					//printf("%s\n",msg1);
					cse4589_print_and_log("[%s:SUCCESS]\n", token);
				}
				
				else{
					cse4589_print_and_log("[%s:ERROR]\n", msg1);
				}
				cse4589_print_and_log("[%s:END]\n", msg1);
			}
				else if(sock == head){
				/* Initialize buffer to receieve response */
				char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
				memset(buffer, '\0', BUFFER_SIZE);

				if(recv(server2, buffer, BUFFER_SIZE, 0) >= 0){
					//char mass[256];
					//strcpy(mass,buffer);
					//printf("Server responded: %s", buffer);
					//fflush(stdout);
					char *rad=strtok(buffer," ");
					if(strncmp(rad,"LISTR",5)==0){
						//printf("\nNo\tName\t\t\tIpaddr\t\tport\n");
						cse4589_print_and_log("[LIST:SUCCESS]\n");
						//printf("detecting rec");
						char *num=strtok(NULL," ");
						k=0;
						j=atoi(num);
						//printf("detecting count%d\n",j);
						
						for(int i=0;i<j;i++){
						struct block b1; 
						char *name=strtok(NULL," ");
						char *ip=strtok(NULL," ");
						char *port=strtok(NULL,"\n");
						cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i+1,name,ip,atoi(port));
						//cse4589_print_and_log("%d\t%s\t",i+1,name);
						//cse4589_print_and_log("%s\t",ip);
						//cse4589_print_and_log("%s\n",port);
						// strcpy(b1.name,name);
						// strcpy(b1.ipad,ip);
						// strcpy(b1.port,port);
					
						
						//printf("%d\t%s\t\t%s\t%s\n",k+1,b1.name,b1.ipad,b1.port);
						//printf("\n%d\n",k);
						//blist[k]=b1;
						
						//k++;
						//printf("%s",buffer);
						
					}
					cse4589_print_and_log("[LIST:END]\n");
					
					}
					else{
					char *mess=strtok(NULL,"\n");
					cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
					cse4589_print_and_log("msg from:%s\n[msg]:%s\n", rad, mess);
					//cse4589_print_and_log("\n[RECIEVED:SUCCESS]\nsender:%s",rad);
					
					//cse4589_print_and_log("\n[MSG:%s]",mess);
					cse4589_print_and_log("[RECEIVED:END]\n");
					fflush(stdout);
				}
				}
				else{printf("receive error");}
				}//sock=head
				}//if(isset)
				}//for loop select
				}//if(sel>0)
			
			}//while true commands
			}//elseif login
			else{
				cse4589_print_and_log("[%s:ERROR]\n", command);
				}
			cse4589_print_and_log("[%s:END]\n",command );
		  }//while(1)
		}//main

int connect_to_host(char *server_ip, int server_port)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0) fdsocket=0;
       

    return fdsocket;
}

