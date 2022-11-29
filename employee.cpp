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
    static bool deleteEmployeeByID(int);
    void getEmployeeByRRN(int);
    void getEmployeeByPIndex(int);
    static void getEmployeesBySIndex(string);
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
};

Employee::Employee(string Id, string dept, string name, string pos)
{
    strcpy(Employee_ID,Id.c_str());
    strcpy(Dept_ID,dept.c_str());
    strcpy(Employee_Name,name.c_str());
    strcpy(Employee_Position,pos.c_str());
};

Employee::~Employee(){};


void Employee::getEmployeeByRRN(int RRN){
    int currentRRN=1,currentRecordSize=-1;
    //reach the required record sequentially.
    fEmployee.seekg(FILE_HEADER_SIZE,ios::beg);
    while(currentRRN<RRN){
        if(fEmployee.eof()){cout<<"This RRN doesn't exist";return;}
        fEmployee.seekg(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fEmployee,4));
        fEmployee.seekg(currentRecordSize,ios::cur);
        currentRRN++;
    }

    //get the record and place it in the object.
    char d;
    d = fEmployee.get();
    if(d==DELETE_FLAG){cout<<"Record is Deleted"<<endl;return;}
    fEmployee.seekg(RECORD_HEADER_SIZE-1,ios::cur);
    readField(fEmployee,Employee_ID,FIELD_DELIMITER);
    readField(fEmployee,Dept_ID,FIELD_DELIMITER);
    readField(fEmployee,Employee_Name,FIELD_DELIMITER);
    readField(fEmployee,Employee_Position,FIELD_DELIMITER);

};

void Employee::getEmployeeByPIndex(int RRN){

    int pos = binarySearch(pIndexEmployee,RRN);
    //reach the required record using the byte offset.
    fEmployee.seekg(pos,ios::beg);


    //get the record and place it in the object.
    char d;
    d = fEmployee.get();
    if(d==DELETE_FLAG){cout<<"Record is Deleted"<<endl;return;}
    fEmployee.seekg(RECORD_HEADER_SIZE-1,ios::cur);
    readField(fEmployee,Employee_ID,FIELD_DELIMITER);
    readField(fEmployee,Dept_ID,FIELD_DELIMITER);
    readField(fEmployee,Employee_Name,FIELD_DELIMITER);
    readField(fEmployee,Employee_Position,FIELD_DELIMITER);
};

//Outputs all employees that meet the search key.
//It does not store any of them in an Employee object and simply prints them.
void Employee::getEmployeesBySIndex(string key){

    vector<int> rrns = binarySearch(sIndexEmployee,key);
    int pos;
    int size = rrns.size();
    string temp;
    char d;

    //reach the required record using the byte offset from the primary index.
    for(int i=0;i<size;i++){
        pos = binarySearch(pIndexEmployee,rrns[i]);
        fEmployee.seekg(pos,ios::beg);

        //Couts the data.
        d = fEmployee.get();
        if(d==DELETE_FLAG){cout<<"Record is Deleted"<<endl;return;}
        fEmployee.seekg(RECORD_HEADER_SIZE-1,ios::cur);

        temp = readField(fEmployee,FIELD_DELIMITER);
        cout<<"Employee ID: "<<temp<<endl;
        temp = readField(fEmployee,FIELD_DELIMITER);
        cout<<"Dept ID: "<<temp<<endl;
        temp = readField(fEmployee,FIELD_DELIMITER);
        cout<<"Employee Name: "<<temp<<endl;
        temp = readField(fEmployee,FIELD_DELIMITER);
        cout<<"Employee Position: "<<temp<<endl;
    }

};

void Employee::writeEmployee(){
    //sets up the size of the record to be inserted in the file.
    //and checks if AvailList have any deleted files
    RRN = -1;
    char c;
    int firstDeletedRecord = -1, nextDeletedRecordRNN = -1,deletedRecordSize=-1;
    int recordSize = string(Employee_ID).length()+string(Dept_ID).length()+
    string(Employee_Name).length()+string(Employee_Position).length() +4;
    fEmployee.seekg(0,ios::beg);
    firstDeletedRecord = stoi(readBytes(fEmployee,4));

    //Decide where to place new Record and any needed update to Avail List
    if(firstDeletedRecord == -1) //Empty file or no deleted records.
    {
    	fEmployee.seekp(0, ios::end);
    }
    else{
        //Reclaiming space procedures. check the available list and puts new record using first-fit technique.
        //TO DO: update to variable length navigation using index;
        int currentRRN = 1;
        int currentRecordSize;
        nextDeletedRecordRNN = firstDeletedRecord;

        //this do while continue to traverse the file using the AvailList Header/RNN values
        //until it reaches the end (nextRecordRNN is -1) or finds a deleted record that can fit the new record.
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

        //checks whether the current record the point is standing on would fit the new record or not.
        if(deletedRecordSize<recordSize){
                cout<<"No suiting deleted Record, adding record at end of file."<<endl;
                fEmployee.seekp(0, ios::end);
        } else{
            //if yes then it will update the AvailHeader at the start of the file
            //to use the Deleted record's next deleted record value.
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

    //actual writing in the file after deciding where to place the record.
    int byteOffset = fEmployee.tellg();
    fEmployee.put(ACTIVE_FLAG);
    int physicalRecordSize = to_string(recordSize).length();

    while(physicalRecordSize<RECORD_HEADER_SIZE-1){
        fEmployee.put(' ');
        physicalRecordSize++;
    }

    //If i am replacing a deleted record, i will keep the old size to not mess up navigation,
    //basically internal fragmantion.
    //TODO: handle external fragmentation if needed.
    if(deletedRecordSize<recordSize){
        //If this is a new file, then add new RRN by


        fEmployee<<recordSize;
    } else {
        fEmployee.seekp(to_string(recordSize).length(),ios::cur);
    }

    //seeing how many records the primary index holds and adding 1
    //then adding it to primary index list to get written in file later;
    //and adding secondary index entry too.
    RRN = pIndexEmployee.size() + 1;
    PrimaryIndexRecord pir;
    pir.RRN = stoi(Employee_ID);
    pir.byteOffset = byteOffset;
    pIndexEmployee.push_back(pir);

    //search existing secondary indexes if the key matches the new record, if not add a new secondary index.
    int sizeT = sIndexEmployee.size();
    int flag =0;
    for(int i=0;i<sizeT;i++){
        if(Dept_ID == sIndexEmployee[i].key){
            sIndexEmployee[i].RRNs.push_back(stoi(Employee_ID));
            flag = 1;
            break;
        }
    }
    if(flag==0){
        SecondaryIndexRecord sir;
        sir.key= Dept_ID;
        sir.RRNs.push_back(stoi(Employee_ID));
        sIndexEmployee.push_back(sir);
    }

    fEmployee<<Employee_ID<<FIELD_DELIMITER;
    fEmployee<<Dept_ID<<FIELD_DELIMITER;
    fEmployee<<Employee_Name<<FIELD_DELIMITER;
    fEmployee<<Employee_Position<<FIELD_DELIMITER;

}

bool Employee::deleteEmployeeByID(int ID){
    Employee e;
    e.getEmployeeByPIndex(ID);
    int currentRRN=1,currentRecordSize=-1,x;
    fEmployee.seekg(0,ios::beg);
    int firstDeletedRecord = stoi(readBytes(fEmployee,4));
    int currentID=-1,currentPos;

    //reach the required record sequentially to get it's RRN.
    while(currentID!=ID){
        currentPos= fEmployee.tellg();
        fEmployee.seekp(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fEmployee,4));
        currentID = stoi(readBytes(fEmployee,EMPLOYEE_ID_FIELD_SIZE));

        fEmployee.seekp(currentRecordSize-EMPLOYEE_ID_FIELD_SIZE,ios::cur);
        currentRRN++;
    }

    fEmployee.seekg(currentPos,ios::beg);
    char c;
    c = fEmployee.get();
    fEmployee.seekp(-1,ios::cur);
    if(c == DELETE_FLAG){cout<<"ALREADY DELETED"<<endl;return false;}
    //if file wasn't already deleted,we will put the delete flag and the header's RRN
    //then move back to the header to update its RRN to the newly deleted Record.

    //Adds the delete flag into the data file record.
    fEmployee<<DELETE_FLAG;
    fEmployee.seekp(RECORD_HEADER_SIZE-1,ios::cur);
    fEmployee<<firstDeletedRecord<<FIELD_DELIMITER;
    fEmployee.seekp(0,ios::beg);
    writeFileHeader(fEmployee,4,currentRRN-1);

    //Delete the record from the Primary index.
    x = pIndexEmployee.size();

    for(int i=0;i<x;i++){
        if(pIndexEmployee[i].RRN == ID){
            pIndexEmployee.erase(pIndexEmployee.begin()+i);
            break;
        }
    }

    //Delete the record from the secondary index by removing it from The vectors.
    x = sIndexEmployee.size();

    for(int i1=0;i1<x;i1++){
        if(sIndexEmployee[i1].key == e.Dept_ID){
            int y = sIndexEmployee[i1].RRNs.size();
            if(y==1){
                sIndexEmployee.erase(sIndexEmployee.begin()+i1);
            } else {
                for(int i2=0;i2<y;i2++){
                    if(sIndexEmployee[i1].RRNs[i2] == ID){
                        sIndexEmployee[i1].RRNs.erase(sIndexEmployee[i1].RRNs.begin()+i2);
                        break;
                    }
                }
            }
            break;
        }
    }

    cout<<"DELETION COMPLETE OF EMPLOYEE: "<<ID<<endl;
    return true;
};
