class Solution {
public:
    int minPartitions(string n) {
        int oup = 0;
        for(auto i : n){
            if (i - '0' > oup && i - '0' <= 9) oup = i - '0';
        }
        return oup;
    }
};
