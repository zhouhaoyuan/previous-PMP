#include "MultipleLinearRegression.h"
#include <iostream>
#include <iomanip>

MultipleLinearRegression::MultipleLinearRegression(int numberOfRegressors)
{
	_numberOfRegressors = numberOfRegressors;
	_rows = _numberOfRegressors+1;
	_columns = _numberOfRegressors+2;


	_dataArray = new double*[_rows];
	for(int i = 0; i < _rows; i++) {
		_dataArray[i] = new double[_columns];
	}

	for(int i = 0; i < _rows; i++)
		for(int j = 0; j < _columns; j++)
			_dataArray[i][j] = 0;

	_isRefresh = false;
}

void MultipleLinearRegression::printData()
{
	for(int i = 0; i < _rows; i++)
		for(int j = 0; j < _columns; j++)
		{
			std::cout << _dataArray[i][j];
			if((j+1)==_columns)
				std::cout << std::endl;
			else
				std::cout << "\t";
		}
}

MultipleLinearRegression::~MultipleLinearRegression()
{
	delete [] _dataArray;
}

void MultipleLinearRegression::addXY(const std::vector<double>& x, const double& y) throw (std::string) 
{
	if(x.size() != _numberOfRegressors)
		throw std::string("Error: Wrong vector size in addXY function.");

	_dataArray[0][_columns-1] += y;
	for(int i = 0; i < _numberOfRegressors; ++i)
	{
		_dataArray[i+1][_columns-1] += y * x.at(i);
	}

	_dataArray[0][0] += 1;
	for(int i = 0; i < _numberOfRegressors; ++i)
	{
		_dataArray[i+1][0] += x.at(i);
	}

	for(int i = 0; i < _numberOfRegressors; ++i)
	{
		_dataArray[0][i+1] += x.at(i);
	}

	for(int i = 0; i < _numberOfRegressors; ++i)
	{
		for(int j = 0; j < _numberOfRegressors; ++j)
		{
			_dataArray[i+1][j+1] += (x.at(i) * x.at(j));
		}
	}
	_isRefresh = false;
}

double MultipleLinearRegression::getA()
{
	if(_isRefresh != true)
		Calculate();

	return A;
}

std::vector<double> MultipleLinearRegression::getB()
{
	if(_isRefresh != true)
		Calculate();
	
	std::vector<double> vectorB(B.begin(),B.end());
	return vectorB;
}

bool MultipleLinearRegression::isValid()
{
	return (_dataArray[0][0] > _numberOfRegressors);
}

void MultipleLinearRegression::Calculate()
{
	double **matrix = new double*[_rows];
	for(int i = 0; i < _rows; i++) {
		matrix[i] = new double[_columns];
	}

	for(int i = 0; i < _rows; i++)
		for(int j = 0; j < _columns; j++)
			matrix[i][j] = _dataArray[i][j];


	int N = _rows;
	int M = _columns;

	int max = 0,pr=0;
    float pom;

	for(int i=0; i<N; i++)              //nalezení pivota
	{
		max=i;
		for(int j=i+1; j<N; j++)
			if(fabs(matrix[j][i])>fabs(matrix[i][i]))
        max=j;

		for(int k=i; k<M; k++)
		{
			pom=matrix[i][k];
			matrix[i][k]=matrix[max][k];
			matrix[max][k]=pom;
		}
		for(int j=i+1; j<N; j++)
		for(int k=N; k>=i; k--)
        if(matrix[i][i]!=0)
			matrix[j][k]= matrix[j][k] - matrix[i][k] * matrix[j][i] / matrix[i][i];
	}

	B.clear();

	for(int i = _numberOfRegressors; i > -1 ; --i)
	{
		double newValue = matrix[i][_columns-1] / matrix[i][i];
		
		if(i == 0)
			A = newValue;
		else
			B.push_front(newValue);

		for(int j = i; j > 0 ;--j)
		{
			matrix[j-1][_columns-1] -= (matrix[j-1][i] * newValue);
		}
	}

	delete [] matrix;
	_isRefresh = true;
}