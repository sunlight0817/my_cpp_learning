#include <iostream>
#include <algorithm>
#include <string_view>
#include <functional>
#include <vector>
#include <array>
using namespace std;
/*
string_view.find()
*/
void simple_search(std::string_view text, std::string_view pattern) {
    size_t pos = text.find(pattern);
    while (pos != std::string_view::npos) {
        std::cout << "找到匹配项:" << pos << "/n";
        pos = text.find(pattern.length() + pos);
    }
}
/*
手写kmp
*/
vector<int> lsp (std::string_view pattern) {
    int m = pattern.length();
    int i = 1;
    int len = 0;
    vector<int> lsp(m, 0);
    while (i < m) {
        if (pattern[len] == pattern[i]) {
            len++;
            lsp[i] = len;
            i++;
        }
        else {
            if (len != 0){
                len = lsp[len-1];
            }
            else {
                lsp[i] = 0;
                i++;
            }
        }
    }
    return lsp;
}
int kmp(std::string_view text, std::string_view pattern) {
    if (pattern.empty()) return 0;
    int i = 0;
    int j = 0;
    vector<int> lsp_ = lsp(pattern);
    int m = text.length();
    int n = pattern.length();
    while (i < m) {
        if (text[i] == pattern[j]) {
            if (j == n - 1) return i - j;
            i++;
            j++;
        }
        else {
            if (j == 0) i++;
            else {
                j = lsp_[j-1];
            }
        }
    }
    return -1;
}
/*
BM算法
采用好后缀(good suffix) 和坏字符(未出现字符) 来进行跳跃 从右往左比对 先比对最后一个字符
取好后缀和坏字符跳跃的最大值
BMH算法
针对坏字符进行优化，进行跳跃
*/

vector<int> bmh(std::string_view text, std::string_view pattern) {
    vector<int> result;
    int m = text.length();
    int n = pattern.length();
    if (m < n || n == 0) return result;
    std::array<int, 256> shift_table;
    shift_table.fill(n);
    int i = 0;
    while (i < n - 1) { // 不记录最后一个字符防止死循环
        unsigned char c = static_cast<unsigned>(pattern[i]);
        shift_table[c] =  n - i - 1;
        i++;
    }
    int shift = 0;
    while (shift <= m - n) {
        int j = n - 1;
        while (pattern[j] == text[shift + j] && j >= 0) {
            j--;
        }

        if (j < 0) {
            result.push_back(shift);
            unsigned char last = static_cast<unsigned>(text[shift + n - 1]);
            shift += shift_table[last];
        }
        //bmh不管在哪里失败，只比较最后一个字符进行跳跃
        else {
            unsigned char last = static_cast<unsigned>(text[shift + n - 1]);
            shift += shift_table[last];
        }
        //跳跃过后从最后一个字符开始比较
    }
    return result;
}
int main() {
    std::string_view text = "FIND A NEEDLE IN A HAYSTACK NEEDLE IN A HAYSTACK";
    std::string_view pattern = "NEEDLE";
    
    std::cout << "文本: " << text << '\n';
    std::cout << "模式: " << pattern << "\n\n";
    
    std::vector<int> matches = bmh(text, pattern);
    
    if (matches.empty()) {
        std::cout << "未找到匹配项。\n";
    } else {
        std::cout << "找到匹配项，索引位置: ";
        for (int pos : matches) {
            std::cout << pos << " ";
        }
        std::cout << '\n';
    }
    
    return 0;
}