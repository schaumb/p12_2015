#include <iostream>
#include <cassert>
#include <vector>
#include <sstream>
#include <fstream>
#include <tuple>

#if !defined(WIN32) && !defined(_WIN32)
void gotoxy(int x, int y)
{
  printf("\033[%d;%df", y, x);
  fflush(stdout);
}
#else // WIN32
#include <windows.h>

void gotoxy(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
#endif // WIN32

static std::string clr(100, ' ');
std::ofstream debug("asd.txt");

class BigNum {
public:
    BigNum(std::ostream& o = std::cout, const std::string& str = "") : number(str), outp(&o) {}

    BigNum& operator=(const std::string& str) {
        auto it = str.begin();

        while(it != str.end() && std::string("123456789").find(*it) == std::string::npos) {
            if(*it == '0') {
                number = "0";
                return *this;
            }
            ++it;
        }

        while(it != str.end()) {
            if(std::string("0123456789").find(*it) == std::string::npos) {
                break;
            }
            number.push_back(*it);
            ++it;
        }
        return *this;
    }
    friend BigNum abs(BigNum&& bn);
    friend BigNum operator-(const BigNum& lhs, const BigNum& rhs);
    friend BigNum operator+(const BigNum& lhs, const BigNum& rhs);
    friend BigNum operator*(const BigNum& lhs, const BigNum& rhs);
    friend std::pair<BigNum, BigNum> operator/(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<(const BigNum& lhs, const BigNum& rhs);

    friend bool operator!=(const BigNum& lhs, const BigNum& rhs) {
        return lhs < rhs || rhs < lhs;
    }

    BigNum sqrt(std::ostream& out = std::cout) {
        std::stringstream ss;
        static BigNum two(std::cout, "2");
        two.outp = &ss;
        this->outp = &ss;


        auto lambda = [this](const BigNum& num) {
            auto doublenum = (num * two);
            auto result = *this / doublenum;
            auto result2 = num / two;
            auto doubledouble = doublenum * two;

            auto first = result.second * doubledouble ;
            auto second = result2.second * doubledouble;

            auto nev = first + second;

            auto div = nev / doubledouble;

            if(doublenum < div.second) {
                ++div.first;
            }
            return result.first + result2.first; // + div.first;
        };
        BigNum bn = *this, bn2;
        bn.outp = &ss;
        int nth = 0;
        bool quit = false;
        while(!quit && bn != (bn2 = lambda(bn))) {
            gotoxy(1, 1);
            out << "We use converge algorithm x_n+1 = x_n / 2 + a / (x_n * 2)" << clr << std::endl;

            gotoxy(1, 2);
            out <<  "(inaccurate, because these are natural numbers, you can stop this with 'q')" << clr;
            gotoxy(1, 3);
            out << clr;
            gotoxy(1, 3);
            out << ++nth << ". step: " << bn2.number << " = " << bn.number << " / 2 + " << this->number << " / " << " (" << bn.number << " * 2) " << clr << std::endl;
            switch(std::cin.get()) {
            case 'q' :
                out << "quited\n";
                quit = true;
                break;
            default:
                break;
            }

            bn = bn2;
        }

        gotoxy(1, 1);
        out << "We used converge algorithm x_n+1 = x_n / 2 + a / (x_n * 2)" << clr << std::endl;

        gotoxy(1, 2);
        out << clr;
        gotoxy(1, 3);
        out << clr;
        gotoxy(1, 3);
        std::cout << ++nth << ". step: " << bn2.number << " = " << " (" << bn.number << " + " << this->number << " / " << bn.number << ") / 2" << clr << std::endl;
        std::cin.get();

        return bn;
    }

    std::ostream& print(std::size_t prevSpaces = 0, std::ostream& out = std::cout) const {
        return out << std::string(prevSpaces, ' ') << number;
    }

    const std::string& getNumber() const {
        return number;
    }

    BigNum& operator+=(char c) {
        number = c + number;
        return *this;
    }

    BigNum& operator++() {
        auto it = number.rbegin();
        while(it != number.rend()) {
            if(*it < '9') {
                ++*it;
                return *this;
            }
            *it = '0';
            ++it;
        }
        *this += '1';
        return *this;
    }

    std::string number;
    std::ostream* outp;
};

BigNum abs(BigNum&& bn) {
    if(bn.number[0] == '-') {
        bn.number = bn.number.substr(1);
    }
    return bn;
}
void prettyUnder(const BigNum& lhs, const BigNum& rhs, const BigNum& result, char op, size_t maxSize, size_t where, std::ostream& out = std::cout) {
    gotoxy(1, 1);

    out << "Calculating " << lhs.number << " " << op << " " << rhs.number << clr << std::endl;

    gotoxy(1, 3);
    out << std::string(maxSize - where, ' ') << '|' << clr << std::endl;
    gotoxy(1, 4);
    lhs.print(maxSize - lhs.number.size(), out) << clr << std::endl;
    gotoxy(1, 5);
    out << op;
    rhs.print(maxSize - rhs.number.size() - 1, out) << clr << std::endl;
    gotoxy(1, 6);
    out << std::string(maxSize, '-') << clr << std::endl;
    gotoxy(1, 7);
    result.print(maxSize - result.number.size(), out) << clr << std::endl;
    gotoxy(1, 8);
}

void prettyMul(const BigNum& lhs, const BigNum& rhs, const std::vector<BigNum>& result, char op, size_t where, std::ostream& out = std::cout) {
    gotoxy(1, 1);
    out << "Calculating " << lhs.number << " " << op << " " << rhs.number << clr << std::endl;

    auto step = lhs.number.size();
    gotoxy(1, 3);
    out << std::string(lhs.number.size() - where % step, ' ') << '|' <<
            std::string(where % step + 1, ' ') <<
            std::string(where / step, ' ') << '|' << clr << std::endl;
    gotoxy(1, 4);
    lhs.print(1, out);
    out << op;
    rhs.print(0, out) << clr << std::endl;
    gotoxy(1, 5);
    out << std::string(step + 2 + rhs.number.size(), '-') << clr;
    for(auto i = 0UL; i < result.size(); ++i) {
        gotoxy(1, i+6);
        auto& bnum = result.at(i);
        bnum.print(step + rhs.number.size() - bnum.number.size(), out) << clr;
    }
    gotoxy(1, 6 + result.size());
    out << std::string(lhs.number.size() - 1 - where % step + result.size(), ' ') << '|' << clr;
    gotoxy(1, 7 + result.size());
}

void printOp(char nd1, char nd2, char op, int rem, int result, std::ostream& out, char op2 = -1) {
    int dititInt1 = nd1 - '0';
    int digitInt2 = nd2 - '0';

    if(op2 == -1) op2 = op;

    out << "Next digit: " << dititInt1;
    if(nd2 != -1) {
        out << " " << op << " " << digitInt2;
    }

    if(rem) {
        out << " " << op2 << " " << rem << " (remainder)";
    }
    out << " = " << result << clr << std::endl;
}


bool operator<(const BigNum& lhs, const BigNum& rhs) {
    debug << lhs.number << " " << (lhs.outp == &std::cout) << " < " << rhs.number << " " << (rhs.outp == &std::cout) << std::endl;

    if(lhs.number.size() != rhs.number.size()) {
        return lhs.number.size() < rhs.number.size();
    }
    auto itl = lhs.number.begin();
    auto itr = rhs.number.begin();

    while(itl != lhs.number.end()) {
        if(*itl != *itr) {
            return *itl < *itr;
        }
        ++itl;
        ++itr;
    }
    return false;
}

BigNum operator+(const BigNum& lhs, const BigNum& rhs) {
    debug << lhs.number << " " << (lhs.outp == &std::cout) << " + " << rhs.number << " " << (rhs.outp == &std::cout) << std::endl;

    BigNum result(*lhs.outp);
    std::ostream& out = *lhs.outp;
    std::size_t maxSize = std::max(lhs.number.size(), rhs.number.size()) + 1;

    bool remainder = false;
    auto itl = lhs.number.rbegin();
    auto itr = rhs.number.rbegin();
    int i = 0;

    while(itl != lhs.number.rend() || itr != rhs.number.rend()) {
        int next = 0;

        if(itl != lhs.number.rend()) {
            next += *itl - '0';
        }

        if(itr != rhs.number.rend()) {
            next += *itr - '0';
        }

        if(remainder) {
            ++next;
            result.number.front() = next % 10 + '0';
        }
        else {
            result += next % 10 + '0';
        }

        if(next > 9) {
            result += '1';
        }

        prettyUnder(lhs, rhs, result, '+', maxSize, ++i, out);

        if(itl == lhs.number.rend()) {
            printOp(*itr, -1, '+', remainder, next, out);
            ++itr;
        }
        else if(itr == rhs.number.rend()) {
            printOp(*itl, -1, '+', remainder, next, out);
            ++itl;
        }
        else {
            printOp(*itl, *itr, '+', remainder, next, out);
            ++itl;
            ++itr;
        }

        remainder = next > 9;
        if(&out == &std::cout) {
            std::cin.get();
        }
    }

    return result;
}


BigNum operator-(const BigNum& lhsx, const BigNum& rhsx) {
    debug << lhsx.number << " " << (lhsx.outp == &std::cout) << " - " << rhsx.number << " " << (rhsx.outp == &std::cout) << std::endl;

    bool swapped = lhsx < rhsx;

    const BigNum& lhs(swapped ? rhsx : lhsx);
    const BigNum& rhs(swapped ? lhsx : rhsx);

    BigNum result(*lhs.outp);
    std::ostream& out = *lhsx.outp;

    if(swapped) {
        gotoxy(1, 1);
        out << "The first argument is smaller. We switch the arguments, and negate the result." << std::endl;
        if(&out == &std::cout) {
            std::cin.get();
        }
    }


    std::size_t maxSize = std::max(lhs.number.size(), rhs.number.size()) + 1;

    bool remainder = false;
    auto itl = lhs.number.rbegin();
    auto itr = rhs.number.rbegin();
    int i = 0;

    while(itl != lhs.number.rend() || itr != rhs.number.rend()) {
        int next = 0;

        if(itl != lhs.number.rend()) {
            next += *itl - '0';
        }

        if(itr != rhs.number.rend()) {
            next -= *itr - '0';
        }

        if(remainder) {
            --next;
            result.number.front() = (next + 10) % 10 + '0';
        }
        else {
            result += (next + 10) % 10 + '0';
        }

        if(next < 0) {
            result += '-';
        }

        prettyUnder(lhs, rhs, result, '-', maxSize, ++i, out);

        if(itr == rhs.number.rend()) {
            if(*itl == 0 && remainder) {
                printOp(*itl + 10, -1, '-', remainder, next + 10, out);
            }
            else {
                printOp(*itl, -1, '-', remainder, next, out);
            }
            ++itl;
        }
        else {
            if(*itr + remainder > *itl) {
                printOp(*itl + 10, *itr, '-', remainder, next + 10, out);
            }
            else {
                printOp(*itl, *itr, '-', remainder, next, out);
            }

            ++itl;
            ++itr;
        }

        remainder = next < 0;

        if(&out == &std::cout) {
            std::cin.get();
        }
    }
    while(result.number.size() > 1 && result.number.front() == '0') {
        result.number = result.number.substr(1);
    }

    if(swapped) {
        out << "We negate the result." << std::endl;
        result += '-';
    }

    return result;
}


BigNum operator*(const BigNum& lhs, const BigNum& rhs) {
    debug << lhs.number << " " << (lhs.outp == &std::cout) << " * " << rhs.number << " " << (rhs.outp == &std::cout) << std::endl;
    if((lhs.number.size() == 1 && lhs.number == "0") ||
        (rhs.number.size() == 1 && rhs.number == "0"))
    {
        std::ostream& out = *lhs.outp;
        BigNum result(*lhs.outp);
        result.number = "0";
        gotoxy(1, 1);
        out << "Calculating " << lhs.number << " * " << rhs.number << clr << std::endl;
        gotoxy(1, 3);
        out << "Zero multiply, the result is zero." << std::endl;
        if(&out == &std::cout) {
            std::cin.get();
        }
        return result;
    }

    std::ostream& out = *lhs.outp;
    std::vector<BigNum> halfResults;
    int remainder = 0;
    std::stringstream ss;

    for(auto i = 0UL; i < rhs.number.size(); ++i) {
        halfResults.emplace_back(ss);
        halfResults.back().number = std::string(rhs.number.size() - i - 1, '0');
        remainder = 0;
        auto itl = lhs.number.rbegin();
        for(auto j = 0UL; j < lhs.number.size(); ++j, ++itl) {

            int next = (*itl - '0') * (rhs.number[i] - '0') + remainder;

            if(remainder) {
                halfResults.back().number.front() = next % 10 + '0';
            }
            else {
                halfResults.back() += next % 10 + '0';
            }

            if(next > 9) {
                halfResults.back() += (next / 10) + '0';
            }

            prettyMul(lhs, rhs, halfResults, '*', i * lhs.number.size() + j, out);
            printOp(*itl, rhs.number[i], '*', remainder, next, out, '+');

            remainder = next / 10;

            if(&out == &std::cout) {
                std::cin.get();
            }
        }
    }

    BigNum result(*lhs.outp);
    result.number = "0";
    for(BigNum& half : halfResults) {
        result = half + result;
    }
    prettyMul(lhs, rhs, halfResults, '*', rhs.number.size() * lhs.number.size() - 1, out);

    gotoxy(1, 5 + rhs.number.size());
    out << "+";
    gotoxy(1, 6 + rhs.number.size());

    auto max = lhs.number.size() + rhs.number.size();
    out << std::string(max, '-') << clr << std::endl;
    gotoxy(1, 7 + rhs.number.size());
    result.print(max - result.number.size(),out) << clr << std::endl;
    result.outp = lhs.outp;

    return result;
}


void prettyDiv(const BigNum& lhs, const BigNum& rhs, const std::vector<BigNum>& remainders, const BigNum& result, std::size_t pg, std::ostream& out = std::cout) {
    out << "Calculating " << lhs.number << " / " << rhs.number << clr << std::endl;

    gotoxy(1, 3);
    out << std::string(pg - 1, ' ') << "|" << clr;
    gotoxy(1, 4);
    out << lhs.number << ":" << rhs.number << "=" << result.number << clr << std::endl;
    for(auto i = 0UL; i < remainders.size(); ++i) {
        gotoxy(1, i+5);
        auto& bnum = remainders.at(i);
        bnum.print(i + 1 - bnum.number.size(), out) << clr;
    }
    gotoxy(1, 5 + remainders.size());
    out << clr;
    gotoxy(1, 6 + remainders.size());
    out << clr;
    gotoxy(1, 7 + remainders.size());
    out << clr;
    gotoxy(1, 6 + remainders.size());
}

std::pair<BigNum, BigNum> operator/(const BigNum& lhs, const BigNum& rhs) {
    debug << lhs.number << " " << (lhs.outp == &std::cout) << " / " << rhs.number << " " << (rhs.outp == &std::cout) << std::endl;
    std::ostream& out = *lhs.outp;

    BigNum result(*lhs.outp);


    if(lhs < rhs) {
        result.number = "0";

        gotoxy(1, 1);
        out << "Calculating " << lhs.number << " / " << rhs.number << clr << std::endl;
        gotoxy(1, 3);
        out << "The first argument is smaller than second." << std::endl;
        out << "The result is 0, and the remainder is the first argument." << std::endl;

        if(&out == &std::cout) {
            std::cin.get();
        }
        return {result, lhs};
    }
    if(rhs.number[0] == '0') {
        result.number = "ERR";
        gotoxy(1, 1);
        out << "Calculating " << lhs.number << " / " << rhs.number << clr << std::endl;
        gotoxy(1, 3);
        out << "Divisor is zero." << std::endl;

        return {result, result};
    }

    std::vector<BigNum> remainders;
    std::stringstream ss;
    remainders.emplace_back(ss);

    BigNum rhscp = rhs;
    rhscp.outp = &ss;

    int i = 0;
    auto itl = lhs.number.begin();
    while(itl != lhs.number.end()) {
        if(remainders.back().number[0] == '0') {
            remainders.back().number = *itl;
        }
        else {
            remainders.back().number.push_back(*itl);
        }

        BigNum bn(ss), prevBn(ss);
        bn.number = "0";
        BigNum mul, prevMul;

        mul = rhscp * bn;

        while(!(remainders.back() < mul)) {
            prevMul = mul;
            prevBn = bn;

            ++bn;
            mul = rhscp * bn;
        }

        if(prevBn.number[0] != '0') {
            if(result.number.size() == 0 || result.number[0] == '0') {
                result.number = prevBn.number;
            }
            else {
                result.number += prevBn.number;
            }
        }

        prettyDiv(lhs, rhs, remainders, result, ++i, out);

        out << remainders.back().number << " : " << rhs.number << " = " << prevBn.number;

        if(&out == &std::cout) {
            std::cin.get();
        }

        remainders.emplace_back(remainders.back() - prevMul);

        remainders.back().number.push_back(' ');
        prettyDiv(lhs, rhs, remainders, result, i, out);
        remainders.back().number.pop_back();
        out << prevBn.number << " * " << rhs.number << " = " << prevMul.number << std::endl;
        out << (++remainders.rbegin())->number << " - " << prevMul.number << " = " << remainders.back().number << std::endl;

        if(&out == &std::cout) {
            std::cin.get();
        }

        ++itl;
    }
    remainders.back().number.push_back(' ');
    prettyDiv(lhs, rhs, remainders, result, i, out);
    remainders.back().number.pop_back();
    remainders.back().outp = lhs.outp;
    return std::make_pair(result, remainders.back());
}

int main()
{
    std::string task;

    while(std::getline(std::cin, task)) {

        if(task.empty()) break;

        std::size_t operatorIndex = task.find_first_of("+-*/g");


        gotoxy(1, 1);
        for(int i = 0; i < 70; ++i)
            std::cout << clr << "\n";

        if(operatorIndex == std::string::npos) {
            assert(false);
        }
        BigNum firstOperand;
        firstOperand = task;

        if(task[operatorIndex] == 'g') {
            auto sqrt = firstOperand.sqrt();
            std::cout << "sqrt("  << firstOperand.number << ") = " << sqrt.number << std::endl;
            std::cout << std::endl << " next : ";
            continue;
        }

        BigNum secondOperand;
        secondOperand = task.substr(operatorIndex);

        BigNum result;
        BigNum result2;
        switch(task[operatorIndex]) {
        case '+':
            result = firstOperand + secondOperand;
        break;
        case '-':
            result = firstOperand - secondOperand;
        break;
        case '*':
            result = firstOperand * secondOperand;
        break;
        case '/':
            std::tie(result, result2) = firstOperand / secondOperand;
        break;
        }
        std::cout << "Result is: ";

        result.print() << std::endl;
        if(result2.number.size()) {
            std::cout << "Remainder: ";
            result2.print() << std::endl;
        }
        std::cout << std::endl << " next : ";
    }
}
