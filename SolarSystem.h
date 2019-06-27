#ifndef _HELLOSKYBOX_H_
#define _HELLOSKYBOX_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <vector>
#include <ctime>


//try static linking glew(opengl)
//#define GLEW_STATIC 1


// OpenGL / GLEW Headers
#include <GL/glew.h>


#ifdef __WIN32__
#include <GL/wglew.h>
#endif



// SDL2 Headers
#include <SDL2/SDL.h>

#include <FreeImagePlus.h>


#ifndef __WIN32__
#define GLM_ENABLE_EXPERIMENTAL
#endif

// Force old construnctor behavior (glm-version >= 0.9.9.x)
//#define GLM_FORCE_CTOR_INIT

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define MAT4_IDENTITY glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
#define QUAT_IDENTITY glm::quat(MAT4_IDENTITY)


#define BUFFER_OFFSET(offset) ((void*)(offset))
#define MEMBER_OFFSET(s,m) ((char*)NULL + (offsetof(s,m)))


#endif //_HELLOSKYBOX_H_