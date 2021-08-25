# Railway-Reservation-System

## Project Description

Access to the reservation database of Indian Railways is a typical example of a critical section. There may be several concurrent processes that wish to access the database for checking the reservation status and for booking reservations. We may broadly classify these processes into two types, namely:

* Readers: These processes simply read the status of the reservations
* Writers: These processes change the reservation status by making a reservation

For each train, we may have a read lock and a write lock. A reader must acquire a read lock in order to study the
reservation status, while a writer must acquire a write lock to write the database. When a process does not get its
desired lock, it must wait for it. We are given the following constraints on the acquisition of locks:

1. Write locks are exclusive -- a process can obtain a write lock only when no process has a read or write
lock.
2. Read locks are shared -- one or more processes can obtain read locks when no process has a write lock.
3. Writers have higher priority -- new readers are not granted as long as there are waiting writers.
4. Locks are non-preemptive

We will assume that each process holds at most one lock at a time. Reservation database will have following structure:

```
struct train
{
int train-id;
int AC2, AC3, SC; // No. of available berths
struct reservation *rlist;
}

struct reservation
{
int pnr;
char passenger-name[20], age[3], sex;
char class[4]; // AC2, AC3, or SC
flag status; // waitlisted or confirmed
}

struct train *rail-data;
```

Each train has 3 classes, namely 2AC, 3AC, and SC (sleeper class). rlist is a dynamically allocated array of
reservations. rail-data is a dynamically allocated array of train records. We assume that all this is for a given date
only. The possible transactions are as follows:

* **Check availability**: given the train-id and class
* **Make a reservation**: if not available, then it goes into waiting list. The position on the waiting list is given
by the position in rlist.
* **Cancel a reservation**

### Task:
Write the following functions:

 - **get-read-lock( train-id )**  // The calling process blocks until it gets a read lock on the given train
 - **get-write-lock( train-id )** // The calling process blocks until it gets a read lock on the given train
 - **release-read-lock( train-id)**
 - **release-write-lock( train-id )**

Create a system of 4 child processes, P1, P2, P3, P4, which respectively reads input files in1, in2, in3, in4. Each
input file contains a list of instructions of the form:
```
reserve <name> <age> <sex> <train-id> <class>
cancel <pnr>
```
#### Flow:
The reservation database is assumed to be in shared memory. Each reservation request must first check the
availability using a read-lock and then obtain a write-lock for making the reservation, if available. If no reservation
is available, then the passenger will be waitlisted. If a passenger cancels a reservation, the first waitlisted
passenger will be confirmed. The processes sleep for 1 second after processing each reservation / cancellation
request. The processes print the outcome of their reservation / cancellation after processing each request.

Assume that there are 3 trains, and 10 berths of each class in each train.    //data can be changed

## Instructions to run the code

check README_20CS60R52.txt file
