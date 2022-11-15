#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

class Employee
{
private:
    char Employee_ID  [13];
    char Dept_ID [30];
    char Employee_Name [50];
    char Employee_Position [50];
    int RRN;


public:
    Employee();
    Employee( string , string , string, string );
    void setRRN(int);
    static bool deleteEmployeeByRRN(int);
    void GetEmployeeByRRN(int);
    void writeEmployee();
    friend istream& operator >> (istream &str,Employee &e){
        cout<<"Enter Employee's ID, Department ID, Name, Position separated  by \"enter\""<<endl;
        string ss;
        str>>e.Employee_ID;
        str>>e.Dept_ID;
        str>>e.Employee_Name;
        str>>e.Employee_Position;
        return str;
    };
    friend ostream& operator << (ostream &str, Employee &e){
        str<<"Employee ID: "<<e.Employee_ID<<endl;
        str<<"Dept ID: "<<e.Dept_ID<<endl;
        str<<"Employee Name: "<<e.Employee_Name<<endl;
        str<<"Employee Position: "<<e.Employee_Position<<endl;
        return str;
    };
    ~Employee();
};

Employee::Employee()
{
    string emptyStr = "";
    strcpy(Employee_ID,emptyStr.c_str());
    strcpy(Dept_ID,emptyStr.c_str());
    strcpy(Employee_Name,emptyStr.c_str());
    strcpy(Employee_Position,emptyStr.c_str());
}
Employee::Employee(string Id, string dept, string name, string pos)
{
    strcpy(Employee_ID,Id.c_str());
    strcpy(Dept_ID,dept.c_str());
    strcpy(Employee_Name,name.c_str());
    strcpy(Employee_Position,pos.c_str());
}

Employee::~Employee(){};

void Employee::GetEmployeeByRRN(int RRN){
    //TODO: create a getEmployee by byteOffset to use indexes later.
    int currentRRN=1,currentRecordSize=-1;
    //reach the required record sequntally
    fEmployee.seekg(0,ios::beg);
    fEmployee.seekp(0,ios::beg);
    fEmployee.seekg(FILE_HEADER_SIZE,ios::beg);
    while(currentRRN<RRN){
        if(fEmployee.eof()){cout<<"This RRN doesn't exist";return;}
        fEmployee.seekg(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fEmployee,4));
        fEmployee.seekg(currentRecordSize,ios::cur);
        currentRRN++;
    }

    char d;
    d = fEmployee.get();
    if(d==DELETE_FLAG){cout<<"Record is Deleted"<<endl;return;}
    fEmployee.seekg(RECORD_HEADER_SIZE-1,ios::cur);
    readField(fEmployee,Employee_ID,FIELD_DELIMITER);
    readField(fEmployee,Dept_ID,FIELD_DELIMITER);
    readField(fEmployee,Employee_Name,FIELD_DELIMITER);
    readField(fEmployee,Employee_Position,FIELD_DELIMITER);

};

void Employee::writeEmployee(){
    RRN = -1;
    char c;
    int firstDeletedRecord = -1, nextDeletedRecordRNN = -1,deletedRecordSize=0;
    int recordSize = string(Employee_ID).length()+string(Dept_ID).length()+
    string(Employee_Name).length()+string(Employee_Position).length() +4;
    fEmployee.seekg(0,ios::beg);
    firstDeletedRecord = stoi(readBytes(fEmployee,4));

    //Decide where to place new Record and any update to Avail List
    if(firstDeletedRecord == -1) //Empty file or no deleted records.
    {
    	fEmployee.seekp(0, ios::end);
    }
    else
    {
        //Reclaiming space procedures. check the available list and puts new record using first-fit technique.
        //TO DO: update to variable length navigation using index;
        int currentRRN = 1;
        int currentRecordSize;
        nextDeletedRecordRNN = firstDeletedRecord;

        do{

            currentRRN = 1;
            char nextDeletedRecord[4];
            RRN = nextDeletedRecordRNN;

            while(currentRRN<RRN){
                fEmployee.seekp(1,ios::cur);//skip the Active or Deleted flag
                currentRecordSize = stoi(readBytes(fEmployee,4));
                fEmployee.seekp(currentRecordSize,ios::cur);
                currentRRN++;
            }
            fEmployee.seekp(1,ios::cur);
            deletedRecordSize = stoi(readBytes(fEmployee,4));
            readField(fEmployee,nextDeletedRecord,FIELD_DELIMITER);
            nextDeletedRecordRNN = stoi(nextDeletedRecord);
            if(nextDeletedRecordRNN==-1){
                break;
            }
        } while(deletedRecordSize<recordSize);

        if(deletedRecordSize<recordSize){
                cout<<"No suiting deleted Record, adding record at end of file."<<endl;
                fEmployee.seekp(0, ios::end);
        } else{
            writeFileHeader(fEmployee,4,nextDeletedRecordRNN);
            currentRRN = 1;
            while(currentRRN<RRN){
                fEmployee.seekp(1,ios::cur);//skip the Active or Deleted flag
                currentRecordSize = stoi(readBytes(fEmployee,4));
                fEmployee.seekp(currentRecordSize,ios::cur);
                currentRRN++;
            }
        }

    }

    //actual writing in the file.
    fEmployee.put(ACTIVE_FLAG);
    int physicalRecordSize = to_string(recordSize).length();

    while(physicalRecordSize<RECORD_HEADER_SIZE-1){
        fEmployee.put(' ');
        physicalRecordSize++;
    }
    fEmployee<<recordSize;
    fEmployee<<Employee_ID<<FIELD_DELIMITER;
    fEmployee<<Dept_ID<<FIELD_DELIMITER;
    fEmployee<<Employee_Name<<FIELD_DELIMITER;
    fEmployee<<Employee_Position<<FIELD_DELIMITER;

}

bool Employee::deleteEmployeeByRRN(int RRN){
    //TODO: create a getEmployee by byteOffset to use indexes later.
    int currentRRN=1,currentRecordSize=-1;
    fEmployee.seekg(0,ios::beg);
    int firstDeletedRecord = stoi(readBytes(fEmployee,4));


    //reach the required record sequentially.
    while(currentRRN<RRN){
        fEmployee.seekp(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fEmployee,4));
        fEmployee.seekp(currentRecordSize,ios::cur);
        currentRRN++;
    }
    char c;
    c = fEmployee.get();
    fEmployee.seekp(-1,ios::cur);
    if(c == DELETE_FLAG){cout<<"ALREADY DELETED"<<endl;return false;}
    //if file wasn't already deleted,we will put the delete flag and the header's RRN
    //then move back to the header to update its RRN to the newly deleted Record.

    fEmployee<<DELETE_FLAG;
    fEmployee.seekp(RECORD_HEADER_SIZE-1,ios::cur);
    fEmployee<<firstDeletedRecord<<FIELD_DELIMITER;
    fEmployee.seekp(0,ios::beg);
    writeFileHeader(fEmployee,4,RRN);
    return true;
};
