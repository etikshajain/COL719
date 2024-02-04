// #include<bits/stdc++.h>
#include<string>
#include<stack>
#include<iostream>
#include<fstream>

using namespace std;

struct DFGNode {
    string value;
    vector<DFGNode*>children;
    bool type;
    int id;

    DFGNode(const string& val, const bool& type, int idd) : value(val), children({}), type(type), id(idd) {}
};

vector<string> tokenize(string expression, unordered_map<string,string>&single_assignment){
    vector<string> tokens;
    tokens.clear();
    string token;

    for (char c : expression) {
        if (c == ' ' || c == '\t') {
            if (!token.empty()) {
                if(single_assignment.find(token)!=single_assignment.end()){
                    token = single_assignment[token];
                }
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }

    }
    if (!token.empty()){
        if(single_assignment.find(token)!=single_assignment.end()){
                token = single_assignment[token];
        }
        tokens.push_back(token);
    }
    return tokens;
}

void generateDOTHelper(DFGNode* root, ofstream& dotFile, vector<bool>&vis) {
    if (root == nullptr) return;
    vis[root->id]=true;

    dotFile << "    \"" << root << "\" [label=\"" << root->value<< ','<<root->id << "\"];" << endl;

    for(DFGNode*child: root->children){
        dotFile << "    \"" << root << "\" -> \"" << child << "\";" << endl;
        if(vis[child->id]==false){
            generateDOTHelper(child, dotFile, vis);
        }
    }
}

void generateDOT(vector<DFGNode*> nodes, const string& fileName) {
    ofstream dotFile(fileName);
    dotFile << "digraph DFG {" << endl;
    // if (root != nullptr) {
    //     generateDOTHelper(root, dotFile);
    // }
    vector<bool>vis(nodes.size(),false);
    for(DFGNode*node: nodes){
        if(vis[node->id]==false){
            generateDOTHelper(node, dotFile, vis);
        }
    }
    dotFile << "}" << endl;
    dotFile.close();
}

void deleteDFG(vector<DFGNode*>nodes) {
    for(DFGNode* node: nodes){
        delete node;
    }
}

int precedence(char op) {
    if (op == '*' || op=='/'){
        return 3;
    }
    if(op=='+'){
        return 1;
    }
    if(op=='-'){
        return 1;
    }
    return 0;
}

vector<string> infixToPrefix(const vector<string>& tokens) {
    stack<char> operators;
    vector<string> output;

    unordered_map<string, bool> isIdentifier;
    for (const auto& token : tokens) {
        if (isalpha(token[0])) {
            isIdentifier[token] = true;
        }
    }
    // b - c + e
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        const string& token = *it;

        if (isdigit(token[0]) || isIdentifier[token]) {
            output.push_back(token);
        } else if (token == ")") {
            operators.push(')');
        } else if (token == "(") {
            while (!operators.empty() && operators.top() != ')') {
                output.push_back(string(1, operators.top()));
                operators.pop();
            }
            operators.pop(); // pop '('
        } else {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token[0])) {
                output.push_back(string(1, operators.top()));
                operators.pop();
            }
            operators.push(token[0]);
        }
    }

    while (!operators.empty()) {
        output.push_back(string(1, operators.top()));
        operators.pop();
    }

    reverse(output.begin(), output.end());

    return output;
}

DFGNode* constructDFG(const vector<string>& prefixExpression, unordered_map<string,DFGNode*>& dfg_node, vector<DFGNode*>&nodesList) {
    if (prefixExpression.empty()) {
        return nullptr;
    }

    stack<DFGNode*> nodes;
    for (int i = prefixExpression.size() - 1; i >= 0; i--) {
        const string& token = prefixExpression[i];

        DFGNode* newNode;
        
        // if the token is an operator or a constant -- always create a new node
        // no need to add this node to the map
        if(isdigit(token[0]) || !isalpha(token[0])){
            newNode = new DFGNode(token, false, nodesList.size());
            nodesList.push_back(newNode);
            // dfg_node[token] = newNode;
        }
        else{
            if(dfg_node.find(token)!=dfg_node.end()){
                newNode = dfg_node[token];
            }
            else{
                newNode = new DFGNode(token, true, nodesList.size());
                nodesList.push_back(newNode);
                // add the new node to the map
                dfg_node[token] = newNode;
            }
        }

        if (isdigit(token[0]) || isalpha(token[0])) {
            nodes.push(newNode);
        } else {
            DFGNode*opr1 = nodes.top();
            nodes.pop();
            DFGNode*opr2 = nodes.top();
            nodes.pop();
            opr1->children.push_back(newNode);
            opr2->children.push_back(newNode);
            nodes.push(newNode);
        }
    }

    return nodes.top();
}

int main() {
    string input_filename = "./io/example.txt";
    std::ifstream infile(input_filename);

    if (!infile) {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    // Read each statement from the file
    string statement;

    unordered_map<string,string>single_assignment;
    unordered_map<string,DFGNode*> dfg_node;
    vector<DFGNode*>nodes_list;

    DFGNode*root = nullptr;

    int line_num = 0;
    while (getline(infile, statement)) {
        line_num++;
        std::cout<<"\n\nstatement := "<<statement<<"\n";
        size_t pos = statement.find('=');
        if (pos != string::npos) {
            string lhs = statement.substr(0, pos - 1);
            string rhs = statement.substr(pos + 2); // Skip space after '='
            std::cout<<"lhs := "<<lhs<<" ; rhs := "<<rhs<<"\n";

            // check re-assignment of lhs
            if(single_assignment.find(lhs)!=single_assignment.end()){
                string new_lhs = lhs + "1";
                single_assignment[lhs] = new_lhs;
            }
            else{
                single_assignment[lhs]=lhs;
            }

            // Tokenize RHS expression with reassigned values
            vector<string> tokens_rhs = tokenize(rhs, single_assignment);
            std::cout<<"tokenized rhs : \n";
            for(auto &token : tokens_rhs){
                std::cout<<token<<";";
            }
            std::cout<<endl;

            // Tokenize LHS expression with reassigned values
            vector<string> tokens_lhs = tokenize(lhs, single_assignment);
            std::cout<<"tokenized lhs : \n";
            for(auto &token : tokens_lhs){
                std::cout<<token<<";";
            }
            std::cout<<"\n";

            // Convert infix to prefix for RHS
            vector<string> prefix_expression_rhs = infixToPrefix(tokens_rhs);
            std::cout<<"prefix expression rhs : \n";
            for(auto &token : prefix_expression_rhs){
                std::cout<<token<<";";
            }
            std::cout<<"\n";

            // Build DFG

            // lhs node is always a new node(single assignment)
            DFGNode*left = new DFGNode(tokens_lhs[0], true, nodes_list.size()); 
            nodes_list.push_back(left);

            // add the new node to map
            dfg_node[tokens_lhs[0]] = left;

            // build dfg for RHS 
            root = constructDFG(prefix_expression_rhs, dfg_node, nodes_list); // always an operator

            // the left node is an output of the operator node
            root->children.push_back(left);
            root=left;
        }
        
    }

    // Generate dot file for the tree
    string fileName = "./io/dfg_3.dot";
    generateDOT(nodes_list, fileName);

    // Delete DFG
    deleteDFG(nodes_list);

    return 0;
}
