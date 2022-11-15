#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "utils.cpp"
#include "employee.cpp"
#include "department.cpp"

using namespace std;

/*
    Menu:
    1- Add Employee 'ae'
    2- Add Department 'ad'
    3- Delete Employee (ID) 'de'
    4- Delete Department (ID) 'dd'
    5- Print Employee(ID) 'pe1'
    6- Print Employee(Dept_ID) 'pe2'
    7- Print Department(ID) 'pd1'
    8- Print Department(name) 'pd2'
    9- Write a query 'q'
    10- Exit  'e'
*/
int main()
{
    char c;
    string s;
    int RRN;
    bool b;
    Employee e;
    Department d;
    if( !fEmployee || !fDepartment || !fPIndexEmployee || !fSIndexEmployee || !fPIndexDepartment || !fSIndexDepartment)
    {
        cout << "Couldn't open one or more files, check if they exist."<<endl;
        return 0;
    }
    setHeaderIfNew();


    do {

        cout<< "Choose the operation to perform (lower or uppercase):"<<endl;
        cout<< "A to Add Employee" <<endl;
        cout<< "B to Add Department" <<endl;
        cout<< "C to Delete Employee by ID" <<endl;
        cout<< "D to Delete Department by ID" <<endl;
        cout<< "E to Print Employee by Employee ID" <<endl;
        cout<< "F to Print Employee by Department ID" <<endl;
        cout<< "G to Print Department by Department ID" <<endl;
        cout<< "H to Print Department by Department name" <<endl;
        cout<< "I to write a Query using the \"select\" attribute \"from\" Employee OR Department \"where\" key \"=\" 'value'" <<endl;
        cout<< "J to exit" <<endl;
        cin>>c;
        c = tolower(c);
        switch(c)
        {
            case 'a':
                cin>>e;
                e.writeEmployee();
                break;
            case 'b':
                cin>>d;
                d.writeDepartment();
                break;
            case 'c':
                cout<<"Enter Employee ID to be deleted"<<endl;
                cin>>s;
                Employee::deleteEmployeeByRRN(stoi(s));
                break;
            case 'd':
                cout<<"Enter Department ID to be deleted"<<endl;
                cin>>s;
                Department::deleteDepartmentByRRN(stoi(s));
                break;
            case 'e':
                cout<<"Enter Employee ID to print the employee"<<endl;
                cin>>s;
                e.GetEmployeeByRRN(stoi(s));
                cout<<e;
                break;
            case 'f':
                cout<<"Enter Department ID to print the Employee"<<endl;
                cin>>s;
                e.GetEmployeeByRRN(stoi(s));
                cout<<e;
                break;
            case 'g':
                cout<<"Enter Department ID to print the Department"<<endl;
                cin>>s;
                d.GetDepartmentByRRN(stoi(s));
                cout<<d;
                break;
            case 'h':
                cout<<"Enter Department Name to print the Department"<<endl;
                cin>>s;
                d.GetDepartmentByRRN(stoi(s));
                cout<<d;
                break;
            case 'i':
                cout<<"Enter Query using the \"select\" attribute \"from\" Employee OR Department \"where\" key \"=\" 'value'"<<endl;
                cin>>s;

                break;
            default:
                break;
        }

    } while (c!='j');

    fEmployee.close();
    fDepartment.close();
    fPIndexEmployee.close();
    fSIndexEmployee.close();
    fPIndexDepartment.close();
    fSIndexDepartment.close();
    cout<< "Closing the program"<<endl;
    return 0;
}
