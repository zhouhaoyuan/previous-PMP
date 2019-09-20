#pragma once

#include <vector>
#include <deque>
#include <string>

class MultipleLinearRegression
{
public:
	MultipleLinearRegression(int numberOfRegressors = 1);
	~MultipleLinearRegression();
	void addXY(const std::vector<double>& x, const double& y) throw (std::string);
	bool isValid();
	void printData();
	double getA();
	std::vector<double> getB();

private:
	int _numberOfRegressors;
	int _rows;
	int _columns;
	double **_dataArray;
	bool _isRefresh;

	std::deque<double> B;
	double A;

	void Calculate();
};