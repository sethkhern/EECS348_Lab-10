#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

bool isValidDouble(const string& input) {
    int len = input.length();
    if (len == 0) return false;

    int i = 0;
    if (input[i] == '+' || input[i] == '-') i++;
    if (i == len) return false;

    bool hasDigitsBeforeDot = false;
    while (i < len && isdigit(input[i])) {
        hasDigitsBeforeDot = true;
        i++;
    }

    bool hasDigitsAfterDot = false;
    if (i < len && input[i] == '.') {
        i++;
        while (i < len && isdigit(input[i])) {
            hasDigitsAfterDot = true;
            i++;
        }
    }

    if (i != len) return false;
    if (!hasDigitsBeforeDot && !hasDigitsAfterDot) return false;
    if (input.find('.') != string::npos && !hasDigitsAfterDot) return false;

    return true;
}

void splitDoubleString(const string& num, string& sign, string& intPart, string& fracPart) {
    size_t i = 0;
    sign = "";
    if (num[0] == '+' || num[0] == '-') {
        sign = num[0];
        i++;
    }

    while (i < num.length() && isdigit(num[i])) {
        intPart += num[i];
        i++;
    }

    if (i < num.length() && num[i] == '.') {
        i++;
        while (i < num.length()) {
            fracPart += num[i];
            i++;
        }
    }
}

int compareParts(const string& intA, const string& fracA, const string& intB, const string& fracB) {
    if (intA.length() != intB.length())
        return intA.length() > intB.length() ? 1 : -1;
    if (intA != intB)
        return intA > intB ? 1 : -1;
    if (fracA != fracB)
        return fracA > fracB ? 1 : -1;
    return 0;
}

string subtractUnsignedStrings(const string& a, const string& b) {
    string intA, fracA, dummy;
    string intB, fracB;
    splitDoubleString(a, dummy, intA, fracA);
    splitDoubleString(b, dummy, intB, fracB);

    int fracLen = max(fracA.length(), fracB.length());
    while (fracA.length() < fracLen) fracA += '0';
    while (fracB.length() < fracLen) fracB += '0';

    int intLen = max(intA.length(), intB.length());
    while (intA.length() < intLen) intA = '0' + intA;
    while (intB.length() < intLen) intB = '0' + intB;

    string resultFrac = "";
    int borrow = 0;
    for (int i = fracLen - 1; i >= 0; --i) {
        int digit = (fracA[i] - '0') - (fracB[i] - '0') - borrow;
        if (digit < 0) {
            digit += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultFrac = char(digit + '0') + resultFrac;
    }

    string resultInt = "";
    for (int i = intLen - 1; i >= 0; --i) {
        int digit = (intA[i] - '0') - (intB[i] - '0') - borrow;
        if (digit < 0) {
            digit += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        resultInt = char(digit + '0') + resultInt;
    }

    size_t nonZero = resultInt.find_first_not_of('0');
    if (nonZero != string::npos) resultInt = resultInt.substr(nonZero);
    else resultInt = "0";

    while (!resultFrac.empty() && resultFrac.back() == '0') resultFrac.pop_back();

    return resultFrac.empty() ? resultInt : resultInt + "." + resultFrac;
}

string addUnsignedStrings(const string& a, const string& b) {
    string intA, fracA, dummy;
    string intB, fracB;
    splitDoubleString(a, dummy, intA, fracA);
    splitDoubleString(b, dummy, intB, fracB);

    int fracLen = max(fracA.length(), fracB.length());
    while (fracA.length() < fracLen) fracA += '0';
    while (fracB.length() < fracLen) fracB += '0';

    int intLen = max(intA.length(), intB.length());
    while (intA.length() < intLen) intA = '0' + intA;
    while (intB.length() < intLen) intB = '0' + intB;

    string resultFrac = "";
    int carry = 0;
    for (int i = fracLen - 1; i >= 0; --i) {
        int sum = (fracA[i] - '0') + (fracB[i] - '0') + carry;
        carry = sum / 10;
        resultFrac = char(sum % 10 + '0') + resultFrac;
    }

    string resultInt = "";
    for (int i = intLen - 1; i >= 0; --i) {
        int sum = (intA[i] - '0') + (intB[i] - '0') + carry;
        carry = sum / 10;
        resultInt = char(sum % 10 + '0') + resultInt;
    }
    if (carry > 0) resultInt = '1' + resultInt;

    while (!resultFrac.empty() && resultFrac.back() == '0') resultFrac.pop_back();

    return resultFrac.empty() ? resultInt : resultInt + "." + resultFrac;
}

string addDoubleStrings(const string& a, const string& b) {
    string signA, intA, fracA;
    string signB, intB, fracB;
    splitDoubleString(a, signA, intA, fracA);
    splitDoubleString(b, signB, intB, fracB);

    if (signA == signB) {
        string result = addUnsignedStrings(a, b);
        return (signA == "-" ? "-" : "") + result;
    } else {
        int cmp = compareParts(intA, fracA, intB, fracB);
        if (cmp == 0) return "0";
        bool aIsGreater = cmp > 0;
        string result = aIsGreater ? subtractUnsignedStrings(a, b)
                                   : subtractUnsignedStrings(b, a);
        return (aIsGreater ? signA : signB) == "-" ? "-" + result : result;
    }
}

int main(int argc, char* argv[]) {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "Failed to open input.txt" << endl;
        return 1;
    }

    const string constant = "123.456";
    string line;
    while (getline(file, line)) {
        if (!isValidDouble(line)) {
            cout << "Invalid input: " << line << endl;
            continue;
        }

        string result = addDoubleStrings(line, constant);
        cout << line << " + (" << constant << ") = " << result << endl;
    }

    file.close();
    return 0;
}
