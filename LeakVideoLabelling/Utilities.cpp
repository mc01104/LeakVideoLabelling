#include "stdafx.h"
#include "Utilities.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "dirent.h"

::std::vector<::std::string> ReadLinesFromFile(const ::std::string& pathToFile)
{
	::std::vector< ::std::string> linesVector;

	::std::ifstream inputFile(pathToFile.c_str());
	
	::std::string tempLine;
	while(::std::getline(inputFile, tempLine))
		linesVector.push_back(tempLine);

	return linesVector;
}


::std::vector< double> DoubleVectorFromString(const ::std::string& inputString)
{
	::std::istringstream ss(inputString);

	::std::vector<double> result;
	while(!ss.eof())
	{
		double tmp;
		ss >> tmp;
		result.push_back(tmp);
	}

	return result;
}

::std::vector< double> DoubleVectorFromString(const ::std::string& inputString, char delim)
{
	::std::vector<::std::string> strs = splitString(inputString, delim);
	
	::std::vector<double> result;

	for (int i = 0; i < strs.size(); ++i)
		result.push_back(atof(strs[i].c_str()));

	return result;
}

std::vector<std::string> splitString(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    splitString(s, delim, std::back_inserter(elems));
    return elems;
}

::std::vector<::std::string> splitString(const ::std::string& inputStr)
{
	::std::istringstream ss(inputStr);

	::std::vector<::std::string> result;
	while(!ss.eof())
	{
		::std::string tmp;
		ss >> tmp;
		result.push_back(tmp);
	}
	result.pop_back(); // HACKY
	return result;
}

double Norm2(const ::std::vector< double>& doubleVector)
{
	double tmp = 0;

	for (::std::vector<double>::const_iterator it = doubleVector.begin(); it != doubleVector.end(); ++ it)
		tmp += ::std::pow(*it, 2);

	return ::std::sqrt(tmp);
}


double NormSquared(const ::std::vector<double>& input)
{
	double sum = 0;
	for(::std::vector<double>::const_iterator it = input.begin(); it != input.end(); ++it)
		sum += ::std::pow(*it, 2);

	return sum;
}

::std::string GetDateString()
{
  time_t rawtime;
  struct tm * timeinfo;

  char buffer [80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime (buffer,80,"%Y-%m-%d-%H-%M-%S", timeinfo);

  return ::std::string(buffer);
}


void PrintCArray(const double* toPrint, size_t size, ::std::ostream& os)
{
	for (size_t i = 0; i < size; ++i)
		os << toPrint[i] << " ";

	os << ::std::endl;

}

double angularDistanceMinusPItoPI(const double angle1, const double angle2)
{
	return ::std::atan2(::std::sin(angle1 - angle2), ::std::cos(angle1 - angle2));	
}

bool is_not_digit(char c)
{
    return !std::isdigit(c);
}

bool numeric_string_compare(const std::string& s1, const std::string& s2)
{
    // handle empty strings...

	const std::string s1_ = s1.substr(0, s1.find_last_of("."));
	const std::string s2_ = s2.substr(0, s2.find_last_of("."));

    std::string::const_iterator it1 = s1_.begin(), it2 = s2_.begin();

    if (std::isdigit(s1_[0]) && std::isdigit(s2_[0])) {

		double n1 = 0;
		std::istringstream ss(s1_);
		ss >> n1;

		double n2 = 0;
		std::istringstream ss2(s2_);
		ss2 >> n2;

        if (n1 != n2) return n1 < n2;

        it1 = std::find_if(s1_.begin(), s1_.end(), is_not_digit);
        it2 = std::find_if(s2_.begin(), s2_.end(), is_not_digit);
    }

    return std::lexicographical_compare(it1, s1_.end(), it2, s2_.end());
}


::std::map<::std::string, double>  createMapFromKeyValuePairs(const ::std::string& msgToParse)
{
	::std::vector<::std::string> strVector = splitString(msgToParse);

	::std::map<::std::string, double> result;
	for (int i = 0; i < strVector.size(); ++i)
		result[strVector[i].c_str()] = (double) atof(strVector[++i].c_str());

	return result;
}


::std::vector<double> linspace(double a, double b, int n) 
{
    ::std::vector<double> array;
    double step = (b-a) / (n-1);

    while(a <= b) {
        array.push_back(a);
        a += step;           // could recode to better handle rounding errors
    }
    return array;
}

::std::vector<double> linspace2(double a, double b, int n) 
{
    ::std::vector<double> array;
    double epsilon = 0.0001;
    double step = (b-a) / (n-1);
    if (a==b)
    {
        for (int i = 0; i < n; i++)
        {
            array.push_back(a);
        }
    }
    else if (step >= 0)
    {
        while(a <= b + epsilon)
        {
            array.push_back(a);
            a += step;           
        }       
    }
    else
    {
        while(a + epsilon >= b )
        {
            array.push_back(a);
            a += step;           
        }       
    }
    return array;
}	

int getImList(std::vector<std::string>& imList, std::string path)
{	
	int count = 0;
    // Read files in the input folder
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			std::string file(ent->d_name);

			std::size_t found = file.find(".png");
			if (found!=std::string::npos) 
			{
				imList.push_back(file);
				count++;
			}
		}
		closedir (dir);
	} else {
		return count;
	}
	return count;
}

::std::string checkPath(::std::string path)
{
    const ::std::string goodPathSeparator =
    #ifdef LINUX
            "/";
    #else
            "\\";
    #endif

    const ::std::string wrongPathSeparator =
    #ifdef LINUX
            "\\";
    #else
            "/";
    #endif

    replaceInStr(path, wrongPathSeparator, goodPathSeparator);

    return path;
}


void replaceInStr( ::std::string& source, const ::std::string& from, const ::std::string& to )
{
    ::std::string newString;
    newString.reserve( source.length() );
   ::std:: string::size_type lastPos = 0;
    ::std::string::size_type findPos;

    while( ::std::string::npos != ( findPos = source.find( from, lastPos )))
    {
        newString.append( source, lastPos, findPos - lastPos );
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr( lastPos );

    source.swap( newString );
}