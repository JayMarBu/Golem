#include "golpch.h"
#include "CellularAutomaton.h"

namespace golem
{

	CellularAutomaton::CellularAutomaton(const CellFunction func, Grid& grid)
		: m_cellFunction{func}, m_grid{std::move(grid)}
	{
		
	}

	void CellularAutomaton::Run()
	{
		RunSteps(m_stepsPerFrame);
	}

	void CellularAutomaton::RunSteps(int steps)
	{
		for (int z = 0; z < m_grid.depth; z++)
		{
			for (int y = 0; y < m_grid.height; y++)
			{
				for (int x = 0; x < m_grid.width; x++)
				{
					m_cellFunction(&m_grid, x,y,z);
				}
			}
		}

		m_grid.FlipBuffer();
	}

	void CellularAutomaton::SetGrid(const Grid& grid)
	{
		m_grid = grid;
	}

	void CellularAutomaton::Dump()
	{
		GOL_TRACE("Cellular Automaton Dump:\n\tGrid size: ({0},{1},{2})\n\tcurrent back buffer: {3}", m_grid.width, m_grid.height, m_grid.depth, m_grid.BackBuffer());

		int x_counter = 0;
		int y_counter = 0;

		int bufferIndex = m_grid.FrontBuffer();

		std::stringstream ss;

		for(int i = 0; i < m_grid.grid[bufferIndex].size(); i++)
		{
			if(x_counter >= m_grid.width)
			{
				x_counter = 0;
				GOL_TRACE("{0}", ss.str());
				ss.clear();
				ss.str("");
			}

			if(y_counter >= m_grid.width * m_grid.height)
			{
				y_counter = 0;
				GOL_TRACE("");
			}

			ss << m_grid.grid[bufferIndex][i] << ", ";

			x_counter++;
			y_counter++;
		}

		GOL_TRACE("{0}", ss.str());
	}

}