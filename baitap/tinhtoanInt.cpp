#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>

using namespace std;

void printIntStack(stack<int> s) {
    cout << "Stack<int>: ";
    while (!s.empty()) {
        cout << s.top() << " ";
        s.pop();
    }
    cout << endl;
}

void printCharStack(stack<char> s) {
    cout << "Stack<char>: ";
    while (!s.empty()) {
        cout << s.top() << " ";
        s.pop();
    }
    cout << endl;
}


int stringToInt (string s){
    int num = 0;
    for(int i = 0; i < s.length() ; i++){
        num += (s[i] -'0') * pow(10,s.length()-i - 1);
    }
    return num;
}

int tinhtoan(int a, int b, char op){
    switch (op) {
    case '+':
        return a + b;
        break;
    case '-':
        return a - b;
        break;
    case '*':
        return a * b;
        break;
    case '/':
        return a / b;
        break;
    default:
        return 0;
        break;
    }
}
vector<string> stringToVector(string s){
    string numStr;
    char *ptrChar = &s[0];
    vector<string> expression;
    int i = 0;
    while(s[i] != '\0'){
        int ascii  = short(*(ptrChar + i)) ;
        if(48 <= ascii && ascii < 58){
            numStr += *(ptrChar + i);
        }
        else if (40 <= ascii && ascii <= 47){
            if(numStr != ""){
                expression.push_back(numStr);
            }
            expression.push_back(std::string("") + *(ptrChar + i));
            numStr = "";
        }
        i++;
    }
    if(numStr != "") {
        expression.push_back(numStr);
    }
    return expression;
}

int checkOps(string str){
    char value = str[0];
    switch (value) {
    case '+':
        return 1;
        break;
    case '-':
        return 1;
        break;
    case '*':
        return 2;
        break;
    case '/':
        return 2;
        break;
    case '(':
        return 3;
        break;
    case ')':
        return 3;
        break;
    default:
        return 0;
        break;
    }
}

int evaluateExpression(vector<string> expressionVector){
    stack<int> number;
    stack<string> operators;
    // cout << expressionVector.size() << endl;
    auto ptr = expressionVector.begin();
    while(ptr != expressionVector.end()){
        int op = checkOps(*ptr);

        if(op > 0){
            if(op == 3){
                vector<string> expressionVectorChild;
                auto ptrChild = ptr + 1;
                // cout << "Vaooo "<< *ptrChild << endl;
                int couter = 1;
                while(couter != 0){
                    if(*ptrChild == "("){
                        couter++;
                    }
                    else if(*ptrChild == ")"){
                        couter--;
                    }
                    if(couter != 0){
                        expressionVectorChild.push_back(*ptrChild);
                    }
                    ptrChild++;
                }
                ptr += ptrChild - ptr;
                cout << "Giá trị trong expressionVectorChild: ";
                for (const string& token : expressionVectorChild) {
                    cout << token << " ";
                }
                cout << endl;

                int value = evaluateExpression(expressionVectorChild);
                cout << "   result " << value << endl;
                number.push(value);
                continue;
            }
            else if(operators.empty()){
                // cout << "operators empty   "<< *ptr << endl;
                operators.push(*ptr);
                ptr++;
                continue;
            }
            else if(op > checkOps(operators.top())){
                operators.push(*ptr);
            }
            else if(op <= checkOps((operators.top()))){
                int a,b;
                char op;
                b = number.top();
                number.pop();
                if(number.empty()){
                    a = 0;
                }else{
                    a = number.top();
                    number.pop();
                }
                if(operators.empty()){
                    op = '+';
                }
                else{
                    op = operators.top()[0];
                }
                int value = tinhtoan(a, b, operators.top()[0]);
                operators.pop();
                number.push(value);
                continue;
            }
        }
        else{
            number.push(stringToInt(*ptr));
        }
        // cout << "end "<< *(expressionVector.end() - 1) << endl;
        ptr++;

    }
    int a,b;
    char op;
    b = number.top();
    number.pop();
    if(number.empty()){
        a = 0;
    }else{
        a = number.top();
        number.pop();
    }
    if(operators.empty()){
        op = '+';
    }
    else{
        op = operators.top()[0];
    }
    // cout << "tinhtoan  "<< "   "<< a<< "  "<< b << "   "<< op <<  endl;
    return tinhtoan(a, b, op);
}
int main() {
    string s = "5+12*2-5/(21-3)+(2*6-5)";
    // s = "(-22 + 1 +(2*6-5))";
    vector<string> expressionVector = stringToVector(s);
    // cout << expressionVector.size() << endl;
    int result = evaluateExpression(expressionVector);
    cout << result << endl;
    return 0;
}
