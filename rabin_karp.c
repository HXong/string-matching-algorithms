#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Function to check if a number is prime
bool isPrime(int num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

// Function to find the next prime greater than or equal to a given number
int findNextPrime(int start) {
    while (!isPrime(start)) {
        start++;
    }
    return start;
}

// Function to dynamically find the best prime based on text length
int findEfficientPrime(int textLength) {
    // Start searching for primes near the text length
    return findNextPrime(textLength);
}

// Hash function
int hash(const char *txt, int m, int d, int q) {
    int h = 0;
    for (int i = 0; i < m; i++) {
        h = (h * d + txt[i]) % q;
    }
    return h;
}

// Rehash function
int rehash(const char *txt, int i, int m, int d, int q, int ht) {
    int oldest = (txt[i] * ((int)pow(d, m - 1)) % q);
    int oldest_removed = ((ht + q) - oldest) % q;
    return (oldest_removed * d + txt[i + m]) % q;
}

// Rabin-Karp Algorithm with Dynamic Prime Selection
void rabin_karp(const char *text, const char *pattern, int *matches, int *count) {
    int n = strlen(text);
    int m = strlen(pattern);
    int k = 0, match_count = 0;

    if (m > n) {
        printf("Pattern is longer than the text. No match.\n");
        return;
    }

    // Dynamically find an efficient prime
    int prime = findEfficientPrime(m);
    //printf("Using dynamically selected prime: %d\n", prime);

    int hp = hash(pattern, m, 256, prime);
    int ht = hash(text, m, 256, prime);

    //printf("Initial hash of pattern (hp): %d\n", hp);
    //printf("Initial hash of first text window (ht): %d\n", ht);

    for (int i = 0; i <= n - m; i++) {
        if (hp == ht) {
            while(k<m){
                if(text[i+k] == pattern[k]) k++;
                else{ 
                    k=0; 
                    break;
                }
            }
        } else if (i < n-m){
            ht = rehash(text, i, m, 256, prime, ht);
        }

        if(k==m){
            //printf("Pattern found at index %d to index %d\n", i, i+m-1);
            matches[match_count++] = i;
            k=0;
        }
    }

    *count = match_count;

    /*
    if (match_count > 0) {
        printf("Pattern found at indices: ");
        for (int i = 0; i < match_count; i++) {
            printf("%d ", matches[i]);
        }
        printf("\n");
    } else {
        printf("Pattern not found\n");
    }
    */

}
