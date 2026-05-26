#include <iostream>
using namespace std;

int main()
{
	string objName = "nothing"; //init variables as generic values
	double len = 0;
	double wid = 0;
	double hei = 0;

	cout << "What type of object would you like to 3D print?: "; //user to type object name
	cin >> objName;
	cout << "Enter length, width, then height (in mm): "; //user to type 3 numbers
	cin >> len >> wid >> hei;

	int time = (len * wid * hei) / 250; //very basic time calcuation based on the volume

	cout << "It will take " << time << " minutes to print your " << objName << endl;

	return 0;
}