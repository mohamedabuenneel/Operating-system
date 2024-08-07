#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/environment_definitions.h>

#include "semaphore_manager.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <kern/mem/kheap.h>
#include <kern/mem/memory_manager.h>


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===============================
// [1] Create "semaphores" array:
//===============================
//Dynamically allocate the "semaphores" array
//initialize the "semaphores" array by 0's and empty = 1
void create_semaphores_array(uint32 numOfSemaphores)
{
#if USE_KHEAP
	MAX_SEMAPHORES = numOfSemaphores ;
	semaphores = (struct Semaphore*) kmalloc(numOfSemaphores*sizeof(struct Semaphore));
	if (semaphores == NULL)
	{
		panic("Kernel runs out of memory\nCan't create the array of semaphores.");
	}
#endif
	for (int i = 0; i < MAX_SEMAPHORES; ++i)
	{
		memset(&(semaphores[i]), 0, sizeof(struct Semaphore));
		semaphores[i].empty = 1;
		LIST_INIT(&(semaphores[i].env_queue));
	}

}


//========================
// [2] Allocate Semaphore:
//========================
//Allocates a new (empty) semaphore object from the "semaphores" array
//Return:
//	a) if succeed:
//		1. allocatedSemaphore (pointer to struct Semaphore) passed by reference
//		2. SempahoreObjectID (its index in the array) as a return parameter
//	b) E_NO_SEMAPHORE if the the array of semaphores is full (i.e. reaches "MAX_SEMAPHORES")
int allocate_semaphore_object(struct Semaphore **allocatedObject)
{
	int32 semaphoreObjectID = -1 ;
	for (int i = 0; i < MAX_SEMAPHORES; ++i)
	{
		if (semaphores[i].empty)
		{
			semaphoreObjectID = i;
			break;
		}
	}

	if (semaphoreObjectID == -1)
	{
		//try to double the size of the "semaphores" array
		#if USE_KHEAP
		{
			semaphores = (struct Semaphore*) krealloc(semaphores, 2*MAX_SEMAPHORES);
			if (semaphores == NULL)
			{
				*allocatedObject = NULL;
				return E_NO_SEMAPHORE;
			}
			else
			{
				semaphoreObjectID = MAX_SEMAPHORES;
				MAX_SEMAPHORES *= 2;
			}
		}
		#else
		{
			panic("Attempt to dynamically allocate space inside kernel while kheap is disabled .. ");
			*allocatedObject = NULL;
			return E_NO_SEMAPHORE;
		}
		#endif
	}

	*allocatedObject = &(semaphores[semaphoreObjectID]);
	semaphores[semaphoreObjectID].empty = 0;

	return semaphoreObjectID;
}

//======================
// [3] Get Semaphore ID:
//======================
//Search for the given semaphore object in the "semaphores" array
//Return:
//	a) if found: SemaphoreObjectID (index of the semaphore object in the array)
//	b) else: E_SEMAPHORE_NOT_EXISTS
int get_semaphore_object_ID(int32 ownerID, char* name)
{
	int i=0;
	for(; i < MAX_SEMAPHORES; ++i)
	{
		if (semaphores[i].empty)
			continue;

		if(semaphores[i].ownerID == ownerID && strcmp(name, semaphores[i].name)==0)
		{
			return i;
		}
	}
	return E_SEMAPHORE_NOT_EXISTS;
}

//====================
// [4] Free Semaphore:
//====================
//delete the semaphore with the given ID from the "semaphores" array
//Return:
//	a) 0 if succeed
//	b) E_SEMAPHORE_NOT_EXISTS if the semaphore is not exists
int free_semaphore_object(uint32 semaphoreObjectID)
{
	if (semaphoreObjectID >= MAX_SEMAPHORES)
		return E_SEMAPHORE_NOT_EXISTS;

	memset(&(semaphores[semaphoreObjectID]), 0, sizeof(struct Semaphore));
	semaphores[semaphoreObjectID].empty = 1;
	LIST_INIT(&(semaphores[semaphoreObjectID].env_queue));

	return 0;
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//======================
// [1] Create Semaphore:
//======================
int createSemaphore(int32 ownerEnvID, char* semaphoreName, uint32 initialValue)
{
	//TODO: [PROJECT MS3] [SEMAPHORES] createSemaphore
	// your code is here, remove the panic and write your code
	//panic("createSemaphore() is not implemented yet...!!");
	int sempahore_index =0;
		 // cretae integer to store the semaphore index
		 // use get function
		 sempahore_index =get_semaphore_object_ID(ownerEnvID, semaphoreName);
		 // after creation
		 // sempahore_index now = semaphore id if exist
	//i 'll check about the semaphore by "if found "
					if(sempahore_index != E_SEMAPHORE_NOT_EXISTS)
					  {
						//IF WE FIND THE SEMAPHORE WE 'LL RETURN IT'S INDEX IN THE ARRAY
					return sempahore_index ;

					//	b) E_SEMAPHORE_EXISTS if the semaphore is already exists

					  }
					// SECOUND CREATE NEW SEMAPHORE
					 struct Semaphore * NEW = NULL ;
					 // THEN ALLOCATE IT
					 // pass by refernce to allocate function
					 // create int
					int the_semaphore = 0;
					// nsaweh be function ell aloocate

					the_semaphore	=allocate_semaphore_object(&NEW);
					//nshof b2a lw ng7h an y7gz mkan wla la2
					 if( the_semaphore !=E_NO_SEMAPHORE)
						{
						 //kda lw hgz mkan
						  semaphores[the_semaphore].ownerID = ownerEnvID ;
						  //n5ly el id
						  semaphores[the_semaphore].value   = initialValue ;
						  // n5ly el name
						  strcpy(semaphores[the_semaphore].name, semaphoreName);
						  //n5ly value
						  return the_semaphore ;
							//	a) SemaphoreID (its index in the array) if succeed

						 }

				   return E_NO_SEMAPHORE;


					//	c) E_NO_SEMAPHORE if the the array of semaphores is full
		return 0;






	//create new semaphore object and initialize it by the given info (ownerID, name, value)
	//Return:
	//	a) SemaphoreID (its index in the array) if succeed
	//	b) E_SEMAPHORE_EXISTS if the semaphore is already exists
	//	c) E_NO_SEMAPHORE if the the array of semaphores is full

	//change this "return" according to your answer
	//return 0;
}

//============
// [2] Wait():
//============
void waitSemaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT MS3] [SEMAPHORES] waitSemaphore
	// your code is here, remove the panic and write your code
	//panic("waitSemaphore() is not implemented yet...!!");

	struct Env* myenv = curenv; //The calling environment

	//////////////////
	// calling env
		// myenv ,curenv
		//  struct Env* myenv = curenv;

		  int sempahore_index =0;
		 	 // cretae integer to store the semaphore index
		 	 // use get function
		 	 sempahore_index =get_semaphore_object_ID(ownerEnvID, semaphoreName);
		 	 // after creation
		 	 // sempahore_index now = semaphore id if exist
		 	//	1) Get the Semaphore

		 				if(sempahore_index != E_SEMAPHORE_NOT_EXISTS)

					{

		 					//	2) Decrement its value

				    semaphores[sempahore_index].value--;
				    //	3) If negative, block the calling environment "myenv"
				    if(semaphores[sempahore_index].value < 0)
						{

				    	enqueue(&semaphores[sempahore_index].env_queue, myenv);
				    	//a) adding it to semaphore queue
				    	myenv->env_status = ENV_BLOCKED;
				    	//		b) changing its status to ENV_BLOCKED

				    	curenv = NULL;
				    	//	c) set curenv with NULL


				    		}
					}
		 				//	4) Call "fos_scheduler()" to continue running the remaining envs

				fos_scheduler();
	/////////////


	// Steps:
	//	1) Get the Semaphore
	//	2) Decrement its value
	//	3) If negative, block the calling environment "myenv", by
	//		a) adding it to semaphore queue		[refer to helper functions in doc]
	//		b) changing its status to ENV_BLOCKED
	//		c) set curenv with NULL
	//	4) Call "fos_scheduler()" to continue running the remaining envs
}

//==============
// [3] Signal():
//==============
void signalSemaphore(int ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT MS3] [SEMAPHORES] signalSemaphore
	// your code is here, remove the panic and write your code
	//panic("signalSemaphore() is not implemented yet...!!");



	///////////////
	// calling env
			// myenv ,curenv
			  struct Env* myenv = curenv;

			  int sempahore_index =0;
			 	 // cretae integer to store the semaphore index
			 	 // use get function
			 	 sempahore_index =get_semaphore_object_ID(ownerEnvID, semaphoreName);
			 	 // after creation
			 	 // sempahore_index now = semaphore id if exist
			 	//	1) Get the Semaphore

			 				if(sempahore_index != E_SEMAPHORE_NOT_EXISTS)

						{
			 					//	2) Increment its value

					    semaphores[sempahore_index].value++;

						//	3) If less than or equal 0, release a blocked environment, by

					    if(semaphores[sempahore_index].value <= 0)
							{
					    	//		a) removing it from semaphore queue

					    	myenv = dequeue(&semaphores[sempahore_index].env_queue);
					    	//		b) adding it to ready queue

					    	sched_insert_ready(myenv);
					    	//		c) changing its status to ENV_READY
							myenv->env_status = ENV_READY;
							}
					}
	/////////////


	// Steps:
	//	1) Get the Semaphore
	//	2) Increment its value
	//	3) If less than or equal 0, release a blocked environment, by
	//		a) removing it from semaphore queue		[refer to helper functions in doc]
	//		b) adding it to ready queue				[refer to helper functions in doc]
	//		c) changing its status to ENV_READY
}

