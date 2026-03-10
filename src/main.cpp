#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <wininet.h>
#include <fstream>
#include <map>

#pragma comment(lib, "wininet.lib")


std::string GetCipherChar(int value) {
    static const std::map<int, std::string> cipher = {
        {0, "H"}, {1, "M"}, {2, "K"}, {3, "J"}, {4, "L"}, {5, "P"}, {6, "X"}, {7, "Z"},
        {8, "A"}, {9, "N"}, {10, "C"}, {11, "E"}, {12, "Y"}, {13, "S"}, {14, "U"}, {15, "B"},
        {16, "V"}, {17, "F"}, {18, "D"}, {19, "T"}, {20, "W"}, {21, "Q"}, {22, "O"}, {23, "G"},
        {24, "R"}, {25, "I"}, {26, "#"}, {27, "&"}
    };
    return cipher.count(value) ? cipher.at(value) : "H";
}

// Fetches the current Date from Google Headers (NIST-style)
SYSTEMTIME GetNetworkTime() {
    SYSTEMTIME st = { 0 };
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, "http://www.google.com", NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect) {
            char dateStr[100];
            DWORD dwSize = sizeof(dateStr);
            if (HttpQueryInfoA(hConnect, HTTP_QUERY_DATE, dateStr, &dwSize, NULL)) {
                
                FILETIME ft;
                if (InternetTimeToSystemTimeA(dateStr, &st, 0)) {
                    
                }
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    return st;
}

int main() {
    SYSTEMTIME st = GetNetworkTime();

    // Fallback to local time only if network fails
    if (st.wYear == 0) {
        std::cout << "Network failed. Using local time (Unsafe)..." << std::endl;
        GetLocalTime(&st);
    }

    
    int expMonth = st.wMonth + 2;
    int expYear = st.wYear;
    if (expMonth > 12) { expMonth -= 12; expYear++; }

    std::string dd = (st.wDay < 10 ? "0" : "") + std::to_string(st.wDay);
    std::string mm = (expMonth < 10 ? "0" : "") + std::to_string(expMonth);
    std::string yyyy = std::to_string(expYear);

    
    long long t1 = (std::stoll(dd) + 29) * 37 + 79;
    long long t2 = (std::stoll(mm) + 57) * 43 + 39;
    long long t3 = (std::stoll(yyyy) + t2) * 12573 + 753;

    std::string s1 = std::to_string(t1);
    std::string s2 = std::to_string(t2);
    std::string s3 = std::to_string(t3);

    
    auto GetBlock = [&](int type) {
        std::string res = "";
        for (int i = 0; i < 4; i++) {
            int val = 0;
            if (type == 0) val = (i == 0 ? 7 : i == 1 ? 5 : i == 2 ? 9 : 3) + (s1[i] - '0');
            else if (type == 1) val = (s1[i] - '0') + (s2[i] - '0');
            else if (type == 2) val = (s1[i] - '0') + (s2[i] - '0') + (s3[i] - '0');
            else if (type == 3) val = (s1[i] - '0') + (s2[i] - '0') + (s3[i + 4] - '0');
            res += GetCipherChar(val);
        }
        return res;
        };

    std::string key = GetBlock(0) + "-" + GetBlock(1) + "-" + GetBlock(2) + "-" + GetBlock(3);

    
    std::ofstream file("License.txt");
    if (file << key) {
        std::cout << "License file created successfully!" << std::endl;
        std::cout << "Key: " << key << std::endl;
        std::cout << "Expiration: " << dd << "." << mm << "." << yyyy << std::endl;
    }

    return 0;
}