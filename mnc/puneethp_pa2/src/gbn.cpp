#include "../include/simulator.h"
#include<iostream>
#include<vector>
#include<cstring>
using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
int window=0,seq=0,buffcount=0;
vector<struct pkt> buffer;
vector<struct pkt> previous;
int ackl=-1,winsize=0;

void A_output(struct msg message)
{
	struct pkt p1;
	
	strcpy(p1.payload,message.data);
	p1.seqnum=seq;
	p1.acknum=10;
	seq++;
    int check=0;
	for(int i=0;i<20;i++){
		check=check+message.data[i];
	}
	check+=p1.seqnum;
	check+=p1.acknum;
	p1.checksum=check;
	cout<<"\nsender:payload"<<p1.payload<<" seq:"<<p1.seqnum<<" ack:"<<p1.acknum<<" check:"<<p1.checksum;
	if(window>=winsize || buffer.size()>0){
		
		buffer.push_back(p1);
		buffcount++;
	}
	else{
	
	tolayer3(0, p1);
	cout<<"\nsending msg: "<<seq;

	previous.push_back(p1);
	window++;

	if(window==1){
		cout<<"send:output\n"<<seq<<endl;
		starttimer(0,50);
	}
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int check=0;
for(int i=0;i<20;i++){
	check+=packet.payload[i];
}
check+=packet.seqnum;
check+=packet.acknum;

	if(packet.checksum==(check)){
	for(int i=0;i<previous.size();i++){
		if(packet.acknum>=previous[i].seqnum) {
			previous.erase(previous.begin()+i);
			window--;
			stoptimer(0);
			cout<<"send:input\n";
			starttimer(0,50);
		}

	}
	}
		while(window<winsize && buffer.size()!=0){
			struct pkt p1=buffer.front();
			tolayer3(0,p1);
			buffer.erase(buffer.begin());
			//buffcount--;
			previous.push_back(p1);
			window++;
		}
	}


/* called when A's timer goes off */
void A_timerinterrupt()
{
	for(int i=0;i<previous.size();i++){
		tolayer3(0,previous[i]);
	}
	//stoptimer(0);
	cout<<"timer\n";
	starttimer(0,50);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	winsize=getwinsize();
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
struct pkt p1;
p1.acknum=ackl;
memset(p1.payload,'x',sizeof(p1.payload));


int check=0;
for(int i=0;i<20;i++){
	check+=packet.payload[i];
}
check+=packet.seqnum;
check+=packet.acknum;
if(check==packet.checksum){
	if(packet.seqnum==(ackl+1)){
	p1.acknum=packet.seqnum;
	ackl=p1.acknum;
	
	char data1[20];
	strcpy(data1,packet.payload);
	cout<<"\nsender:payload"<<packet.payload<<" seq:"<<packet.seqnum<<" ack:"<<packet.acknum<<" check:"<<packet.checksum;
	tolayer5(1,data1);
}
}
check=0;
for(int i=0;i<20;i++){
	check+=p1.payload[i];
}
check+=p1.seqnum;
check+=p1.acknum;
p1.checksum=check;
tolayer3(1,p1);
}


/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
