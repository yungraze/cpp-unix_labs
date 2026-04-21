#include <iostream>
#include <chrono>

double calculate (double x) {
    return x*x - x*x + x*4 - x*5 + x + x;
}



int main()

{
    long long n;
    double res, time;
    int x = 10;

    while(1)
    {
        if (!(std::cin >> n)) 
            break; 
        
        clock_t start = clock();

        for (long long i =0; i<n; ++i)
        {
            res = calculate(x);
        }

        clock_t end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;

        std::cout <<"time =  "<< time <<";  "<<"result(x=10) =  "<< res << "\n";
    }
}