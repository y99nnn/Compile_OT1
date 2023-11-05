#include<iostream>
#include<unordered_map>
#include<vector>
#include<list>
#include<set>
#include<string>
using namespace std;

enum NODETYPE { START, PATH, END };
struct NFANode {
    NODETYPE NodeType;
    int NodeID;
    NFANode(NODETYPE t) : NodeType(t) {};
    NFANode(NODETYPE t, int nodeid) : NodeType(t), NodeID(nodeid) {};
};

struct NFA {
    NFANode* start_NFANode, * end_NFANode;
    list<NFANode*> NodeinNFA;
    string InputAction;
    NFA(NFANode* start, NFANode* end) : start_NFANode(start), end_NFANode(end) {};
};

unordered_map<NFANode*, unordered_map<char, vector<NFANode*>>> NFAtransitions;

// 初始化一个 NFANode* 节点后，创建一个新行
NFANode* New_NFANode(NODETYPE nodetype, int nodeid) {
    NFANode* theNewNode = new NFANode(nodetype, nodeid);
    NFAtransitions[theNewNode] = unordered_map<char, vector<NFANode*>>();
    return theNewNode;
}

// 添加字符对应的下一个 NFANode*
void addTransition(NFANode* from, char input, NFANode* to) {
    NFAtransitions[from][input].push_back(to);
}

// 通过字符查找下一个 NFANode*
vector<NFANode*> getNextNodes(NFANode* current, char input) {
    if (NFAtransitions.find(current) != NFAtransitions.end()) {
        if (NFAtransitions[current].find(input) != NFAtransitions[current].end()) {
            return NFAtransitions[current][input];
        }
    }
    return {}; // 没有找到对应的节点
}

// 使用基本规则构造NFA
NFA* BasicNFA(char action) {
    NFANode* node1 = New_NFANode(START, 0); // 创建一个开始状态
    NFANode* node2 = New_NFANode(END, 1); // 创建一个接收状态
    addTransition(node1, action, node2); // 添加一条新的边
    struct NFA* nfa = new NFA(node1, node2); // 创建一个NFA
    nfa->NodeinNFA.push_front(node1); // 将开始结点添加到节点列表中
    nfa->NodeinNFA.push_back(node2); // 将结束结点添加到节点列表中
    if (action != 0)
        nfa->InputAction + action; // 更新NFA所接受的正规表达式
    return nfa; // NFA构造完毕
}

NFA* unionNFA(NFA* nfa1, NFA* nfa2) {
    NFANode* node1 = New_NFANode(START, 0);//创建一个开始状态
    NFANode* node2 = New_NFANode(END, 1);//创建一个接收状态

    addTransition(node1, 0, nfa1->start_NFANode);//添加开始结点指向N(t)的边
    addTransition(node1, 0, nfa2->start_NFANode);//添加开始结点指向N(s)的边
    addTransition(nfa1->end_NFANode, 0, node2);//添加N(t)指向接收结点的边
    addTransition(nfa2->end_NFANode, 0, node2);//添加N(s)指向接收结点的边

    NFA* nfa = new NFA(node1, node2);//创建一个NFA

    nfa->NodeinNFA.splice(nfa->NodeinNFA.end(), nfa1->NodeinNFA);//将N(t)中的结点添加到节点列表中
    nfa->NodeinNFA.splice(nfa->NodeinNFA.end(), nfa2->NodeinNFA);//将N(s)中的结点添加到节点列表中
    nfa->NodeinNFA.push_front(node1);//将开始结点添加到节点列表中
    nfa->NodeinNFA.push_back(node2);//将结束结点添加到节点列表中

    //更新NFA所接受的正规表达式，由于是并操作，因此s和t有一个为空，直接增加即可。
    nfa->InputAction = nfa->InputAction + nfa1->InputAction + nfa2->InputAction;//更新NFA所接受的正规表达式
    return nfa;
}

NFA* concatenateNFA(NFA* nfa1, NFA* nfa2) {
    NFANode* node1 = New_NFANode(START, 0);//创建一个开始状态
    NFANode* node2 = New_NFANode(END, 1);//创建一个接收状态

    NFA* nfa = new NFA(node1, node2); // 创建一个新的 NFA，起始节点为第一个 NFA的起始节点，结束节点为第二个 NFA的结束节点

    addTransition(node1, 0, nfa1->start_NFANode); // 添加开始结点指向第一个NFA的开始节点的边
    addTransition(nfa1->end_NFANode, 0, nfa2->start_NFANode); // 添加第一个NFA的结束节点指向第二个NFA的开始节点的边
    addTransition(nfa2->end_NFANode, 0, node2); // 添加第二个NFA的结束节点指向结束节点的边

    nfa->NodeinNFA.splice(nfa->NodeinNFA.end(), nfa1->NodeinNFA); // 将第一个 NFA 的节点列表合并到新的 NFA 的节点列表中
    nfa->NodeinNFA.splice(nfa->NodeinNFA.end(), nfa2->NodeinNFA); // 将第二个 NFA 的节点列表合并到新的 NFA 的节点列表中
    nfa->NodeinNFA.push_front(node1); // 将开始结点添加到节点列表中
    nfa->NodeinNFA.push_back(node2); // 将结束结点添加到节点列表中

    // 更新 NFA 的输入动作
    nfa->InputAction = nfa1->InputAction + nfa2->InputAction; // 修正输入动作连接的问题

    return nfa;
}

NFA* closureNFA(NFA* nfa1) {
    NFANode* node1 = New_NFANode(START, 0);//创建一个开始状态
    NFANode* node2 = New_NFANode(END, 1);//创建一个接收状态

    addTransition(node1, 0, nfa1->start_NFANode);//增加开始结点指向N(s)的边
    addTransition(nfa1->end_NFANode, 0, node2);//添加N(s)指向接收结点的边
    addTransition(nfa1->end_NFANode, 0, nfa1->start_NFANode);//添加N(s)结束结点指向N(s)开始结点的边
    addTransition(node1, 0, node2);//增加开始结点指向接收结点的边

    NFA* nfa = new NFA(node1, node2);// 创建一个新的 NFA

    nfa->NodeinNFA.splice(nfa->NodeinNFA.end(), nfa1->NodeinNFA);//将N(s)中的结点添加到节点列表中
    nfa->NodeinNFA.push_front(node1);//将开始结点添加到节点列表中
    nfa->NodeinNFA.push_back(node2);//将结束结点添加到节点列表中

    nfa->InputAction = nfa->InputAction + nfa1->InputAction;//更新NFA所接受的正规表达式
    return nfa;
}



// 重置 NFA 中节点的标识符 ID
void makeID(NFA* nfa) {
    int id = 0; // 初始化标识符 ID
    for (auto& node : nfa->NodeinNFA) {
        // 为 NFA 中的每个节点设置新的 ID
        node->NodeID = id++;
    }
}

void PrintNFA(NFA* nfa) {
    makeID(nfa); // 为NFA中的节点分配ID
    cout << "正则表达式转NFA：" << endl; 

    // 遍历NFA中的每个节点
    for (auto node : nfa->NodeinNFA) {
        // 遍历当前节点关联的转移关系
        for (auto& transition : NFAtransitions[node]) {
            char input = transition.first; // 获取转移关系对应的输入字符

            vector<NFANode*> nextNodes = transition.second; // 获取当前字符下的下一个状态节点集合

            // 遍历下一个状态节点集合
            for (auto nextNode : nextNodes) {
                // 输出当前节点到下一个节点的转移关系，包括节点ID和输入字符
                cout << "State ID-" << node->NodeID << " -[" << input << "]-> State ID-" << nextNode->NodeID << endl;
            }
        }
    }

}
