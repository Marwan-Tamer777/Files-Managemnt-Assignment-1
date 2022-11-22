#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

struct PrimaryIndexRecord {
  int RRN;
  int byteOffset;
};

const char DELETE_FLAG = '*';
const char HEADER_PADDING_FLAG = ' ';
const char FIELD_DELIMITER = '|';
const char ACTIVE_FLAG = ' ';
const int EMPLOYEE_RECORD_SIZE = 144;
const int DEPARTMENT_RECORD_SIZE = 130;
const int FILE_HEADER_SIZE = sizeof(int);//each file starts with an Avail vector Header.
const int RECORD_HEADER_SIZE = sizeof(int)+1;//each record starts with a fixed length header consisting of Active/Delete flag and size header
const int INDEX_RECOD_SIZE = 8;

//linked vector to store index records for sorting and binary searches.
//they are read in the inititalise function, modified during data file change operation
//and written back in file when use choose to close the program.
vector<PrimaryIndexRecord> pIndexEmployee,pIndexDepartment;


//File Structure Examples.

// header 4 bits + 1 bit free for delete/Active flag + 4 bit for record size + fields and delimiters
//if deleted the first field will be overwritten to the next RRN in the Avali vector + field deleimiter
//applies to both employee and department files
//   2   23123|213|Ahmed|Engineer|*  28-1||213|ALI|Engineer|
fstream fEmployee("employee.txt", ios::in | ios::out);
fstream fDepartment("depatment.txt", ios::in | ios::out);

//Primary and Secondary index files for both employee and department.
fstream fPIndexEmployee("pindexemployee.txt", ios::in | ios::out);
fstream fSIndexEmployee("sindexemployee.txt", ios::in | ios::out);
fstream fPIndexDepartment("pindexdepartment.txt", ios::in | ios::out);
fstream fSIndexDepartment("sindexdepartment.txt", ios::in | ios::out);

//this function can be used to read any field from a file given they are delimiter seperated.
//pass the file,char array and delimiter to it and it will places the read value in the char array.
//example
//file1: ahmed|Doctor|
//file1.seekg(6,ios::begin);
//char job[20];
//readField(file1,job,'|');
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

//this function is used to read a fixed size of bytes and returns the result as a string.
//usually used to read Headers of file or records.
//example
//file1: ahmed|Doctor|  23Ali|
//file1.seekg(13,ios::begin);
//string s =readBytes(file1,4);
//s will contain "  23";
string readBytes(fstream &f,int byteCount){
    char temp = ' ';
    string value= "";
    for(int i=0;i<byteCount;i++){
        temp = f.get();
        value.push_back(temp);
    }
    return value;
}

/* TODO: possibly obsolete helping function once we add indexes.
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

//This function is used when you want to overWrite/Write a header for a whole file.
//example:
//file1 should have an avali vector header of size 8 letters and value of 234.
//so we can call writeFileHeader(file1,8,234);
void writeFileHeader(fstream& f,int hSize,int header){
    f.seekp(0,ios::beg);
    int actualHeaderSize = to_string(header).length();

    while(actualHeaderSize<hSize){
        f.put(' ');
        actualHeaderSize++;
    }

    f<<header;
}

//This function is used when you want to write a relative sized value in a fixed length field.
void writeFixedField(fstream& f,int fSize,string s){
    int actualSize = s.length();

    while(actualSize<fSize){
        f.put(' ');
        actualSize++;
    }

    f<<s;
}

int getFileSize(fstream& f){
    int current,full;
    current = f.tellg();
    f.seekg(0,ios::end);
    full = f.tellg();
    f.seekg(current,ios::beg);
    return full;
}

//this function initialise data file and indexes arrays
void initialise(){

    int temp1;
    int temp2;

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

    fPIndexEmployee.seekg(0,ios::end);
    temp1 = fPIndexEmployee.tellg();
    fPIndexEmployee.seekg(0,ios::beg);
    if(temp1 != 0){
        temp2 =0;
        while(temp2!=temp1){
            PrimaryIndexRecord pir;
            pir.RRN = stoi(readBytes(fPIndexEmployee,4));
            pir.byteOffset = stoi(readBytes(fPIndexEmployee,4));
            pIndexEmployee.push_back(pir);
            temp2+= INDEX_RECOD_SIZE;
        }
    }

    fPIndexDepartment.seekg(0,ios::end);
    temp1 = fPIndexDepartment.tellg();
    fPIndexDepartment.seekg(0,ios::beg);
    if(temp1 != 0){
        temp2 =0;
        while(temp2!=temp1){
            PrimaryIndexRecord pir;
            pir.RRN = stoi(readBytes(fPIndexDepartment,4));
            pir.byteOffset = stoi(readBytes(fPIndexDepartment,4));
            pIndexDepartment.push_back(pir);
            temp2+= INDEX_RECOD_SIZE;
        }
    }

};


//Parameter function to sort the indexes vector.
bool indexSorterAscending(PrimaryIndexRecord const& lpir, PrimaryIndexRecord const& rpir) {
    return lpir.byteOffset < rpir.byteOffset;
};

//this functions closes the files, sorts the indexes and
// write them into files.
void closeFiles(){
    int x;

    //sort the vector for primary employee indexes using byteoffset/ and the sort function above
    //and clears the datafile using trunc and write the sorted data into the file
    fPIndexEmployee.close();
    fPIndexEmployee.open("pindexemployee.txt",ios::trunc | ios::out);
    sort(pIndexEmployee.begin(), pIndexEmployee.end(), &indexSorterAscending);
    fPIndexEmployee.clear();
    x = pIndexEmployee.size();
    for (int i=0;i<x; i++){
        writeFixedField(fPIndexEmployee,INDEX_RECOD_SIZE/2,to_string(pIndexEmployee[i].RRN));
        writeFixedField(fPIndexEmployee,INDEX_RECOD_SIZE/2,to_string(pIndexEmployee[i].byteOffset));
    }

    fPIndexDepartment.close();
    fPIndexDepartment.open("pindexdepartment.txt",ios::trunc | ios::out);
    fPIndexDepartment.clear();
    x = pIndexDepartment.size();
    for (int i=0;i<x; i++){
        writeFixedField(fPIndexDepartment,INDEX_RECOD_SIZE/2,to_string(pIndexDepartment[i].RRN));
        writeFixedField(fPIndexDepartment,INDEX_RECOD_SIZE/2,to_string(pIndexDepartment[i].byteOffset));
    }

    fEmployee.close();
    fDepartment.close();
    fPIndexEmployee.close();
    fSIndexEmployee.close();
    fPIndexDepartment.close();
    fSIndexDepartment.close();
};

//testing function
void showvector(vector<PrimaryIndexRecord> g)
{
    int x = g.size();
    for (int i=0;i<x; i++){
        cout << '\t' << g[i].RRN<<"  :  "<< g[i].byteOffset;
    }
    cout << '\n';
};
