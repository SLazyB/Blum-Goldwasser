//
//  blum_gold.cpp
//  
//
//  Created by Cavell on 11/4/18.
//
//

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h>
#include <unistd.h>

using namespace std;

long int pub_key_gen(int p, int q){
    long int key = p * q;
    return key;
}

int modexp(long int base, long int exponent, long int mod)
{
    long long int x = 1;
    long long int y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        x = (x * y) % mod;
        y = (y * y) % mod;
        exponent = exponent / 2;
    }
    return x % mod;
}

//Bob encrypting a message for Alice
pair<string, long long int> encrypt(string m, long int key, long int seed){
    
    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(m.length()/h);
    
    //Stores the x values
    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";
    for(int i = 0; i < num_text; i++){
        temp = m.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        
        out += tmp_bit.to_string();
    }
    
    prev = pow(prev,2);
    prev = prev % key;
    
    return pair<string, long long int>(out,prev);
}

//Alice decrypting a message from Bob
string decrypt(string c, long int key, int p, int q, int a, int b, long int l_x){
    
    //Calculate text size for each chunk of message
    int k = floor(log2(key));
    int h = floor(log2(k));
    
    //Number of chunks of text
    int num_text = ceil(c.length()/h);

    //rp, rq calculated from p and q
    int rp = modexp(((p+1)/4.0), num_text+1, p-1);
    int rq = modexp(((q+1)/4.0), num_text+1, q-1);
    
    //Calculate initial seed
    int u = modexp(l_x,rp,p);
    int v = modexp(l_x,rq,q);
    long int seed = (v*a*p + u*b*q) % key;
    long long int prev = seed;
    
    //Output string
    string out = "";
    
    string temp = "";
    for(int i = 0; i < num_text; i++){
        temp = c.substr(i*h, h);
        
        //Calculates x
        prev = pow(prev,2);
        prev = prev % key;
        
        //Obtains the least significant 4 bits
        bitset<4> tmp_bit(prev);
        
        //XOR
        tmp_bit ^= bitset<4>(temp);
        out += tmp_bit.to_string();
    }
    
    return out;
}
int main(int argc, char *argv[]){
    //Alice only values
    int p = 499;
    int q = 547;
    long int n = pub_key_gen(p,q);

    //Universally known
    int a = -57;
    int b = 52;
    
    //Bob only value
    long int seed = 159201;
    
    //Plain text
    string m = "10011100000100001100";
    
    pair<string, long long int> k = encrypt(m, n, seed);
    cout << "Cipher Text: " << k.first << endl;
    string plain = decrypt(k.first, n, p, q, a, b, k.second);
    cout << "Plain Text: " << plain << endl;
    
}