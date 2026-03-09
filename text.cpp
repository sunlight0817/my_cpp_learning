#include <iostream>
#include <memory>
#include <string>
using namespace std;
void print(string&& a) {
    string b = std::move(a);
    cout << b;
}
int main(){
    string a = "jdkj";
    print(std::move(a));
    cout << a;
    return 0;
}