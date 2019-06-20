#pragma once
#include "CImg.h"
#include <vector>
#include <string>
using namespace cimg_library;
/*
================================================
#  ___________ _                _
# |___  / ____| |              | |
#    / / |    | |__   __ _ _ __| |_ ___ _ __
#   / /| |    | '_ \ / _` | '__| __/ _ \ '__|
#  / /_| |____| | | | (_| | |  | ||  __/ |
# /_____\_____|_| |_|\__,_|_|   \__\___|_|
#
#
#====================================================
# Main header file that can draw line and bar charts
# using the CImg library
#
# Colors are BYTE[3], in RGB values.
#
# Copyright (C) 2019 YOUR MOM GAY LOLOLOL
*/
namespace Faxon {
	namespace EPointShape {
		enum Enum {
			None = 0,
			FullCircle, // Filled circle
			OutCircle, // Outline of circle
			FullSquare // Filled rectangle

		};
	}
	using std::vector;
	
	// Assistant function to assign a color
	void ascol(BYTE in[3], const BYTE& red, const BYTE& green, const BYTE& blue);

	void DrawThickLine(cimg_library::CImg<BYTE>& image,
		const int x1, const int y1,
		const int x2, const int y2,
		const BYTE* const color,
		const unsigned int line_width);

	// Assistant function, returns % of in
	// perc is from 0 - 100 or more
	int percof(const int& in, const int& perc);

	// Theme that controls the chart background
	struct Theme
	{
		// Generic data color
		BYTE gdcolor[3];

		// Background color
		BYTE bgcolor[3];

		float guidelineopacity;

		// Color of the vertical and horizontal guide lines
		BYTE guidelinecolor[3];

		// Color of the border lines
		BYTE bordercolor[3];

		// Thickness of the border lines
		unsigned borderthick;

		// Size of the font
		int fsize;
		BYTE textcolor[3];

		// Show data labels?
		bool datalabels;

	
	};
	// Represents a single bar
	struct Bar {
		Bar() {
			value = 0;
			ascol(color, 0, 0, 255);
		}
		Bar(const int& val, const BYTE& r, const BYTE& g, const BYTE& b) {
			value = val;
			ascol(color, r, g, b);


		}
		int value;
		BYTE color[3];
	};
	// Represents a bar item made out of one or multiple stacked bars
	struct BarItem 
	{
		void UTotalValue() {
			tval = 0;
			auto Iter = Bars.begin();
			while (Iter != Bars.end())
			{
				tval += Iter->value;

				++Iter;
			}

		};
		BarItem()
		{
			tval = 0;
			Name = "NULL";
		}
		BarItem(const std::string& name,const vector<Bar>& cont) {
			Bars = cont;
			Name = name;
			UTotalValue();

		}
		BarItem(const std::string& name, Faxon::Bar& inb) {
			Bars.push_back(inb);
			tval = inb.value;
			Name = name;

		}
		int tval;
		vector<Bar> Bars;
		std::string Name;
		
		

	};
	struct LinePoint {
		int val;
		std::string label;

		LinePoint()
		{
			val = 0;
			label = "";
		}
		LinePoint(int inv,const std::string& lb)
		{
			val = inv;
			label = lb;
		
		}
		operator int() { return val; }
	};
	// Represents a line item containing value points
	struct LineItem {
		std::string Name;
		vector<int> Vals;
		BYTE color[3];
		EPointShape::Enum Point;
		int pointsz;

		LineItem() {
			Name = "";
		
		}
		LineItem(const std::string& inname,const std::vector<int>& invals,EPointShape::Enum ep,BYTE r,BYTE g,BYTE b,int ipsz = 3)
		{
			Name = inname;
			Vals = invals;

			ascol(color, r, g, b);
			Point = ep;
			pointsz = ipsz;
		}
	

	};

	// Assistant overridable class that formats numbers, insert your own if you want
	class Formatter {
	public:
		virtual std::string FormatNum(const int& num);
		

	};

	// Legend entry
	struct LegendEntry {
		BYTE color[3];
		std::string name;

	};

	struct Index {
		int val;
		int y;
	};
	// ZCharter: Main chart class
	class ZCharter
	{
	private:
		// Get width of font
		int GetFontSz(const std::string& txt, const int& cus);
		// Image is 3 channel RGB, alpha channel is for gays
		CImg<BYTE> Image;
	
		Theme CuTheme;
		Formatter DefaultFormatter;
		Formatter* CuFormatter;
		
		vector<LegendEntry> Legend;
		vector<std::string> Notes;
		int ydepth;

		void InitGenericTheme();
		
		void DrawTxt(const int& x0, const int& y0, const std::string& str, BYTE col[3], const int& fsize, const float& opacity = 1.f);
		void DrawLegend(const int& lbx, const int& lby);

		void DrawChartArea(const int& latbx, const int& lowby);
		void DrawChartArea(const int& latbx, const int& lowby,const int& xdest);

		void DrawYStretchLine(int x1, int y1, int x2, int y2, BYTE col[3],int thick = 3, float opacity = 1.f);
	public:
		void SetTheme(const Theme& Tm);
		void Init(const int& width, const int& height);
		void SetDepth(const int& nydepth);
		inline void SetNotes(const vector<std::string>& nts) { Notes = nts; }
		inline void AddNote(const std::string& ant) { Notes.push_back(ant); }
		// Set the legend, which will be drawn on the BuildXXXcharts functions.
		void SetLegend(vector<LegendEntry>& Leg);

		// Set a custom number formatter, note that the one calling has the responsibility of handling that the pointer is valid
		void SetFormatter(Formatter* inpForm);
		// Makes a bar chart, use GetImage() after calling this to get it
		void BuildBarChart(vector<BarItem>& Items);

		// Makes a line chart containing one or multiple lines, use GetImage() after calling this.
		void BuildLineChart(vector<LineItem>& Items, const vector<std::string>& XLabels,const unsigned& lwidth = 3);

		CImg<BYTE>& GetImage() { return Image; }
		ZCharter();
		~ZCharter();
	};


	
};

