#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
	ofstream out;
	
	out.open("List.txt", ios::app); //append mode

	string pName; //init variables 
	string base;
	string status;
	int colors = 0;

	cout << "Welcome to the project tracker-" << endl;
	
	cout << "Enter project name: "; //user to type project name (TODO: check for project with existing name)
	getline(cin, pName); //getline allows spaces
	
	cout << "Enter name of base figure (or 'None'): "; //user to type object name
	getline(cin, base);
	
	cout << "Enter details of current project status: "; //user to type project status
	getline(cin, status);
	
	cout << "Enter # of colors required (0 if unknown): "; //user to type project status
	cin >> colors;

	out << "Project: " << pName << ", Base: " << base << ", Status: " << status << ", Colors: " << colors << endl; //appending to file

	out.close();

	return 0;
}