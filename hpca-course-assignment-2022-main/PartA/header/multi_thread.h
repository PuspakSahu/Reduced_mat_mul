#include <pthread.h>

void* rmm_mult(void* arg){
    int *data = (int *)arg;
    int k=0,i=0;
    int N = data[0];
    int *result_t = (int*)malloc(((N*N)/4)*sizeof(int));
    for(int i=0;i<N/2;i++){
        for(int a=0;a<(N/2);a++){
            int k=0;
            for(int j=0;j<N;j++){
                k+=(data[(i*N)+j+1]*data[((N*N)/2)+j+1+(a*N)]);
            }
            result_t[i*(N/2)+a] = k;
        }
    }
    pthread_exit((void*) result_t);
}

void multiThread(int N, int *matA, int *matB, int *output){

    int max = 4;
    pthread_t *threads;
    threads = (pthread_t*)malloc(max*sizeof(pthread_t));
     
    int count = 0;
    int* data = NULL;
    for (int i = 0; i < max; i++){
            data = (int *)malloc(((N*N)+1)*sizeof(int));
            data[0] = N;
            int m=0;
            int a=0,b=0;
            if(i==0) {a=0;b=0;}
            else if(i==1) {a=1;b=0;}
            else if(i==2) {a=0;b=1;}
            else{a=1;b=1;}
            
            for(int j=a;j<N;j+=2){
                for (int k = 0; k < N; k++){
                    data[++m] = matA[j*N + k];
                }
            }
            
            for(int j=b;j<N;j+=2){
                for(int k=0;k<N;k++){
                    data[++m] = matB[k*N + j];
                }
            }
            int error_code = pthread_create(&threads[count++],NULL,rmm_mult, data);
            if(error_code){
                cout<<"Thread creation failure with error code ::: "<< error_code <<endl;
                exit(0);
            }
    }
    int* result[max];
    for (int i = 0; i < max; i++){
        int *result_t;
        int error_code = pthread_join(threads[i],(void**)&result_t);
        if(error_code){
            cout<<"Thread join failure with error code ::: "<<error_code<<endl;
            exit(0);
        }
        result[i] = (int *) malloc(((N*N)/4)*sizeof(int));
        for(int j=0;j<(N*N)/4;j++){
            result[i][j]=result_t[j];
        }
        free(result_t);
    }
    for(int i=0;i<max;i++){
        for(int j=0;j<((N*N)/4);j++){
            output[j]+=result[i][j];
        }
    }
}

