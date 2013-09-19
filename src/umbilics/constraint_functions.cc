#include "constraint_functions.hh"

double (*constraint_function[7])(double,double,double,double,double,double) =
	{ &fx, &fy1, &fy2, &fy3, &fz1, &fz2, &fz3 };

