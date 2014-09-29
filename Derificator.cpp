// Derificator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Constraints.h"
using namespace std;
using namespace GCS;


int _tmain(int argc, _TCHAR* argv[])
{
	cout << "test!";

	Line l; Ellipse el;
	ConstraintEllipseTangentLine cstr(l,el);

	int i;
	double testresult=cstr.DoSelfTest(i);
	cout << "ConstraintEllipseTangentLine derivative test results: worst result by parameter "<<i<<" failing " << testresult*100.0 << "% of tests.\n";

	cin.get();
	return 0;
}

