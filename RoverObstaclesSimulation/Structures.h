#pragma once


struct Coordinates {
	int X;
	int Y;
};


struct Obst
{
	Coordinates North, South, East, West;
};


struct ScanPt
{
	Coordinates Source, Goal, Igoal;
	//Coordinates ReachPtCords [4]; // coordinates of reach points
	int ScanPtNumsOfReachPts [4]; // stores scan point number of each of reach points
	int ScanPtNum; // number to identify a scan point
	int GoalPtNum; // number to identify Goal of current scan point
	int ReachPtAccessibility [10][10]; // identifies accessibility of each reach point
	int NumOfReachPts; // total number of reach points for a particular scan point
	int Visited; // indicates of the scan point is already visited
	int IsScanPointReachable; // indicates if the scan point can be reached
	int ObstacleNo; // identifies the obstacle which makes this point unreachbale
	int AltGoalPtNum; // alternate goal pt in case of known obstacles
	int Weightage; //Weightage for every ScanPoint. The ones visited more times get more Weightage
};

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX/96.0f;
		scaleY = dpiY/96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};