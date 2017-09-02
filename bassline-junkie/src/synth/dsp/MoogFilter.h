/* moog filter clas by toast[AT]nowhere[DOT]com
from http://www.musicdsp.org/showone.php?id=24 */

class MoogFilter {
	public: MoogFilter();
	void init();
	void calc();
	double process(double x);
	~MoogFilter();
	double getCutoff();
	void setCutoff(double c);
	double getRes();
	void setRes(double r);
	
	protected:
	double cutoff;
	double res;
	double fs;
	double y1,y2,y3,y4;
	double oldx;
	double oldy1,oldy2,oldy3;
	double x;
	double r;
	double p;
	double k;
}; 
