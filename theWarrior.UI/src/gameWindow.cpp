#include "gameWindow.hpp"
#include <fmt/format.h>
#include <iostream>

using namespace std;

GameWindow::GameWindow(const string &title,
                       int x, int y,
                       int width, int height) 
    : m_WindowSize(width, height),
      m_tileSize({1.0F, 1.0F, 1.0F}),
      m_mustExit(false),
      m_interactionMode(InteractionMode::Game),
      m_tileService(std::make_shared<GLTileService>()),
      m_textBox(std::make_shared<GLTextBox>()),
      m_glPlayer(std::make_shared<GLPlayer>("Ragnar")),
      m_toggleFPS(false),
      m_blockKeyDown(false)
{
    if (!initializeOpenGL(title, x, y, width, height)) {
        return;
    }

    if (!m_controller.loadItemStore(fmt::format("{0}/items/itemstore.itm", m_controller.getResourcesPath()))) {
        cerr << "Unable to load the item store : " << m_controller.getLastError() << "\n";
        return;
    }

    if (!m_tileService->initShader(fmt::format("{0}/shaders/tile_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/tile_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_tileService->getLastError() << "\n";
        return;
    }
    if (!m_textBox->initShader(fmt::format("{0}/shaders/textbox_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/textbox_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_textBox->getLastError() << "\n";
        return;
    }
    if (!m_textService.initShader(fmt::format("{0}/shaders/text_330_vs.glsl", m_controller.getResourcesPath()),
                          fmt::format("{0}/shaders/text_330_fs.glsl", m_controller.getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }
    if (!m_textService.initFont(fmt::format("{0}/fonts/verdana.ttf", m_controller.getResourcesPath()))) {
        cerr << m_textService.getLastError() << "\n";
        return;
    }

    m_textureService.setResourcesPath(m_controller.getResourcesPath());
    loadItemStoreTextures();

    SDL_JoystickEventState(SDL_ENABLE);
    m_joystick = SDL_JoystickOpen(0);

    m_windowSizeChanged.connect(boost::bind(&GameMapMode::gameWindowSizeChanged, &m_gameMapMode, boost::placeholders::_1));
    m_windowSizeChanged.connect(boost::bind(&GLPlayer::onGameWindowSizeChanged, m_glPlayer, boost::placeholders::_1));
    m_windowSizeChanged.connect(boost::bind(&GLTextService::gameWindowSizeChanged, &m_textService, boost::placeholders::_1));
    m_windowSizeChanged.connect(boost::bind(&GLTextBox::gameWindowSizeChanged, m_textBox, boost::placeholders::_1));
    m_tileSizeChanged.connect(boost::bind(&GameMapMode::gameWindowTileSizeChanged, &m_gameMapMode, boost::placeholders::_1));
    m_tileSizeChanged.connect(boost::bind(&GLPlayer::onGameWindowTileSizeChanged, m_glPlayer, boost::placeholders::_1));
    m_windowUpdate.connect(boost::bind(&GLPlayer::onGameWindowUpdate, m_glPlayer, boost::placeholders::_1));

    calculateTileSize();
    m_textBox->setItemStore(m_controller.getItemStore());
    m_textBox->setTextService(&m_textService);
    m_textBox->setItemStoreTextureMap(&m_texturesGLItemStore);

    m_glPlayer->initialize(m_controller.getResourcesPath());
    m_gameMapMode.initialize(m_controller.getResourcesPath(), 
                             m_glPlayer,
                             m_controller.getItemStore(),
                             m_controller.getMessagePipeline(),
                             m_tileService,
                             m_textBox,
                             m_joystick);
    m_fpsCalculator.initialize();
}

GameWindow::~GameWindow() 
{
    m_gameMapMode.unloadGLMapObjects();
    m_glPlayer->unloadGLPlayerObject();
    SDL_JoystickClose(m_joystick);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void GameWindow::show() 
{
    SDL_ShowWindow(m_window);
}

void GameWindow::hide() 
{
    SDL_HideWindow(m_window);
}

bool GameWindow::isAlive() const
{
    return !m_mustExit;
}

void GameWindow::processEvents() 
{
    SDL_Event e;
    SDL_JoystickUpdate();
    
    while(SDL_PollEvent(&e) != 0) {

        if(e.type == SDL_KEYUP) {
            m_blockKeyDown = false;
        }
        if(e.type == SDL_QUIT){
            m_mustExit = true;
            continue;
        }   
        switch (m_interactionMode)
        {
        case InteractionMode::Game:
            m_gameMapMode.processEvents(e);
            break;
        default:
            break;
        }
        
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                int screenWidth = 0, screenHeight = 0;
                SDL_GetWindowSize(m_window, &screenWidth, &screenHeight);
                m_WindowSize.setSize(screenWidth, screenHeight);
                glViewport(0, 0, m_WindowSize.width(), m_WindowSize.height());
                calculateTileSize();
                m_windowSizeChanged(m_WindowSize);
            }
        } 
    }
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if ((keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL]) && keystate[SDL_SCANCODE_F]) {
        if (!m_blockKeyDown) {
            m_toggleFPS = !m_toggleFPS;
            m_blockKeyDown = true;
        }  
    }
    m_windowUpdate(1.0F / 90.0F);
	render();
    if (m_toggleFPS) {
        m_fpsCalculator.calculate();
    }
}

bool GameWindow::initializeOpenGL(const std::string &title,
                                  int x, int y,
                                  int width, int height)
{
//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		cerr << fmt::format("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
	}

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Create window
    m_window = SDL_CreateWindow(title.c_str(), 
        x, 
        y, 
        width, 
        height, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
    if(m_window == NULL) {
        cerr << fmt::format("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    //Create context
    m_gContext = SDL_GL_CreateContext(m_window);
    if(m_gContext == nullptr) {
        cerr << fmt::format("OpenGL context could not be created! SDL Error: {0}\n", SDL_GetError());
        return false;
    }

    //Initialize GLEW
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK ) {
        cerr << fmt::format("Error initializing GLEW! {0}\n", glewGetErrorString(glewError));
        return false;
    }

    //Use Vsync
    if(SDL_GL_SetSwapInterval(-1) < 0) {
        cerr << fmt::format("Warning: Unable to set VSync! SDL Error: {0}\n", SDL_GetError());
        return false;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void GameWindow::render()
{
    m_gameMapMode.render();    
    //Display the FPS
    if (m_toggleFPS) {
        m_textService.useShader();
        m_textService.renderText(m_fpsCalculator.getFPSDisplayText(), 
                               1.0f,                               // X
                               static_cast<float>(m_WindowSize.height()) - 24.0f, // Y
                               0.5f,                               // Scale
                               glm::vec3(1.0f, 1.0f, 1.0f));       // Color
    }
    SDL_GL_SwapWindow(m_window);
}

void GameWindow::loadItemStoreTextures()
{
    //Clear existing textures in graphics memory
    for(auto &glTexture : m_texturesGLItemStore) {
        glDeleteTextures(1, &glTexture.second);
    }
    m_texturesGLItemStore.clear();
    for(const auto &texture : m_controller.getItemStore()->getTextureContainer().getTextures()) {
        const auto &textureName { texture.getName() }; 
        m_textureService.loadTexture(texture, m_texturesGLItemStore[textureName]);
    }
}

void GameWindow::calculateTileSize() 
{
    Size<float> screenSizeFloat(static_cast<float>(m_WindowSize.width()), static_cast<float>(m_WindowSize.height()));
    
    m_tileSize.tileWidth = (1.0F / (screenSizeFloat.width() / 51.2F)) * 2.0F;
    m_tileSize.tileHalfWidth = m_tileSize.tileWidth / 2.0F;
    m_tileSize.tileHalfHeight = (screenSizeFloat.width() * m_tileSize.tileHalfWidth) / screenSizeFloat.height();
    m_tileSizeChanged(m_tileSize);   
}