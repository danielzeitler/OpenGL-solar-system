#ifndef __TEXTURE_H__
#define	__TEXTURE_H__


using namespace std;



class Texture {


public:

    static GLuint LoadTexture(
            const string& filenameString,
            GLenum minificationFilter = GL_LINEAR,
            GLenum magnificationFilter = GL_LINEAR
    );



    static GLuint LoadCubeMap(vector<string> faces);


    static GLuint LoadSkybox(const string& path, const string& ext);


private:

    static FIBITMAP* LoadTextureFile(const GLchar* filename, int& width, int& height);



};

#endif	/* __TEXTURE_H __*/

