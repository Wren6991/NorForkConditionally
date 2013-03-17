#include <iostream>

const std::string digits[] = {
"zero",
"one",
"two",
"three",
"four",
"five",
"six",
"seven",
"eight",
"nine"
};

const std::string tens[] = {
"XXX",
"teen", // don't use these two
"twenty",
"thirty",
"forty",
"fifty",
"sixty",
"seventy",
"eighty",
"ninety"
};

const std::string teens[] = {
"ten",
"eleven",
"twelve",
"thirteen",
"fourteen",
"fifteen",
"sixteen",
"seventeen",
"eighteen",
"nineteen"
};

const std::string powers[] = {
"thousand",
"million",
"billion",
"trillion",
"quadrillion",
"pentillion",
"sextillion",
"septillion",
"octillion"
};

std::string getdoubledecade(int tensdigit, int unitsdigit)
{
    std::string output;
    if (tensdigit == 0)
    {
        output += digits[unitsdigit];
    }
    else if (tensdigit == 1)
    {
        output += teens[unitsdigit];
    }
    else
    {
        output += tens[tensdigit] + (unitsdigit ? " " + digits[unitsdigit] : "");
    }
    return output;
}

std::string gettripledecade(long long n)
{
    int hundredsdigit = (n / 100) % 10;
    int tensdigit = (n / 10) % 10;
    int unitsdigit = n % 10;
    std::string output = "";
    if (hundredsdigit)
    {
        output += digits[hundredsdigit ] + " hundred ";
        if (tensdigit || unitsdigit)
        {
            output += "and " + getdoubledecade(tensdigit, unitsdigit);
        }
    }
    else
    {
        return getdoubledecade(tensdigit, unitsdigit);
    }
    return output;
}

std::string numbertoword(long long n)
{
    std::string output;
    output = gettripledecade(n);
    int power = 0;
    while ((n = n / 1000))
    {
        std::string tripledecade = gettripledecade(n);
        if (tripledecade != "zero")
            output = tripledecade + " " + powers[power] + (output == "zero"? "" : ", " + output);
        power++;
    }
    return output;
}

int main()
{
    long long n;
    std::cin >> n;
    std::cout << numbertoword(n);
}
