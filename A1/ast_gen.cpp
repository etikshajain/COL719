#include<bits/stdc++.h>

using namespace std;


struct TreeNode {
    string value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const string& val) : value(val), left(nullptr), right(nullptr) {}
};

TreeNode* constructAST(const vector<string>& prefixExpression) {
    if (prefixExpression.empty()) {
        return nullptr;
    }

    stack<TreeNode*> nodes;

    for (int i = prefixExpression.size() - 1; i >= 0; --i) {
        const string& token = prefixExpression[i];

        TreeNode* newNode = new TreeNode(token);

        if (isdigit(token[0]) || isalpha(token[0])) {
            nodes.push(newNode);
        } else {
            newNode->left = nodes.top();
            nodes.pop();
            newNode->right = nodes.top();
            nodes.pop();
            nodes.push(newNode);
        }
    }

    return nodes.top();
}

void deleteTree(TreeNode* root) {
    if (root) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

// void printTree(TreeNode* root) {
//     if (root) {
//         cout << "(" << root->value << " ";
//         printTree(root->left);
//         printTree(root->right);
//         cout << ")";
//     }
// }

// void printTree(TreeNode* root, string prefix = "", bool isLeft = true) {
//     if (root == nullptr) return;

//     cout << prefix;
//     cout << (isLeft ? "├──" : "└──" );

//     cout << root->value << endl;

//     printTree(root->left, prefix + (isLeft ? "│   " : "    "), true);
//     printTree(root->right, prefix + (isLeft ? "│   " : "    "), false);
// }
void generateDOT(TreeNode* root, ofstream& dotFile) {
    if (root == nullptr) return;

    dotFile << "    \"" << root << "\" [label=\"" << root->value << "\"];" << endl;

    if (root->left != nullptr) {
        dotFile << "    \"" << root << "\" -> \"" << root->left << "\" [label=\"\"];" << endl;
        generateDOT(root->left, dotFile);
    }

    if (root->right != nullptr) {
        dotFile << "    \"" << root << "\" -> \"" << root->right << "\" [label=\"\"];" << endl;
        generateDOT(root->right, dotFile);
    }
}

vector<string> tokenize(string expression){
    vector<string> tokens;
    tokens.clear();
    string token;

    for (char c : expression) {
        if (c == ' ' || c == '\t') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }

    }
    if (!token.empty()){
        tokens.push_back(token);
    }
    return tokens;
}

int precedence(char op) {
    if (op == '+') return 1;
    if (op == '-') return 2;
    if (op == '*' || op == '/') return 3;
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

void generateDOTHelper(TreeNode* root, ofstream& dotFile) {
    if (root == nullptr) return;

    dotFile << "    \"" << root << "\" [label=\"" << root->value << "\"];" << endl;

    if (root->left != nullptr) {
        dotFile << "    \"" << root << "\" -> \"" << root->left << "\";" << endl;
        generateDOTHelper(root->left, dotFile);
    }

    if (root->right != nullptr) {
        dotFile << "    \"" << root << "\" -> \"" << root->right << "\";" << endl;
        generateDOTHelper(root->right, dotFile);
    }
}

void generateDOT(TreeNode* root, const string& fileName) {
    ofstream dotFile(fileName);
    dotFile << "digraph AST {" << endl;
    if (root != nullptr) {
        generateDOTHelper(root, dotFile);
    }
    dotFile << "}" << endl;
    dotFile.close();
}

int main() {
    string input_filename = "example.txt";
    std::ifstream infile(input_filename);

    if (!infile) {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    // Read each statement from the file
    string statement;
    unordered_map<string, vector<string>> dfg; // Data Flow Graph

    int line_num = 0;
    while (getline(infile, statement)) {
        line_num++;
        cout<<"\n\nstatement := "<<statement<<"\n";
        size_t pos = statement.find('=');
        if (pos != string::npos) {
            string lhs = statement.substr(0, pos - 1);
            string rhs = statement.substr(pos + 2); // Skip space after '='
            // cout<<"lhs := "<<lhs<<" ; rhs := "<<rhs<<"\n";

            // Build AST for RHS expression
            vector<string> tokens = tokenize(statement);
            cout<<"tokenized rhs : \n";
            for(auto &token : tokens){
                cout<<token<<";";
            }
            cout<<"\n";
            vector<string> prefix_expression = infixToPrefix(tokens);
            cout<<"prefix expression : \n";
            for(auto &token : prefix_expression){
                cout<<token<<";";
            }
            cout<<"\n";
            TreeNode* root = constructAST(prefix_expression);

            // cout << "Abstract Syntax Tree (AST): ";
            // printTree(root);
            // cout << endl;

            
            string fileName = "ast_" + to_string(line_num) + ".dot";
            generateDOT(root, fileName);

            cout << "DOT file generated: " << fileName << endl;
            // Clean up the tree
            deleteTree(root);

        }
        
    }

    return 0;
}
