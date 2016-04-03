# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include <numeric>

struct GameData
{
	Image image;
	DynamicTexture texture;
	Array<uint32> pixels;
	double distance = 0;
	size_t index = 0;
	Stopwatch stopwatch;

	void reset()
	{
		const Size size = Window::Size();
		image.resize(size);
		image.fill(Palette::Black);
		texture.fill(image);
		pixels.resize(pixelCount());
		std::iota(pixels.begin(), pixels.end(), 0);
		Shuffle(pixels);
		distance = 0;
		index = 0;
		stopwatch.restart();
	}

	void update()
	{
		if (index >= pixelCount())
		{
			return;
		}

		distance += Mouse::Pos().distanceFrom(Mouse::PreviousPos());

		if (distance >= 1)
		{
			while (distance >= 1)
			{
				distance -= 1;

				const size_t pixel = pixels[index++];

				image[0][pixel] = Palette::White;

				if (index >= pixels.size())
				{
					stopwatch.pause();
					break;
				}
			}

			texture.tryFill(image);
		}
	}

	Size size() const
	{
		return image.size;
	}

	uint32 pixelCount() const
	{
		return image.num_pixels;
	}
};

using MyApp = SceneManager<String, GameData>;

struct Game : MyApp::Scene
{
	void init() override
	{		
		m_data->reset();
	}

	void update() override
	{
		m_data->update();

		if (m_data->index >= m_data->pixelCount())
		{
			changeScene(L"Result", 2000);
		}
	}

	void draw() const override
	{
		Graphics::SetBackground(Palette::Black);
		m_data->texture.draw();
		FontAsset(L"Title")(L"WHITE").drawCenter(m_data->size() / 2, AlphaF((41000.0 - m_data->index)/40000));
	}
};

struct Result : MyApp::Scene
{
	void update() override
	{
		if (Input::MouseL.clicked)
			changeScene(L"Game", 2000);
	}

	void draw() const override
	{
		Graphics::SetBackground(Palette::White);
		const Point center(m_data->size() / 2);
		const ColorF color(0.5, 0.7, 1.0, 1.0);
		FontAsset(L"Title")(L"{} pixels"_fmt, m_data->pixelCount()).drawCenter(center.movedBy(0, -60), color);
		FontAsset(L"Time")(L"YOU HAVE LOST {} SECONDS"_fmt, m_data->stopwatch.s()).drawCenter(center.movedBy(0, 100), color);
	}
};

void Main()
{
	FontAsset::Register(L"Title", 100, Typeface::Heavy);
	FontAsset::Register(L"Time", 52, Typeface::Heavy);
	Window::SetFullscreen(true, Window::GetState().screenSize);
	MyApp manager;
	manager.setFadeColor(Palette::White);
	manager.add<Game>(L"Game");
	manager.add<Result>(L"Result");
	
	while (System::Update())
	{
		if (!manager.updateAndDraw())
			break;
	}
}
