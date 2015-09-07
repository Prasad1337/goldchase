#include <iostream>

using namespace std;

int main()
{
    string S="test-this";
    int K=4;
    long int N=S.length();
    long int rem=N%K;
    
    string S1[9];
    for(int i=0;i<N;i++)
    {
            S1[i]=(char)S[i];
    }
    
    cout<<S1<<endl;
}
