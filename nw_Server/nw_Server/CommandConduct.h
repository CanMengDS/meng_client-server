#pragma once
#include<iostream>
#include<vector>
#include<cctype>
using namespace std;

class CommandConduct {
public:
	CommandConduct();
	vector<string> queryKeyWord(string target);
private:
	char frist;
	string lv1Key[10];
	string lv2Key[10];
	string lv3Key[10];
};