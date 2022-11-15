#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "utils.cpp"
#include "employee.cpp"
#include "department.cpp"

using namespace std;

int main()
{
    char c;
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

    //cin>>e;
    //e.writeEmployee();
    //e.GetEmployeeByRRN(2);
    //Employee::deleteEmployeeByRRN(2);
    //cout<<e

    cin>>d;
    d.writeDepartment();
    //d.GetDepartmentByRRN(3);
    //Department::deleteDepartmentByRRN(2);
    cout<<d;

/*
    do {

        cout<< "Choose the operation to perform:"<<endl;
        cout<< "V for View" <<endl;
        cout<< "I for insert" <<endl;
        cout<< "D for delete" <<endl;
        cout<< "E for Exit" <<endl;
        cin>>c;

        switch(c)
        {
            case 'V':
            case 'v':
                cout<< "enter RRN"<<endl;
                cin>> RRN;
                RRN--;
                s = Student::GetStudentByRRN(RRN,fStudent);
                cout << s;
                break;
            case 'I':
            case 'i':
                cout<< "Enter student name, password, and ID" <<endl;
                cin >> s;
                RRN = s.writeStudentFixedLegnth(fStudent);
                RRN++;
                cout << "Student inserted at record Number : " << RRN <<endl;
                break;
            case 'D':
            case 'd':
                cout<< "Enter RRN"<<endl;
                cin>> RRN;
                RRN--;
                b = Student::deleteStudentByRRN(RRN,fStudent);
                if(b) cout << "sucessfully deleted"<<endl;
                else cout<< "Error, Invalid RRN or already deleted"<<endl;
                break;
            default:
                break;
        }

    } while (c!='E' && c!='e');
*/
    fEmployee.close();
    fDepartment.close();
    fPIndexEmployee.close();
    fSIndexEmployee.close();
    fPIndexDepartment.close();
    fSIndexDepartment.close();

    cout<< "Closing the program"<<endl;
    return 0;
}
