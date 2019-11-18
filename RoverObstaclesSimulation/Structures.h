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

