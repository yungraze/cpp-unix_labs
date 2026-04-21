#include <iostream>

int power(int osn, int pok)
{
    int res = 1;
    for (int i =0; i<pok; ++i)
        res = res*osn;
    return res;
}


int plus(int slag1, int slag2)
{
    return slag1+slag2;
}


int minus(int slag1, int slag2)
{
    return slag1 - slag2;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cout << "opd1, opd2, opr - 3args\n";
        return 1;
    }

    int opd1 = std::atoi(argv[1]);
    int opd2 = std::atoi(argv[2]);
    char opr = argv[3][0];

    if (opr == '+')
        std::cout << plus(opd1, opd2) << "\n";
    else if (opr == '-')
        std::cout << minus(opd1, opd2) << "\n";
    else if (opr == '^')
        std::cout << power(opd1, opd2) << "\n";
    else
        std::cout << "error: unknown operator\n";

    return 0;
}