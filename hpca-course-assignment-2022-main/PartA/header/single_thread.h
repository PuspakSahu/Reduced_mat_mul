typedef int vec __attribute__(( vector_size(32) ));

//dynamically change the vector size
#define vecsize 8
vec* memalloc(int n) {
    int size=32*n;
    vec* p = (vec*) aligned_alloc(32, size);
    //set to 0
    memset(p, 0, size);
    return p;
}


void singleThread(int n,int *_a, int *_b,int *c) {
    int vin = n / vecsize; 
    int totalvec=n*vin;
    vec *a = memalloc(totalvec);
    vec *b = memalloc(totalvec);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            //I is the index of I th vector
            //J is the index of vector element
            int I= i * vin + j / vecsize;
            int J =j % vecsize;
            a[I][J] = _a[i * n + j];
            b[I][J] = _b[j * n + i]; 
        }
    }

    int start;
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
            //multiply internal vectors
            while(k<vin){
                s += a[i * vin + k] * b[j * vin + k];
                s += a[(i+1) * vin + k] * b[j * vin + k];
                s += a[i * vin + k] * b[(j+1) * vin + k];
                s += a[(i+1) * vin + k] * b[(j+1) * vin + k];
                k+=1;
            }

            //update the s and sum it vertically
            for (int k = vecsize-1 ; k >=0; k--)
                c[(i>>1) * (n>>1) + (j>>1)] += s[k];
        }
    }

    //freeing the rest 
    std::free(a);
    std::free(b);
}

