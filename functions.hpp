#ifndef FUNCTIONS_H_GUARD
#define FUNCTIONS_H_GUARD

#include "TFile.h"
#include "TChain.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace fun {

bool replace(std::string& str, const std::string& from, const std::string& to);

}

#endif