#ifndef H2DE_ENGINE_H
#define H2DE_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <H2DE/H2DE_loader.h>
#include <H2DE/H2DE_calculator.h>
#include <H2DE/H2DE_types.h>
#include <H2DE/H2DE_graphic.h>
#include <H2DE/H2DE_timeline.h>
#include <H2DE/H2DE_json.h>
#include <algorithm>
#include <climits>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class H2DE_Timeline;

namespace fs = std::filesystem;

/**
 * The type used to identify an engine
 * 
 * \since H2DE-1.0.0
 */
class H2DE_Engine {
private:
    int fps;
    H2DE_Size size;
    H2DE_Size maxSize = { -1, -1 };
    bool isRunning = true;
    SDL_Renderer* renderer;
    int dataToLoad, loadedData = 0;
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, Mix_Music*> songs;
    std::unordered_map<std::string, Mix_Chunk*> sfxs;
    std::unordered_map<std::string, TTF_Font*> fonts;
    std::vector<H2DE_GraphicObject*> graphicObjects;
    std::vector<H2DE_Timeline*> timelines;
    
    /**
     * Counts the number of file to be loaded from a parent directory
     * 
     * \param dir parent directory
     * 
     * \return the number of files
     * 
     * \since H2DE-1.0.0
     */
    static int countFiles(const fs::path& dir);
    /**
     * Imports the files
     * 
     * \param dir parent directory
     * 
     * \since H2DE-1.0.0
     */
    void importFiles(const fs::path& dir);
    /**
     * Imports a texture from a png file
     * 
     * \param img png file
     * 
     * \since H2DE-1.0.0
     */
    void importTexture(const fs::path& img);
    /**
     * Imports a song from a mp3 file
     * 
     * \param song mp3 file
     * 
     * \since H2DE-1.0.0
     */
    void importSong(const fs::path& song);
    /**
     * Imports a SFX from a wav file
     * 
     * \param sfx wav file
     * 
     * \since H2DE-1.0.0
     */
    void importSFX(const fs::path& sfx);
    /**
     * Imports a font from a ttf file
     * 
     * \param font ttf file
     * 
     * \since H2DE-1.0.0
     */
    void importFont(const fs::path& font);
    /**
     * Indicates that an asset has been loaded
     * 
     * \since H2DE-1.0.0
     */
    void assetImported();
    
    /**
     * Duplicates `H2DE_GraphicObject` which have the `repeatX` property
     * 
     * \return a graphic object vector of the duplicated graphic objects
     * 
     * \since H2DE-1.0.0
     */
    std::vector<H2DE_GraphicObject*> getRepeatXGraphicObjects();
    /**
     * Duplicates `H2DE_GraphicObject` which have the `repeatY` property
     * 
     * \return a graphic object vector of the duplicated graphic objects
     * 
     * \since H2DE-1.0.0
     */
    std::vector<H2DE_GraphicObject*> getRepeatYGraphicObjects();

    /**
     * Renders a pixel on the renderer
     * 
     * \param pos position of the pixel
     * \param color color of the pixel
     * 
     * \since H2DE-1.0.0
     */
    void renderPixel(H2DE_Pos pos, H2DE_Color color);
    /**
     * Renders an image on the renderer
     * 
     * \param g the image to render
     * 
     * \since H2DE-1.0.0
     */
    void renderImage(H2DE_GraphicObject* g);
    /**
     * Renders a polygon on the renderer
     * 
     * \param g the polygon to render
     * 
     * \since H2DE-1.0.0
     */
    void renderPolygon(H2DE_GraphicObject* g);
    /**
     * Renders a circle on the renderer
     * 
     * \param g the circle to render
     * 
     * \since H2DE-1.0.0
     */
    void renderCircle(H2DE_GraphicObject* g);
    /**
     * Renders a text on the renderer
     * 
     * \param g the text to render
     * 
     * \since H2DE-1.0.0
     */
    void renderText(H2DE_GraphicObject* g);

    friend class H2DE_Calculator;

public:
    H2DE_Engine(SDL_Renderer* renderer, int w, int h, int fps);
    ~H2DE_Engine();

    /**
     * Renders all graphic objects added during this frame
     * 
     * \param engine a pointer to an engine
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_RenderEngine(H2DE_Engine* engine);
    /**
     * Loads every assets from a parent directory
     * 
     * \param engine a pointer to an engine
     * \param dir parent directory
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_LoadAssets(H2DE_Engine* engine, const fs::path& dir);
    /**
     * Adds a graphic object to be rendered this frame
     * 
     * \param engine a pointer to an engine
     * \param g a pointer the graphic object
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_AddGraphicObject(H2DE_Engine* engine, H2DE_GraphicObject* g);

    /**
     * Gets the size of the engine
     * 
     * \param engine a pointer to the engine
     * 
     * \return the size of the engine
     * 
     * \since H2DE-1.0.4
     */
    friend H2DE_Size H2DE_GetEngineSize(H2DE_Engine* engine);
    /**
     * Sets a new size for the engine
     * 
     * \param engine a pointer to an engine
     * \param size the new size
     * 
     * \since H2DE-1.0.4
     */
    friend void H2DE_SetEngineSize(H2DE_Engine* engine, int w, int h);
    /**
     * Sets the maximum size for the specified engine
     * 
     * \param engine a pointer to an engine
     * \param w maxmum width for the engine
     * \param h maxmum height for the engine
     * 
     * \since H2DE-1.0.5
     */
    friend void H2DE_SetEngineMaximumSize(H2DE_Engine* engine, int w, int h);
    /**
     * Gets the maximum size for the specified engine
     * 
     * \param engine a pointer to an engine
     * 
     * \return the maximum size of the engine
     * 
     * \since H2DE-1.0.5
     */
    friend H2DE_Size H2DE_GetEngineMaximumSize(H2DE_Engine* engine);
    /**
     * Gets the FPS on an engine
     * 
     * \param engine a pointer to an engine
     * 
     * \return fps
     * 
     * \since H2DE-1.0.9
     */
    friend int H2DE_GetEngineFPS(H2DE_Engine* engine);
    /**
     * Creates a timeline
     * 
     * \param engine a pointer to an engine
     * \param start starting value
     * \param end ending value
     * \param duration time in ms
     * \param effect timeline effect
     * \param update function called every frame
     * \param completed function called once the timeline is finished (`NULL` not to call)
     * 
     * \return a pointer to a timeline
     * 
     * \since H2DE-1.0.9
     */
    friend void H2DE_CreateTimeline(H2DE_Engine* engine, float start, float end, int duration, H2DE_TimelineEffect effect, std::function<void(float)> update, std::function<void()> completed);

    /**
     * Sets the song volume
     * 
     * \param engine a pointer to an engine
     * \param volume the volume (0-100)
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_SetSongVolume(H2DE_Engine* engine, int volume);
    /**
     * Plays a song
     * 
     * \param engine a pointer to an engine
     * \param song the name of the loaded song
     * \param loop number of loop (-1 = infinite)
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_PlaySong(H2DE_Engine* engine, std::string song, int loop);
    /**
     * Pauses the current song
     * 
     * \param engine a pointer to an engine
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_PauseSong(H2DE_Engine* engine);
    /**
     * Resumes the current song
     * 
     * \param engine a pointer to an engine
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_ResumeSong(H2DE_Engine* engine);

    /**
     * Sets the volume for a specific sfx
     * 
     * \param engine a pointer to an engine
     * \param channel the channel of the target (-1 for all)
     * \param volume the volume (0-100)
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_SetSFXVolume(H2DE_Engine* engine, int channel, int volume);
    /**
     * Plays a sfx
     * 
     * \param engine a pointer to an engine
     * \param song the name of the loaded sfx (-1 for all)
     * \param loop number of loop (-1 = infinite)
     * 
     * \return the channel of the target
     * 
     * \since H2DE-1.0.0
     */
    friend int H2DE_PlaySFX(H2DE_Engine* engine, std::string sfx, int loop);
    /**
     * Pauses a sfx
     * 
     * \param engine a pointer to an engine
     * \param channel the channel of the target (-1 for all)
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_PauseSFX(H2DE_Engine* engine, int channel);
    /**
     * Resumes a sfx
     * 
     * \param engine a pointer to an engine
     * \param channel the channel of the target (-1 for all)
     * 
     * \since H2DE-1.0.0
     */
    friend void H2DE_ResumeSFX(H2DE_Engine* engine, int channel);
};

/**
 * Creates an engine
 * 
 * \param renderer a pointer to the renderer
 * \param w the width of the engine
 * \param h this heigth of the engine
 * \param fps the fps the window is running at
 * 
 * \return a pointer to an engine
 * 
 * \since H2DE-1.0.0
 */
extern H2DE_Engine* H2DE_CreateEngine(SDL_Renderer* renderer, int w, int h, int fps);
/**
 * Destroys an engine
 * 
 * \param engine the engine to destroy
 * 
 * \since H2DE-1.0.0
 */
extern void H2DE_DestroyEngine(H2DE_Engine* engine);

#endif