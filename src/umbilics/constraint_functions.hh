#pragma once

inline double fx(double a, double b, double c, double d, double e, double f)
{ return a * (b*b - c*c + d*d - f*f) + b * (c*c - a*a + e*e - d*d) + c * (a*a - b*b + f*f - e*e); }

inline double fy1(double a, double b, double c, double d, double e, double f)
{ return e * (2*(e*e - a*a) - f*f - d*d + 2*(b*a + c*a - b*c)) + d*f * (2*a - c - b); }

inline double fy2(double c, double a, double b, double f, double d, double e)
{ return e * (2*(e*e - a*a) - f*f - d*d + 2*(b*a + c*a - b*c)) + d*f * (2*a - c - b); }

inline double fy3(double b, double c, double a, double e, double f, double d)
{ return e * (2*(e*e - a*a) - f*f - d*d + 2*(b*a + c*a - b*c)) + d*f * (2*a - c - b); }

inline double fz1(double a, double b, double c, double d, double e, double f)
{ return e * (f*f - d*d) + d*f * (b - c); }

inline double fz2(double c, double a, double b, double f, double d, double e)
{ return e * (f*f - d*d) + d*f * (b - c); }

inline double fz3(double b, double c, double a, double e, double f, double d)
{ return e * (f*f - d*d) + d*f * (b - c); }

extern double (*constraint_function[7])(double,double,double,double,double,double);

