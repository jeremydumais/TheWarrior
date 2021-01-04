#include "gameWindow.hpp"
#include <GL/glu.h>
#include <iostream>
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <boost/archive/binary_iarchive.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : width(width),
      height(height),
      mustExit(false),
      playerMovement(PlayerMovement::None),
      texCoordBuf { { 0.0f, 0.0f },
                    { 0.0f, 0.0f }, 
                    { 0.0f, 0.0f }, 
                    { 0.0f, 0.0f } },
      texColorBuf { { 1.0f, 1.0f, 1.0f },   /* Red */
                    { 1.0f, 1.0f, 1.0f },   /* Green */
                    { 1.0f, 1.0f, 1.0f },   /* Blue */
                    { 1.0f, 1.0f, 1.0f } }
{
    //Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cerr << fmt::format("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Create window
    window = SDL_CreateWindow(title.c_str(), 
        x, 
        y, 
        width, 
        height, 
        SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    if(window == NULL)
    {
        cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    //Create context
    gContext = SDL_GL_CreateContext(window);
    if(gContext == nullptr)
    {
        cerr << fmt::format("OpenGL context could not be created! SDL Error: {0}\n", SDL_GetError());
        return;
    }

    //Initialize GLEW
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        cerr << fmt::format("Error initializing GLEW! {0}\n", glewGetErrorString(glewError));
        return;
    }

    //Use Vsync
    if( SDL_GL_SetSwapInterval( 1 ) < 0 )
    {
        cerr << fmt::format("Warning: Unable to set VSync! SDL Error: {0}\n", SDL_GetError());
        return;
    }

    if (!compileShaders()) {
        cerr << "Unable to compile shaders\n";
        return;
    }

    TILEWIDTH = 0.1f;
    TILEHALFWIDTH = TILEWIDTH / 2.0f;
    TILEHALFHEIGHT = (static_cast<float>(width) * TILEHALFWIDTH) / static_cast<float>(height);

    loadMap(fmt::format("{0}/maps/homeHouse.map", getResourcesPath()));
    loadTextures();
    generateGLMapObjects();
    generateGLPlayerObject();
    linkShaders();
    glPlayer.x = 7;
    glPlayer.y = 14;
    glPlayer.xMove = 0.0f;
    glPlayer.yMove = 0.0f;
    glPlayer.currentMovementTexture = 1;
    setPlayerPosition();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GameWindow::~GameWindow() 
{
    glDetachShader(shaderprogram, vertexshader);
    glDetachShader(shaderprogram, fragmentshader);
    glDeleteProgram(shaderprogram);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);
    for(auto &item : glTiles) {
       glDeleteBuffers(1, &item.vboPosition); 
       glDeleteBuffers(1, &item.vboColor); 
       glDeleteBuffers(1, &item.vboTexture); 
       glDeleteBuffers(1, &item.vboTextureObject); 
       glDeleteVertexArrays(1, &item.vao);
       if (item.hasObjectTexture) {
        glDeleteVertexArrays(1, &item.vaoObject);
       }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GameWindow::show() 
{
    SDL_ShowWindow(window);
}

void GameWindow::hide() 
{
    SDL_HideWindow(window);
}

bool GameWindow::isAlive() const
{
    return !mustExit;
}

void GameWindow::processEvents() 
{
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT){
            mustExit = true;
        }
        else if(e.type == SDL_KEYDOWN && playerMovement == PlayerMovement::None) {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP:
                    playerMovement = PlayerMovement::MoveUp;
                    glPlayer.y -= 1;
                    glPlayer.yMove = 1.0f;
                    glPlayer.currentMovementTexture = 0;
                    setPlayerTexture();
                    break;
                case SDLK_DOWN:
                    playerMovement = PlayerMovement::MoveDown;
                    glPlayer.y += 1;
                    glPlayer.yMove = -1.0f;
                    glPlayer.currentMovementTexture = 36;
                    setPlayerTexture();
                    break;
                case SDLK_LEFT:
                    playerMovement = PlayerMovement::MoveLeft;
                    glPlayer.x -= 1;
                    glPlayer.xMove = 1.0f;
                    glPlayer.currentMovementTexture = 24;
                    setPlayerTexture();
                    break;
                case SDLK_RIGHT:
                    playerMovement = PlayerMovement::MoveRight;
                    glPlayer.x += 1;
                    glPlayer.xMove = -1.0f;
                    glPlayer.currentMovementTexture = 12;
                    setPlayerTexture();
                    break;
            };
        }
    }
    update(1.0f / 60.0f);
	render();
	SDL_GL_SwapWindow(window);
}

void GameWindow::update(double delta_time) 
{
    const float SPEED = 8.0f;
    if (playerMovement == PlayerMovement::MoveUp) {
        glPlayer.yMove -= SPEED * delta_time;
        if (glPlayer.yMove < 0.0f) {
            glPlayer.yMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(glPlayer.yMove < 0.3f) {
            if (glPlayer.currentMovementTexture != 1) {
                glPlayer.currentMovementTexture = 1;
                setPlayerTexture();
            }
        }
        else if(glPlayer.yMove < 0.6f) {
            if (glPlayer.currentMovementTexture != 2) {
                glPlayer.currentMovementTexture = 2;
                setPlayerTexture();
            }
        }
        setPlayerPosition();
    }
    else if (playerMovement == PlayerMovement::MoveDown) {
        glPlayer.yMove += SPEED * delta_time;
        if (glPlayer.yMove > 0.0f) {
            glPlayer.yMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(glPlayer.yMove > -0.3f) {
            if (glPlayer.currentMovementTexture != 37) {
                glPlayer.currentMovementTexture = 37;
                setPlayerTexture();
            }
        }
        else if(glPlayer.yMove > -0.6f) {
            if (glPlayer.currentMovementTexture != 38) {
                glPlayer.currentMovementTexture = 38;
                setPlayerTexture();
            }
        }
        setPlayerPosition();
    }
    else if (playerMovement == PlayerMovement::MoveLeft) {
        glPlayer.xMove -= SPEED * delta_time;
        if (glPlayer.xMove < 0.0f) {
            glPlayer.xMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(glPlayer.xMove < 0.3f) {
            if (glPlayer.currentMovementTexture != 25) {
                glPlayer.currentMovementTexture = 25;
                setPlayerTexture();
            }
        }
        else if(glPlayer.xMove < 0.6f) {
            if (glPlayer.currentMovementTexture != 26) {
                glPlayer.currentMovementTexture = 26;
                setPlayerTexture();
            }
        }
        setPlayerPosition();
    }    
    else if (playerMovement == PlayerMovement::MoveRight) {
        glPlayer.xMove += SPEED * delta_time;
        if (glPlayer.xMove > 0.0f) {
            glPlayer.xMove = 0.0f;
            playerMovement = PlayerMovement::None;
        }
        else if(glPlayer.xMove > -0.3f) {
            if (glPlayer.currentMovementTexture != 13) {
                glPlayer.currentMovementTexture = 13;
                setPlayerTexture();
            }
        }
        else if(glPlayer.xMove > -0.6f) {
            if (glPlayer.currentMovementTexture != 14) {
                glPlayer.currentMovementTexture = 14;
                setPlayerTexture();
            }
        }
        setPlayerPosition();
    }
}

string GameWindow::loadShaderFile(const string &file)
{
    stringstream retVal;
    ifstream shaderFileStream(file, ios::in);
    if (shaderFileStream.is_open()) {
        string line;
        while (getline(shaderFileStream, line)) {
            retVal << line << '\n';
        }
        shaderFileStream.close();
    }
    return retVal.str();
}


void GameWindow::generateGLMapObjects() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY { 1.0f - TILEHALFHEIGHT };

    GLfloat tileCoord[4][2] = {
    { -TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Left point */
    {  TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Right point */
    {  TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    int indexRow {0};
    const Texture *lastUsedTexture { nullptr };
    for(const auto &row : map->getTiles()) {
        int indexCol {0};
        for(const auto &tile : row) { 
            GLTile glTile;
            glTile.hasTexture = !tile.getTextureName().empty() && tile.getTextureIndex() != -1;
            if (glTile.hasTexture) {
                glTile.textureName = tile.getTextureName();
                glTile.textureIndex = tile.getTextureIndex();
            }
            glTile.hasObjectTexture = !tile.getObjectTextureName().empty() && tile.getObjectTextureIndex() != -1;
            if (glTile.hasObjectTexture) {
                glTile.objectTextureName = tile.getObjectTextureName();
                glTile.objectTextureIndex = tile.getObjectTextureIndex();
            }
            
            if (indexCol > 0) {
                for (int indexVertex=0; indexVertex<4; indexVertex++) {
                    tileCoord[indexVertex][0] += TILEWIDTH;
                }
            }
            glGenBuffers(1, &glTile.vboPosition);
            glGenBuffers(1, &glTile.vboColor);
            GenerateGLObjectInfo infoGenTexture {
                    lastUsedTexture,
                    &glTile.vao,
                    tile.getTextureName(),
                    tile.getTextureIndex(),
                    &glTile.vboPosition,
                    &glTile.vboColor,
                    &glTile.vboTexture };
            generateGLObject(infoGenTexture, tileCoord, texColorBuf);
            if (glTile.hasObjectTexture) {
                GenerateGLObjectInfo infoGenObject {
                    lastUsedTexture,
                    &glTile.vaoObject,
                    tile.getObjectTextureName(),
                    tile.getObjectTextureIndex(),
                    &glTile.vboPosition,
                    &glTile.vboColor,
                    &glTile.vboTextureObject };
                generateGLObject(infoGenObject, tileCoord, texColorBuf);
            }
            indexCol++;
            glTiles.push_back(glTile);
        }
        indexRow++;
        float topOffset { static_cast<float>(indexRow) * (TILEHALFHEIGHT * 2.0f) };
        tileCoord[0][0] = -TILEHALFWIDTH + startPosX;
        tileCoord[0][1] =  TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[1][0] =  TILEHALFWIDTH + startPosX;
        tileCoord[1][1] =  TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[2][0] =  TILEHALFWIDTH + startPosX;
        tileCoord[2][1] = -TILEHALFHEIGHT + startPosY - topOffset;
        tileCoord[3][0] = -TILEHALFWIDTH + startPosX;
        tileCoord[3][1] = -TILEHALFHEIGHT + startPosY - topOffset;
    }
}

void GameWindow::generateGLObject(GenerateGLObjectInfo &info, const GLfloat tileCoord[4][2], const GLfloat colors[4][3]) 
{
    if (info.lastUsedTexture == nullptr || info.textureName != info.lastUsedTexture->getName()) {
        auto texture { map->getTextureByName(info.textureName) };
        if (texture.has_value()) {
            info.lastUsedTexture = &texture.get();
        }
        else {
            info.lastUsedTexture = nullptr;
        }
    }
    if (info.lastUsedTexture != nullptr && !info.textureName.empty() && info.textureIndex != -1) {
        setTextureUVFromIndex(info.lastUsedTexture, texCoordBuf, info.textureIndex);
    }
    glBindVertexArray(0);
    glGenVertexArrays(1, info.vao);
    glBindVertexArray(*info.vao);

    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tileCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboColor);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    /* Bind our third VBO as being the active buffer and storing vertex attributes (textures) */
    glGenBuffers(1, info.vboTexture);
    glBindBuffer(GL_ARRAY_BUFFER, *info.vboTexture);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void GameWindow::generateGLPlayerObject() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY { 1.0f - TILEHALFHEIGHT };

    GLfloat tileCoord[4][2] = {
    { -TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Left point */
    {  TILEHALFWIDTH + startPosX,  TILEHALFHEIGHT + startPosY },     /* Top Right point */
    {  TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY },     /* Bottom Right point */
    { -TILEHALFWIDTH + startPosX, -TILEHALFHEIGHT + startPosY } };   /* Bottom Left point */
    
    glPlayer.textureName = "NPC1";
    glPlayer.textureIndex = 1;
    glGenBuffers(1, &glPlayer.vboPosition);
    glGenBuffers(1, &glPlayer.vboColor);
    GenerateGLObjectInfo infoGenTexture {
            nullptr,
            &glPlayer.vao,
            glPlayer.textureName,
            glPlayer.textureIndex,
            &glPlayer.vboPosition,
            &glPlayer.vboColor,
            &glPlayer.vboTexture };
    generateGLObject(infoGenTexture, tileCoord, texColorBuf);
}

void GameWindow::linkShaders() 
{
    shaderprogram = glCreateProgram();
    glAttachShader(shaderprogram, vertexshader);
    glAttachShader(shaderprogram, fragmentshader);

    /* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
    glBindAttribLocation(shaderprogram, 0, "in_Position");
    glBindAttribLocation(shaderprogram, 1, "in_Color");
    glBindAttribLocation(shaderprogram, 2, "in_VertexUV");

    glLinkProgram(shaderprogram);
    int IsLinked;
    glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int *)&IsLinked);
    if(IsLinked == false)
    {
       int maxLength;
       glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
       char *shaderProgramInfoLog = (char *)malloc(maxLength);
       glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);
       cerr << shaderProgramInfoLog << '\n';
       free(shaderProgramInfoLog);
    }
}

void GameWindow::render()
{
    //int vertexTranslationLocation = glGetUniformLocation(shaderprogram, "translation");
    glUseProgram(shaderprogram);
    //glUniform2f(vertexTranslationLocation, 0.5f, 0.5f);
    GLuint TextureID  = glGetUniformLocation(shaderprogram, "myTextureSampler");
    glUniform1i(TextureID, 0);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    for(auto &item : glTiles) {
        glBindVertexArray(item.vao);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboPosition);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, item.vboColor);
        glEnableVertexAttribArray(1);
        if (item.hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturesGLMap[item.textureName]);
            glBindBuffer(GL_ARRAY_BUFFER, item.vboTexture);
            glEnableVertexAttribArray(2);
        }
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        //Object
        if (item.hasTexture && item.hasObjectTexture) {
            glBindTexture(GL_TEXTURE_2D, texturesGLMap[item.objectTextureName]);
            glBindVertexArray(item.vaoObject);
            glBindBuffer(GL_ARRAY_BUFFER, item.vboPosition);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, item.vboColor);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, item.vboTextureObject);
            glEnableVertexAttribArray(2);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    //Render the player
    glBindTexture(GL_TEXTURE_2D, texturesGLMap[glPlayer.textureName]);
    glBindVertexArray(glPlayer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboPosition);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboColor);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboTexture);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    /* Swap our buffers to make our changes visible */
    SDL_GL_SwapWindow(window);
           
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

bool GameWindow::compileShaders() 
{
    int IsCompiled_VS;
    int IsCompiled_FS;
    char *vertexInfoLog;
    char *fragmentInfoLog;
    /* Read our shaders into the appropriate buffers */
    vertexShaderContent = loadShaderFile(fmt::format("{0}/shaders/tile_330_vs.glsl", getResourcesPath()));
    fragmentShaderContent = loadShaderFile(fmt::format("{0}/shaders/tile_330_fs.glsl", getResourcesPath()));

    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexShaderSource = vertexShaderContent.c_str();
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexShaderSource, 0);
    glCompileShader(vertexshader);
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
    if(IsCompiled_VS == false)
    {
       int maxLength;
       glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
       vertexInfoLog = (char *)malloc(maxLength);
       glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
       cerr << vertexInfoLog << '\n';
       free(vertexInfoLog);
       return false;
    }

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = fragmentShaderContent.c_str();
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentShaderSource, 0);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
    if(IsCompiled_FS == false)
    {
       int maxLength;
       glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
       fragmentInfoLog = (char *)malloc(maxLength);
       glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
       cerr << fragmentInfoLog << '\n';
       free(fragmentInfoLog);
       return false;
    }
    return true;
}

void GameWindow::loadMap(const std::string &filePath) 
{
    ifstream ofs(filePath, ifstream::binary);
	boost::archive::binary_iarchive oa(ofs);
    map = make_shared<GameMap>(1, 1);
	oa >> *map;
}

void GameWindow::loadTextures() 
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : texturesGLMap) {
        glDeleteTextures(1, &glTexture.second);
    }
    texturesGLMap.clear();
    //Load texture in graphics memory
    for(const auto &texture : map->getTextures()) {  
        const auto &textureName { texture.getName() }; 
        glGenTextures(1, &texturesGLMap[textureName]);
        glBindTexture(GL_TEXTURE_2D, texturesGLMap[textureName]); 
        //texturesObjMap.emplace(textureName, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        string texFileName { texture.getFilename() };
        string fullResourcePath = fmt::format("{0}/{1}", getResourcesPath(), texFileName);
        unsigned char *imageBytes = stbi_load(fullResourcePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (imageBytes) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBytes);
        }
        else {
            throw runtime_error(fmt::format("Failed to load texture {0}", fullResourcePath));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(imageBytes);
    }
}


void GameWindow::setTextureUVFromIndex(const Texture *texture, GLfloat uvMap[4][2], int index) 
{
    float indexTile { static_cast<float>(index) };
    const int NBTEXTUREPERLINE { texture->getWidth() / texture->getTileWidth() };
    float lineIndex = static_cast<int>(indexTile / NBTEXTUREPERLINE);
    const float TEXTURETILEWIDTH { texture->getTileWidthGL() };
    const float TEXTURETILEHEIGHT { texture->getTileHeightGL() };
    GLfloat lowerLeftCorner { indexTile / static_cast<float>(NBTEXTUREPERLINE) - floor(indexTile / static_cast<float>(NBTEXTUREPERLINE))  };

    uvMap[0][0] = (lowerLeftCorner);
    uvMap[0][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT;
    uvMap[1][0] = lowerLeftCorner + TEXTURETILEWIDTH;
    uvMap[1][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex) - TEXTURETILEHEIGHT;
    uvMap[2][0] = lowerLeftCorner + TEXTURETILEWIDTH;;
    uvMap[2][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex);
    uvMap[3][0] = lowerLeftCorner;
    uvMap[3][1] = 1.0f-(TEXTURETILEHEIGHT * lineIndex);
}

void GameWindow::setTileCoordToOrigin() 
{
    float startPosX { -1.0f + TILEHALFWIDTH };
    float startPosY {  1.0f - TILEHALFHEIGHT };
    tileCoordBuf[0][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[0][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[1][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[1][1] =  TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[2][0] =  TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[2][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
    tileCoordBuf[3][0] = -TILEHALFWIDTH + startPosX + TILEWIDTH;
    tileCoordBuf[3][1] = -TILEHALFHEIGHT + startPosY - TILEHALFHEIGHT;
}

void GameWindow::setPlayerPosition() 
{
    glBindVertexArray(glPlayer.vao);
    setTileCoordToOrigin();
    for(int i = 0; i < 4; i++) {
        tileCoordBuf[i][0] += ((static_cast<float>(glPlayer.x) - 1.0f) * TILEWIDTH) + 
                              (glPlayer.xMove * TILEWIDTH);
        tileCoordBuf[i][1] -= (((static_cast<float>(glPlayer.y) * (TILEHALFHEIGHT * 2.0f)) - TILEHALFHEIGHT)) + 
                              (glPlayer.yMove * (TILEHALFHEIGHT * 2.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboPosition);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tileCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void GameWindow::setPlayerTexture() 
{

    auto texture { map->getTextureByName(glPlayer.textureName) };
    if (texture.has_value() && !glPlayer.textureName.empty() && glPlayer.textureIndex != -1) {
        setTextureUVFromIndex(texture.get_ptr(), texCoordBuf, glPlayer.currentMovementTexture);
    }
    glBindVertexArray(glPlayer.vao);

    glBindBuffer(GL_ARRAY_BUFFER, glPlayer.vboTexture);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoordBuf, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glDisableVertexAttribArray(2);
}


const std::string &GameWindow::getExecutablePath() 
{
	if (executablePath.empty()) {
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		if (count != -1) {
			executablePath = dirname(result);
		}
	}
	return executablePath;
}

const std::string &GameWindow::getResourcesPath() 
{
	if (resourcesPath.empty()) {
		resourcesPath = fmt::format("{0}/resources/", getExecutablePath());
	}
	return resourcesPath;
}