#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#define n 10
int c[n][n];
int a[n][n];
int b[n][n];
pthread_t tid[n];
void printc(){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			printf("%d ",c[i][j]);
		}
		printf("\n");
	}
}
void *gustovson(void *v){
	
	int* m=(int *)v;
	int i=*m;
	if(i<n-1){
		i++;
		pthread_create(&tid[i+1],NULL,gustovson,(void*)&i);
		pthread_join(tid[i+1],NULL);
		i--;
	}
	for(int k =0;k<n;k++){
		for(int j=0;j<n;j++){
			int value = a[i][k]*b[k][j];
			if(c[i][j]==-1){
				c[i][j]=value;
			}else{
				c[i][j]+=value;
			}
		}
	}
	if(i==0){
		printc();
	}
}

int main(){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			c[i][j]=-1;
		}
	}
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			a[i][j]=i+1;
			b[i][j]=i+1;
		}
	}
	int i=0;
	pthread_create(&tid[0],NULL,gustovson,(void*)&i);
	pthread_join(tid[0],NULL);
}