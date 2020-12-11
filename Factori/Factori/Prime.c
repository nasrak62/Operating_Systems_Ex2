#include <stdio.h> 
#include <stdlib.h>
#include <math.h> 

int how_many_prime_factors(int n)
{
    int j = 0;
    while (n % 2 == 0)
    {
        n = n / 2;
        j++;
    }
    for (int i = 3; i <= sqrt(n); i = i + 2)
    {
        while (n % i == 0)
        {
            n = n / i;
            j++;
        }
    }
    if (n > 2)
    {
        j++;
    }
    return j;
}
int* primeFactors(int n)
{
    int max_size = how_many_prime_factors(n);
    int* factors = (int*)malloc((max_size * sizeof(int)));
    int j = 0;
	while (n % 2 == 0)
	{
        factors[j] = 2;
		n = n / 2;
        j++;
	}
	for (int i = 3; i <= sqrt(n); i = i + 2)
	{
		while (n % i == 0)
		{
            factors[j] = i;
			n = n / i;
            j++;
		}
	}
    if (n > 2)
    {
        factors[j] = n;
        j++;
    }
    return factors;
}
