#include "CommandConduct.h"

CommandConduct::CommandConduct()
{
    frist = '//';
    memset(&lv1Key, 0, sizeof(lv1Key));
    memset(&lv2Key, 0, sizeof(lv2Key));
    memset(&lv3Key, 0, sizeof(lv3Key));
    lv1Key[0] = "toClient";
    lv2Key[0] = "image";
}

vector<string> CommandConduct::queryKeyWord(string target)
{
    vector<string> temp;
    const char frist_char = target[0];
    if (strcmp(&frist_char, &frist) != 0) {
        temp.push_back("null");
        return temp;
    }
    int ind1_num = 0, ind2_num = 0;
    string lv1_word, lv2_word, lv3_word;
    ind1_num = target.find("_") + 1;
    ind2_num = target.find("*") - ind1_num;
    lv1_word = target.substr(0, ind1_num);
    lv2_word = target.substr(ind1_num, ind2_num);
    lv3_word = target.substr(ind2_num, target.length() - ind2_num);
    temp.push_back(lv1_word);
    temp.push_back(lv2_word);
    temp.push_back(lv3_word);
    return temp;
}

bool CommandConduct::runCommand(vector<string>& commands)
{
    if (commands.size() != 3) {
        return false;
    }
    vector<string>::const_iterator it = commands.begin();
    string command_lv1 = *it;
    ++it;
    string command_lv2 = *it;
    ++it;
    string command_lv3 = *it;
    if (!command_lv1.compare("toClient")) {
        return false;
    }
    if (!command_lv2.compare("image")) {
        return false;
    }
    
}
