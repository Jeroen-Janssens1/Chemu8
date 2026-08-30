// This class specifically defines the graphics interface of the hardware / interpreter, unlike the renderer which is simply responsible for rendering the graphics onto the SDL window.
// Basically, main calls on CPU to process an opcode. If the opcode interacts with the Graphics it will do so through this class. The renderer class then reads the data in here to 
// print to the screen properly

// The screen is a 64x32 pixel display. Coordinates are determined in hexadecimal. Each pixel can be on or off, no colour information.

#pragma once
class Graphics
{
public:
	void SetPixel(unsigned char xCoord, unsigned char yCoord);

#ifdef _DEBUG
	void PrintScreenVals();
#endif

private:
	unsigned char m_Screen[2048]{}; // we use a char for each pixel instead of a single bit because using a single bit would require more effor to find correct indices.
						// basically, using bits is slower than using bools but decreases the size by 8. The size is however more than small enough that using chars is preferable here.
						// If you would try to run this on a very limited embedded system then you might consider using bitfields instead to save on memory space.
	const unsigned char m_ScreenWidth = 64;
	const unsigned char m_ScreenHeight = 32;
};

