#include <iostream>
#include <string> 
#include "SDL_GraphicsEngine.h"
#include <condition_variable>
#include <atomic>
#include <complex>
#include <cstdlib>
#include <immintrin.h>
#include <Windows.h>
#undef main

static const uint32_t WINDOW_WIDTH = 1280;
static const uint32_t WINDOW_HEIGHT = 720;
static const int maxNumberOfThreads = 64;

//Example usage / testing
//copied and adapted from : https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Mandelbrot.cpp
//Formal thank you
class MandleBrotSet : public blsp::SDL_GraphicsEngine {
public:
    MandleBrotSet() {
        appName = "MandleBrotSet";
    }
public:
	struct WorkerThread
	{
		std::condition_variable cvStart;
		blsp::vector2i pix_tl = { 0, 0 };
		blsp::vector2i pix_br = { 0, 0 };
		blsp::vector2db frac_tl = { 0, 0 };
		blsp::vector2db frac_br = { 0, 0 };
		int mode = 0; //0 mandlebrot, 1 julia
		int iterations = 0;
		bool alive = true;
		std::mutex mux;
		int screen_width = 0;
		int* fractal = nullptr;
		blsp::vector2db cpos = {0, 0};
		std::thread thread;


		void Start(const blsp::vector2i & ptl, const blsp::vector2i & pbr, const blsp::vector2db & ftl, const blsp::vector2db & fbr, const int it, const int md, blsp::vector2db cps)
		{
			cpos = cps;
			mode = md;
			pix_tl = ptl;
			pix_br = pbr;
			frac_tl = ftl;
			frac_br = fbr;
			iterations = it;
			std::unique_lock<std::mutex> lm(mux);
			cvStart.notify_one();
		}
		void createFractal() {
			while (alive)
			{
				switch (mode) {
				case 0: CreateFractalJuliaSetIntrin(); break;
				case 1: CreateFractalMandleIntrin(); break;
				}
				nWorkerComplete++;
			}
		}
		void CreateFractalMandleIntrin()
		{
				std::unique_lock<std::mutex> lm(mux);
				cvStart.wait(lm);

				double x_scale = (frac_br.x - frac_tl.x) / (double(pix_br.x) - double(pix_tl.x));
				double y_scale = (frac_br.y - frac_tl.y) / (double(pix_br.y) - double(pix_tl.y));

				double y_pos = frac_tl.y;

				int y_offset = 0;
				int row_size = screen_width;

				int x, y;

				__m256d _a, _b, _two, _four, _mask1;
				__m256d _zr, _zi, _zr2, _zi2, _cr, _ci;
				__m256d _x_pos_offsets, _x_pos, _x_scale, _x_jump;
				__m256i _one, _c, _n, _iterations, _mask2;

				_one = _mm256_set1_epi64x(1);
				_two = _mm256_set1_pd(2.0);
				_four = _mm256_set1_pd(4.0);
				_iterations = _mm256_set1_epi64x(iterations);

				_x_scale = _mm256_set1_pd(x_scale);
				_x_jump = _mm256_set1_pd(x_scale * 4);
				_x_pos_offsets = _mm256_set_pd(0, 1, 2, 3);
				_x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);


				for (y = pix_tl.y; y < pix_br.y; y++)
				{
					// Reset x_position
					_a = _mm256_set1_pd(frac_tl.x);
					_x_pos = _mm256_add_pd(_a, _x_pos_offsets);

					_ci = _mm256_set1_pd(y_pos);

					for (x = pix_tl.x; x < pix_br.x; x += 4)
					{
						_cr = _x_pos;
						_zr = _mm256_setzero_pd();
						_zi = _mm256_setzero_pd();
						_n = _mm256_setzero_si256();

					repeat:
						_zr2 = _mm256_mul_pd(_zr, _zr);
						_zi2 = _mm256_mul_pd(_zi, _zi);
						_a = _mm256_sub_pd(_zr2, _zi2);
						_a = _mm256_add_pd(_a, _cr);
						_b = _mm256_mul_pd(_zr, _zi);
						_b = _mm256_fmadd_pd(_b, _two, _ci);
						_zr = _a;
						_zi = _b;
						_a = _mm256_add_pd(_zr2, _zi2);
						_mask1 = _mm256_cmp_pd(_a, _four, _CMP_LT_OQ);
						_mask2 = _mm256_cmpgt_epi64(_iterations, _n);
						_mask2 = _mm256_and_si256(_mask2, _mm256_castpd_si256(_mask1));
						_c = _mm256_and_si256(_one, _mask2); // Zero out ones where n < iterations													
						_n = _mm256_add_epi64(_n, _c); // n++ Increase all n
						if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask2)) > 0)
							goto repeat;

						fractal[y_offset + x + 0] = int(_n.m256i_i64[3]);
						fractal[y_offset + x + 1] = int(_n.m256i_i64[2]);
						fractal[y_offset + x + 2] = int(_n.m256i_i64[1]);
						fractal[y_offset + x + 3] = int(_n.m256i_i64[0]);
						_x_pos = _mm256_add_pd(_x_pos, _x_jump);
					}

					y_pos += y_scale;
					y_offset += row_size;
				}
		}
		void CreateFractalJuliaSetIntrin()
		{
			std::unique_lock<std::mutex> lm(mux);
			cvStart.wait(lm);

			double x_scale = (frac_br.x - frac_tl.x) / (double(pix_br.x) - double(pix_tl.x));
			double y_scale = (frac_br.y - frac_tl.y) / (double(pix_br.y) - double(pix_tl.y));

			double y_pos = frac_tl.y;

			int y_offset = 0;
			int row_size = screen_width;

			int x, y;

			__m256d _a, _b, _two, _four, _mask1;
			__m256d _zr, _zi, _zr2, _zi2, _cr, _ci;
			__m256d _x_pos_offsets, _x_pos, _x_scale, _x_jump;
			__m256i _one, _c, _n, _iterations, _mask2;

			_cr = _mm256_set1_pd(cpos.x);
			_ci = _mm256_set1_pd(cpos.y);

			_one = _mm256_set1_epi64x(1);
			_two = _mm256_set1_pd(2.0);
			_four = _mm256_set1_pd(4.0);
			_iterations = _mm256_set1_epi64x(iterations);

			_x_scale = _mm256_set1_pd(x_scale);
			_x_jump = _mm256_set1_pd(x_scale * 4);
			_x_pos_offsets = _mm256_set_pd(0, 1, 2, 3);
			_x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);


			for (y = pix_tl.y; y < pix_br.y; y++)
			{
				// Reset x_position
				_a = _mm256_set1_pd(frac_tl.x);
				_x_pos = _mm256_add_pd(_a, _x_pos_offsets);

				//_ci = _mm256_set1_pd(y_pos);

				for (x = pix_tl.x; x < pix_br.x; x += 4)
				{
					//_cr = _x_pos;
					_zr = _x_pos;
					_zi = _mm256_set1_pd(y_pos);
					_n = _mm256_setzero_si256();

				repeat:
					_zr2 = _mm256_mul_pd(_zr, _zr);
					_zi2 = _mm256_mul_pd(_zi, _zi);
					_a = _mm256_sub_pd(_zr2, _zi2);
					_a = _mm256_add_pd(_a, _cr);
					_b = _mm256_mul_pd(_zr, _zi);
					_b = _mm256_fmadd_pd(_b, _two, _ci);
					_zr = _a;
					_zi = _b;
					_a = _mm256_add_pd(_zr2, _zi2);
					_mask1 = _mm256_cmp_pd(_a, _four, _CMP_LT_OQ);
					_mask2 = _mm256_cmpgt_epi64(_iterations, _n);
					_mask2 = _mm256_and_si256(_mask2, _mm256_castpd_si256(_mask1));
					_c = _mm256_and_si256(_one, _mask2); // Zero out ones where n < iterations													
					_n = _mm256_add_epi64(_n, _c); // n++ Increase all n
					if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask2)) > 0)
						goto repeat;

					fractal[y_offset + x + 0] = int(_n.m256i_i64[3]);
					fractal[y_offset + x + 1] = int(_n.m256i_i64[2]);
					fractal[y_offset + x + 2] = int(_n.m256i_i64[1]);
					fractal[y_offset + x + 3] = int(_n.m256i_i64[0]);
					_x_pos = _mm256_add_pd(_x_pos, _x_jump);
				}

				y_pos += y_scale;
				y_offset += row_size;
			}
		}

	};
public:
	static std::atomic<int> nWorkerComplete;
	int iterations = 64;
	int* pFractal = nullptr;
	blsp::Texture fractalTexture = blsp::Texture(blsp::vector2i(0, 0), blsp::vector2i(WINDOW_WIDTH, WINDOW_HEIGHT));
	// Pan & Zoom variables
	blsp::vector2db vOffset = { -1.5, -0.5 };
	blsp::vector2db vStartPan = { 0.0, 0.0 };
	blsp::vector2db vScale = { WINDOW_WIDTH / 2.0, WINDOW_HEIGHT };
	WorkerThread threadPool[maxNumberOfThreads];
	blsp::vector2db oldMousePOS = { 0, 0 };
	int zoomLevels = 0;
	int mode = 0;
	bool followMouse = true;
	int i = 0;
public:
	void InitialiseThreadPool()
	{
		for (int i = 0; i < maxNumberOfThreads; i++)
		{
			threadPool[i].alive = true;
			threadPool[i].fractal = pFractal;
			threadPool[i].screen_width = WINDOW_WIDTH;
			threadPool[i].thread = std::thread(&WorkerThread::createFractal, &threadPool[i]);
		}
	}
	void CreateFractalThreadPool(const blsp::vector2i & pix_tl, const blsp::vector2i & pix_br, const blsp::vector2db & frac_tl, const blsp::vector2db & frac_br, const int iterations)
	{
		int nSectionWidth = (pix_br.x - pix_tl.x) / maxNumberOfThreads;
		double dFractalWidth = (frac_br.x - frac_tl.x) / double(maxNumberOfThreads);
		nWorkerComplete = 0;

		for (size_t i = 0; i < maxNumberOfThreads; i++)
			threadPool[i].Start(
				blsp::vector2i(pix_tl.x + nSectionWidth * i, pix_tl.y),
				blsp::vector2i(pix_tl.x + nSectionWidth * (i + 1), pix_br.y),
				blsp::vector2db(frac_tl.x + dFractalWidth * double(i), frac_tl.y),
				blsp::vector2db(frac_tl.x + dFractalWidth * double(i + 1), frac_br.y),
				iterations, mode, oldMousePOS);


		while (nWorkerComplete < maxNumberOfThreads) // Wait for all workers to complete
		{
		}
	}
    void OnUserCreate() override {
        pFractal = (int*)_aligned_malloc(size_t(windowWidth) * size_t(WINDOW_HEIGHT) * sizeof(int), 128);
		InitialiseThreadPool();
    }
    bool OnUserDestroy() override {
        _aligned_free(pFractal);
        return true;
    }

	bool OnUserUpdate(float elaspedTimeMillS) override {
		if (GetKey(blsp::KEYBOARD::ESC).pressed) return false;

		ClearScreen(blsp::Color(0, 0, 0, 0));

		blsp::vector2db mousePOS = mouseButtonStates.mousePOS;

		if (GetMouseState(blsp::MOUSE_EVENTS::LEFT_CLICK).pressed)
		{
			//vStartPan = mousePOS;
			vOffset = vOffset - ((mousePOS - vStartPan) / vScale);
			vStartPan = mousePOS;
		}
		if (GetMouseState(blsp::MOUSE_EVENTS::LEFT_CLICK).released)
		{
			vStartPan = mousePOS;
		}

		blsp::vector2db vMouseBeforeZoom = ScreenToWorld(mousePOS);
		if (mouseButtonStates.scrolledUp || GetKey(blsp::W).pressed) {
			vScale *= 1.1;
			zoomLevels++;
		}
		if (mouseButtonStates.scrolledDown || GetKey(blsp::Q).pressed) {
			vScale *= .9;
			zoomLevels--;
		}

		blsp::vector2db vMouseAfterZoom = ScreenToWorld(mousePOS);
		vOffset = vOffset + (vMouseBeforeZoom - vMouseAfterZoom);

		blsp::vector2i pix_tl(0, 0);
		blsp::vector2i pix_br(windowWidth, windowHeight);
		blsp::vector2db frac_tl(-2.0, -1.0);

		if (mode == 0) {
			frac_tl = { -1.0, -1.0 };
		}
		blsp::vector2db frac_br(1.0, 1.0);

		frac_tl = ScreenToWorld(pix_tl);
		frac_br = ScreenToWorld(pix_br);

		if (GetKey(blsp::KEYBOARD::F).pressed) {
			iterations -= 16;
		}
		if (GetKey(blsp::KEYBOARD::R).pressed) {
			iterations += 16;
		}
		if (GetKey(blsp::KEYBOARD::NUM_1).pressed) {
			mode = 0;
		}
		if (GetKey(blsp::KEYBOARD::NUM_2).pressed) {
			mode = 1;
		}
		if (GetKey(blsp::KEYBOARD::X).pressed) {
			followMouse = !followMouse;
		}
		if (followMouse) oldMousePOS = vMouseBeforeZoom;

		CreateFractalThreadPool(pix_tl, pix_br, frac_tl, frac_br, iterations);
		
		for (int y = 0; y < windowHeight; y++)
		{
			for (int x = 0; x < windowWidth; x++)
			{
				int i = pFractal[y * windowWidth + x];
				float n = (float)i;
				float a = 0.1f;
				// Thank you @Eriksonn - Wonderful Magic Fractal Oddball Man
				fractalTexture.DrawPixelToTexture(blsp::ColorF(128 * sin(a * n) + 128, 0, 64 * cos(a * n) + 64, 64 * cos(a * n) + 64), blsp::vector2i(x, y));
			}
		}
		DrawTexture(fractalTexture);
		
		int i = 0;
		DrawString(blsp::BLUE, "MousePOS: " + std::to_string(mousePOS.x) + " " + std::to_string(mousePOS.y), blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, std::to_string(1000.f / elaspedTimeMillS), blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "Q/W : zoom in/out ", blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "F/R key : " + std::to_string(iterations), blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "1 key : toggle to Julia Set", blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "2 key : toggle to MandleBrot Set", blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "x key : toggle c = mousePOS", blsp::vector2i(10, i++ * 16));
		DrawString(blsp::BLUE, "zoom level : " + std::to_string(zoomLevels), blsp::vector2i(10, i++ * 16));
		if(mode == 0) DrawString(blsp::BLUE, "Julia Set at c = " + std::to_string(oldMousePOS.x) + " + (" + std::to_string(oldMousePOS.y * -1) + ")i", blsp::vector2i(10, i++ * 16));

		RenderScreen();
		return true;
	}

	blsp::vector2db ScreenToWorld(blsp::vector2i n)
	{
		blsp::vector2db v(0.0, 0.0);
		v.x = ((double)n.x / vScale.x) + vOffset.x;
		v.y = ((double)n.y / vScale.y) + vOffset.y;
		return v;
	}
};
std::atomic<int> MandleBrotSet::nWorkerComplete = 0;

class BasicShapesDemo : public blsp::SDL_GraphicsEngine {
public:
	BasicShapesDemo() {
		appName = "BasicShapesDemo";
	}
public:
	int i = 0;
	bool jiggletest = true;
	std::string keyboardtest = "";
	blsp::RoundedRectangle rect = blsp::RoundedRectangle(blsp::vector2f(0, 0), blsp::vector2f(600, 800), 12, 12, 100);
public:
	void OnUserCreate() override {
	}
	bool OnUserDestroy() override {
		return true;
	}
	bool OnUserUpdate(float elaspedTimeMS) override {
		//Sleep(100);
		ClearScreen(blsp::Color(0, 0, 0, 0));
		DrawPixel(blsp::LIME_GREEN, 50, 50);
		DrawRoundedRectFill(blsp::CYAN, 350, 350, 200, 350, 30);
		DrawLine(blsp::LIME_GREEN, blsp::vector2i(20, 20), blsp::vector2i(100, 120));
		DrawTriangleOutline(blsp::CYAN, 10, 10, 29, 19, 40, 50);
		if (i > 1280 - 200) jiggletest = false;
		if (i <= 0) jiggletest = true;
		if (jiggletest) {
			DrawRoundedRectFill(blsp::ORANGE, blsp::vector2f(i++ / 100, 50), blsp::vector2i(200, 100), 15);
		}
		else {
			DrawRoundedRectFill(blsp::ORANGE, blsp::vector2f(i-- / 100, 50), blsp::vector2i(200, 100), 15);
		}
		std::string time = std::to_string(1000.f / elaspedTimeMS);
		DrawString(blsp::RED, time, blsp::vector2i(0, 0));

		if (GetKey(blsp::A).pressed) keyboardtest += "a";
		if (GetKey(blsp::B).pressed) keyboardtest += "b";
		if (GetKey(blsp::C).pressed) keyboardtest += "c";
		if (GetKey(blsp::D).pressed) keyboardtest += "d";
		if (GetKey(blsp::E).pressed) keyboardtest += "e";
		if (GetKey(blsp::F).pressed) keyboardtest += "f";
		if (GetKey(blsp::G).pressed) keyboardtest += "g";
		if (GetKey(blsp::H).pressed) keyboardtest += "h";
		if (GetKey(blsp::I).pressed) keyboardtest += "i";
		if (GetKey(blsp::J).pressed) keyboardtest += "j";
		if (GetKey(blsp::K).pressed) keyboardtest += "k";
		if (GetKey(blsp::L).pressed) keyboardtest += "l";
		if (GetKey(blsp::M).pressed) keyboardtest += "m";
		if (GetKey(blsp::N).pressed) keyboardtest += "n";
		if (GetKey(blsp::O).pressed) keyboardtest += "o";
		if (GetKey(blsp::P).pressed) keyboardtest += "p";
		if (GetKey(blsp::Q).pressed) keyboardtest += "q";
		if (GetKey(blsp::R).pressed) keyboardtest += "r";
		if (GetKey(blsp::S).pressed) keyboardtest += "s";
		if (GetKey(blsp::T).pressed) keyboardtest += "t";
		if (GetKey(blsp::U).pressed) keyboardtest += "u";
		if (GetKey(blsp::V).pressed) keyboardtest += "v";
		if (GetKey(blsp::W).pressed) keyboardtest += "w";
		if (GetKey(blsp::X).pressed) keyboardtest += "x";
		if (GetKey(blsp::Y).pressed) keyboardtest += "y";
		if (GetKey(blsp::Z).pressed) keyboardtest += "z";

		calculateNextPoint2d(rect.position, rect.acceleration, elaspedTimeMS);

		DrawRoundedRectFill(blsp::MAGENTA, rect.position, rect.size, rect.radius);

		DrawString(blsp::RED, keyboardtest, blsp::vector2i(0, 10));

		RenderScreen();
		return true;
	}
};

int main() {
    //MandleBrotSet mdset;
    //mdset.ConstructWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	BasicShapesDemo shapeDemo;
	shapeDemo.ConstructWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	return 0;
}