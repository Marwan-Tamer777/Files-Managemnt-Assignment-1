#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

const char DELETE_FLAG = '*';
const char HEADER_PADDING_FLAG = ' ';
const char FIELD_DELIMITER = '|';
const char ACTIVE_FLAG = ' ';
const int EMPLOYEE_RECORD_SIZE = 144;
const int DEPARTMENT_RECORD_SIZE = 130;
const int FILE_HEADER_SIZE = sizeof(int);
const int RECORD_HEADER_SIZE = sizeof(int)+1;

//File Structure Examples.

// header 4 bits + 1 bit free for delete/Active flag + 4 bit for record size + fields and delimiters
//if deleted the first field will be overwritten to the next RRN in the Avali List + field deleimiter
//applies to both employee and department files
//   2   23123|213|Ahmed|Engineer|*  28-1||213|ALI|Engineer|
fstream fEmployee("employee.txt", ios::in | ios::out);
fstream fDepartment("depatment.txt", ios::in | ios::out);

//Primary and Secondary index files for both employee and department.
fstream fPIndexEmployee("pindexemployee.txt", ios::in | ios::out);
fstream fSIndexEmployee("sindexemployee.txt", ios::in | ios::out);
fstream fPIndexDepartment("pindexdepartment.txt", ios::in | ios::out);
fstream fSIndexDepartment("sindexdepartment.txt", ios::in | ios::out);

void readField(fstream &f,char *key,char delimeter){
    char temp = ' ';
    string value= "";
    temp = f.get();
    while(temp!=delimeter){
        value.push_back(temp);
        temp = f.get();
    }
    strcpy(key,value.c_str());
}

string readBytes(fstream &f,int byteCount){
    char temp = ' ';
    string value= "";
    for(int i=0;i<byteCount;i++){
        temp = f.get();
        value.push_back(temp);
    }
    return value;
}
/*
void seekByRRN(fstream &f,int RRN){
    //TODO: create a method to navigate file by byteOffset using indexes later.
    int currentRRN=1,currentRecordSize=-1;
    //reach the required record sequentially
    f.seekg(FILE_HEADER_SIZE,ios::beg);
    while(currentRRN<RRN){
        f.seekg(1,ios::cur);//skip the Active or Deleted flag
        f.seekp(1,ios::cur);
        string s = readBytes(fEmployee,4);
        currentRecordSize = stoi(s);
        f.seekg(currentRecordSize,ios::cur);
        f.seekp(currentRecordSize,ios::cur);
        currentRRN++;
    }
}*/

void writeFileHeader(fstream& f,int hSize,int header){
    f.seekp(0,ios::beg);
    int actualHeaderSize = to_string(header).length();

    while(actualHeaderSize<hSize){
        f.put(' ');
        actualHeaderSize++;
    }

    f<<header;
}

void setHeaderIfNew(){

    fEmployee.seekp(0,ios::end);
    int header = -1;
    if(fEmployee.tellp() <= 0)
    {
        writeFileHeader(fEmployee,FILE_HEADER_SIZE,header);
    }

    fDepartment.seekp(0,ios::end);
    if(fDepartment.tellp()<= 0)
    {
        writeFileHeader(fDepartment,FILE_HEADER_SIZE,header);
    }
};
