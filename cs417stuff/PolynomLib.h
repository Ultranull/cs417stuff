#pragma once

#ifndef POLYNIMLIB
#define POLYNIMLIB

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <functional>


using namespace std;

struct term {
	double e;
	double c;
	term(double c, double e);
	term();
	double eval(double x);
	bool operator==(term &t);
};
ostream& operator<<(ostream &out, term t);

struct polynomial {
	vector<term> function;
	polynomial(vector<term>f);
	polynomial(int n);
	polynomial();
	double operator()(double x);
	double highestOrder();
	void sortFunction();
	int size();
	term& operator[](int i);
};
ostream& operator<<(ostream &out, polynomial f);
polynomial ddx(polynomial f);
polynomial sdx(polynomial f);

polynomial randomPolynom(default_random_engine &gen, double minc, double maxc, double mine, double maxe, int n, bool rounded);
double NewtonsMethod(polynomial f, double guess, int max, double epsilon); 
double NewtonsMethod(ofstream &out, polynomial f, double guess, int max, double epsilon);
double Bisection(polynomial f, double xL, double xR, int max = 100);
double Bisection(const std::function <double(double)>& f, double xL, double xR, int max,ofstream &out);

#ifdef POLY_IMPL
term::term(double c, double e) :e(e), c(c) {}
term::term() : term(0, 0) {}

double term::eval(double x) {
	return c * pow(x, e);
}

bool term::operator==(term &t) {
	return t.e == e && t.c&&c;
}

ostream& operator<<(ostream &out, term t) {
	if (t.c == 0.)
		return out;
	out << " ";
	if (t.c < 0.)
		out << "-";
	if (t.c != 1.)
		out << abs(t.c);
	if (t.e != 0.)
		out << "x";
	if (t.e != 1.&&t.e != 0.)
		out << "^";
	if (t.e > 1.)
		out << t.e;
	else if (t.e < 0.)
		out << "(" << t.e << ")";

	return out;
}


polynomial::polynomial(vector<term>f) :function(f) {}
polynomial::polynomial(int n) {
	function = vector<term>(n, term());
}
polynomial::polynomial() {}

double polynomial::operator()(double x) {
	double sum = 0;
	for (int i = 0; i < function.size(); i++)
		sum += function[i].eval(x);
	return sum;
}
double polynomial::highestOrder() {
	double max = 0;
	for (int i = 0; i < function.size(); i++)
		if (function[i].e > max)
			max = function[i].e;
	return max;
}

void polynomial::sortFunction() {
	sort(function.begin(), function.end(), [](term a, term b) {
		return a.e > b.e;
	});
}

int polynomial::size() {
	return function.size();
}

term& polynomial::operator[](int i) {
	return function[i];
}

ostream& operator<<(ostream &out, polynomial f) {
	for (int i = 0; i < f.size(); i++) {
		if (f[i].c == 0.)
			continue;
		out << " ";
		if (f[i].c > 0. && i > 0)
			out << "+ ";
		if (f[i].c < 0.) {
			out << "-";
			if (i > 0) out << " ";
		}
		if(abs(f[i].c)!=1 || (f[i].e ==0&& abs(f[i].c) == 1)) // !eo && eo && ez
			out << abs(f[i].c);
		if (f[i].e != 0.)
			out << "x";
		if (f[i].e != 1.&&f[i].e != 0.)
			out << "^";
		if (f[i].e > 1.)
			out << f[i].e;
		else if (f[i].e < 0.)
			out << "(" << f[i].e << ")";
	}
	//out << " = 0";
	return out;
}

polynomial ddx(polynomial f) {
	vector<term> fp;
	for (int i = 0; i < f.size(); i++)
		fp.push_back(term(f[i].c*f[i].e, f[i].e - 1));
	return polynomial(fp);
}
polynomial sdx(polynomial f) {
	vector<term> fp;
	for (int i = 0; i < f.size(); i++)
		fp.push_back(term(f[i].c / (f[i].e + 1), f[i].e + 1));
	return polynomial(fp);
}

polynomial randomPolynom(default_random_engine &gen, double minc, double maxc, double mine, double maxe, int n = 4, bool rounded = false) {
	uniform_real_distribution<double> distc(minc, maxc);
	uniform_real_distribution<double> diste(mine, maxe);
	polynomial out(n);
	for (int i = 0; i < n; i++) {
		term t;
		if (rounded)
			t = term(round(distc(gen)), round(diste(gen)));
		else
			t = term(distc(gen), round(diste(gen)));

		bool found = false;
		for (int c = 0; c <= i; c++)
			if (out[c].e == t.e) {
				out[c].c += t.c;
				found = true;
			}
		if (!found)
			out[i] = t;
	}
	return out;
}

double NewtonsMethod(polynomial f, double guess = 1., int max = 100, double epsilon = -15) {
	double x0 = guess, xnew = 0, error = abs(f(x0)), tol = pow(10, epsilon);
	int ct = 0;
	polynomial dfdx = ddx(f);
	while (error > tol&&ct < max) {
		xnew = x0 - (f(x0) / dfdx(x0));
		error = abs(f(xnew));
		x0 = xnew;
		ct++;
	}
	return xnew;
}

double NewtonsMethod(ofstream &out,polynomial f, double guess = 1., int max = 100, double epsilon = -7) {
	double x0 = guess, xnew = 0, error = abs(f(x0)), tol = pow(10, epsilon);
	int ct = 0;
	polynomial dfdx = ddx(f);
	out << "derivative: " << dfdx <<" = 0"<< endl;
	while (error > tol&&ct < max) {
		xnew = x0 - (f(x0) / dfdx(x0));
		error = abs(f(xnew));
		out << ct << " x0: " << x0 << " xnew: " << xnew <<" abs(f(xnew))= "<<error<<endl;
		x0 = xnew;
		ct++;
	}
	return xnew;
}

double Bisection(polynomial f, double xL, double xR, int max) {

	double xMid;
	double fL, fR, fMid;

	xMid = (xL + xR) / 2.0;
	int ct = 1;
	double error = abs(f(xMid));
	double tol = pow(10, -5);
	while (error > tol && ct<=max) {
		fL = f(xL);
		fR = f(xR);
		fMid = f(xMid);
		error = abs(fMid);
		if (error > tol) {
			if ((fL > 0) && (fR > 0) && (fMid > 0)) {
				if (fR > fL) { xR = xMid; }
				else { xL = xMid; }
			}

			else if (fMid > 0) {
				if (fL < 0) { xR = xMid; }
				else if (fR < 0) { xL = xMid; }
			}
			else if (fMid < 0)
			{
				if (fL > 0) { xR = xMid; }
				else if (fR > 0) { xL = xMid; }
			}

			xMid = (xR + xL) / 2.0;
		}
		ct++;

	}
	return xMid;

}
double Bisection(const std::function <double(double)>& f, double xL, double xR, int max, ofstream &out) {

	double xMid;
	double fL, fR, fMid;

	xMid = (xL + xR) / 2.0;
	int ct = 0;
	double error = abs(f(xMid));
	double tol = pow(10, -15);
	while (error > tol && ct <= max) {
		fL = f(xL);
		fR = f(xR);
		fMid = f(xMid);
		error = abs(fMid);
		if (error > tol) {
			if ((fL > 0) && (fR > 0) && (fMid > 0)) {
				if (fR > fL) { xR = xMid; }
				else { xL = xMid; }
			}

			else if (fMid > 0) {
				if (fL < 0) { xR = xMid; }
				else if (fR < 0) { xL = xMid; }
			}
			else if (fMid < 0)
			{
				if (fL > 0) { xR = xMid; }
				else if (fR > 0) { xL = xMid; }
			}

			xMid = (xR + xL) / 2.0;
		}
		out << ct << " left: " << xL << " right: " << xR << " middle: " << xMid<<" f of middle: "<<fMid<< endl;
		ct++;

	}
	return xMid;

}

#endif // POLY_IMPL
#endif