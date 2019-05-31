#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<errno.h>


int n,k,result;

typedef struct numerator{
	long numer;
	sem_t *semNum;
}numerator_t;

typedef struct denominator{
	long denom;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int done;
}denominator_t;

numerator_t numerator;
denominator_t denominator;

static void * numer_thread(void * arg){
	sleep(2);
	int i;
	int count = n - (n-k);
	if(count%2 == 0){
		for(i = n; i>= n-k+1; i-=2){
			sleep(1);
			numerator.numer = numerator.numer * (i*(i-1));
		}
	} else{
		for(i=n; i>=n-k+1; i-=2){
			sleep(1);
			if(i == n-k+1)
				numerator.numer = numerator.numer * (n-k+1);
			else
				numerator.numer = numerator.numer * (i*(i-1));
		}
	}
	pthread_mutex_lock(&denominator.lock);
	while(denominator.done == 0){
		pthread_cond_wait(&denominator.cond, &denominator.lock);	
	}
	result = numerator.numer / denominator.denom;	
	pthread_mutex_unlock(&denominator.lock);
	return NULL;	
}


static void * denom_thread(void * arg){
	int i;
	sleep(6);
	if(k%2 ==0){
		for(i=1; i<=k; i+=2){
			sleep(1);
			denominator.denom = denominator.denom * (i*(i+1));
		}	
	} else{
		for(i=1; i<=k; i+=2){
			sleep(1);
			if(i==k)
				denominator.denom = denominator.denom * k;
			else
				denominator.denom = denominator.denom * (i*(i+1));
		}	
	}
	denominator.done = 1;
	pthread_cond_signal(&denominator.cond);	
	return NULL;
}

void init(){
	numerator.numer = 1;
	numerator.semNum = (sem_t *)malloc(sizeof(sem_t));
	sem_init(numerator.semNum, 0, 0);
	denominator.denom = 1;
	pthread_mutex_init(&denominator.lock, NULL);
	pthread_cond_init(&denominator.cond, NULL);
	denominator.done = 0;
	numerator.done = 0;
}

int main(int argc, char *argv[]){
	if(argc != 3){
		printf("usage: %s n k\n", argv[0]);
		exit(1);
	}	
	n = atoi(argv[1]);
	k = atoi(argv[2]);
	
	init();
	pthread_t th_num, th_den;
	pthread_create(&th_num, NULL, numer_thread, (void *)NULL);
	pthread_create(&th_den, NULL, denom_thread, (void *)NULL);
	
	void *retval;
	pthread_join(th_num, &retval);
	pthread_join(th_den, &retval);
	

	
	printf("result = %d\n",result);
	return 0;	
}


