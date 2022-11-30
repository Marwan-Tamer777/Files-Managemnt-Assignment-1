

void regexMatch(string s){
    //Example select attribute from table where key = value
    regex rgx(".*select (.*) from (.*) where (.*) = ['`’\"](.*)['`’\"].*");
    smatch match;
    string attribute,table,key,value;

    Employee e;
    Department d;
    //Possible values:
    //Attribute: All Employee_ID Department_ID Department_Name
    //Table: Employee Department
    //Key: Employee_ID Department_ID Department_Name
    //Value: anything
    if (regex_search(s, match, rgx)){
        attribute = match[1];
        table = match[2];
        key = match[3];
        value = match[4];
        transform(table.begin(), table.end(), table.begin(), ::tolower);
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        transform(value.begin(), value.end(), value.begin(), ::tolower);
        //cout << "match: " << attribute<<" : "<<table<<" : "<<key<<" : "<<value << '\n';
    }else {cout<< "No matches to your string. Please make sure you inputted it correctly."<<endl;return;}


    if(key == "employee_id" && table == "employee"){
        e.getEmployeeByPIndex(stoi(value));
    } else if(key == "department_id" && table == "employee"){
        e.getFirstEmployeeBySIndex(value);
    } else if(key == "department_id" && table == "department"){
        d.getDepartmentByPIndex(stoi(value));
    } else if(key == "department_name" && table == "department"){
        d.getFirstDepartmentBySIndex(value);
    } else {
        cout<<"Bad input please check what value you inputted"<<endl;return;
    }


    if(attribute == "employee_id"){
        cout<<" The Employee ID is: "<<e.Employee_ID<<endl;
    } else if(attribute == "department_id"){
        cout<<" The Department ID is: "<<e.Dept_ID<<d.Dept_ID<<endl;
    } else if(attribute == "department_name"){
        cout<<" The Department name is: "<<d.Dept_Name<<endl;
    }
}
