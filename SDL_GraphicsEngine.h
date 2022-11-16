#pragma once
#include <iostream>
#include <cstdint>
#include <SDL.h>
#include <SDL_ttf.h>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>

namespace blsp
{
    //utils, thanks javid..

    std::string hex(uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::chrono::seconds;

    template<class TYPE>
    struct vector2d {
        TYPE x, y;
        vector2d()                 { this->x = 0;  this->y = 0; }
        vector2d(TYPE xy)          { this->x = xy; this->y = xy; }
        vector2d(TYPE x, TYPE y)   { this->x = x;  this->y = y; }
        void setAll(TYPE xyz)      { this->x = xyz; this->y = xyz; }
        void setXY(TYPE x, TYPE y) { this->x = x; this->y = y; }
        std::string to_string()    { return std::to_string(this->x) + ", " + std::to_string(this->y); }

        vector2d& operator + (vector2d const& obj)  { vector2d result; result.x = x + obj.x; result.y = y + obj.y; return result; }
        vector2d& operator - (vector2d const& obj)  { vector2d result; result.x = x - obj.x; result.y = y - obj.y; return result; }
        vector2d& operator * (vector2d const& obj)  { vector2d result; result.x = x * obj.x; result.y = y * obj.y; return result; }
        vector2d& operator / (vector2d const& obj)  { vector2d result; result.x = x / obj.x; result.y = y / obj.y; return result; }

        vector2d& operator += (vector2d const& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
        vector2d& operator -= (vector2d const& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
        vector2d& operator *= (vector2d const& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
        vector2d& operator /= (vector2d const& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }

        vector2d& operator *= (TYPE const& rhs)     { this->x *= rhs;   this->y *= rhs;    return *this; }
        vector2d& operator /= (TYPE const& rhs)     { this->x /= rhs;   this->y /= rhs;   return *this; }
        vector2d& operator += (TYPE const& rhs)     { this->x += rhs;   this->y += rhs;    return *this; }
        vector2d& operator -= (TYPE const& rhs)     { this->x -= rhs;   this->y -= rhs;   return *this; }

        bool operator == (vector2d const& rhs)      { return this->x == rhs.x && this->y == rhs.y ? true : false; }
        bool operator != (vector2d const& rhs)      { return this->x == rhs.x && this->y == rhs.y ? false : true; }
        bool operator <= (vector2d const& rhs)      { return (this->x <= rhs.x && this->y <= rhs.y) ? true : false; }
        bool operator >= (vector2d const& rhs)      { return (this->x >= rhs.x && this->y >= rhs.y) ? true : false; }
        bool operator < (vector2d const& rhs)       { return this->x < rhs.x && this->y < rhs.y ? true : false; }
        bool operator > (vector2d const& rhs)       { return this->x > rhs.x && this->y > rhs.y ? true : false; }

        operator vector2d<int8_t>() const           { return { static_cast<int8_t>(this->x), static_cast<int8_t>(this->y) }; }
        operator vector2d<int16_t>() const          { return { static_cast<int16_t>(this->x), static_cast<int16_t>(this->y) }; }
        operator vector2d<int32_t>() const          { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
        operator vector2d<int64_t>() const          { return { static_cast<int64_t>(this->x), static_cast<int64_t>(this->y) }; }
        operator vector2d<float>() const            { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
        operator vector2d<double>() const           { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
        operator vector2d<long double>() const      { return { static_cast<long double>(this->x), static_cast<long double>(this->y) }; }
        template<class TYPE>
        vector2d<TYPE> cast_to() { return vector2d<TYPE>{ (TYPE)(x), (TYPE)(y) }; };
    };

    template<class TYPE>
    struct vector3d {
        TYPE x, y, z;
        vector3d()                                  { this->x = 0;   this->y = 0;   this->z = 0; }
        vector3d(TYPE xyz)                          { this->x = xyz; this->y = xyz; this->z = xyz; }
        vector3d(TYPE x, TYPE y, TYPE z)            { this->x = x;   this->y = y;   this->z = z; }
        void setAll(TYPE xyz)                       { this->x = xyz; this->y = xyz; this->z = xyz; }

        vector3d operator + (vector3d const& obj)   { vector3d result; result.x = x + obj.x; result.y = y + obj.y; result.z = z + obj.z; return result; }
        vector3d operator - (vector3d const& obj)   { vector3d result; result.x = x - obj.x; result.y = y - obj.y; result.z = z - obj.z; return result; }
        vector3d operator * (vector3d const& obj)   { vector3d result; result.x = x * obj.x; result.y = y * obj.y; result.z = z * obj.z; return result; }
        vector3d operator / (vector3d const& obj)   { vector3d result; result.x = x / obj.x; result.y = y / obj.y; result.z = z / obj.z; return result; }

        vector3d& operator += (vector3d const& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs; return *this; }
        vector3d& operator -= (vector3d const& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs; return *this; }
        vector3d& operator *= (vector3d const& rhs) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs; return *this; }
        vector3d& operator /= (vector3d const& rhs) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs; return *this; }

        vector3d& operator += (TYPE const& rhs)     { this->x += rhs; this->y += rhs; this->z += rhs; return *this; }
        vector3d& operator -= (TYPE const& rhs)     { this->x -= rhs; this->y -= rhs; this->z -= rhs; return *this; }
        vector3d& operator *= (TYPE const& rhs)     { this->x *= rhs;   this->y *= rhs;   this->z *= rhs; return *this; }
        vector3d& operator /= (TYPE const& rhs)     { this->x /= rhs;   this->y /= rhs;   this->z /= rhs; return *this; }

        bool operator == (vector3d const& rhs)      { return this->x == rhs.x && this->y == rhs.y && this->y == rhs.y ? true : false; }
        bool operator != (vector3d const& rhs)      { return this->x == rhs.x && this->y == rhs.y && this->y == rhs.y ? false : true; }
        bool operator <= (vector3d const& rhs)      { return this->x <= rhs.x && this->y <= rhs.y && this->y <= rhs.y ? true : false; }
        bool operator >= (vector3d const& rhs)      { return this->x >= rhs.x && this->y >= rhs.y && this->y >= rhs.y ? true : false; }
        bool operator < (vector3d const& rhs)       { return this->x < rhs.x&& this->y < rhs.y && this->y < rhs.y ? true : false; }
        bool operator > (vector3d const& rhs)       { return this->x > rhs.x && this->y > rhs.y && this->y > rhs.y ? true : false; }

        operator vector3d<int8_t>() const           { return { static_cast<int8_t>(this->x), static_cast<int8_t>(this->y), static_cast<int8_t>(this->z) }; }
        operator vector3d<int16_t>() const          { return { static_cast<int16_t>(this->x), static_cast<int16_t>(this->y), static_cast<int16_t>(this->z) }; }
        operator vector3d<int32_t>() const          { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y), static_cast<int32_t>(this->z) }; }
        operator vector3d<uint64_t>() const         { return { static_cast<uint64_t>(this->x), static_cast<uint64_t>(this->y), static_cast<uint64_t>(this->z) }; }
        operator vector3d<float>() const            { return { static_cast<float>(this->x), static_cast<float>(this->y), static_cast<float>(this->z) }; }
        operator vector3d<double>() const           { return { static_cast<double>(this->x), static_cast<double>(this->y), static_cast<double>(this->z) }; }
        operator vector3d<long double>() const      { return { static_cast<long double>(this->x), static_cast<long double>(this->y), static_cast<long double>(this->z) }; }
        template<class TYPE>
        vector3d<TYPE> cast_to() { return vector3d<TYPE>{ static_cast<TYPE>(x), static_cast<TYPE>(y), static_cast<TYPE>(z) }; };
    };

    template<class TYPE>
    struct vector4d {
        TYPE x, y, z, a;
        vector4d()                                 { this->x = 0;   this->y = 0;   this->z = 0; this->a = 0; }
        vector4d(TYPE xyz)                         { this->x = xyz; this->y = xyz; this->z = xyz; }
        vector4d(TYPE x, TYPE y, TYPE z, TYPE a)   { this->x = x;   this->y = y;   this->z = z; this->a = a; }
        void setAll(TYPE xyz)                      { this->x = xyz; this->y = xyz; this->z = xyz; }

        vector4d operator + (vector4d const& obj)  { vector4d result; result.x = x + obj.x; result.y = y + obj.y; result.z = z + obj.z; return result; }
        vector4d operator - (vector4d const& obj)  { vector4d result; result.x = x - obj.x; result.y = y - obj.y; result.z = z - obj.z; return result; }
        vector4d operator * (vector4d const& obj)  { vector4d result; result.x = x * obj.x; result.y = y * obj.y; result.z = z * obj.z; return result; }
        vector4d operator / (vector4d const& obj)  { vector4d result; result.x = x / obj.x; result.y = y / obj.y; result.z = z / obj.z; return result; }

        vector4d operator *= (vector4d const& rhs) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.y; this->a *= rhs.a; }
        vector4d operator /= (vector4d const& rhs) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.y; this->a /= rhs.a; }
        vector4d operator -= (vector4d const& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.y; this->a -= rhs.a; }
        vector4d operator += (vector4d const& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.y; this->a += rhs.a; }

        vector4d& operator += (TYPE const& rhs)    { this->x += rhs; this->y += rhs; this->z += rhs; this->a += rhs; return *this; }
        vector4d& operator -= (TYPE const& rhs)    { this->x -= rhs; this->y -= rhs; this->z -= rhs; this->a -= rhs; return *this; }
        vector4d& operator *= (TYPE const& rhs)    { this->x *= rhs;   this->y *= rhs; this->z *= rhs; this->a *= rhs; return *this; }
        vector4d& operator /= (TYPE const& rhs)    { this->x /= rhs;   this->y /= rhs; this->z /= rhs; this->a /= rhs; return *this; }


        bool operator == (vector4d const& rhs)     { return this->x == rhs.x && this->y == rhs.y && this->y == rhs.y && this->a == rhs.a ? true : false; }
        bool operator != (vector4d const& rhs)     { return this->x == rhs.x && this->y == rhs.y && this->y == rhs.y && this->a == rhs.a ? false : true; }
        bool operator <= (vector4d const& rhs)     { return this->x <= rhs.x && this->y <= rhs.y && this->y <= rhs.y && this->a <= rhs.a ? true : false; }
        bool operator >= (vector4d const& rhs)     { return this->x >= rhs.x && this->y >= rhs.y && this->y >= rhs.y && this->a >= rhs.a ? true : false; }
        bool operator < (vector4d const& rhs)      { return this->x < rhs.x&& this->y < rhs.y&& this->y < rhs.y && this->a < rhs.a ? true : false; }
        bool operator > (vector4d const& rhs)      { return this->x > rhs.x && this->y > rhs.y && this->y > rhs.y && this->a > rhs.a ? true : false; }

        operator vector4d<int8_t>() const          { return { static_cast<int8_t>(this->x), static_cast<int8_t>(this->y), static_cast<int8_t>(this->z), static_cast<int8_t>(this->a)}; }
        operator vector4d<int16_t>() const         { return { static_cast<int16_t>(this->x), static_cast<int16_t>(this->y), static_cast<int16_t>(this->z), static_cast<int16_t>(this->a) }; }
        operator vector4d<int64_t>() const         { return { static_cast<int64_t>(this->x), static_cast<int64_t>(this->y), static_cast<int64_t>(this->z), static_cast<int64_t>(this->a) }; }
        operator vector4d<float>() const           { return { static_cast<float>(this->x), static_cast<float>(this->y), static_cast<float>(this->z), static_cast<float>(this->a) }; }
        operator vector4d<double>() const          { return { static_cast<double>(this->x), static_cast<double>(this->y), static_cast<double>(this->z), static_cast<double>(this->a) }; }
        operator vector4d<long double>() const     { return { static_cast<long double>(this->x), static_cast<long double>(this->y), static_cast<long double>(this->z), static_cast<long double>(this->a) }; }

        template<class TYPE>
        vector4d<TYPE> cast_to() { return vector4d<TYPE>{ static_cast<TYPE>(x), static_cast<TYPE>(y), static_cast<TYPE>(z), static_cast<TYPE>(a) }; };
    };

    typedef vector2d<int>         vector2i;
    typedef vector2d<float>       vector2f;
    typedef vector2d<double>      vector2db;
    typedef vector2d<long double> vector2ld;
    typedef vector2d<uint32_t>    vector2i32t;
    typedef vector2d<uint16_t>    vector2i16t;
    typedef vector2d<uint8_t>     vector2di8t;

    typedef vector3d<int>         vector3i;
    typedef vector3d<float>       vector3f;
    typedef vector3d<double>      vector3db;
    typedef vector3d<long double> vector3ld;
    typedef vector3d<uint16_t>    vector3i16t;
    typedef vector3d<uint8_t>     vector3di8t;

    typedef vector4d<int>         vector4i;
    typedef vector4d<float>       vector4f;
    typedef vector4d<double>      vector4db;
    typedef vector4d<long double> vector4ld;
    typedef vector4d<uint16_t>    vector4i16t;
    typedef vector4d<uint8_t>     vector4di8t;

    struct Color : public vector4di8t { Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { this->x = r; this->y = g; this->z = b; this->a = a; } };
    struct ColorF : public vector4f { ColorF(float r, float g, float b, float a) { this->x = r; this->y = g; this->z = b; this->a = a; } };

    static const Color RED(204, 0, 0, 255), DARK_RED(102, 0, 0, 255), ORANGE(255, 128, 0, 255), BROWN(102, 51, 0, 255), DARK_BROWN(51, 25, 0, 255),
            GREEN(25, 51, 0, 255), LIME_GREEN(128, 255, 0, 255), FOREST_GREEN(0, 51, 0, 255), CYAN(0, 255, 255, 255), TEAL(0, 153, 153, 255), BLUE(0, 128, 255, 255),
            DARK_BLUE(0, 0, 102, 255), DARK_PURPLE(25, 0, 51, 255), PURPLE(102, 0, 204, 255), PINK(152, 0, 153, 255), BRIGHT_PINK(255, 0, 255, 255), MAGENTA(204, 0, 102, 255),
            BLACK(0, 0, 0, 255), DARK_GREY(40, 40, 40, 255), LIGHT_GREY(128, 128, 128, 255);

    typedef enum KEYBOARD {
        A = 'a', B = 'b', C = 'c', D = 'd', E = 'e', F = 'f', G = 'g', H = 'h', I = 'i', J = 'j', K = 'k', L = 'l', M = 'm', 
        N = 'n', O = 'o', P = 'p', Q = 'q', R = 'r', S = 's', T = 't', U = 'u', V = 'v', W = 'w', X = 'x', Y = 'y', Z = 'z',
        NUM_0 = '0', NUM_1 = '1', NUM_2 = '2', NUM_3 = '3', NUM_4 = '4', NUM_5 = '5', NUM_6 = '6', NUM_7 = '7', NUM_8 = '8', NUM_9 = '9',
        SPACE = ' ', BKSP = '\b', ENTER = '\r', ESC = '\x1B', DASH = '-', MINUS = '-', PLUS = '+', TAB = '\t', SEMICOLIN = ';'
    };
    typedef enum ARROW_KEYS {
        RIGHT_ARROW = 0x4f, LEFT_ARROW = 0x50, DOWN_ARROW = 0x51, UP_ARROW = 0x52
    };

    typedef enum MOUSE_EVENTS {
        LEFT_CLICK = 0x01, SCROLL_BTN_CLICK = 0x02, RIGHT_CLICK = 0x03
    };

    //int i = SDLK_0;

    struct keyState {
        bool pressed = false;
    };

    struct MouseButtonsState : public keyState {
        bool released = true;
    };

    struct Texture {
        vector2i topLeft;
        vector2i bottomRight;
        vector2i size;
        SDL_Texture* pixelTexture = nullptr;
        Uint32* pixels = nullptr;

        Texture(vector2i topLeft, vector2i bottomRight) {
            this->topLeft = topLeft;
            this->bottomRight = bottomRight;
            this->size = bottomRight - topLeft;
            this->pixels = new Uint32[size.x * size.y];
        }
        ~Texture() {
            delete[] pixels;
        }
        void ClearPixelTexture() {
            memset(pixels, 0, size.x * size.y * sizeof(Uint32));
        }
        void DrawPixelToTexture(uint32_t pixel, vector2i xy) {
            if (xy >= this->topLeft && xy < this->bottomRight) pixels[xy.y * size.x + xy.x] = pixel;
        }
        void DrawPixelToTexture(Color color, vector2i pos) {
            uint32_t pixel = (color.a << 24) | (color.x << 16) | (color.y << 8) | (color.z);
            DrawPixelToTexture(pixel, pos);
        }
        void DrawPixelToTexture(ColorF color, vector2i pos) {
            DrawPixelToTexture(Color((int)color.x, (int)color.y, (int)color.z, (int)color.a), pos);
        }
    };

    struct RectangleShapeTexture : public Texture {
        using Texture::Texture;
    };

    /*
        SDL Graphics Engine Implementation
    */

    class SDL_GraphicsEngine {
    public:
        SDL_GraphicsEngine() {}
        ~SDL_GraphicsEngine() {
            winRenderer.deleteSelf();
        };
    private:
        struct renderPack {
            SDL_Renderer* renderer = nullptr;
            SDL_Window* window = nullptr;
            SDL_Texture* pixelTexture = nullptr;
            Uint32* pixels = nullptr;
            TTF_Font* font = nullptr;
            SDL_Event event;
            bool deleteSelf() {

                SDL_DestroyWindow(this->window);
                TTF_CloseFont(this->font);

                delete[] this->pixels;
                SDL_DestroyTexture(this->pixelTexture);
                SDL_DestroyRenderer(this->renderer);
                SDL_Quit();

                return true;
            }
            bool createSelf(uint32_t windowWidth, uint32_t windowHeight, std::string appName) {
                SDL_SetMainReady();
                SDL_Init(SDL_INIT_VIDEO);
                SDL_CreateWindowAndRenderer(windowWidth, windowHeight, 0, &this->window, &this->renderer);
                TTF_Init();
                this->font = TTF_OpenFont("./RobotoRegular.ttf", 13);
                SDL_SetWindowTitle(this->window, appName.c_str());

                this->pixelTexture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, windowWidth, windowHeight);
                this->pixels = new Uint32[windowWidth * windowHeight];

                return true;
            }
            bool createSurface() {

            }

        };
        renderPack winRenderer;
    private:
        struct buttons {
            std::unordered_map<int, keyState> keyStates;
            void resetKeys() {
                for (auto& x : keyStates) {
                    if (x.first <= 0x03)
                        continue;

                    x.second.pressed = false;
                }
            }
        };
        struct mouseButtons {
            std::unordered_map<short, MouseButtonsState> keyStates;
            vector2i mousePOS;
            bool scrolledUp = false;
            bool scrolledDown = false;
            void resetStates() {
                this->scrolledDown = false;
                this->scrolledUp = false;
            }
        };
    public:
        bool done = false;
    protected:
        std::string         appName;
        buttons             buttonStates;
        mouseButtons        mouseButtonStates;
        uint32_t            windowWidth = 0;
        uint32_t            windowHeight = 0;
    private:
        high_resolution_clock::time_point start;
        high_resolution_clock::time_point finish;
    public:
        virtual void OnUserCreate() = 0;
        virtual bool OnUserUpdate(float elaspedTimeMS) = 0;
        virtual bool OnUserDestroy() = 0;
    public:
        void ConstructWindow(uint32_t width, uint32_t height) {
            windowWidth = width;
            windowHeight = height;
            OnUserCreate();
            winRenderer.createSelf(windowWidth, windowHeight, appName);
            mainThread();
        }
    private:
        bool keyListener() {
            if (SDL_PollEvent(&winRenderer.event) == 0) return false;
            if (winRenderer.event.type == SDL_QUIT) return true;
            if (winRenderer.event.type == SDL_KEYDOWN) {
                buttonStates.keyStates[winRenderer.event.key.keysym.sym].pressed = true;
                int i = winRenderer.event.key.keysym.sym;
                int keycodes = i;
                int arrow_keyMask = 0x000000ff;
                keycodes = i & arrow_keyMask;
                //79 right, 80 left, 81 down, 82 up
                if (keycodes >= 0x46 && keycodes <= 0x52) {
                    buttonStates.keyStates[keycodes].pressed = true;
                }
            }
            else if (winRenderer.event.type == SDL_MOUSEBUTTONDOWN) {
                mouseButtonStates.keyStates[winRenderer.event.button.button].pressed = true;
                mouseButtonStates.keyStates[winRenderer.event.button.button].released = false;
            }
            else if (winRenderer.event.type == SDL_MOUSEBUTTONUP) {
                mouseButtonStates.keyStates[winRenderer.event.button.button].pressed = false;
                mouseButtonStates.keyStates[winRenderer.event.button.button].released = true;
            }
            else if (winRenderer.event.type == SDL_MOUSEMOTION) {
                mouseButtonStates.mousePOS.setXY(winRenderer.event.button.x, winRenderer.event.button.y);
            }
            else if (winRenderer.event.type == SDL_MOUSEWHEEL) {
                if (winRenderer.event.wheel.y > 0) {
                    mouseButtonStates.scrolledUp = true;
                }
                else if (winRenderer.event.wheel.y < 0) {
                    mouseButtonStates.scrolledDown = true;
                }
            }
            return false;
        }
    private:
        void mainThread() {
            finish = high_resolution_clock::now();

            while (!keyListener()) {
                float timeElaspedMilli = duration_cast<microseconds>(this->finish - this->start).count() / 1000.f;
                start = high_resolution_clock::now();
                if (!OnUserUpdate(timeElaspedMilli)) {
                    break;
                }
                buttonStates.resetKeys();
                mouseButtonStates.resetStates();
                finish = high_resolution_clock::now();
            }
            OnUserDestroy();
        }
    protected:
        Color AdjustColorSat(Color colorVector, float f, float r_bias = 0.3, float g_bias = 0.6, float b_bias = 0.1) {
            float r = (float)colorVector.x;
            float g = (float)colorVector.y;
            float b = (float)colorVector.z;

            float L = r_bias * r
                + g_bias * g
                + b_bias * b;

            float new_r = r + f * (L - r);
            float new_g = g + f * (L - g);
            float new_b = b + f * (L - b);
            return Color((int)new_r, (int)new_g, (int)new_b, colorVector.a);
        };
    protected:
        keyState GetKey(char keyName) {
            return buttonStates.keyStates[keyName];
        }
        MouseButtonsState GetMouseState(short _MOUSE_EVENTS) {
            if (_MOUSE_EVENTS <= MOUSE_EVENTS::RIGHT_CLICK) {
                return mouseButtonStates.keyStates[_MOUSE_EVENTS];
            }
        }
    protected:
        void SetDrawColor(Color color) {
            SDL_SetRenderDrawColor(winRenderer.renderer, color.x, color.y, color.z, color.a);
        }
        void ClearScreen(Color color) {
            SetDrawColor(color);
            SDL_RenderClear(winRenderer.renderer);
        }
        void RenderScreen() {
            SDL_RenderPresent(winRenderer.renderer);
        }

    protected:
        void DrawTexture(Texture& texture) {
            texture.pixelTexture = SDL_CreateTexture(winRenderer.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, texture.size.x, texture.size.y);
            SDL_UpdateTexture(texture.pixelTexture, NULL, texture.pixels, texture.size.x * sizeof(Uint32));
            SDL_RenderCopy(winRenderer.renderer, texture.pixelTexture, NULL, NULL);
            SDL_DestroyTexture(texture.pixelTexture);
        }

        void DrawPixel(Color color, int x, int y) {
            SDL_RenderDrawPoint(winRenderer.renderer, x, y);
        }
        void DrawPixel(Color color, uint32_t x, uint32_t y) {
            DrawPixel(color, x, y);
        }
        void DrawPixel(ColorF color, uint32_t x, uint32_t y) {
            DrawPixel(Color((int)color.x, (int)color.y, (int)color.z, (int)color.a), x, y);
        }
        void DrawPixel(ColorF color, int x, int y) {
            DrawPixel(Color((int)color.x, (int)color.y, (int)color.z, (int)color.a), x, y);
        }

        void DrawLine(Color color, float x, float y, float x2, float y2) {
            SetDrawColor(color);
            SDL_RenderDrawLineF(winRenderer.renderer, x, y, x2, y2);
        }
        void DrawLine(Color color, vector2i start, vector2i end) {
            DrawLine(color, (float)start.x, (float)start.y, (float)end.x, (float)end.y);
        }
        void DrawLine(Color color, vector2i32t start, vector2i32t end) {
            DrawLine(color, (float)start.x, (float)start.y, (float)end.x, (float)end.y);
        }
        void DrawLine(Color color, vector2f start, vector2f end) {
            DrawLine(color, start.x, start.y, end.x, end.y);
        }

        void DrawRectOutline(Color color, float x, float y, float x2, float y2) {
            SetDrawColor(color);
            SDL_FRect rect = { x, y, x2, y2 };
            SDL_RenderDrawRectF(winRenderer.renderer, &rect);
        }
        void DrawRectOutline(Color color, vector2i pos, vector2i size) {
            DrawRectOutline(color, (float)pos.x, (float)pos.y, (float)size.x, (float)size.y);
        }
        void DrawRectOutline(Color color, vector2i32t pos, vector2i32t size) {
            DrawRectOutline(color, (float)pos.x, (float)pos.y, (float)size.x, (float)size.y);
        }
        void DrawRectOutline(Color color, vector2f pos, vector2f size) {
            DrawRectOutline(color, pos.x, pos.y, size.x, size.y);
        }

        void DrawRectFill(Color color, float x, float y, float x2, float y2) {
            SetDrawColor(color);
            SDL_FRect rect = { x, y, x2, y2 };
            SDL_RenderFillRectF(winRenderer.renderer, &rect);
        }
        void DrawRectFill(Color color, vector2i pos, vector2i size) {
            DrawRectFill(color, (float)pos.x, (float)pos.y, (float)size.x, (float)size.y);
        }
        void DrawRectFill(Color color, vector2i32t pos, vector2i32t size) {
            DrawRectFill(color, (float)pos.x, (float)pos.y, (float)size.x, (float)size.y);
        }
        void DrawRectFill(Color color, vector2f pos, vector2f size) {
            DrawRectFill(color, pos.x, pos.y, size.x, size.y);
        }

        void DrawRoundedRectFill(Color color, int posx, int posy, int sizex, int sizey, int radius) {
            SetDrawColor(color);
            if (sizex % 2 == 0) sizex = sizex - 1;
            if (sizey % 2 == 0) sizey = sizey - 1;

            int x0 = (int)posx + (sizex / 2);
            int y0 = (int)posy + (sizey / 2);
            int x = radius - 1;
            int y = 0;
            int dx = 1;
            int dy = 1;
            int err = dx - (radius << 1);
            while (x >= y) {
                SDL_RenderDrawLine(winRenderer.renderer, x0 + y + (sizex / 2) - radius, y0 + x + (sizey / 2) - radius, x0 - y - (sizex / 2) + radius, y0 + x + (sizey / 2) - radius);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + x + (sizex / 2) - radius, y0 + y + (sizey / 2) - radius, x0 - x - (sizex / 2) + radius, y0 + y + (sizey / 2) - radius);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + x + (sizex / 2) - radius, y0 - y - (sizey / 2) + radius, x0 - x - (sizex / 2) + radius, y0 - y - (sizey / 2) + radius);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + y + (sizex / 2) - radius, y0 - x - (sizey / 2) + radius, x0 - y - (sizex / 2) + radius, y0 - x - (sizey / 2) + radius);
                if (err <= 0)
                {
                    y++;
                    err += dy;
                    dy += 2;
                }
                if (err > 0)
                {
                    x--;
                    dx += 2;
                    err += dx - (radius << 1);
                }
            }
            SDL_Rect rect = { posx + 1, posy + radius, sizex - 2, sizey - (radius * 2) };
            SDL_RenderFillRect(winRenderer.renderer, &rect);
        }
        void DrawRoundedRectFill(Color color, vector2i pos, vector2i size, int radius) {
            DrawRoundedRectFill(color, pos.x, pos.y, size.x, size.y, radius);
        }


        //https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
        void DrawCircleOutline(Color color, int x0, int y0, int radius) {
            SetDrawColor(color);
            int x = radius - 1;
            int y = 0;
            int dx = 1;
            int dy = 1;
            int err = dx - (radius << 1);
            while (x >= y) {
                SDL_RenderDrawPoint(winRenderer.renderer, x0 + x, y0 + y);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 + y, y0 + x);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 - y, y0 + x);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 - x, y0 + y);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 - x, y0 - y);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 - y, y0 - x);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 + y, y0 - x);
                SDL_RenderDrawPoint(winRenderer.renderer, x0 + x, y0 - y);
                if (err <= 0)
                {
                    y++;
                    err += dy;
                    dy += 2;
                }

                if (err > 0)
                {
                    x--;
                    dx += 2;
                    err += dx - (radius << 1);
                }
            }
        }
        void DrawCircleOutline(Color color, vector2i pos, int radius) {
            DrawCircleOutline(color, pos.x, pos.y, radius);
        }

        void DrawCircleFill(Color color, int x0, int y0, int radius) {
            SetDrawColor(color);
            int x = radius - 1;
            int y = 0;
            int dx = 1;
            int dy = 1;
            int err = dx - (radius << 1);
            while (x >= y) {
                SDL_RenderDrawLine(winRenderer.renderer, x0 + y, y0 + x, x0 - y, y0 + x);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + x, y0 + y, x0 - x, y0 + y);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + x, y0 - y, x0 - x, y0 - y);
                SDL_RenderDrawLine(winRenderer.renderer, x0 + y, y0 - x, x0 - y, y0 - x);
                if (err <= 0)
                {
                    y++;
                    err += dy;
                    dy += 2;
                }

                if (err > 0)
                {
                    x--;
                    dx += 2;
                    err += dx - (radius << 1);
                }
            }
        }
        void DrawCircleFill(Color color, vector2i pos, int radius) {
            DrawCircleFill(color, pos.x, pos.y, radius);
        }

        void DrawString(uint8_t r, uint8_t g, uint8_t b, std::string& text, int x, int y) {
            SDL_Surface* surface = TTF_RenderText_Solid(winRenderer.font, text.c_str(), { r, g, b });
            SDL_Texture* texture = SDL_CreateTextureFromSurface(winRenderer.renderer, surface);
            SDL_Rect rect = { x, y, surface->w, surface->h };
            SDL_RenderCopy(winRenderer.renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        void DrawString(Color color, std::string text, vector2i pos) {
            DrawString(color.x, color.y, color.z, text, pos.x, pos.y);
        }

        void DrawTriangleFill(float ax, float ay, float bx, float by, float cx, float cy, Color a, Color b, Color c) {
            const std::vector<SDL_Vertex> verts =
            {
                { SDL_FPoint{ ax, ay }, SDL_Color{ a.x, a.y, a.z, a.a }, SDL_FPoint{ 0 }, },
                { SDL_FPoint{ bx, by }, SDL_Color{ b.x, b.y, b.z, b.a }, SDL_FPoint{ 0 }, },
                { SDL_FPoint{ cx, cy }, SDL_Color{ c.x, c.y, c.z, c.a }, SDL_FPoint{ 0 }, },
            };
            SDL_RenderGeometry(winRenderer.renderer, nullptr, verts.data(), verts.size(), nullptr, 0);
        }
        void DrawTriangleFill(vector2f apos, vector2f bpos, vector2f cpos, Color a, Color b, Color c) {
            DrawTriangleFill(apos.x, apos.y, bpos.x, bpos.y, cpos.x, cpos.y, a, b, c);
        }
        void DrawTriangleFill(Color color, vector2f apos, vector2f bpos, vector2f cpos) {
            DrawTriangleFill(apos.x, apos.y, bpos.x, bpos.y, cpos.x, cpos.y, color, color, color);
        }

        void DrawTriangleOutline(Color color, float ax, float ay, float bx, float by, float cx, float cy) {
            SetDrawColor(color);
            SDL_RenderDrawLineF(winRenderer.renderer, ax, ay, bx, by);
            SDL_RenderDrawLineF(winRenderer.renderer, ax, ay, cx, cy);
            SDL_RenderDrawLineF(winRenderer.renderer, cx, cy, bx, by);
        }
        void DrawTriangleOutline(Color color, vector2f a, vector2f b, vector2f c) {
            DrawTriangleOutline(color, a.x, a.y, b.x, b.y, c.x, c.y);
        }

        void DrawGeometry(std::vector<SDL_Vertex> mesh) {
            SDL_RenderGeometry(winRenderer.renderer, nullptr, mesh.data(), mesh.size(), nullptr, 0);
        }
    };
}