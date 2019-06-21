// FaxonCharts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ZCharter.h"
#include <random>


int Random32(const INT32& in_iMin, const INT32& in_iMax)
{
	std::random_device rand_d;
	std::mt19937 gen(rand_d());
	std::uniform_int_distribution<int> distrib(in_iMin, in_iMax);


	return distrib(gen);

}


const static std::string cnames[10] = { "NZL","USA","GBR","AUS","BRA","PRC","RUS", "CAN","DRC","PRK" };
using namespace Faxon;

void TestBar() {
	vector<BarItem> bars;

	const int dos = 9;
	for (int i = 0; i < dos; ++i)
	{

		Bar Ba1(90 + (rand() % 10), 0, 0, 255);
		Bar Ba2(rand() % 50 + 5, 255, 255, 0);
		Bar Ba3(rand() % 50 + 5, 255, 0, 0);

		vector<Bar> DaVec;
		DaVec.push_back(Ba1); DaVec.push_back(Ba2); DaVec.push_back(Ba3);


		BarItem BI(cnames[i], DaVec);

		bars.push_back(BI);


	}
	
	LegendEntry Leg1;
	ascol(Leg1.color, 255, 0, 0);
	Leg1.name = "Ground";

	LegendEntry Leg2;
	ascol(Leg2.color, 255, 255, 0);
	Leg2.name = "Air";

	LegendEntry Leg3;
	ascol(Leg3.color, 0, 0, 255);
	Leg3.name = "Naval";
	
	vector<LegendEntry> Lege;
	Lege.push_back(Leg1);
	Lege.push_back(Leg2);
	Lege.push_back(Leg3);


	ZCharter Chart;
	Chart.Init(900, 350);
	Chart.SetLegend(Lege);
	Chart.AddNote("Press Delete to close");


	//	Chart.BuildLineChart(lis,lbls,6);
	Chart.BuildBarChart(bars);



	Chart.GetImage().display();

}
void TestLine() {
	vector<std::string> lbls = { " 01/01/01","01/01/31","23/31/22","2004","2005","2006","2007" };
	vector<LineItem> lis;
	for (int f = 0; f < 1; f++)
	{
		vector<int> vs;
		for (int i = 0; i < 7; i++)
		{
			vs.push_back(Random32(11000,13000));

		}
		LineItem Lix(cnames[f], vs, EPointShape::FullCircle,255,0,0);

		lis.push_back(Lix);
		srand(time(NULL));


	}



	LegendEntry Leg1;
	ascol(Leg1.color, lis[0].color[0], lis[0].color[1], lis[0].color[2]);
	Leg1.name = "United Kingdom";

	//LegendEntry Leg2;
	//Leg2.name = "United States";
//	ascol(Leg2.color, lis[1].color[0], lis[1].color[1], lis[1].color[2]);

	//LegendEntry Leg3;
	//Leg3.name = "Canada";
	//ascol(Leg3.color, lis[2].color[0], lis[2].color[1], lis[2].color[2]);

	vector<LegendEntry> Lege;
	Lege.push_back(Leg1);
	//Lege.push_back(Leg2);
//	Lege.push_back(Leg3);


	ZCharter Chart;
	Chart.Init(650, 340);
	Chart.SetLegend(Lege);
	Chart.AddNote("Press Delete to close");


		Chart.BuildLineChart(lis,lbls,3);



	Chart.GetImage().display();

}
int main()
{
    std::cout << "Hello World!\n"; 
	
	srand(time(NULL));

	
	
	srand(time(NULL));
	

	TestLine();

	return 0;
}

