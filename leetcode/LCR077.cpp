/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
/*
O(n) 的解法 转换为数组赋值
*/
class Solution {
public:
    ListNode* sortList(ListNode* head) {
        if (head == nullptr || head->next == nullptr) return head;  
        ListNode* cur = head;
        vector<int> a;
        while(cur != nullptr){
            a.push_back(cur->val);
            cur = cur->next;
        }
        sort(a.begin(), a.end());
        cur = head;
        for (int val : a){
            cur->val = val;
            cur = cur->next;
        }
        return head;
    }
    
};
/*
O(log(n)) 的链表归并 自顶向下
*/
class Solution {
public:
    ListNode* sortList(ListNode* head) {
        if (head == nullptr || head->next == nullptr) return head;  
        ListNode* slow = head;
        ListNode* fast = head->next;
        
        while(fast != nullptr && fast->next != nullptr){
            fast = fast->next->next;
            slow = slow->next;
        }

        ListNode* mid = slow->next;
        slow->next =nullptr;

        head = sortList(head);
        mid = sortList(mid);

        return merge(head, mid);
    }
    ListNode* merge(ListNode* l1, ListNode* l2){
        ListNode dummy(0);//栈空间的虚拟头节点
        ListNode* tail = &dummy;
        while(l1 != nullptr && l2 != nullptr){
            if(l1->val < l2->val){
                tail->next = l1;
                l1 = l1->next;
            }else{
                tail->next = l2;
                l2 = l2->next;
            }
            tail = tail->next;
        }
        tail->next = (l1 == nullptr ? l2 :l1);
        return dummy.next;
    }
};
/*
O(1)的实现 自底向上的链表处理
*/
class Solution {
public:
    ListNode* sortList(ListNode* head) {
        if (head == nullptr || head->next == nullptr) return head;  
        int len = 0;
        ListNode dummy(0, head);
        ListNode* cur = dummy.next;
        while(cur != nullptr){
            len++;
            cur = cur->next;
        }
        for(int step = 1; step < len; step <<= 1){
            ListNode* prev = &dummy; 
            cur = dummy.next;

            while(cur != nullptr){
                ListNode* left = cur;
                ListNode* right = split(left, step);
                cur = split(right, step);
                prev->next = merge(left, right);
                while(prev->next != nullptr){
                    prev = prev->next;
                }
            }
        }
        return dummy.next;
    }
    ListNode* split(ListNode* head, int n){
        while(--n && head != nullptr){
            head = head->next;
        }
        if(head == nullptr) return nullptr;
        
        ListNode* rest = head->next;
        head->next = nullptr;
        
        return rest;
    }
    ListNode* merge(ListNode* l1, ListNode* l2){
        ListNode dummy(0);//栈空间的虚拟头节点
        ListNode* tail = &dummy;
        while(l1 != nullptr && l2 != nullptr){
            if(l1->val < l2->val){
                tail->next = l1;
                l1 = l1->next;
            }else{
                tail->next = l2;
                l2 = l2->next;
            }
            tail = tail->next;
        }
        tail->next = (l1 == nullptr ? l2 :l1);
        return dummy.next;
    }
};