#include <pthread.h>

void* rmm_mult(void* arg){
    int *data = (int *)arg;
    int k=0,i=0;
    int N = data[0];
    int *result_t = (int*)malloc(((N*N)/4)*sizeof(int));
   
    int nB = (N + 7) / 8; // number of 8-element vectors in a row (rounded up)

    vec *a = alloc(N * nB);
    vec *b = alloc(N * nB);

    // move both matrices to the aligned region
    for (int i = 0; i < N/2; i+=nB) {
        for (int j = 0; j < N/2; j+=nB) {
            int sum=0;
            vec s; // initialize the accumulator with zeros
            for (int ll=0;ll<8;++ll){
                s[ll]=0;
            }
            for(int k=0;k<N;k+=8){
                vec *a = alloc(N * nB);
                vec *b = alloc(N * nB);
                for(int l=0;l<nB;l++){
                    for(int m=0;m<8;m++){
                        a[l][m] = data[(i *N) +(l*nB)+m+1];
                        b[l][m] = data[(N*N)/2 + (l*nB)+m+1+(j*N)];
                    }
                }

                
/*                 cout<<"VecA :::"<<endl;
                for(int i=0;i<nB;i++){
                    for(int j=0;j<8;j++){
                        cout<<a[i][j]<<" ";
                    }
                    cout<<endl;
                }
                cout<<"VecB :::"<<endl;
                for(int i=0;i<nB;i++){
                    for(int j=0;j<8;j++){
                        cout<<b[i][j]<<" ";
                    }
                    cout<<endl;
                }
                 */
                s =a[k/8]*b[k/8];
                for(int p=0;p<8;p++){
                    sum+=s[p];
                }
                std::free(a);
                std::free(b);
            }
            
            result_t[i*(N/2)+j] = sum;
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

