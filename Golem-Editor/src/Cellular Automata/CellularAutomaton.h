#pragma once

namespace golem
{
	struct Grid
	{
		int width, height, depth = 2;
		std::vector<uint32_t> grid[2];
		int backBufferIndex = 0;

		Grid(int w, int h, int d)
			: width{w}, height{h}, depth{d}
		{
			grid[0].resize(width*height*depth);
			grid[1].resize(width*height*depth);
		}

		uint32_t& Get(int x, int y, int z, int buffer = -1)
		{
			if (buffer < 0) buffer = backBufferIndex;

			// x_offset = x
			// y_offset = width * y
			// z_offset = width * height * z

			return grid[buffer][x + (width*y) +(width*height*z)];
		}

		void Set(int x, int y, int z, uint32_t value, int buffer = -1)
		{
			if (buffer < 0) buffer = backBufferIndex;

			// x_offset = x
			// y_offset = width * y
			// z_offset = width * height * z

			grid[buffer][x + (width * y) + (width * height * z)] = value;
		}

		bool ValidX(int x) {return (x >= 0) && (x < width);}
		bool ValidY(int y) {return (y >= 0) && (y < height);}
		bool ValidZ(int z) {return (z >= 0) && (z < depth);}

		void FlipBuffer()
		{
			if(backBufferIndex == 1)	backBufferIndex = 0;
			else						backBufferIndex = 1;
		}

		int BackBuffer()
		{
			return backBufferIndex;
		}

		int FrontBuffer() 
		{
			if (backBufferIndex == 1)	return 0;
			else						return 1;
		}
	};

	class CellularAutomaton
	{
	public:
		using CellFunction = std::function<void(Grid* grid, int x, int y, int z)>;

		// Members ********************************************************************************
	private:
		CellFunction m_cellFunction;

		int m_stepsPerFrame = 1;

		Grid m_grid;

		// Methods ********************************************************************************
	public:
		CellularAutomaton(const CellFunction func, Grid& grid);
		CellularAutomaton(const CellFunction func, int width, int height, int depth)
			: CellularAutomaton(func,Grid(width,height,depth)) {}
		
		void Run();
		void RunSteps(int steps);

		void SetGrid(const Grid& grid);
		void SetGrid(int width, int height, int depth) {SetGrid(Grid(width,height,depth));}
		
		void SetStepsPerFrame(int steps) {m_stepsPerFrame = steps;}
		int GetStepsPerFrame() const {return m_stepsPerFrame;} 

		Grid& GetGrid() {return m_grid;}

		void Dump();
	private:
	};

	
}