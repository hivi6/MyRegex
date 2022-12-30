#include <iostream>

#include "myregex.h"

int main()
{
    try
    {
        cout << "enter a regex pattern: ";
        string pattern;
        cin >> pattern;
        MyRegex r(pattern);
        while (cin)
        {
            cout << "enter string to match: ";
            string str;
            cin >> str;
            cout << "match(" << pattern << ", " << str << "): " << r.match(str) << endl;
        }
    }
    catch (string e)
    {
        cout << e << endl;
    }
    catch (const char* e)
    {
        cout << e << endl;
    }

    return 0;
}