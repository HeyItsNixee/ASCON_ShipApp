#ifndef __FILE_MANIPULATION_H
#define __FILE_MANIPULATION_H

#include "curve.h"
#include "conv_model_exchange.h"
#include "assembly.h"
#include "plane_instance.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "tool_cstring.h"


bool SaveSectionToFile(const MbCurve& section, const c3d::path_string& filePath);
SPtr<MbCurve> GetSectionFromFile(const c3d::path_string& filePath);

#endif // __FILE_MANIPULATION_H