/*  Name : Yogesh Porwal
    Roll No : 20CS60R52

    
    To complie : g++ 20CS60R52_6.cpp
                
    To Run : ./a.out <input1.txt> <input2.txt> <input3.txt> <input.txt>
*/

#include <iostream>
#include<stdio.h>
#include<fstream>
#include<sstream>
#include<string.h>
#include <unistd.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define P(s) semop(s,&pop,1) 
#define V(s) semop(s,&vop,1) 

using namespace std;

struct sembuf pop,vop;

struct reservation{
int pnr;
string passenger_name;
int age;
char sex;
string class_type; // AC2, AC3, or SC 
bool status; // confirmed or not confirmed
};

struct train{
 int train_id;
 int AC2, AC3, SC; // No. of available berths in each class
 struct reservation *rlist; //pointer to reservation list
 struct reservation *wlist; //pointer to waiting list
 int r_index,w1_index,w2_index;
}* rail_data;

void get_read_lock(int train_id ,int r1,int r2,int r3,int w1,int w2,int w3,int wait[])
{
 int flag=1;
 switch(train_id)
 {
  case 0: while(flag)
          {
           if(semctl(w1,0,GETVAL)==1)   //when no writer is in
           {
            if(wait[0]==0)              //when no writer is waiting
            {
             P(r1);
             flag=0;
           }
           
           }
           
          }
          
          break;
          
  case 1:  while(flag)
          {
           if(semctl(w2,0,GETVAL)==1) //when no writer is in
           {
            if(wait[1]==0)           //when no writer is waiting
            {
             P(r2);
             flag=0;
           }
           
           }
          }
          break;
          
 case 2:  while(flag)
          {
           if(semctl(w3,0,GETVAL)==1)             //when no writer is in
           {
            if(wait[2]==0)                      //when no writer is waiting
            {
             P(r3);
             flag=0;
           }
           
           }
          }
          break;
 }
}

void get_write_lock(int train_id ,int r1,int r2,int r3,int w1,int w2,int w3,int wait[])
{
 int flag=1;
 switch(train_id)
 {
  case 0: while(flag)
          {
           if(semctl(r1,0,GETVAL)==4 && semctl(w1,0,GETVAL)==1)   //when no reader and no writer is in
          {
           P(w1);
            wait[0]=0;
           flag=0;
          }
          else 
          {
          wait[0]=1;
          }
       
              }
          break;
          
 case 1: while(flag)
          {
           if(semctl(r2,0,GETVAL)==4 && semctl(w2,0,GETVAL)==1)     //when no reader and no writer is in
          {
           P(w2);
           wait[1]=0;
           flag=0;
          }
          else
          {
           wait[1]=1;
          }
          }
          break;
          
case 2: while(flag)
          {
           if(semctl(r3,0,GETVAL)==4 && semctl(w3,0,GETVAL)==1)       //when no reader and no writer is in
          {
           P(w3);
           wait[2]=0;
           flag=0;
          }
          else 
          {
           wait[2]=1;
          }
          }
          break;
  
  
 }
}

void release_read_lock(int train_id ,int r1,int r2,int r3)
{
 switch(train_id)
 {
  case 0: V(r1);
          break;
          
 case 1: V(r2);
          break;
 
 case 2: V(r3);
          break;
 
 }

}

void release_write_lock(int train_id ,int w1,int w2,int w3)
{
 switch(train_id)
 {
  case 0: V(w1);
          break;
          
 case 1: V(w2);
          break;
 
 case 2: V(w3);
          break;
 
 }

}

bool check_availability(int train_id,string class_type)  //check availability of ticket given train id and class
{
 if(class_type=="AC2")
 {
  if(rail_data[train_id].AC2>0)
  return 1;
  else
  return 0;
 }
 else if(class_type=="AC3")
 {
  if(rail_data[train_id].AC3>0)
  return 1;
  else
  return 0;
 }
 else
 {
  if(rail_data[train_id].SC>0)
  return 1;
  else
  return 0;
 }
 
}

 void Make_reservation(int pid,string name,int age,char sex,int train_id,string class_type,int r1,int r2,int r3,int w1,int w2,int w3,int wait[])
 {
 
   get_write_lock(train_id ,r1, r2, r3, w1, w2, w3, wait);
   
   bool is_available=check_availability(train_id, class_type);   //checking availability
   if(is_available)
   {
    int r_index=rail_data[train_id].r_index;
    rail_data[train_id].rlist[r_index].pnr=10*r_index+train_id;
    rail_data[train_id].rlist[r_index].passenger_name=name;
    rail_data[train_id].rlist[r_index].age=age;
    rail_data[train_id].rlist[r_index].sex=sex;
    rail_data[train_id].rlist[r_index].class_type=class_type;
    rail_data[train_id].rlist[r_index].status=1;
    rail_data[train_id].r_index++;
    
    if(class_type=="AC2")
    rail_data[train_id].AC2--;
    else if(class_type=="AC3")
    rail_data[train_id].AC3--;
    else
    rail_data[train_id].SC--;
    
    cout<<"Reservation done for passenger "<<name<<" for class "<<class_type<<" by process "<<pid<<",pnr is: "<<rail_data[train_id].rlist[r_index].pnr<<endl;
    
   }
   else
   {
    int w2_index=rail_data[train_id].w2_index;
    rail_data[train_id].wlist[w2_index].pnr=-1;
    rail_data[train_id].wlist[w2_index].passenger_name=name;
    rail_data[train_id].wlist[w2_index].age=age;
    rail_data[train_id].wlist[w2_index].sex=sex;
    rail_data[train_id].wlist[w2_index].class_type=class_type;
    rail_data[train_id].wlist[w2_index].status=1;
    rail_data[train_id].w2_index++;
    
    cout<<"Added passenger "<<name<<" to waitlist by process "<<pid<<endl;
   }
   
  release_write_lock(train_id , w1, w2, w3); 
   
 }
 
 void Cancel_reservation(int pnr,int pid,int r1,int r2,int r3,int w1,int w2,int w3,int wait[])  //cancel reservation for given pnr
 {
 int train_id=pnr%10;
  int index=pnr/10;
  
  if(train_id>2 || index>=rail_data[train_id].r_index)
  {
   cout<<"pnr "<<pnr<<" provided to cancel ticket is invalid!!"<<endl;
   return;
  }
  
  get_read_lock(train_id , r1, r2, r3, w1, w2, w3, wait);
   
   if(rail_data[train_id].rlist[index].status==0)
  {
   cout<<"Ticket with pnr "<<pnr<<" is already cancelled"<<endl;
   release_read_lock(train_id , r1, r2, r3);
   return;
  }
  
  release_read_lock(train_id , r1, r2, r3);
  
  
  get_write_lock(train_id ,r1, r2, r3, w1, w2, w3, wait);
  
  rail_data[train_id].rlist[index].status=0;
  
  int flag=1;
   int i;
  
  
  for(i=rail_data[train_id].w1_index;i<rail_data[train_id].w2_index;i++)
  {
   if(rail_data[train_id].wlist[i].status==1 && rail_data[train_id].wlist[i].class_type==rail_data[train_id].rlist[index].class_type)
   {
    rail_data[train_id].wlist[i].status=0;
    flag=0;
    break;
   }
  }
  
  
  if(flag)
  {
   cout<<"Ticket with pnr "<<pnr<<" get cancelled,no passenger is waiting so cancelled seat is free"<<endl;
   
    if(rail_data[train_id].rlist[index].class_type=="AC2")
    rail_data[train_id].AC2++;
    else if(rail_data[train_id].rlist[index].class_type=="AC3")
    rail_data[train_id].AC3++;
    else
    rail_data[train_id].SC++;
  }
  else
  {
    int r_index=rail_data[train_id].r_index;
    rail_data[train_id].rlist[r_index].pnr=10*r_index+train_id;
    rail_data[train_id].rlist[r_index].passenger_name=rail_data[train_id].wlist[i].passenger_name;
    rail_data[train_id].rlist[r_index].age=rail_data[train_id].wlist[i].age;
    rail_data[train_id].rlist[r_index].sex=rail_data[train_id].wlist[i].sex;
    rail_data[train_id].rlist[r_index].class_type=rail_data[train_id].wlist[i].class_type;
    rail_data[train_id].rlist[r_index].status=1;
    rail_data[train_id].r_index++;
    rail_data[train_id].w1_index++;
    
    cout<<"Ticket with pnr "<<pnr<<" get cancelled,another passenger "<<rail_data[train_id].wlist[i].passenger_name<<"who was waiting is confirmed"<<endl;
  }
  
  release_write_lock(train_id , w1, w2, w3); 
 }











int main(int argc,char * argv[])
{


int r1,r2,r3,w1,w2,w3;
int* wait;

r1=semget(IPC_PRIVATE,1,0777|IPC_CREAT);  //getting semaphores
r2=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
r3=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
w1=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
w2=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
w3=semget(IPC_PRIVATE,1,0777|IPC_CREAT);


semctl(r1,0,SETVAL,4);        //setting semaphore values
semctl(r2,0,SETVAL,4);
semctl(r3,0,SETVAL,4);
semctl(w1,0,SETVAL,1);
semctl(w2,0,SETVAL,1);
semctl(w3,0,SETVAL,1);

//cout<<"semaphores "<<r1<<" "<<r2<<" "<<r3<<" "<<w1<<" "<<w2<<" "<<w3<<endl;
//cout<<"semaphores "<<semctl(r1,0,GETVAL)<<" "<<semctl(r2,0,GETVAL)<<" "<<semctl(r3,0,GETVAL)<<" "<<semctl(w1,0,GETVAL)<<" "<<semctl(w2,0,GETVAL)<<" "<<endl;

pop.sem_num=vop.sem_num=0;
pop.sem_flg=vop.sem_flg=SEM_UNDO;
pop.sem_op=-1;
vop.sem_op=1;

 
 int shmid_1,s1;
 shmid_1 = shmget( IPC_PRIVATE,3*sizeof(struct train), 0777|IPC_CREAT); //getting shared memory
 s1 = shmget( IPC_PRIVATE,3*sizeof(int), 0777|IPC_CREAT);               //attatching shared memory
 
 rail_data=(struct train *) shmat( shmid_1, 0, 0 );
 wait=(int *) shmat( s1, 0, 0 );
 
 wait[0]=wait[1]=wait[2]=0;
 
 
 int shmid[3],wid[3];
 
 
 for(int i=0;i<3;i++)          //initializing rail data
 {
  rail_data[i].train_id=i;
  rail_data[i].AC2=rail_data[i].AC3=rail_data[i].SC=10;
  rail_data[i].r_index=rail_data[i].w1_index=rail_data[i].w2_index=0;
  shmid[i]= shmget(IPC_PRIVATE,1000*sizeof(struct reservation),0777|IPC_CREAT) ;  //getting shared memory
  wid[i]= shmget(IPC_PRIVATE,1000*sizeof(struct reservation),0777|IPC_CREAT) ;    //getting shared memory
  rail_data[i].rlist=(struct reservation *) shmat( shmid[i], 0, 0 );              //attatching shared memory
  rail_data[i].wlist=(struct reservation *) shmat( wid[i], 0, 0 );                //attatching shared memory
  
  //cout<<rail_data[i].train_id<<endl;
  //cout<<rail_data[i].AC2<<endl;
 // cout<<shmid[i]<<endl; 
  }
 
 

 
//cout<<" I am parent"<<endl;

int p1=fork();
sleep(1);
if(p1==0)
{
 //cout<<"I am first child"<<endl;
 rail_data=(struct train *) shmat( shmid_1, 0, 0 );      //attatching shared memory
 wait=(int *) shmat( s1, 0, 0 );
 rail_data[0].rlist=(struct reservation *) shmat( shmid[0], 0, 0 );
 rail_data[1].rlist=(struct reservation *) shmat( shmid[1], 0, 0 );
 rail_data[2].rlist=(struct reservation *) shmat( shmid[2], 0, 0 );
 rail_data[0].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[1].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[2].wlist=(struct reservation *) shmat( wid[2], 0, 0 );
 
 
 
 ifstream f1;                    //creation of ifstream class object to read the file
 
 if(argv[1]==NULL)
 {
  cout<<"ERROR! ,input file for process-0 not found!!"<<endl;
  return 0;
 }
 else
 f1.open(argv[1]);              //opening the passed file
 
 //cout<<"semaphores "<<semctl(r1,0,GETVAL)<<" "<<semctl(r2,0,GETVAL)<<" "<<semctl(r3,0,GETVAL)<<" "<<semctl(w1,0,GETVAL)<<" "<<semctl(w2,0,GETVAL)<<" "<<endl;
 
 
 string line;
 while(getline(f1,line))
 {
  stringstream ss(line);
  string word;
  ss>>word;
  
  if(word=="reserve")
  {
    string s1,s2;
    ss>>s1; ss>>s2;
    string name=s1+" "+s2;
    ss>>word;
    int age=stoi(word);
    ss>>word;
    char sex=word[0];
    ss>>word;
    int train_id=stoi(word);
    ss>>word;
    string class_type=word;
    
   // cout<<name<<" "<<age<<" "<<sex<<" "<<train_id<<" "<<class_type<<endl;
    Make_reservation(0,name,age,sex, train_id, class_type, r1, r2, r3, w1, w2, w3, wait);
    sleep(1);
  }
  else
  {
   ss>>word;
   int pnr=stoi(word);
   Cancel_reservation(pnr,0, r1, r2, r3, w1, w2, w3, wait);
   sleep(1);
  }
 
 }
 
}
else
{


//waitpid(p1,NULL,0);

//return 0;
int p2=fork();
sleep(1);
if(p2==0)
{
 //cout<<"I am second child"<<endl;
  rail_data=(struct train *) shmat( shmid_1, 0, 0 );       //attatching shared memory
  wait=(int *) shmat( s1, 0, 0 );
 rail_data[0].rlist=(struct reservation *) shmat( shmid[0], 0, 0 );
 rail_data[1].rlist=(struct reservation *) shmat( shmid[1], 0, 0 );
 rail_data[2].rlist=(struct reservation *) shmat( shmid[2], 0, 0 );
 rail_data[0].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[1].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[2].wlist=(struct reservation *) shmat( wid[2], 0, 0 );
 
 
 
 
 ifstream f2;                    //creation of ifstream class object to read the file
 
 if(argv[2]==NULL)
 {
  cout<<"ERROR! ,input file for process-1 not found!!"<<endl;
  return 0;
 }
 else
 f2.open(argv[2]);              //opening the passed file
 
 string line;
 while(getline(f2,line))
 {
  stringstream ss(line);
  string word;
  ss>>word;
  
  if(word=="reserve")
  {
    string s1,s2;
    ss>>s1; ss>>s2;
    string name=s1+" "+s2;
    ss>>word;
    int age=stoi(word);
    ss>>word;
    char sex=word[0];
    ss>>word;
    int train_id=stoi(word);
    ss>>word;
    string class_type=word;
    
    Make_reservation(1,name,age,sex, train_id, class_type, r1, r2, r3, w1, w2, w3, wait);
    sleep(1);
  }
  else
  {
   ss>>word;
   int pnr=stoi(word);
   Cancel_reservation(pnr,1, r1, r2, r3, w1, w2, w3, wait);
   sleep(1);
  }
 
 }
}
else
{
//waitpid(p2,NULL,0);
int p3=fork();
sleep(1);
if(p3==0)
{
 //cout<<"I am third child"<<endl;
 rail_data=(struct train *) shmat( shmid_1, 0, 0 );    //attatching shared memory
 wait=(int *) shmat( s1, 0, 0 );
  rail_data[0].rlist=(struct reservation *) shmat( shmid[0], 0, 0 );
 rail_data[1].rlist=(struct reservation *) shmat( shmid[1], 0, 0 );
 rail_data[2].rlist=(struct reservation *) shmat( shmid[2], 0, 0 );
 rail_data[0].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[1].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[2].wlist=(struct reservation *) shmat( wid[2], 0, 0 );


 ifstream f3;                    //creation of ifstream class object to read the file
 
 if(argv[3]==NULL)
 {
  cout<<"ERROR! ,input file for process-2 not found!!"<<endl;
  return 0;
 }
 else
 f3.open(argv[3]);              //opening the passed file
 
 string line;
 while(getline(f3,line))
 {
  stringstream ss(line);
  string word;
  ss>>word;
  
  if(word=="reserve")
  {
    string s1,s2;
    ss>>s1; ss>>s2;
    string name=s1+" "+s2;
    ss>>word;
    int age=stoi(word);
    ss>>word;
    char sex=word[0];
    ss>>word;
    int train_id=stoi(word);
    ss>>word;
    string class_type=word;
    
    Make_reservation(2,name,age,sex, train_id, class_type, r1, r2, r3, w1, w2, w3, wait);
    sleep(1);
  }
  else
  {
   ss>>word;
   int pnr=stoi(word);
   Cancel_reservation(pnr,2, r1, r2, r3, w1, w2, w3, wait);
   sleep(1);
  }
 
 }
}
else
{
//waitpid(p3,NULL,0);
int p4=fork();
sleep(1);
if(p4==0)
{
 //cout<<"I am fourth child"<<endl;
 rail_data=(struct train *) shmat( shmid_1, 0, 0 );   //attatching shared memory
 wait=(int *) shmat( s1, 0, 0 );
  rail_data[0].rlist=(struct reservation *) shmat( shmid[0], 0, 0 );
 rail_data[1].rlist=(struct reservation *) shmat( shmid[1], 0, 0 );
 rail_data[2].rlist=(struct reservation *) shmat( shmid[2], 0, 0 );
 rail_data[0].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[1].wlist=(struct reservation *) shmat( wid[0], 0, 0 );
 rail_data[2].wlist=(struct reservation *) shmat( wid[2], 0, 0 );
 
 ifstream f4;                    //creation of ifstream class object to read the file
 
 if(argv[4]==NULL)
 {
  cout<<"ERROR! ,input file for process-0 not found!!"<<endl;
  return 0;
 }
 else
 f4.open(argv[4]);              //opening the passed file
 
 string line;
 while(getline(f4,line))
 {
  stringstream ss(line);
  string word;
  ss>>word;
  
  if(word=="reserve")
  {
    string s1,s2;
    ss>>s1; ss>>s2;
    string name=s1+" "+s2;
    ss>>word;
    int age=stoi(word);
    ss>>word;
    char sex=word[0];
    ss>>word;
    int train_id=stoi(word);
    ss>>word;
    string class_type=word;
    
    Make_reservation(3,name,age,sex, train_id, class_type, r1, r2, r3, w1, w2, w3, wait);
    sleep(1);
  }
  else
  {
   ss>>word;
   int pnr=stoi(word);
   Cancel_reservation(pnr,3, r1, r2, r3, w1, w2, w3, wait);
   sleep(1);
  }
 
 }
}
else
{
waitpid(p4,NULL,0);

semctl(r1,0,IPC_RMID);  //returning acquired semaphores
semctl(r2,0,IPC_RMID);
semctl(r3,0,IPC_RMID);
semctl(w1,0,IPC_RMID);
semctl(w2,0,IPC_RMID);
semctl(w3,0,IPC_RMID);
cout<<"I am parent,I have returned semaphores,I am going to terminate"<<endl;
}
}
}

}


return 0;
}
