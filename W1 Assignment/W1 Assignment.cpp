#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	ofstream out;
	bool on = true; //script will run while set to true

	cout << "Welcome to the project tracker-" << endl;

while (on == true)
{
	vector<string> list;
	ifstream view("List.txt");
	string line;
	while (getline(view, line)) //iterates through each line
	{
		cout << line << endl; //print out any existing lines
		list.insert(list.begin(), line); //saves it to a vector/list because the file will need to be cleared
	}
	
	string pName; //init variables 
	string base;
	string status;
	int colors = 0;

	cout << "Enter project name (Existing or new): "; //user to type project name 
	getline(cin, pName); //getline allows spaces
	
	cout << "Enter name of base figure (or 'None'): "; //user to type object name
	getline(cin, base);

	cout << "Enter details of current project status: "; //user to type project status
	getline(cin, status);

	cout << "Enter # of colors required (0 if unknown): "; //user to type project status
	cin >> colors;

	out.open("List.txt");
	out << endl; //clear file
	out.close();

	out.open("List.txt", ios::app); //append mode

	for (const auto& txt : list) //iterates through each line
	{
		if (not txt.starts_with(pName) && txt != "") //check if there is a matching project name
		{
			out << txt << endl; //append existing projects without the same name
		}
	}

	out << pName << ": Base: " << base << ", Status: " << status << ", Colors: " << colors; //append new project

	out.close();

	string next;
	cout << "Type 'yes' if you want to continue.";
	cin >> next;
	if (next != "yes")
		{
		on = false; //script will stop running
		}
	else
		{
		cout << endl;
		cin.get(); // waits for user to press enter
		}
}
	on = false;
	return 0;
}