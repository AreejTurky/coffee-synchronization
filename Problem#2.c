#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> // C program to demonstrate working of Semaphores 
#include <unistd.h> 
#include <threads.h> 
#define max 5		//max variable starting with 5

sem_t capsule;	//counting semaphore able to decreasing till 0
sem_t cups;	//counting semaphore able to increasing till 5
sem_t scheduler; //counting semaphore to schedule between the machine
sem_t ready;	//counting semaphore able to increasing till 5

int capsules=0;	//normal varibale, only used for printing

//Sara's runner
void prepare_capsule(){

   sem_wait(&capsule);	//I'm waiting till there is an available capsule need to grind
   sem_wait(&scheduler);// I'm waiting if the machine is busy

   printf("turn to Sara with ready capsule: %d\n",++capsules);	//Sara's CS
   sem_post(&scheduler);	//release the machine, in case Mohamad wants to use 
   sem_post(&cups);		//increase num. of ready capsules
 
}
//Mohammad runner
void make_coffee(){

   sem_wait(&cups);	//I'm waiting for sara to prepare the coffee
   sem_wait(&scheduler);	// I'm waiting if the machine is busy
   printf("turn to Mohammad with capsule: %d\n",capsules);	//Mohamad's CS
   sem_post(&scheduler);	//release the machine, in case Sara wants to use 
   sem_post(&capsule);		//release num. of empty capsules, In case Sara want's to prepare more
   sem_post(&ready);		//increase num. of ready cups to serve
   
}

//Abdullah's runner
void serve_coffee(){		

   sem_wait(&ready);		//I'm waiting for Mohamad to make a coffee
   printf("turn to Abdullah with ready cups: %d\n",capsules--);	//Abdullah's CS
//It's OK to Abdullah to work while Mohamad/Sara work, Since they work in different machine,,No need to schedule him
}


//////////////////////////////////////////////////////
int main(void){
   pthread_t workers[3]; 	//Sara, Mohamad and Abdullah
   
   sem_init(&capsule, 0,(int)max);	//initially, there's always place to sara to prepare till the max without ahead to Mohamad
   sem_init(&cups, 0, 0);		//cups depends in ready capsules, so it's 0
   sem_init(&scheduler, 0, 1);		//initially, Sara can go first in the machine 
   sem_init(&ready, 0, 0);		//ready depends in cups, so it's zero

  
   for(int i=0 ; i<max; i++){
   pthread_create(&workers[0], NULL, prepare_capsule, NULL);
   pthread_create(&workers[1], NULL, make_coffee, NULL);
   pthread_create(&workers[2], NULL, serve_coffee, NULL);		
   pthread_join(workers[1], NULL);
   pthread_join(workers[0], NULL);		//let's make a difference in order of join to make sure there's synchronization+ see the sample output
   pthread_join(workers[2], NULL);
	}	
   
   sem_destroy(&capsule);		//No overhead to the system, delete it
   sem_destroy(&cups);
   sem_destroy(&scheduler);
   sem_destroy(&ready);
   exit(0);
   
return 0;
}
