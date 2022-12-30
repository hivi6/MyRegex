#if !defined(MY_CPP_REGEX_H)
#define MY_CPP_REGEX_H

#define TODO(s) cout << "TODO: " << __PRETTY_FUNCTION__ << ": " << (s) << endl

#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <set>

using namespace std;

class MyRegex
{
    struct State
    {
        // transition states
        // using only 2 outputs for now
        // character -> output state
        // character 0 is for empty character
        pair<char, int> out[2];
    };

    // states of a given NFA
    vector<State> states;

    // start and end state of the NFA
    int startState;
    int endState;

    // regular expression pattern
    string currentPattern;

private:
    // debug related

    int GRAMMAR_DEBUG = 1;

private:
    /**
     * @brief
     * Generate a new state for the NFA
     *
     * @param out transition vector(element = {character, output state})
     * @return int generated state number
     */
    int createState(pair<char, int> out1 = {0, -1}, pair<char, int> out2 = {0, -1})
    {
        states.push_back(State{out1, out2});
        return states.size() - 1;
    }

    /**
     * @brief Set the State object
     *
     * @param state state number
     * @param outNo output number
     * @param out output transition{character, output state}
     */
    void setState(int state, int outNo, pair<char, int> out)
    {
        states[state].out[outNo] = out;
    }

    /**
     * @brief
     * Check if the index i
     * is out of bound
     *
     * @param pattern regular expression pattern
     * @param index
     * @return bool
     */
    bool isEnd(const string &pattern, int index)
    {
        return index >= pattern.size();
    }

    /**
     * @brief
     * Character At the given index
     *
     * @param pattern regular expression pattern
     * @param index index of the pattern
     * @return char character at the given index, returns 0 is isEnd(index) is true
     */
    char charAt(const string &pattern, int index)
    {
        if (index >= pattern.size())
            return 0;
        return pattern[index];
    }

    /**
     * @brief
     *
     * rule:
     *      regex := list
     *
     * check for starting and ending words
     *
     * @param pattern regular expression pattern
     * @param index current index in the pattern
     * @param tabs for debuging purposes
     * @return pair<int, int>
     */
    pair<int, int> _regexRule(const string &pattern, int &index, string tabs = "")
    {
        // DEBUG: related to printing
        if (GRAMMAR_DEBUG == 1)
        {
            cout << tabs << "_regexRule(" << pattern << ", " << index << ");\n";
        }

        return _listRule(pattern, index, tabs + " ");
    }

    /**
     * @brief
     *
     * rule:
     *      list := element ["|" list]
     *
     * checks for 'or' operator
     *
     * @param pattern regular expression pattern
     * @param index current index in the pattern
     * @param tabs for debuging purposes
     * @return pair<int, int> starting and ending states
     */
    pair<int, int> _listRule(const string &pattern, int &index, string tabs = "")
    {
        // DEBUG: related to printing
        if (GRAMMAR_DEBUG == 1)
        {
            cout << tabs << "_listRule(" << pattern << ", " << index << ");\n";
        }

        // first parse the left-hand side, in this case element rule
        // and get the start and end states
        auto left = _elementRule(pattern, index, tabs + " ");

        // now check for the or operator
        if (charAt(pattern, index) == '|')
        {
            index++; // |

            // now get the right hand side
            auto right = _listRule(pattern, index, tabs + " ");

            // now add the or functionality between left and right
            auto start = createState({0, left.first}, {0, right.first});
            auto end = createState();
            setState(left.second, 0, {0, end});
            setState(right.second, 0, {0, end});
            left = {start, end};
        }

        return left;
    }

    /**
     * @brief
     * rule:
     *      element := ("(" list ")" | value) ("*"|"+"|"?") [element]
     *
     * ("(" list ")" | value)   := this is related to grouping(check if group or just value)
     * ("*"|"+"|"?")            := this is related to special characters
     * [element]                := this is related to extending the element further
     *
     * special characters:
     *      `*` := Zero or more characters
     *      `+` := One or more characters
     *      `?` := Zero or one character
     *
     * @param pattern regular expression pattern
     * @param index current index in the pattern
     * @param tabs for debuging purposes
     * @return pair<int, int> starting and ending states
     */
    pair<int, int> _elementRule(const string &pattern, int &index, string tabs = "")
    {
        // DEBUG: related to printing
        if (GRAMMAR_DEBUG == 1)
        {
            cout << tabs << "_elementRule(" << pattern << ", " << index << ");\n";
        }

        pair<int, int> left;
        // check if grouping
        if (charAt(pattern, index) == '(')
        {
            index++; // (

            // now get the list
            left = _listRule(pattern, index, tabs + " ");

            // check if the group is closed
            if (charAt(pattern, index) != ')')
                throw "Error: Expected ')' at the end of group.";

            index++; // )
        }
        else
            left = _valueRule(pattern, index, tabs + " ");

        // check for special characters
        if (charAt(pattern, index) == '*' || // zero or more
            charAt(pattern, index) == '+' || // one or more
            charAt(pattern, index) == '?')   // zero or one
        {
            // DEBUG: related to printing
            if (GRAMMAR_DEBUG == 1)
            {
                cout << tabs << "special character: " << charAt(pattern, index) << "\n";
            }

            // get the special character
            auto special = charAt(pattern, index);
            index++; // special character

            // create state accordingly
            switch (special)
            {
            case '*': // zero of more characters
            {
                auto start = createState({0, left.first});
                auto end = createState();
                setState(left.second, 0, {0, start});
                setState(start, 1, {0, end});
                left = {start, end};
                break;
            }
            case '+': // one or more character
            {
                auto end = createState();
                setState(left.second, 0, {0, left.first});
                setState(left.second, 1, {0, end});
                left = {left.first, end};
                break;
            }
            case '?': // zero or one character
            {
                auto start = createState({0, left.first});
                auto end = createState();
                setState(start, 1, {0, end});
                setState(left.second, 0, {0, end});
                left = {start, end};
                break;
            }
            }
        }

        if (isEnd(pattern, index) ||
            charAt(pattern, index) == '|')
            return left;

        // extend element rule
        auto right = _elementRule(pattern, index, tabs + " ");
        setState(left.second, 1, {0, right.first});
        return {left.first, right.second};
    }

    /**
     * @brief
     *
     * rule:
     *      value := [0-9A-Za-z!~`!@#%&_-=:;\"\'\\\r\t\n] |
     *               "[" (c ["-" c])+ "]"
     *
     * [0-9A-Za-z!~`!@#%&_-=:;\"\'\\\r\t\n] := single character value
     * "[" (c ["-" c])+ "]"                 := range character value
     *
     * @param pattern regular expression pattern
     * @param index current index in the pattern
     * @param tabs for debuging purposes
     * @return pair<int, int> starting and ending states
     */
    pair<int, int> _valueRule(const string &pattern, int &index, string tabs = "")
    {
        // DEBUG: related to printing
        if (GRAMMAR_DEBUG == 1)
        {
            cout << tabs << "_valueRule(" << pattern << ", " << index << ");\n";
        }

        // check if range value
        if (charAt(pattern, index) == '[')
        {
            index++; // [

            // create the new pattern
            // convert range value to list
            // if '[a-z]' then convert to 'a|b|c|d|....|z'
            string newPattern;
            while (index < pattern.size() && pattern[index] != ']')
            {
                // check '-' and then a character that is not ']'
                if (index + 1 < pattern.size() && pattern[index + 1] == '-' && index + 2 < pattern.size() && pattern[index + 2] != ']')
                {
                    for (auto x = pattern[index]; x <= pattern[index + 2]; x++)
                        newPattern += x + string("|");
                    index += 3;
                }
                else
                    newPattern += pattern[index++] + string("|");
            }
            if (newPattern.size())
                newPattern.pop_back();

            // now get the states using this converted pattern
            int newIndex = 0;
            // don't print the debug from here
            auto prevDebug = GRAMMAR_DEBUG;
            GRAMMAR_DEBUG = 0;
            auto left = _listRule(newPattern, newIndex); 
            GRAMMAR_DEBUG = prevDebug;

            if (charAt(pattern, index) != ']')
                throw "Error: Expected ']' at the end of range values.";
            index++; // ]
            return left;
        }

        // check if not value
        if (!isalpha(charAt(pattern, index)) &&
            !isdigit(charAt(pattern, index)) &&
            string("!~`!@#%&_-=:;\"\'\\\r\t\n").find(charAt(pattern, index)) == string::npos)
            throw "Error: Expected a value type.";

        // create a value type state
        // DEBUG: related to printing
        if (GRAMMAR_DEBUG == 1)
        {
            cout << tabs << "value: " << charAt(pattern, index) << endl;
        }

        auto end = createState();
        auto start = createState({charAt(pattern, index), end});
        index++;
        return {start, end};
    }

    // TODO: Clean up
    bool _match(set<int> &visited, vector<int> &resStates, const char &ch, int curState)
    {
        if (curState == -1)
            return false;
        if (visited.count(curState))
            return false;
        visited.insert(curState);
        bool res = false;
        for (int i = 0; i < 2; i++)
        {
            if (states[curState].out[i].first == ch)
            {
                res = true;
                resStates.push_back(states[curState].out[i].second);
            }
            if (states[curState].out[i].first == 0)
                res |= _match(visited, resStates, ch, states[curState].out[i].second);
        }
        visited.erase(curState);
        return res;
    }

public:
    /**
     * @brief
     * Constructs a Regex object which is nothing but a NFA,
     * from a given regular expression pattern
     *
     * @param pattern
     */
    MyRegex(const string &pattern)
    {
        // compile the regular expression from the given pattern
        compile(pattern);
    }

    // utility functions
    /**
     * @brief
     * Compile the NFA state machine from the regular expression pattern
     *
     * @param pattern a regular expression
     */
    void compile(const string &p)
    {
        // first clean the last pattern states
        clear();

        // generate the states according to the regular expression patter
        // using the grammar(check grammar file)
        int index = 0;
        auto s = _regexRule(p, index);
        startState = s.first;
        endState = s.second;

        // set the pattern
        currentPattern = p;
    }

    bool match(const string &str)
    {
        if (empty())
            return false;
        vector<int> v1 = {startState};
        vector<int> v2;
        set<int> visited;
        for (auto ch : str)
        {
            bool res = false;
            for (auto v : v1)
                res |= _match(visited, v2, ch, v);
            if (!res)
                return false;
            v1.swap(v2);
            v2.clear();
            visited.clear();
        }
        while (v1.size())
        {
            auto t = v1.back();
            visited.insert(t);
            v1.pop_back();
            for (int i = 0; i < 2; i++)
            {
                if (states[t].out[i].first == 0 &&
                    states[t].out[i].second != -1 &&
                    !visited.count(states[t].out[i].second))
                {
                    v1.push_back(states[t].out[i].second);
                }
            }
        }
        return visited.count(endState);
    }

    void printStates() const
    {
        cout << "startState: " << startState << endl;
        cout << "endState: " << endState << endl;
        for (int i = 0; i < states.size(); i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (states[i].out[j].second != -1)
                    cout << i << "\t--'" << states[i].out[j].first << "'->\t" << states[i].out[j].second << endl;
            }
        }
    }

    /**
     * @brief 
     * clear the states of the regex
     * 
     */
    void clear()
    {
        currentPattern = "";
        states.clear();
        startState = endState = -1;
    }

    bool empty() const
    {
        return states.empty();
    }
};

#endif // MY_CPP_REGEX_H
