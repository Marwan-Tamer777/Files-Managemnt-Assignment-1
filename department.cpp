#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

class Department
{
private:
    char Dept_ID  [30];
    char Dept_Name [50];
    char Dept_Manger [50];
    int RRN;


public:
    Department();
    Department( string , string , string);
    void setRRN(int);
    static bool deleteDepartmentByRRN(int);
    void GetDepartmentByRRN(int);
    void readDepartment();
    void writeDepartment();
    friend istream& operator >> (istream &str, Department &d){
        string ss;
        cout<<"Enter Department's ID, Name, Manager separated  by \"enter\""<<endl;
        str>>ss;
        strcpy(d.Dept_ID, ss.c_str());
        str>>ss;
        strcpy(d.Dept_Name, ss.c_str());
        str>>ss;
        strcpy(d.Dept_Manger, ss.c_str());
        return str;
    };
    friend ostream& operator << (ostream & str, Department & d){
        str<<"Department ID: "<<d.Dept_ID<<endl;
        str<<"Department' Name: "<<d.Dept_Name<<endl;
        str<<"Department's Manager: "<<d.Dept_Manger<<endl;
        return str;
    };
    ~Department();
};


Department::Department()
{
    string emptyStr = "";
    strcpy(Dept_ID,emptyStr.c_str());
    strcpy(Dept_Name,emptyStr.c_str());
    strcpy(Dept_Manger,emptyStr.c_str());
}
Department::Department(string Id, string name, string manger)
{
    strcpy(Dept_ID,Id.c_str());
    strcpy(Dept_Name,name.c_str());
    strcpy(Dept_Manger,manger.c_str());
}

Department::~Department(){};

void Department::GetDepartmentByRRN(int RRN){
    //TODO: create a getDepartment by byteOffset to use indexes later.
    int currentRRN=1,currentRecordSize=-1;
    //reach the required record sequentially
    fDepartment.seekg(0,ios::beg);
    fDepartment.seekp(0,ios::beg);
    fDepartment.seekg(FILE_HEADER_SIZE,ios::beg);
    while(currentRRN<RRN){
        if(fDepartment.eof()){cout<<"This RRN doesn't exist";return;}
        fDepartment.seekg(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fDepartment,4));
        fDepartment.seekg(currentRecordSize,ios::cur);
        currentRRN++;
    }

    char d;
    d = fDepartment.get();
    if(d==DELETE_FLAG){cout<<"Record is Deleted"<<endl;return;}
    fDepartment.seekg(RECORD_HEADER_SIZE-1,ios::cur);
    readField(fDepartment,Dept_ID,FIELD_DELIMITER);
    readField(fDepartment,Dept_Name,FIELD_DELIMITER);
    readField(fDepartment,Dept_Manger,FIELD_DELIMITER);

};

void Department::writeDepartment(){
    RRN = -1;
    char c;
    int firstDeletedRecord = -1, nextDeletedRecordRNN = -1,deletedRecordSize=0;
    int recordSize = string(Dept_ID).length()+string(Dept_Name).length()+
    string(Dept_Manger).length()+3;
    fDepartment.seekg(0,ios::beg);
    firstDeletedRecord = stoi(readBytes(fDepartment,4));

    //Decide where to place new Record and any update to Avail List
    if(firstDeletedRecord == -1) //Empty file or no deleted records.
    {
    	fDepartment.seekp(0, ios::end);
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
                fDepartment.seekp(1,ios::cur);//skip the Active or Deleted flag
                currentRecordSize = stoi(readBytes(fDepartment,4));
                fDepartment.seekp(currentRecordSize,ios::cur);
                currentRRN++;
            }
            fDepartment.seekp(1,ios::cur);
            deletedRecordSize = stoi(readBytes(fDepartment,4));
            readField(fDepartment,nextDeletedRecord,FIELD_DELIMITER);
            nextDeletedRecordRNN = stoi(nextDeletedRecord);
            if(nextDeletedRecordRNN==-1){
                break;
            }
        } while(deletedRecordSize<recordSize);

        if(deletedRecordSize<recordSize){
                cout<<"No suiting deleted Record, adding record at end of file."<<endl;
                fDepartment.seekp(0, ios::end);
        } else{
            writeFileHeader(fDepartment,4,nextDeletedRecordRNN);
            currentRRN = 1;
            while(currentRRN<RRN){
                fDepartment.seekp(1,ios::cur);//skip the Active or Deleted flag
                currentRecordSize = stoi(readBytes(fDepartment,4));
                fDepartment.seekp(currentRecordSize,ios::cur);
                currentRRN++;
            }
        }

    }

    //actual writing in the file.
    fDepartment.put(ACTIVE_FLAG);
    int physicalRecordSize = to_string(recordSize).length();

    while(physicalRecordSize<RECORD_HEADER_SIZE-1){
        fDepartment.put(' ');
        physicalRecordSize++;
    }
    fDepartment<<recordSize;
    fDepartment<<Dept_ID<<FIELD_DELIMITER;
    fDepartment<<Dept_Name<<FIELD_DELIMITER;
    fDepartment<<Dept_Manger<<FIELD_DELIMITER;

};

bool Department::deleteDepartmentByRRN(int RRN){
    //TODO: create a getDepartment by byteOffset to use indexes later.
    int currentRRN=1,currentRecordSize=-1;
    fDepartment.seekg(0,ios::beg);
    int firstDeletedRecord = stoi(readBytes(fDepartment,4));


    //reach the required record sequentially.
    while(currentRRN<RRN){
        fDepartment.seekp(1,ios::cur);//skip the Active or Deleted flag
        currentRecordSize = stoi(readBytes(fDepartment,4));
        fDepartment.seekp(currentRecordSize,ios::cur);
        currentRRN++;
    }
    char c;
    c = fDepartment.get();
    fDepartment.seekp(-1,ios::cur);
    if(c == DELETE_FLAG){cout<<"ALREADY DELETED"<<endl;return false;}
    //if file wasn't already deleted,we will put the delete flag and the header's RRN
    //then move back to the header to update its RRN to the newly deleted Record.

    fDepartment<<DELETE_FLAG;
    fDepartment.seekp(RECORD_HEADER_SIZE-1,ios::cur);
    fDepartment<<firstDeletedRecord<<FIELD_DELIMITER;
    fDepartment.seekp(0,ios::beg);
    writeFileHeader(fDepartment,4,RRN);
    return true;
};

