#include "lib.h" 
int n;
int A[10][10], B[10][10], C[10][10]; 
void *test(void *arg) { 
    int k = ((int *)arg)[0]; 
    int i, j; 
    while (k < n) { 
        for (i = 0; i < n; i++) { 
            for (j = 0; j < n; j++) { 
                C[i][j] += A[i][k] * B[k][j]; 
            } 
        }
        k += 4; 
    } 
}

void umain() { 
    n = 5; int i, j; 
    for (i = 0; i < n; i++) { 
        for (j = 0; j < n; j++) { 
            A[i][j] = i; B[i][j] = j; 
        } 
    }
    int args[100]; 
    args[0] = 0; 
    args[1] = 1; 
    args[2] = 2; 
    args[3] = 3; 
    pthread_t t0, t1, t2, t3; 
    pthread_create(&t0,NULL,test,(void *) args); 
    pthread_create(&t1,NULL,test,(void *) (args + 1)); 
    pthread_create(&t2,NULL,test,(void *) (args + 2)); 
    pthread_create(&t3,NULL,test,(void *) (args + 3)); 
    int *r; pthread_join(t0, &r); 
    pthread_join(t1, &r); 
    pthread_join(t2, &r); 
    pthread_join(t3, &r); 
    for (i = 0; i < n; i++) { 
        for (j = 0; j < n; j++) { 
            writef("%d ", C[i][j]); 
        }writef("\n"); 
    }
    for(;;); 
}
