#pragma once

#include <glew.h>
#include <wglew.h>
#include <freeglut.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "src/math.hpp"
#include "src/shader_setup.hpp"
#include "src/texture_loader.hpp"
