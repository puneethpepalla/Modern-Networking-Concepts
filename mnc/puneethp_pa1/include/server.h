#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/ioctl.h>
#include "global.h"
#include "logger.h"
#include <net/if.h>

#include <ifaddrs.h>
#include <string.h>
#include <unistd.h>
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256
struct details{
	int fds;
	char ipaddr[30];
	int port;
	int msg_sent;
	int msg_rec;
	char* status;
	int bc;//number of messages buffered
	char name[50];
	char bufmsg[20][50];
	char blocked[10][20];
	char *exitted;
	int nbk;//number of blocked clients
};
int compare2(const void *s1, const void *s2)
					{
					  struct details *e1 = (struct details *)s1;
					  struct details *e2 = (struct details *)s2;
					  return e1->port - e2->port;
					  
					}
 // struct Node{
	 // char* data;
	 // struct Node* next;
 // };
 //struct Node
int server(char* por)
{
	
	struct details clist[10];

	int port, server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
	struct sockaddr_in server_addr, client_addr;
	fd_set master_list, watch_list;
	int i=0,j=0;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
		perror("Cannot create socket");
	port = atoi(por);
	bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* Bind */
    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
    	perror("Bind failed");

    /* Listen */
    if(listen(server_socket, BACKLOG) < 0)
    	perror("Unable to listen on port");

    /* ---------------------------------------------------------------------------- */

    /* Zero select FD sets */
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);
    
    /* Register the listening socket */
    FD_SET(server_socket, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    while(TRUE){
        memcpy(&watch_list, &master_list, sizeof(master_list));

        printf("\n[PA1-Server@CSE489/589]$ ");
		fflush(stdout);

        /* select() system call. This will BLOCK */
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if(selret < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if(selret > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(sock_index=0; sock_index<=head_socket; sock_index+=1){
				//printf("looping ");
                if(FD_ISSET(sock_index, &watch_list)){
		
                    /* Check if new command on STDIN */
                    if (sock_index == STDIN){
						//printf("input\n");
                    	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                    	memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);

						//printf("\nI got: %s\n", cmd);
						//fgets(cmd, CMD_SIZE-1, stdin);
						//Process PA1 commands here ...
						strtok(cmd,"\n");
						if(strncmp("AUTHOR",cmd,strlen(cmd))==0){
							char *ubit="puneethp";
							cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",ubit);
							
							
						}
						else if(strncmp("PORT",cmd,strlen("PORT"))==0){
							cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
							cse4589_print_and_log("PORT:%s\n",por);
							
						}
						
						else if(strncmp(strtok(cmd," "),"BLOCKED",7)==0){
							char *ip=strtok(NULL,"\n");
							//printf("%s\n",ip);
							printf("list of blocked clients by %s\t%d\n",ip,j);
							for(int l1=0;l1<j;l1++){
								//printf("\nchecking ip with:%s",clist[l1].ipaddr);
								//printf("\n%d\t%d",strlen(clist[l1].ipaddr),strlen(ip));
								
								if(strncmp(clist[l1].ipaddr,ip,strlen(clist[l1].ipaddr))==0){
								//printf("\nip found\t number blcked:%d\n ",clist[l1].nbk);
								for(int k=0;k<clist[l1].nbk;k++){
								cse4589_print_and_log("\n%d: %s",k+1,clist[l1].blocked[k]);
								}
								break;
						}
							}
							cse4589_print_and_log("\n[%s:SUCCESS]\n", cmd);
						}
						else if(strncmp("LIST",cmd,strlen("LIST"))==0){
							printf("Sno\tNAME\t\t\tipaddr\t\tport\n");
								
							cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
							for(int l=0;l<j;l++){
							if(strncmp(clist[l].status,"loggedin",8)==0){
								cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",l+1,clist[l].name,clist[l].ipaddr,clist[l].port);
							}
							}
							
						}
						else if(strncmp("IP",cmd,strlen("IP"))==0){
						 int fd;
						 struct ifreq ifr;
						 fd = socket(AF_INET, SOCK_DGRAM, 0);
						 ifr.ifr_addr.sa_family = AF_INET;
						 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
						
						 ioctl(fd, SIOCGIFADDR, &ifr);

						 close(fd);

						 /* display result */
						cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
						cse4589_print_and_log("IP:%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
						
						}
						else if(strncmp("STATISTICS",cmd,strlen(cmd))==0){
							//qsort(clist,j, sizeof(struct details), compare2);	
							//printf("Sno\tNAME\t\t\tMsgs_sent\tmsgs_received\tstatus\n");
							cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
							for(int l=0;l<j;l++){
								//printf("file desc\tipaddr\tport\n");
								
								cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n",l+1,clist[l].name,clist[l].msg_sent,clist[l].msg_rec,clist[l].status);
							}
							
						}
						else{
							cse4589_print_and_log("[%s:ERROR]\n", cmd);
						}
						cse4589_print_and_log("[%s:END]\n", cmd);
						free(cmd);
                    }
                    /* Check if new client is requesting connection */
                    else if(sock_index == server_socket){
						//printf("server sock\n");
						//bzero(&client_addr, sizeof(client_addr));
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
						if(fdaccept < 0)
                            perror("Accept failed.");
						FD_SET(fdaccept, &master_list);
						if(fdaccept > head_socket) head_socket = fdaccept;
						printf("\n%d client connected!\n",i+1);
						//i++;
						
				
                        /* Add to watched socket list */
                        
						int flag=0;
						for(int l3=0;l3<j;l3++){
							//printf("%s\t%s\n",clist[l3].ipaddr,inet_ntoa(client_addr.sin_addr));
							 if(strncmp(clist[l3].ipaddr,inet_ntoa(client_addr.sin_addr),strlen(clist[l3].ipaddr))==0){
							flag=1;
							printf("matched");
							clist[l3].status="loggedin";
							clist[l3].fds=fdaccept;
							i++;
							//break;
							printf("\nbuff messages:%d",clist[l3].bc);
							 for(int i=0;i<clist[l3].bc;i++){
							 send(clist[l3].fds, clist[l3].bufmsg[i], strlen(clist[l3].bufmsg[i]), 0);
							 clist[l3].msg_rec++;
							 }
							 clist[l3].bc=0;
							}
							
							
						}
									
					if(flag==0){
						struct details c1;
						strcpy(c1.ipaddr,inet_ntoa(client_addr.sin_addr));
						//memcpy(&c1.ipaddr,&client_addr.sin_addr,sizeof(client_addr.sin_addr));
						//c1.port=client_addr.sin_port;
						c1.fds=fdaccept;
						//strcpy(c1.status,"logged_in");
						c1.status="loggedin";
						c1.exitted="no";
						c1.bc=0;
						c1.msg_sent=0;
						c1.msg_rec=0;
						//gethostname(c1.name,sizeof(c1.name));
						clist[j]=c1;
						if(j<10) j++;
						i++;
                        
                    }
					qsort(clist,j, sizeof(struct details), compare2);
					// char buffer[500]="LISTR ";
								// char num[3];
					
					            // sprintf(num,"%d",i);
								// strcat(buffer,num);
								// strcat(buffer," ");
								
								// for(int in=0;in<j;in++){
									// if(strncmp(clist[in].status,"loggedin",8)==0){
									// strcat(buffer,clist[in].name);//+clist[i].port+clist[i].ipaddr
									// //printf("%s\n",clist[in].name);
									// strcat(buffer," ");
									// strcat(buffer,clist[in].ipaddr);
									// strcat(buffer," ");
									// //buffer=buffer+clist[i].port;
									// char prt[10];
									// sprintf(prt,"%d",clist[in].port);
									// strcat(buffer,prt);
									// strcat(buffer,"\n");
									// }
								// }
								// //printf("%s",buffer);
								// send(fdaccept,buffer,strlen(buffer),0);
					}
                    /* Read from existing clients */
                    else{
						
						// fflush(stdout);
						
                        // fflush(stdout);
						/* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
						//struct msg buffer;
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                            
							for(int l2=0;l2<j;l2++){
								if(sock_index==clist[l2].fds){
									clist[l2].status="loggedout";
									clist[l2].fds=-1;
								printf("%s has been logged out!\n",clist[l2].name);
								i--;
								break;
								}
								}
								
								
							
							close(sock_index);
                            
                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                        }
                        else {
							
							char* ip=strtok(buffer," ");
							// if(strncmp("LOGOUT",ip,strlen("LOGOUT"))==0){
								// //printf("\nlogging out1");
								// for(int l1=0;l1<j;l1++){
								// if(sock_index==clist[l1].fds){
								// clist[l1].status="loggedout";
								// clist[l1].fds=-1;
								// //fflush(stdout);
								// close(sock_index);
								
								// FD_CLR(sock_index, &master_list);
								// i--;
								 // }
								 // }
							 // }
							 if(strncmp("REFRESH",ip,7)==0){
								 printf("refreshed");
							 }
							 else if(strncmp("LIST",ip,4)==0){
								char buffer[500]="LISTR ";
								char num[3];
					
					            sprintf(num,"%d",i);
								strcat(buffer,num);
								strcat(buffer," ");
								
								for(int in=0;in<j;in++){
									if(strncmp(clist[in].status,"loggedin",8)==0){
									strcat(buffer,clist[in].name);//+clist[i].port+clist[i].ipaddr
									strcat(buffer," ");
									strcat(buffer,clist[in].ipaddr);
									strcat(buffer," ");
									//buffer=buffer+clist[i].port;
									char prt[10];
									sprintf(prt,"%d",clist[in].port);
									strcat(buffer,prt);
									strcat(buffer,"\n");
									
									}
								}
								//printf("\n%s",buffer);
								send(sock_index,buffer,strlen(buffer),0);
							}
							else if(strncmp("PORT",ip,4)==0){
								char *ipp=strtok(NULL,"\n");
								//printf("%s",ipp);
								for(int i=0;i<j;i++){
									
									if(clist[i].fds==sock_index) {
									clist[i].port=atoi(ipp);
									//printf("%d\n",clist[i].port);
									}
																
								}
							}
							else if(strncmp("NAME",ip,4)==0){
								char *ipp=strtok(NULL,"\n");
								//printf("%s",ipp);
								for(int i=0;i<j;i++){
									
									if(clist[i].fds==sock_index) {
									strcpy(clist[i].name,ipp);
									//printf("%d\n",clist[i].port);
									}
																
								}
							}
							else if(strncmp("EXIT",ip,4)==0){
								
									for(int l2=0;l2<j;l2++){
								if(sock_index==clist[l2].fds){
									clist[l2].exitted="yes";
									clist[l2].fds=-1;
									printf("%s has been exitted!\n",clist[l2].name);
									i--;
									break;
									
								}
								}
								struct details nlist[10];
								int y=0;
								for(int z=0;z<j;z++){
									if(strncmp(clist[z].exitted,"no",2)==0){
										nlist[y]=clist[z];
										y++;
									}
							}
							struct details clist[10];
							memcpy(clist, nlist, sizeof(nlist));
							j=y;
							close(sock_index);
							FD_CLR(sock_index, &master_list);
							}
							else if(strncmp("BLOCK",ip,5)==0){
								char *ipp=strtok(NULL,"\n");
								//char ip[20];strcpy(ip,ipp);
								for(int i=0;i<j;i++){
									if(clist[i].fds==sock_index){
									strcpy(clist[i].blocked[clist[i].nbk],ipp);
									clist[i].nbk++;
									}
								}
								for(int i=0;i<j;i++){
									if(clist[i].fds==sock_index) {
										for(int b=0;b<clist[i].nbk;b++){
										printf("\n%s\t%d\n",clist[i].blocked[b],clist[i].nbk);
										}
										break;
									}
									//clist[i].nbk++;
								}
							}
							else if(strncmp("BROADCAST",ip,9)==0){
								char *text=strtok(NULL,"\n");
								for(int l1=0;l1<j;l1++){
								if(sock_index==clist[l1].fds){
									clist[l1].msg_sent++;
									char ip[40];
									strcpy(ip,clist[l1].ipaddr);
									strcat(ip,text);
									//continue;
									}
								}
								for(int l1=0;l1<j;l1++){
								if(sock_index==clist[l1].fds){continue;}
								else if(strncmp(clist[l1].status,"loggedout",strlen("loggedout"))==0){
									strcpy(clist[l1].bufmsg[clist[l1].bc],ip);
								    clist[l1].bc++;
									//printf("\nbuffer :%d",clist[l1].bc);
								}
								else{
								 send(clist[l1].fds, ip, strlen(text), 0);
								// //printf("\nsending message from %s to %s",ip,inet_ntoa(clist[l1].ipaddr));
								printf("\nsending message from %d to %d",sock_index,clist[l1].fds);
								}
								clist[l1].msg_rec++;
								}
							
							}
							else if(strncmp("UNBLOCK",ip,7)==0){
								char *ipp=strtok(NULL,"\n");
								//printf("%s",ipp);
								//printf("\nserver");
								//printf("\nloops:%d",j);
								for(int i=0;i<j;i++){
									//printf("\nlooping %d:%d",clist[i].fds,sock_index);
									//printf("\n %d",(clist[i].fds == sock_index));
									if(clist[i].fds == sock_index){ 
									//printf("\ndetect sockindex");
									for(int k=0;k<clist[i].nbk;k++){
										printf("\ncomparing:%s:%s",clist[i].blocked[k],ipp);
										if(strncmp(clist[i].blocked[k],ipp,strlen(ipp))==0){
											//printf("\nfound hai");
											strcpy(clist[i].blocked[k],"del");
											break;
										}
										
									}
									char blocked[10][20];
									int l=0;
									for(int k=0;k<clist[i].nbk;k++){
										
										if(strncmp(clist[i].blocked[k],"del",3)==0) continue;
										//printf("\nstarted copying");
										strcpy(blocked[l],clist[i].blocked[k]);
										l++;
										
									}
									//printf("\ncopied into blcked");
									memset(clist[i].blocked[0], 0, sizeof(char)*20*10);
									//printf("\nset arr to null");
									for(int z=0;z<l;z++){
										//printf("\ncop in main arr");
									strcpy(clist[i].blocked[z],blocked[z]);}
									//printf("\ndone");
									clist[i].nbk=l;
									
									}
								
								//printf("unblocked %s",ipp);
								break;
							}
							}
							else{
							char* text=strtok(NULL,"\n");
								char sdr[256];//msg size
								char sadr[30];
								for(int l1=0;l1<j;l1++){
								
								if(sock_index==clist[l1].fds){
									clist[l1].msg_sent++;
									
									strcpy(sadr,clist[l1].ipaddr);
									strcpy(sdr,clist[l1].ipaddr);
									strcat(sdr," ");
									
									}
								}
								
								for(int l1=0;l1<j;l1++){
								if(strncmp(clist[l1].ipaddr,ip,strlen(ip))==0){
									int b=0;
									char *ss;
									for(int i=0;i<j;i++){
										if(clist[i].fds==sock_index){
										ss=clist[i].ipaddr;
										break;
										}
									}
									for(int i=0;i<clist[l1].nbk;i++){
										printf("\ncomparing %s:%s",clist[l1].blocked[i],ss);
									if(strncmp(clist[l1].blocked[i],ss,strlen(ss))==0) {
										printf("\nblocked message");
										b=1;
										break;
										}
									}
									if(b==0){
									if(strncmp(clist[l1].status,"loggedout",strlen("loggedout"))==0){
									strcat(sdr,text);
									strcpy(clist[l1].bufmsg[clist[l1].bc],sdr);
								    clist[l1].bc++;
									break;
									}
									else{
									strcat(sdr,text);
									send(clist[l1].fds, sdr, strlen(sdr), 0);
								// //printf("\nsending message from %s to %s",ip,inet_ntoa(clist[l1].ipaddr));
								cse4589_print_and_log("[RELAYED:SUCCESS]\n");
								cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sadr,clist[l1].ipaddr,text);
								cse4589_print_and_log("[RELAYED:END]\n");
								}
									
								clist[l1].msg_rec++;
									}
								}
								}
								}
								//for(int i=0;i<j;i++){if(sock_index==clist[i].fds) clist[i].msg_sent++;}
								
						}
						free(buffer);
                    }//closing client rec msg
                }//isset
           }//for loop select
        }//if(sel)
	}//while true
    return 0;
}
