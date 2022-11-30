typedef int vec __attribute__(( vector_size(32) ));

//dynamically change the vector size

#define vecsize 8
vec* memalloc(int n) {
    vec* ptr = (vec*) aligned_alloc(32, 32 * n);
    //set to 0
    memset(ptr, 0, 32 * n);
    return ptr;
}


void singleThread(int n,int *_a, int *_b,int *c) {
    int vin = n / vecsize; 
    int totalvec=n*vin;
    vec *a = memalloc(totalvec);
    vec *b = memalloc(totalvec);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i * vin + j / vecsize][j %vecsize] = _a[i * n + j];
            b[i * vin + j / vecsize][j % vecsize] = _b[j * n + i]; 
        }
    }


    for (int i = 0; i < n; i+=2) {
        for (int j = 0; j < n; j+=2) {
            vec s; 

            //initialise to zero
            //for no garbage value
            for (int ll=0;ll<vecsize;++ll)
            {
                s[ll]=0;
            }
            int k=0;
            //s is a vector
            while(k<vin){
                s += a[i * vin + k] * b[j * vin + k];
                s += a[(i+1) * vin + k] * b[j * vin + k];
                s += a[i * vin + k] * b[(j+1) * vin + k];
                s += a[(i+1) * vin + k] * b[(j+1) * vin + k];
                k+=1;
            }

            //update the s and sum it vertically
            for (int k = 0; k < vecsize; k++)
                c[(i>>1) * (n>>1) + (j>>1)] += s[k];
        }
    }

    //freeing the rest 
    std::free(a);
    std::free(b);
}

