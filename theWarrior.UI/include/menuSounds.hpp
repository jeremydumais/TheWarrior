#pragma once

#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <memory>
#include <string>

namespace thewarrior::ui {

class MenuSounds {
 public:
    void initialize(const std::string &resourcePath) {
        m_move = load(resourcePath, "menu_move.wav");
        m_click = load(resourcePath, "menu_click.wav");
        m_back = load(resourcePath, "menu_back.wav");
    }

    void playMove() const { play(m_move); }
    void playClick() const { play(m_click); }
    void playBack() const { play(m_back); }

 private:
    std::shared_ptr<Mix_Chunk> m_move;
    std::shared_ptr<Mix_Chunk> m_click;
    std::shared_ptr<Mix_Chunk> m_back;

    static std::shared_ptr<Mix_Chunk> load(const std::string &resourcePath,
                                         const std::string &filename) {
        auto sound = std::shared_ptr<Mix_Chunk>(
            Mix_LoadWAV((resourcePath + "/sounds/" + filename).c_str()), Mix_FreeChunk);
        if (!sound) {
            std::cerr << "Mix_LoadWAV error (" << filename << "): " << Mix_GetError() << '\n';
        }
        return sound;
    }

    static void play(const std::shared_ptr<Mix_Chunk> &sound) {
        if (sound) {
            Mix_PlayChannel(-1, sound.get(), 0);
        }
    }
};

}  // namespace thewarrior::ui
