// Derificator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Constraints.h"
using namespace std;
using namespace GCS;


double Constraint::DoSelfTest(int &iworst_param)
{
	//Purpose: tests if partial derivatives in grad() compare well to numeric ones. 
	//Arguments:
	//  iworst_param: a return value. The index in pvec of the partial drivvative that failed most tests.
	//Return value: the fraction of tests the worst derivative has failed. (anything above say 0.05 should be considered a problem).
	//Usage: just construct the constraint object and call this func. It is not necessary to set geometry's coordinates to point to real stuff.

	const int NUM_TESTS_PER_PARAM = 100;

	
	//save parameters pointers to restore them after the test
	std::vector<double*> savedParams(pvec);
	
	//make new pvec using new array
	std::vector<double> tmpParamValues(pvec.size());
	for(int iparam =0   ;   iparam < pvec.size()  ;   iparam++){
		pvec[iparam]=&(tmpParamValues[iparam]);
	};

	//prepare pass-fail counters
	std::vector<int> cntPass(pvec.size(),0);
	std::vector<int> cntFail(pvec.size(),0);

	//lets do it!
	for(int itest=0;itest<NUM_TESTS_PER_PARAM;itest++){
		//fill parameters with random data
		for(int iparam =0;iparam<pvec.size();iparam++){
			double r=static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
			*pvec[iparam] = r*10.0 - 5.0;//hard-coding here.. i'm lazy! range -5..5
		};

		//for each parameter, get analytic derivative, calculate numeric derivative and compare them
		for(int iparam =0;iparam<pvec.size();iparam++){
			double* param=pvec[iparam];

			//get analytic derivative
			double driv_analytic = grad(param);

			//calculate two numerical derivatives: to the left and to the right
			const double eps=0.00001;//hard-coding again ;)
			double el,er,ez;//values of error function to the left, to the right, and spot on
			double oldParamVal=*param;
			ez=error();
			*param=oldParamVal+eps;
			er=error();
			*param=oldParamVal-eps;
			el=error();
			*param=oldParamVal;//not needed actually...
			double numderivLeft=(ez-el)/eps;
			double numderivRight=(er-ez)/eps;
			double tol=abs(numderivLeft-numderivRight)+0.0001;//a tolerance estimate.

			if(    abs((numderivLeft+numderivRight)*0.5 - driv_analytic)    <=    tol    ) {cntPass[iparam]++;} else {cntFail[iparam]++;};
		};

	};
	//tests done!

	//restore parameter values
	pvec=savedParams;


	//process results
	double worst_val=0.0;
	iworst_param=0;
	for(int iparam =0   ;   iparam < pvec.size()  ;   iparam++){
		std::cout << "param" << iparam << "    pass count=" << cntPass[iparam] << ";  fail count=" << cntFail[iparam] << "\n";
		double paramFailsFrac =  (double)cntFail[iparam] / (double)(cntPass[iparam]+cntFail[iparam]);
		if( paramFailsFrac > worst_val ) {
			worst_val=paramFailsFrac;
			iworst_param=iparam;
		};
	}
	return worst_val;

}

#define GENERATE_CONSTRAINT_TEST_HERE(constraint_classname, constructor_arguments) { \
	constraint_classname cstr constructor_arguments; \
	testresult=cstr.DoSelfTest(i); \
	cout << #constraint_classname << " derivative test results: worst result by parameter "<<i<<" failing " << testresult*100.0 << "% of tests.\n"; \
	};

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "test!";
	int i;
	double testresult;

	Line li; Ellipse el; Point po; Arc ar; Circle ci; ArcOfEllipse ae; //a bunch of useless geometrical objects to make constraints' constructors happy =)
	double foo=0.2; double* dp=&foo;
	po.x=dp;
	po.y=dp;
	li.p1=po;
	li.p2=po;
	/*macro generated snippets like this:
	ConstraintEllipseTangentLine cstr(l,el);
	testresult=cstr.DoSelfTest(i);
	cout << "ConstraintEllipseTangentLine derivative test results: worst result by parameter "<<i<<" failing " << testresult*100.0 << "% of tests.\n";
	*/

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintEqual,(dp,dp));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintDifference,(dp,dp,dp));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintP2PDistance,(po,po,dp));

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintP2PAngle,(po,po,dp));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintP2LDistance,(po,li,dp));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintPointOnLine,(po,li));

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintPointOnPerpBisector,(po,li));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintParallel,(li,li));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintPerpendicular,(li,li));

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintL2LAngle,(li,li,dp));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintMidpointOnLine,(li,li));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintTangentCircumf,(po,po,dp,dp));

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintPointOnEllipse,(po,el));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintEllipseTangentLine,(li,el));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintInternalAlignmentPoint2Ellipse,(el,po,EllipsePositiveMajorX));

	GENERATE_CONSTRAINT_TEST_HERE(ConstraintEqualMajorAxesEllipse,(el,el));
	GENERATE_CONSTRAINT_TEST_HERE(ConstraintEllipticalArcRangeToEndPoints,(po,ae,dp));

	cin.get();
	return 0;
}

