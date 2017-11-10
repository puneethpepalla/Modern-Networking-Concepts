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

static struct pkt previous;
static int state=0,seq=0,exp=0,expb=0;
static vector<struct pkt> buffer;
void A_output(struct msg message)
{
	//cout<<"\ncomping from up";
	struct pkt p1;
	
	memset(p1.payload,'\0',sizeof (p1.payload));
	p1.seqnum=seq%2;
	seq++;
	strcpy(p1.payload,message.data);
	int check=0;
	 p1.acknum=10;
	for(int i=0;i<20;i++){
		 check=check+p1.payload[i];
	 }
	
	check=check+p1.seqnum;
	check=check+p1.acknum;
	p1.checksum=check;
	//cout<<"\nbuff size:"<<buffer.size()<<"\tstate:"<<state;
	if(buffer.size()>0 || state==1){
	//if(state==1){
		//cout<<"\nentered";
		buffer.push_back(p1);
		cout<<"\npushing to buff"<<buffer.size();
	}
	else{ 
	//cout<<"\nsending checksum:"<<p1.checksum;
	tolayer3(0, p1);
	state=1;
	starttimer(0,15);
	exp=p1.seqnum;
	previous=p1;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	if(state==1){
	if(packet.checksum == packet.acknum+1){
		
	if(packet.acknum==exp){
		//cout<<"ack not curr and exp\n";
		state=0;
		stoptimer(0);
	}
	}
	if(buffer.size()>0 && state==0){
	//cout<<"buff pack going\n";
	struct pkt p1=buffer.front();
	tolayer3(0, p1);
	state=1;
	previous=p1;
	exp=p1.seqnum;
	buffer.erase(buffer.begin());
	
	starttimer(0,15);
	}
}
}
/* called when A's timer goes off */
void A_timerinterrupt()
{
	tolayer3(0,previous);
	//cout<<"\ntime int";
	//cout<<"\nTimer->msg:"<<previous.payload<<" seq:"<<previous.seqnum<<" ack:"<<previous.acknum<<" check:"<<previous.checksum;
	//cout<<"\ntimer checksum:"<<previous.checksum;
	starttimer(0,15);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	struct pkt p1;
	p1.acknum=(expb+1)%2;
	int check2=0;
     for(int i=0;i<20;i++){
	   check2=check2+packet.payload[i];
	 }
	check2=check2+packet.seqnum;
	check2=check2+packet.acknum;
	//cout<<"\nrec->msg:"<<packet.payload<<" seq:"<<packet.seqnum<<" ack:"<<packet.acknum<<" check:"<<packet.checksum;
	//cout<<"\npack checksum: "<<packet.checksum<<"\tcalc check:"<<check2;
	if(check2 == packet.checksum){
	if(expb == packet.seqnum){
		//cout<<"\ngoing up to layer 5\n";
		p1.acknum=packet.seqnum;
		char datas[20];
		memset(datas,'\0',sizeof (datas));
		strcpy(datas,packet.payload);
		tolayer5(1,datas);
		expb++;
		expb=expb%2;
	}
	}
	p1.checksum=p1.acknum+1;
    tolayer3(1,p1);
}
/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
