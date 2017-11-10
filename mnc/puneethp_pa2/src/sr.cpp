#include "../include/simulator.h"
#include<iostream>
#include<vector>
#include<cstring>
#include<list>
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
vector<struct pkt> rbuffer;
int basse=0,winsize=0,rwin=0;
float timm=0;
struct node{
	float timer;
	int seq;
	node(int a=0,float b=0){seq=a;timer=b;}
	// bool operator <(const node& lhs, const node& rhs)
// {
  // return lhs.timer < rhs.timer;
// }
bool operator==(const node& hai){
	return seq==hai.seq;
}
};
list<node> pq;
void A_output(struct msg message)
{
	//cout<<"\nenter the dragon";
	struct pkt p1;
	memset(p1.payload,'\0',sizeof(p1.payload)+1);
	strncpy(p1.payload,message.data,sizeof(message.data));
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
	//cout<<"\ncheck calculated winsize:"<<winsize<<"window:"<<window;
	if(window>=winsize){
		cout<<"\nbuffer send:"<<buffer.size();
	    buffer.push_back(p1);
	}
	else{
	//cout<<"\nsending message";
	tolayer3(0, p1);
	cout<<"\nsending msg: "<<seq-1<<" msg: "<<p1.payload;
	if(window==0) starttimer(0,timm);
	node n(p1.seqnum,timm+get_sim_time());
	pq.push_back(n);
	previous.push_back(p1);
	window++;
	//cout<<"\na output end";
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	
	if(packet.checksum==(packet.acknum+1)){
	//if(pq.size()>0){
	cout<<"\n ack received for: "<<packet.acknum;
	node n=pq.front();
	
	//cout<<"\nrec ack:"<<packet.ack<<"first pack in timer:"<<n.seq;
	if(packet.acknum==n.seq){
	
	pq.pop_front();stoptimer(0);
	if(pq.size()>0)starttimer(0,pq.front().timer-get_sim_time());
	}
	else {
		cout<<"\nbefore rem:";
		for (list<node>::iterator it = pq.begin(); it != pq.end(); it++)
			std::cout << it->seq << ' ';
		cout<<"\nremoving: "<<packet.acknum;
		pq.remove(packet.acknum);
		cout<<"\nafter rem: ";
		for (list<node>::iterator it = pq.begin(); it != pq.end(); it++)
			std::cout << it->seq << ' ';
	}


	for(int i=0;i<previous.size();i++){
		
		
		if(packet.acknum==previous[i].seqnum) {
			previous.erase(previous.begin()+i);
			window--;
			//stoptimer(0);
			//cout<<"send:input\n";
			//starttimer(0,50);
		}

	}
	
		while(window<winsize && buffer.size()>0){
			struct pkt p1=buffer.front();
			tolayer3(0,p1);
			cout<<"\nsent from send buff :"<<p1.seqnum;
			buffer.erase(buffer.begin());
			//buffcount--;
			previous.push_back(p1);
			node n(p1.seqnum,get_sim_time()+timm);
			pq.push_back(n);
			window++;
		}
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	// for(int i=0;i<previous.size();i++){
		// tolayer3(0,previous[i]);
	// }
	//stoptimer(0);
	
	if(pq.size()>0){
	cout<<"\n timer interr for: "<<pq.front().seq;
	node n=pq.front();
	for(int i=0;i<previous.size();i++){
		
		
		if(n.seq==previous[i].seqnum) {
			//previous.erase(previous.begin()+i);
			//window--;
			tolayer3(0,previous[i]);
			cout<<"\ntimer sending: "<<previous[i].seqnum;
			//stoptimer(0);
			//cout<<"send:input\n";
			//starttimer(0,50);
		}

	}
	
	pq.pop_front();
	n.timer=get_sim_time()+timm;
	pq.push_back(n);
	if(pq.size()>0) starttimer(0,pq.front().timer-get_sim_time());
	
	//pq.push_back(node(n.seq,get_sim_time()+15));
	//cout<<"timer\n";
	//starttimer(0,50);
}  
}
/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	winsize=getwinsize();
	timm=30;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{

int check=0;
cout<<"\nrec packet: "<<packet.seqnum<<"base: "<<basse;
for(int i=0;i<20;i++){
	check+=packet.payload[i];
}
check+=packet.seqnum;
check+=packet.acknum;
if(check==packet.checksum){
if(packet.seqnum==basse){

	//p1.acknum=packet.seqnum;
	//ackl=p1.acknum;
	basse++;
	char data1[20];
	strcpy(data1,packet.payload);
	tolayer5(1,data1);
	int flag=1; 
	cout<<"\nrec at lay5 :"<<packet.seqnum;
cout<<"\nrbuffer before:";
for(int i=0;i<rbuffer.size();i++) cout<<rbuffer[i].seqnum<<" ";
while(flag==1){
flag=0;
//cout<<"\nflag part";
for(int i=0;i<rbuffer.size();i++){
		cout<<"\nloop buff size:"<<rbuffer.size();
		cout<<"\ncomparing: "<<rbuffer[i].seqnum<<"->"<<basse;
		if((basse)==rbuffer[i].seqnum) {
			cout<<"\nnext  elem found: "<<rbuffer[i].seqnum;
			//pkt pp=rbuffer[i];
			char mesg[20];
			memset(mesg,'\0',sizeof(mesg));
			strcpy(mesg,rbuffer[i].payload);
			cout<<"\nseqnum: "<<rbuffer[i].seqnum<<"msg: "<<mesg;
			tolayer5(1,mesg);
			
			cout<<"\nsent from rec buff to l5 :"<<rbuffer[i].seqnum;
			rbuffer.erase(rbuffer.begin()+i);
			//rwin--;
			flag=1;
			break;
		}

	}
	if(flag==1) basse++;
}
cout<<"\nrbuffer after:";
for(int i=0;i<rbuffer.size();i++) cout<<rbuffer[i].seqnum<<" ";

//cout<<"\nexitted loop";
}
int fla=0;
for(int i=0;i<rbuffer.size();i++){
	if(rbuffer[i].seqnum==packet.seqnum)
		fla=1;
}
if(rbuffer.size()<winsize && packet.seqnum>basse && fla==0){
	rbuffer.push_back(packet);
	cout<<"\n rec buffered packet:"<<packet.seqnum;
	//rwin++;
}

//else if(packet.seqnum>basse-winsize){
struct pkt p1;
p1.acknum=packet.seqnum;
p1.checksum=p1.acknum+1;
cout<<"\nack sent for:"<<p1.acknum;
tolayer3(1,p1);
//}
}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
