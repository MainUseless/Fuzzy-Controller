#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <stack>

using namespace std;

typedef string fuzzyVariableName;
typedef string fuzzySetName;

vector<string> variableType = {"IN", "OUT"};
vector<string> fuzzySetType = {"TRI", "TRAP"};
vector<string> operatorType = {"AND", "AND_NOT", "OR", "OR_NOT"};

struct fuzzySet
{
    string name;
    string type;
    vector<int> points;
};

struct variableSetValue
{
    string fuzzySetName;
    string fuzzyVariableName;
    float value;
};

struct linguisticVariable
{
    string type;
    string name;
    pair<int, int> range;
    unordered_map<fuzzySetName, fuzzySet> fuzzySets;
};

struct fuzzyRule
{
    //(var,set) AND (var,set) OR
    vector<string> rule;
    pair<fuzzyVariableName, fuzzySetName> outputVariable;
};

void checkInputType(string &input, const vector<string> &valid, const string &type)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);

    for (const auto &x : valid)
        if (x == input)
        {
            return;
        }
    cout << "WRONG " << type << " INPUT TRY AGAIN OR EXIT!!!!!\n";
    input = "-1";
};

bool checkPoints(const linguisticVariable &variable, const vector<int> &points)
{
    if (!is_sorted(points.begin(), points.end()))
    {
        cout << "THE POINTS ARE INVALID (WRONG ORDER)!!!!!\n";
        return false;
    }

    for (const auto &x : points)
    {
        if (x > variable.range.second || x < variable.range.first)
        {
            cout << "THE POINTS ARE OUT OF RANGE!!!!!\n";
            return false;
        }
    }

    return true;
};

bool checkBeforeRun(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables, const vector<fuzzyRule> &fuzzyRules)
{
    if (fuzzyRules.size() < 1)
    {
        cout << "CAN’T START THE SIMULATION! Please add the rules first.\n";
        return false;
    }

    if (fuzzyVariables.size() < 2)
    {
        cout << "CAN’T START THE SIMULATION! Please add atleast 2 variables first.\n";
        return false;
    }

    vector<int> inputTypeFreq(variableType.size(), 0);

    for (const auto &x : fuzzyVariables)
    {
        auto it = find(variableType.begin(), variableType.end(), x.second.type);
        ++inputTypeFreq[it - variableType.begin()];
    }

    for (int i = 0; i < inputTypeFreq.size(); ++i)
    {
        if (inputTypeFreq[i] == 0)
        {
            cout << "CAN’T START THE SIMULATION! Please add " << variableType[i] << " variables first.\n";
            return false;
        }
    }

    return true;
};

void addVariables(unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    cout << "Enter the variable’s name, type (IN/OUT) and range ([lower, upper]):\n(Press x to finish)\n----------------------------------------------------------\n";

    string input;

    while (true)
    {
        cin >> input;
        if (input == "x")
            break;

        linguisticVariable temp;
        temp.name = input;
        cin >> input;

        checkInputType(input, variableType, "variableType");
        if (input == "-1")
            continue;

        temp.type = input;

        cin >> input;
        input.pop_back();
        input.erase(0, 1);

        temp.range.first = stoi(input);

        cin >> input;
        input.pop_back();

        temp.range.second = stoi(input);

        if (temp.range.second < temp.range.first)
        {
            cout << "UPPER MUST BE GREATER THAN LOWER\n";
            continue;
        }

        // if(temp.range.second>100 || temp.range.second<0){
        //     cout<<"UPPER MUST NOT BE GREATER THAN 100 OR LESS THAN 0 \n";
        //     continue;
        // }

        // if(temp.range.first>100 || temp.range.first<0){
        //     cout<<"LOWER MUST NOT BE GREATER THAN 100 OR LESS THAN 0 \n";
        //     continue;
        // }

        fuzzyVariables[temp.name] = temp;
    }
}

void addFuzzySet(unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    string fuzzyVariableName;
    cout << "Enter the variable’s name:\n------------------------\n";
    cin >> fuzzyVariableName;

    if (fuzzyVariableName == "x")
        return;

    auto tempLinguistic = fuzzyVariables.find(fuzzyVariableName);

    if (tempLinguistic == fuzzyVariables.end())
    {
        cout << "VARIABLE DOESN'T EXIST !!!!!!\n";
        return;
    }

    string input;

    cout << "Enter the fuzzy set name, type (TRI/TRAP) and values: (Press x to finish)\n--------------------------------------------------\n";

    while (true)
    {

        cin >> input;

        if (input == "x")
            break;

        fuzzySet fuzzySet;

        // name
        fuzzySet.name = input;

        // type
        cin >> input;
        checkInputType(input, fuzzySetType, "fuzzySet type");
        if (input == "-1")
            continue;

        fuzzySet.type = input;

        // values
        int size = (fuzzySet.type == "TRAP") ? 4 : 3;
        vector<int> tempPoints;
        for (int i = 0; i < size; ++i)
        {
            int value;
            cin >> value;
            tempPoints.emplace_back(value);
        }
        if (!checkPoints(tempLinguistic->second, tempPoints))
            continue;

        fuzzySet.points = tempPoints;

        tempLinguistic->second.fuzzySets[fuzzySet.name] = fuzzySet;
    }
}

int getOperationRank(string operation)
{
    auto it = find(operatorType.begin(), operatorType.end(), operation);
    return it - operatorType.begin();
}

bool isOperator(string operation)
{
    return (find(operatorType.begin(), operatorType.end(), operation) - operatorType.begin()) < operatorType.size();
}

void convertToPostfix(vector<fuzzyRule> &fuzzyRules)
{

    for (int r = 0; r < fuzzyRules.size(); r++)
    {

        stack<string> operations;
        vector<string> ordered_rule;

        for (int i = 0; i < fuzzyRules[r].rule.size(); ++i)
        {
            if (!isOperator(fuzzyRules[r].rule[i]))
            {
                ordered_rule.emplace_back(fuzzyRules[r].rule[i]);
            }
            else
            {
                if (!operations.empty())
                {
                    if (getOperationRank(operations.top()) < getOperationRank(fuzzyRules[r].rule[i]))
                    {
                        ordered_rule.emplace_back(operations.top());
                        operations.pop();
                    }
                }
                operations.push(fuzzyRules[r].rule[i]);
            }
        }

        while (!operations.empty())
        {
            ordered_rule.emplace_back(operations.top());
            operations.pop();
        }

        fuzzyRules[r].rule = ordered_rule;
    }
}

void addRules(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables, vector<fuzzyRule> &fuzzyRules)
{
    cout << "Enter the rules in this format: (Press x to finish)\nIN_variable set operator IN_variable set => OUT_variable set\n-------------------------------------------------------\n";
    cin.ignore();

    string input;
    while (true)
    {

        getline(cin, input);

        if (input == "x")
        {
            convertToPostfix(fuzzyRules);
            return;
        }

        stringstream ss(input);

        string fuzzyVariableName;
        string fuzzySetName;
        string tempOperator;
        vector<string> tempRule;
        fuzzyRule tempFuzzyRule;
        bool isValid = true;
        bool isInput = true;
        int index = 0;

        while (true)
        {
            ss >> fuzzyVariableName;
            auto tempLinguistic = fuzzyVariables.find(fuzzyVariableName);
            if (tempLinguistic == fuzzyVariables.end())
            {
                cout << fuzzyVariableName << " VARIABLE DOESN'T EXIST !!!!!!\n";
                isValid = false;
                break;
            }

            ss >> fuzzySetName;
            auto tempSet = tempLinguistic->second.fuzzySets.find(fuzzySetName);
            if (tempSet == tempLinguistic->second.fuzzySets.end())
            {
                cout << fuzzySetName << " SET DOESN'T EXIST !!!!!!\n";
                isValid = false;
                break;
            }

            if (isInput)
            {
                tempRule.emplace_back(fuzzyVariableName + "," + fuzzySetName);

                ss >> tempOperator;

                if (tempOperator == "=>")
                {
                    isInput = false;
                }
                else
                {
                    checkInputType(tempOperator, operatorType, "operator");
                    if (tempOperator == "-1")
                    {
                        cout << tempOperator << " IS NOT A REAL OPERATOR !!!!!!\n";
                        isValid = false;
                        break;
                    }
                    tempRule.emplace_back(tempOperator);
                }
            }
            else
            {
                tempFuzzyRule.outputVariable = make_pair(fuzzyVariableName, fuzzySetName);
                break;
            }
        }

        tempFuzzyRule.rule = (tempRule);

        if (isValid)
            fuzzyRules.emplace_back(tempFuzzyRule);
    }
}

void showVariables(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    if (fuzzyVariables.empty())
        cout << "Sorry, There is no fuzzy variables yet.\n";

    else
    {
        for (const auto &it : fuzzyVariables)
        {
            cout << "variable name: " << it.second.name
                 << "\nvariable range: [" << it.second.range.first << ", " << it.second.range.second
                 << "]\nvariable type: " << it.second.type
                 << "\nfuzzySets: {";
            for (const auto &fuzzySetEntry : it.second.fuzzySets)
            {
                cout << "\n\tset name: " << fuzzySetEntry.second.name
                     << "\n\tset type: " << fuzzySetEntry.second.type
                     << "\n\tset points: {";
                for (const auto &point : fuzzySetEntry.second.points)
                {
                    cout << point << ", ";
                }
                cout << "}\n";
            }
            cout << "}\n===================================\n";
        }
    }
}

void showFuzzyRules(const vector<fuzzyRule> &fuzzyRules)
{
    if (fuzzyRules.empty())
        cout << "Sorry, There are no fuzzy rules yet.\n";

    for (const auto &rule : fuzzyRules)
    {

        stack<string> stack;

        for (const auto &ruleString : rule.rule)
        {
            if (isOperator(ruleString))
            {
                string a1 = stack.top();
                stack.pop();
                string a2 = stack.top();
                stack.pop();

                stack.push(a2 + " " + ruleString + " " + a1);
            }
            else
            {
                int commaIndex = ruleString.find(",");
                string fuzzyVariableName = ruleString.substr(0, commaIndex);
                string fuzzySetName = ruleString.substr(commaIndex + 1, ruleString.size());

                stack.push(fuzzyVariableName + " " + fuzzySetName);
            }
        }

        cout << stack.top() << " => " << rule.outputVariable.first << " " << rule.outputVariable.second << endl;
    }
    cout << "=========================================================\n";
}

void init(bool &isOn, string &systemName, string &description)
{
    char choice;

    cout << "Fuzzy Logic Toolbox\n-----------------------\n1- Create a new fuzzy system\n2- Quit\n";

    do
    {
        cin >> choice;

        if (choice == '2')
        {
            cout << "CY@";
            isOn = false;
            return;
        }
        else if (choice != '1')
        {
            cout << "INVALID!!!!\n";
            continue;
        }
        else
        {
            isOn = true;
            break;
        }

    } while (true);

    cout << "Enter the system’s name and a brief description(one line):\n";
    cin >> systemName;
    cin.ignore();

    getline(cin, description);
}

float valueOnTheLine(float y2, float y1, float x2, float x1, float val)
{
    // y=mx+b        (1)if i have slope and y intersection
    // y-y1=m(x-x1)  (2)if i have  slope and point
    // y = m(x-x1)+y1
    //  slope = y2-y1/x1-x2
    float slope = (y2 - y1) / (x2 - x1);
    return (slope * (val - x1) + y1);
}

unordered_map<fuzzyVariableName, vector<variableSetValue>> getAllOutputCentroids(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    unordered_map<fuzzyVariableName, vector<variableSetValue>> output;

    for (const auto &fuzzyVariable : fuzzyVariables)
    {

        if (fuzzyVariable.second.type == "IN")
            continue;

        vector<variableSetValue> currCentroids;

        for (const auto &fuzzySet : fuzzyVariable.second.fuzzySets)
        {
            float sum = 0;

            for (int i = 0; i < fuzzySet.second.points.size(); ++i)
            {
                sum += fuzzySet.second.points[i];
            }

            variableSetValue temp;
            temp.fuzzySetName = fuzzySet.first;
            temp.fuzzyVariableName = fuzzyVariable.second.name;
            temp.value = sum / fuzzySet.second.points.size();

            currCentroids.emplace_back(temp);
        }

        output[fuzzyVariable.second.name] = (currCentroids);
    }

    return output;
}

vector<variableSetValue> getMemberships(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables, const unordered_map<fuzzyVariableName, float> &crispValues)
{
    vector<variableSetValue> memberships;

    for (auto crispValue : crispValues)
    {
        auto fuzzyVariable = fuzzyVariables.find(crispValue.first);

        for (auto fuzzySet : fuzzyVariable->second.fuzzySets)
        {

            bool isIntersect = false;

            for (int i = 0; i < fuzzySet.second.points.size() - 1; ++i)
            {

                float y1 = (i == 0) ? 0 : 1;
                float y2 = (i + 1 == fuzzySet.second.points.size() - 1) ? 0 : 1;
                float x1 = fuzzySet.second.points[i];
                float x2 = fuzzySet.second.points[i + 1];

                if (crispValue.second >= fuzzySet.second.points[i] && crispValue.second <= fuzzySet.second.points[i + 1])
                {
                    variableSetValue temp;
                    temp.fuzzySetName = fuzzySet.second.name;
                    temp.fuzzyVariableName = crispValue.first;
                    temp.value = valueOnTheLine(y1, y2, x1, x2, crispValue.second);

                    memberships.emplace_back(temp);
                    isIntersect = true;
                    break;
                }
            }
            if (!isIntersect)
            {
                variableSetValue temp;
                temp.fuzzySetName = fuzzySet.second.name;
                temp.fuzzyVariableName = crispValue.first;
                temp.value = 0;

                memberships.emplace_back(temp);
            }
        }
    }

    return memberships;
}

float applyOperation(float operand1, float operand2, string Operator)
{

    if (Operator.find("NOT") < Operator.size())
    {
        operand1 = 1 - operand1;
    }
    if (Operator.find("OR") < Operator.size())
    {
        return max(operand1, operand2);
    }
    else if (Operator.find("AND") < Operator.size())
    {
        return min(operand1, operand2);
    }

    return -1.0;
}

float findFuzzyVariableAndSetValue(const pair<fuzzyVariableName, fuzzySetName> &input, const vector<variableSetValue> &memberships)
{
    int i = 0;

    for (; i < memberships.size(); ++i)
    {
        if (memberships[i].fuzzyVariableName == input.first && memberships[i].fuzzySetName == input.second)
            return memberships[i].value;
    }

    return -1;
}

// Remeber:
// you have crisp value for each variable (proj_funding: 50)
// for each crisp value solve the equation (y=mx+b => y=1/20x-2)
// to find the value on its set (y= 0.5hi)
// look at each rule and evaluate it on your fuzzified value (0.5 or 0.4 = 0.5)
// done.
vector<variableSetValue> evaluateRules(const vector<fuzzyRule> &fuzzyRules, const vector<variableSetValue> &memberships)
{

    vector<variableSetValue> outputMemberships;

    for (const auto &rule : fuzzyRules)
    {

        stack<float> stack;

        for (const auto &ruleString : rule.rule)
        {
            if (isOperator(ruleString))
            {
                float a1 = stack.top();
                stack.pop();
                float a2 = stack.top();
                stack.pop();

                stack.push(applyOperation(a1, a2, ruleString));
            }
            else
            {
                int commaIndex = ruleString.find(",");
                string fuzzyVariableName = ruleString.substr(0, commaIndex);
                string fuzzySetName = ruleString.substr(commaIndex + 1, ruleString.size());

                stack.push(findFuzzyVariableAndSetValue(make_pair(fuzzyVariableName, fuzzySetName), memberships));
            }
        }

        variableSetValue temp;
        temp.value = stack.top();
        temp.fuzzySetName = rule.outputVariable.second;
        temp.fuzzyVariableName = rule.outputVariable.first;

        outputMemberships.emplace_back(temp);
    }

    return outputMemberships;
}

unordered_map<fuzzyVariableName, float> getCrispInput(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    cout << "Enter the crisp values:\n---------------------------\n";

    unordered_map<fuzzyVariableName, float> crispInputs;

    float crispValue;
    for (const auto &fuzzyVariable : fuzzyVariables)
    {

        if (fuzzyVariable.second.type == "OUT")
            continue;

        do
        {
            cout << fuzzyVariable.first << ": ";
            cin >> crispValue;

            if (crispValue < fuzzyVariable.second.range.first || crispValue > fuzzyVariable.second.range.second)
            {
                cout << "INVALID ENTER A VALUE IN THE RANGE [" << fuzzyVariable.second.range.first << "," << fuzzyVariable.second.range.second << "]\n";
            }
            else
            {
                break;
            }
        } while (true);

        crispInputs[fuzzyVariable.first] = crispValue;
    }

    return crispInputs;
}

variableSetValue defuzzification(const vector<variableSetValue> &outputMemberships, const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables)
{
    auto outputCentroids = getAllOutputCentroids(fuzzyVariables);

    float denominator = 0;
    float numerator = 0;
    for (const auto &outputMembership : outputMemberships)
    {
        denominator += outputMembership.value;

        auto centroids = outputCentroids[outputMembership.fuzzyVariableName];

        float centroidValue;

        for (const auto &centroid : centroids)
        {
            if (centroid.fuzzySetName != outputMembership.fuzzySetName)
                continue;

            centroidValue = centroid.value;
            break;
        }

        numerator += (outputMembership.value * centroidValue);
    }

    float crispOutputValue = numerator / denominator;
    fuzzyVariableName outputVariableName = outputMemberships[0].fuzzyVariableName;

    unordered_map<fuzzyVariableName, float> crispOutputValues;
    crispOutputValues[outputVariableName] = crispOutputValue;

    auto finalMemberships = getMemberships(fuzzyVariables, crispOutputValues);

    variableSetValue output;
    output.value = 0;
    for (const auto &outputMembership : finalMemberships)
    {
        if (output.value <= outputMembership.value)
        {
            output.fuzzySetName = outputMembership.fuzzySetName;
            output.fuzzyVariableName = outputMembership.fuzzyVariableName;
            output.value = outputMembership.value;
        }
    }

    output.value = crispOutputValue;

    return output;
}

void runSimulation(const unordered_map<fuzzyVariableName, linguisticVariable> &fuzzyVariables, const vector<fuzzyRule> &fuzzyRules)
{
    // gets input
    auto crispValues = getCrispInput(fuzzyVariables);
    cout << "\nRunning the simulation…\n";
    auto inputMemberships = getMemberships(fuzzyVariables, crispValues);
    cout << "Fuzzification => done\n";
    auto outputMemberships = evaluateRules(fuzzyRules, inputMemberships);
    cout << "Inference => done\n";
    auto output = defuzzification(outputMemberships, fuzzyVariables);
    cout << "Defuzzification => done\n\nThe predicted " << output.fuzzyVariableName << " is " << output.fuzzySetName << " (" << output.value << ").\n\n";
}

int main()
{

    ios_base::sync_with_stdio(false);

    do
    {
        bool isOn = true;
        string systemName, description;

        init(isOn, systemName, description);

        if (!isOn)
            return 0;

        unordered_map<fuzzyVariableName, linguisticVariable> fuzzyVariables;
        vector<fuzzyRule> fuzzyRules;

        while (isOn)
        {
            char choice;

            cout << "Main Menu:\n==========\n1- Add variables.\n2- Add fuzzy sets to an existing variable.\n3- Add rules.\n4- Run the simulation on crisp values.\n5- Show fuzzy Variables.\n6- Show fuzzy rules.\n7- Close\n:";
            cin >> choice;

            switch (choice)
            {
            case '1':
                addVariables(fuzzyVariables);
                break;
            case '2':
                addFuzzySet(fuzzyVariables);
                break;
            case '3':
                addRules(fuzzyVariables, fuzzyRules);
                break;
            case '4':
                runSimulation(fuzzyVariables, fuzzyRules);
                break;
            case '5':
                showVariables(fuzzyVariables);
                break;
            case '6':
                showFuzzyRules(fuzzyRules);
                break;
            case '7':
                isOn = false;
                break;
            default:
                cout << "Invalid option TRY AGAIN!\n";
                break;
            }
        }
    } while (true);
}