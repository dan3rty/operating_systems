#include <iostream>
#include <string>

char DecodeChar(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        int edi = ch;
        int eax = 90 - (edi - 65);
        int rdx = eax * eax * 1321528399 >> 35;
        rdx *= 26;
        eax -= rdx;
        return static_cast<char>(eax + 65);
    } else if (ch >= 'a' && ch <= 'z') {
        int edi = ch;
        int eax = 122 - (edi - 97);
        int rdx = eax * eax * 1321528399 >> 35;
        rdx *= 26;
        eax -= rdx;
        return static_cast<char>(eax + 97);
    } else if (ch >= '0' && ch <= '9') {
        int edi = ch;
        int eax = 57 - (edi - 48);
        int rdx = eax * 23;
        rdx *= 3435973837;
        rdx >>= 35;
        rdx += rdx * 4;
        eax -= rdx;
        return static_cast<char>(eax + 48);
    } else {
        return ch; // Возвращаем символ без изменений
    }
}

std::string DecodeString(const std::string &str) {
    std::string decoded;
    for (char ch: str) {
        decoded += DecodeChar(ch);
    }
    return decoded;
}

int main() {
    std::string encryptedMessage = "Naq rju 5rj fu gorj qsw!";
    std::string decryptedMessage = DecodeString(encryptedMessage);

    std::cout << decryptedMessage << std::endl;

    return 0;
}
