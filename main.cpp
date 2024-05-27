#include <bits/stdc++.h>
// #include <pthread.h>
#define RAPID cout.tie(0);cin.tie(0);ios::sync_with_stdio(0);
using namespace std;
#define ll long long

static int a =5;


void* hello(void * arg){
    a++;
    cout << "hello world ! from " << pthread_self() << " " << a << endl; 
    return NULL;
}

int32_t main(int argc, char*argv[]){
    pthread_t t1,t2;
    pthread_create(&t1,NULL,hello,NULL);
    pthread_create(&t2,NULL,hello,NULL);
    pthread_join(t1,NULL);


    pthread_join(t2,NULL);


    
}