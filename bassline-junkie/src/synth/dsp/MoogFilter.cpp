/* moog filter clas by toast[AT]nowhere[DOT]com
from http://www.musicdsp.org/showone.php?id=24 */

#include "MoogFilter.h"
#include "Stk.h"

MoogFilter::MoogFilter()
{
	fs=1./stk::Stk::sampleRate();
	init();
}
MoogFilter::~MoogFilter() { }

void MoogFilter::init()
{
	// initialize values
	y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
	calc();
};

void MoogFilter::calc()
{
	double f = (cutoff+cutoff) * fs;
	//[0 - 1]
	p=f*(1.8f-0.8f*f);
	k=p+p-1.f;
	double t=(1.f-p)*1.386249f;
	double t2=12.f+t*t;
	r = res*(t2+6.f*t)/(t2-6.f*t);
};

double MoogFilter::process(double input)
{
	// process input
	x = input - r*y4;
	//Four cascaded onepole filters (bilinear transform)
	y1= x*p + oldx*p - k*y1;
	y2=y1*p + oldy1*p - k*y2;
	y3=y2*p + oldy2*p - k*y3;
	y4=y3*p + oldy3*p - k*y4;
	//Clipper band limited sigmoid
	y4-=(y4*y4*y4)/6.f;
	oldx = x;
	oldy1 = y1;
	oldy2 = y2;
	oldy3 = y3;
	return y4;
}

double MoogFilter::getCutoff() { return cutoff; }

void MoogFilter::setCutoff(double c) { cutoff=c; calc(); }

double MoogFilter::getRes() { return res; }

void MoogFilter::setRes(double r) { res=r; calc(); }
