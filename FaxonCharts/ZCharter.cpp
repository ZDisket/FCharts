#include "ZCharter.h"
#include <sstream>
using namespace Faxon;

#define IF_THEN_EL(cond,cons,el) if (cond){cons;}else{el;}


void Faxon::ZCharter::DrawTxt(const int & x0, const int & y0, const std::string & str, BYTE col[3], const int & fsize, const float & opacity)
{
	Image.draw_text(x0, y0, str.c_str(), (BYTE*)col, fsize, opacity);

}
void ZCharter::InitGenericTheme()
{
	ascol(CuTheme.bgcolor, 255, 255, 255);
	ascol(CuTheme.bordercolor, 0, 0, 0);
	ascol(CuTheme.gdcolor, 0, 0, 255);
	ascol(CuTheme.guidelinecolor, 64, 64, 64);
	ascol(CuTheme.textcolor, 0, 0, 0);

	CuTheme.borderthick = 2;
	CuTheme.guidelineopacity = 0.4f;
	CuTheme.fsize = 12;
	CuTheme.datalabels = true;
}


void Faxon::ZCharter::DrawLegend(const int & lbx, const int & lby)
{
	if (!Legend.size())
		return;

	const int w = Image.width();
	const int h = Image.height();

	
	int xincr = lbx;
	int basey = lby + (CuTheme.fsize * 2) + 10;

	auto Lit = Legend.begin();
	while (Lit != Legend.end())
	{
		// Draw the color square

		// Upper left corner
		int ulcx = xincr;
		int ulcy = basey;

		// Lower right corner
		int lrcx = xincr + 10;
		int lrcy = ulcy + 10;


		Image.draw_rectangle(ulcx, ulcy, lrcx, lrcy, (BYTE*)&Lit->color, 1.f);

		// Now draw the color square  outline

		// From left upper corner to right upper corner
		Image.draw_line(ulcx, ulcy, lrcx, ulcy,(BYTE*)CuTheme.bordercolor,1.f);

		// From right upper corner to right lower corner
		Image.draw_line(lrcx, ulcy, lrcx, lrcy,(BYTE*)CuTheme.bordercolor,1.f);

		// From right lower corner to left lower corner
		Image.draw_line(lrcx, lrcy, ulcx, lrcy,(BYTE*)CuTheme.bordercolor,1.f);

		// From left lower corner to left upper corner
		Image.draw_line(ulcx, lrcy, ulcx, ulcy, (BYTE*)CuTheme.bordercolor, 1.f);

		// Draw the text
		int atx = lrcx + 4;
		int aty = ulcy - 1;

		DrawTxt(atx, aty, Lit->name, CuTheme.textcolor, CuTheme.fsize);

		// Square size, font size, separation
		xincr += 10 + (CuTheme.fsize * Lit->name.length()) + 5;
	
		++Lit;
	}
	

}

void Faxon::ZCharter::DrawChartArea(const int & latbx, const int & lowby)
{

	// fill it with background color
	Image.fill(CuTheme.bgcolor[0], CuTheme.bgcolor[1], CuTheme.bgcolor[2]);
	// Low border: Thick line from (latborx,lowbory) to (width,lowbory)
	DrawThickLine(Image, latbx, lowby, Image.width(), lowby, (const BYTE*)&CuTheme.bordercolor, CuTheme.borderthick);

	// Lateral border: Thick line from (latborx,lowbory) to (latborx,top)
	DrawThickLine(Image, latbx, lowby, latbx, 0, (const BYTE*)&CuTheme.bordercolor, CuTheme.borderthick);

}

void Faxon::ZCharter::DrawChartArea(const int & latbx, const int & lowby, const int & xdest)
{
	// fill it with background color
	Image.fill(CuTheme.bgcolor[0], CuTheme.bgcolor[1], CuTheme.bgcolor[2]);
	// Low border: Thick line from (latborx,lowbory) to (xdest,lowbory)
	DrawThickLine(Image, latbx, lowby, xdest, lowby, (const BYTE*)&CuTheme.bordercolor, CuTheme.borderthick);

	// Lateral border: Thick line from (latborx,lowbory) to (latborx,top)
	DrawThickLine(Image, latbx, lowby, latbx, 0, (const BYTE*)&CuTheme.bordercolor, CuTheme.borderthick);
}

void Faxon::ZCharter::DrawYStretchLine(int x1, int y1, int x2, int y2, BYTE col[3], int thick, float opacity)
{
	for (int t = 0; t < thick;t++)
	{
		Image.draw_line(x1, y1 - t, x2, y2 - t, (const BYTE*)col, opacity);
	
	}

}


void Faxon::ZCharter::SetTheme(const Theme & Tm)
{
	CuTheme = Tm;

}

void Faxon::ZCharter::Init(const int & width, const int & height)
{
	Image.assign(width, height, 1, 3);
	InitGenericTheme();
	ydepth = 10;
}

void Faxon::ZCharter::SetDepth(const int & nydepth)
{
	ydepth = nydepth;
}

void Faxon::ZCharter::SetLegend(vector<LegendEntry>& Leg)
{
	Legend = Leg;
}

void Faxon::ZCharter::SetFormatter(Formatter * inpForm)
{
	CuFormatter = inpForm;
}

void Faxon::ZCharter::BuildBarChart(vector<BarItem>& Items)
{
	if (!Items.size())
		throw std::exception("Tried to make empty chart!!!");



	// to help
	const int w = Image.width();
	const int h = Image.height();

	// Find the minimium and max values
	int min = Items[0].tval;
	int max = Items[0].tval;

	auto It = Items.begin();
	while (It != Items.end())
	{
		BarItem& Bar = *It;

		if (Bar.tval > max)
			max = Bar.tval;
		
		if (Bar.tval < min)
			min = Bar.tval;
	
		++It;
	}

	// Since we only use these values for the margins, expand the max a little
	max += percof(max, 20);

	// Build the borders

	// set the starting positions
	// Low border y = 20%
	int lowbory = h - percof(h, 20);
	
	// Lateral (left) border x = 15%
	int latborx = percof(w, 9);


	DrawChartArea(latborx,lowbory);

	// Build the lateral indicator labels
	// since there must be some space, we do font sz * 2

	int reqs = CuTheme.fsize * 2;
	// get how much space we can allocate with h
	int fincr = lowbory / ydepth;

	// As we go up, the Y decreases
	int whr = lowbory;

	// X coord of text
	int tx = latborx - CuTheme.fsize;

	// X starting coord of lateral guide line
	int x1lg = latborx - (CuTheme.fsize / 2);

	// Get the label numbers
	const int labinc = max / ydepth;
	int clabi = labinc;

	// Independent incrementor
	int fis = fincr;
	//clabi = fis / K;



	const double K = (double)fincr / (double)labinc;
	clabi = 0;
	for (int i = 0;i < ydepth;i++)
	{
	

		std::string txt = CuFormatter->FormatNum(clabi);
		Image.draw_text(latborx - (CuTheme.fsize * (txt.length() * 0.8)), whr - (CuTheme.fsize * 0.5), txt.c_str(), (BYTE*)&CuTheme.textcolor, CuTheme.fsize, 1.f);
		
		// Lateral guide lines
		Image.draw_line(x1lg, whr, w, whr, (BYTE*)CuTheme.guidelinecolor, 1.f);

		clabi = fis / K;

		whr -= fincr;

		fis += fincr;

	
	}
	// Now build the bars

	// The constant for converting value to top Y

	// Width of the chart area
	int cawidth = w - latborx;
	// Calculate the possible X width of the bars, keeping in mind a 10% sepa
	int barw = percof(cawidth,90) / Items.size();

	// Take off 10% to separate some
	int realbarw = percof(barw, 90);
	
	int basex = latborx + percof(latborx,5);
	int cbasex = basex;
	It = Items.begin();
	
	while (It != Items.end())
	{
		BarItem& Bar = *It;

		// Only one bar, we can be simple
		if (Bar.Bars.size() == 1)
		{
			// Calculate the coordinates
			// Upper left corner 
			int remr = K * Bar.tval;

			int ulcx = cbasex;
			int ulcy = lowbory - remr - CuTheme.borderthick;

			// Lower right corner
			int lrcx = cbasex + realbarw;
			int lrcy = lowbory - CuTheme.borderthick;


			Image.draw_rectangle(ulcx, ulcy, lrcx, lrcy, (BYTE*)Bar.Bars[0].color, 1.f);
			if (CuTheme.datalabels)
			{
				Image.draw_text(cbasex + (realbarw / 2), ulcy - (CuTheme.fsize), CuFormatter->FormatNum(Bar.tval).c_str(), (BYTE*)CuTheme.textcolor, CuTheme.fsize, 1.f);

			}
		
		}
		else 
		{
			// Controls how they will stack on top of eachother
			int yneg = 0;
		
			// Stack these bars on top of each other
			auto Mit = Bar.Bars.begin();
			while (Mit != Bar.Bars.end())
			{
				Faxon::Bar& B = *Mit;
				int rem = K * B.value;

				// Upper left corner
				int ulcx = cbasex;
				int ulcy = lowbory - rem - yneg - CuTheme.borderthick;

				// Lower right corner
				int lrcx = cbasex + realbarw;

				int lrcy = lowbory - yneg;

				// If this is the first item, we take into account the border thickness
				if (yneg == 0)
					lrcy -= CuTheme.borderthick;

			

				// add to the negator



				Image.draw_rectangle(ulcx, ulcy, lrcx, lrcy, (BYTE*)B.color, 1.f);
				yneg += rem;

				// Draw data labels for every one and then the total
				if (CuTheme.datalabels)
				{
					// Get the most contrasting color whether it's white or black
					int ir = (int)B.color[0]; int ig = (int)B.color[1]; int ib = (int)B.color[2];
					int br = ((int)B.color[0] + (int)B.color[1] + (int)B.color[2]) / 3 > 382 ? 0 : 1;

					// TODO: Shorten this to only one conditional macro
					IF_THEN_EL((ir + ig + ib) / 3 > 127, br = 0, br = 1);
					BYTE col;
					IF_THEN_EL(br == 0, col = 0, col = 255);

					// Draw the value text at the center of each square
					int xcenter = cbasex + (realbarw / 2);
					int ycenter = ulcy + (rem / 4);

					BYTE stdc[3] = { col,col,col };

					DrawTxt(xcenter, ycenter, CuFormatter->FormatNum(B.value), stdc, CuTheme.fsize);


					// This indicates that we're on the last value, where we draw the total text of
					// everything stacked
					if (Mit == Bar.Bars.end() - 1)
					{
						// xcenter doesn't change
						ycenter = ulcy - CuTheme.fsize;

						DrawTxt(xcenter, ycenter, CuFormatter->FormatNum(Bar.tval), CuTheme.textcolor, CuTheme.fsize);
					}


				}



				++Mit;
			}



		}
		

		// Draw the item text
		Image.draw_text(cbasex + (CuTheme.fsize * 0.9), lowbory + 0 + CuTheme.borderthick, Bar.Name.c_str(), (BYTE*)CuTheme.textcolor, CuTheme.fsize, 1.f);

			
		
	
	
		cbasex += barw;
		++It;
	}
	
	
	DrawLegend(latborx, lowbory);
}

void Faxon::ZCharter::BuildLineChart(vector<LineItem>& Items, const vector<std::string>& XLabels, const unsigned& lwidth )
{
	// Find the max

	if (!Items.size())
		throw std::exception("Tried to make empty chart!!!");



	// to help
	const int w = Image.width();
	const int h = Image.height();

	// Find the minimium and max values
	int min = Items[0].Vals[0];
	int max = Items[0].Vals[0];

	int maxsz = (int)Items[0].Vals.size();
	auto It = Items.begin();
	while (It != Items.end())
	{
		LineItem& Lin = *It;

		auto Lit = Lin.Vals.begin();
		while (Lit != Lin.Vals.end())
		{
			int& vl = *Lit;
			if (vl > max)
				max = *Lit;

			if (vl < min)
				min = *Lit;

			++Lit;
		}
		if ((int)Lin.Vals.size() > maxsz)
			maxsz = (int)Lin.Vals.size();
	
		++It;
	}
	// Since we only use these values for the margins, expand the max a little
	max += percof(max, 20);

	// Build the borders

	// set the starting positions
	// Low border y = 20%
	int lowbory = h - percof(h, 20);

	// Lateral (left) border x = 9%
	int latborx = percof(w, 9);



	// Build the lateral indicator labels
	// since there must be some space, we do font sz * 2

	int reqs = CuTheme.fsize * 2;
	// get how much space we can allocate with h
	int fincr = lowbory / ydepth;

	// As we go up, the Y decreases
	int whr = lowbory;

	// X coord of text
	int tx = latborx - CuTheme.fsize;

	// X starting coord of lateral guide line
	int x1lg = latborx - (CuTheme.fsize / 2);

	// Get the label numbers
	const int labinc = max / ydepth;
	int clabi = labinc;
	int fis = fincr;

	//clabi = fis / K;

	// We get the X constant calcs here since we need it for the length of the lateral guide lines
	const int cawidth = w - latborx;

	const int xinc = cawidth / maxsz;


	const int xbase = latborx + CuTheme.borderthick;
	const double XK = (double)xinc;



	const int xbor = xbase + (XK * (maxsz - 1));


	const double K = (double)fincr / (double)labinc;

	DrawChartArea(latborx, lowbory,xbor);


	clabi = 0;
	for (int i = 0; i < ydepth; i++)
	{


		std::string txt = CuFormatter->FormatNum(clabi);
		Image.draw_text(latborx - (CuTheme.fsize * (txt.length() * 0.8)), whr - (CuTheme.fsize * 0.5), txt.c_str(), (BYTE*)&CuTheme.textcolor, CuTheme.fsize, 1.f);

		// Lateral guide lines
		Image.draw_line(x1lg, whr, xbor, whr, (BYTE*)CuTheme.guidelinecolor, 1.f);

		clabi = fis / K;

		whr -= fincr;

		fis += fincr;


	}
	// Draw the X labels 
	int xdepth = maxsz;


	int curx = latborx;

	int laby = lowbory + CuTheme.borderthick + 3;
	// X constant


	for (int i = 0; i < xdepth; i++) 
	{

		// Short guide line
		Image.draw_line(curx, lowbory + 5 + CuTheme.borderthick, curx, lowbory - CuTheme.borderthick - 5, (BYTE*)CuTheme.guidelinecolor, 1.f);

		DrawTxt(curx - CuTheme.fsize, laby + (CuTheme.fsize / 4), XLabels[i], CuTheme.textcolor, CuTheme.fsize);


		curx += xinc;
	}

	// Draw right line
	DrawThickLine(Image, xbor, lowbory, xbor, 0, (const BYTE*)CuTheme.bordercolor, CuTheme.borderthick);
	
	// Now draw the lines
	It = Items.begin();

	while (It != Items.end())
	{
		LineItem& Lin = *It;


		auto Lit = Lin.Vals.begin();
		int lastx = -1;
		int lasty = -1;
		int idx = 0;

		while (Lit != Lin.Vals.end())
		{
			int& First = *Lit;
			++Lit;
			int& Sec = *Lit;

			


			int x1 = xbase + (XK * idx );
			int x2 = xbase + (XK * (idx + 1));

			int y1 = lowbory - CuTheme.borderthick - (K * First);
			int y2 = lowbory - CuTheme.borderthick - (K * Sec);

			
			// Connect
			if (lastx != -1 && lasty != -1)
			{
				DrawYStretchLine(lastx, lasty, x1, y1, Lin.color, lwidth);
				//DrawThickLine(Image, lastx, lasty, x1, y1, (const BYTE*)Lin.color, lwidth);


			}
			//DrawThickLine(Image, x1, y1, x2, y2, (const BYTE*)Lin.color, lwidth);
			DrawYStretchLine(x1, y1, x2, y2, Lin.color, lwidth);



			lastx = x2;
			lasty = y2;

			idx += 2;
			
			// Point shape draw handler

			if (Lin.Point != EPointShape::None)
			{
				if (Lin.Point == EPointShape::FullCircle)
				{
					Image.draw_circle(x1, y1 - (lwidth / 2), Lin.pointsz, (const BYTE*)Lin.color, 1.f);
					Image.draw_circle(x2, y2 - (lwidth / 2), Lin.pointsz , (const BYTE*)Lin.color, 1.f);

				}
				else if (Lin.Point == EPointShape::OutCircle) {
					Image.draw_circle(x1, y1 - (lwidth / 2), Lin.pointsz, (const BYTE*)Lin.color, 1.f, ~0U);
					Image.draw_circle(x2, y2 - (lwidth / 2), Lin.pointsz, (const BYTE*)Lin.color, 1.f, ~0U);

				}else if (Lin.Point == EPointShape::FullSquare){
					Image.draw_rectangle(x1, y1 + (Lin.pointsz / 2), x1 + Lin.pointsz, y1 - Lin.pointsz, (const BYTE*)Lin.color, 1.f);
					Image.draw_rectangle(x2, y2 + (Lin.pointsz / 2), x2 + Lin.pointsz, y2 - Lin.pointsz, (const BYTE*)Lin.color, 1.f);

				}
			
			
			}
			// Data label handler
			if (CuTheme.datalabels)
			{
			

			  DrawTxt(x1, y1 - CuTheme.fsize - lwidth, CuFormatter->FormatNum(First), CuTheme.textcolor, CuTheme.fsize);
			  DrawTxt(x2, y2 - CuTheme.fsize - lwidth, CuFormatter->FormatNum(Sec), CuTheme.textcolor, CuTheme.fsize);

			}
			
			++Lit;
			
		}

		++It;
	}

	DrawLegend(latborx,lowbory);

	// Draw another line
	
}

ZCharter::ZCharter()
{
	CuFormatter = &DefaultFormatter;
}


ZCharter::~ZCharter()
{
}

void Faxon::ascol(BYTE in[3], const BYTE & red, const BYTE & green, const BYTE & blue)
{
	in[0] = red;
	in[1] = green;
	in[2] = blue;
}

void Faxon::DrawThickLine(cimg_library::CImg<BYTE>& image, const int x1, const int y1, const int x2, const int y2, const BYTE * const color, const unsigned int line_width)
{
	if (x1 == x2 && y1 == y2) {
		return;
	}


	// Convert line (p1, p2) to polygon (pa, pb, pc, pd)
	const double x_diff = std::abs(x1 - x2);
	const double y_diff = std::abs(y1 - y2);
	const double w_diff = line_width / 2.0;

	const int x_adj = y_diff * w_diff / std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));
	const int y_adj = x_diff * w_diff / std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));

	// Points are listed in clockwise order, starting from top-left
	cimg_library::CImg<int> points(4, 2);
	points(0, 0) = x1 - x_adj;
	points(0, 1) = y1 + y_adj;
	points(1, 0) = x1 + x_adj;
	points(1, 1) = y1 - y_adj;
	points(2, 0) = x2 + x_adj;
	points(2, 1) = y2 - y_adj;
	points(3, 0) = x2 - x_adj;
	points(3, 1) = y2 + y_adj;

	image.draw_polygon(points, color);
}

int Faxon::percof(const int & in, const int & perc)
{
	float result = in * (float)(perc / 100.0);
	return (int)result;
}

std::string Faxon::Formatter::FormatNum(const int & num)
{
	std::ostringstream ss;
	ss << num;
	return ss.str();

}
