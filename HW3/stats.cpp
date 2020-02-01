
#include "stats.h"

// hist[x][y][k]
stats::stats(PNG & im){
	for (unsigned int i=0; i<im.width(); i++) {
		// bins only for this column
		vector<int> bins (36,0);
		vector<vector<int>> histY;
		vector<double> hueX;
		vector<double> hueY;
		vector<double> sat;
		vector<double> lum;
		for (unsigned int j=0; j<im.height(); j++) {
			HSLAPixel* curr = im.getPixel(i,j);
			// curr->h is in deg, and cos operates in rad
			hueX.push_back(cos(curr->h*PI/180));
			hueY.push_back(sin(curr->h*PI/180));
			sat.push_back(curr->s);
			lum.push_back(curr->l);
			bins[curr->h/10]++;

			if(i == 0) {
				histY.push_back(bins);
			}
			else {
				// get the bins from the prev x column if there exists a prev x column
				vector<int> prev_bins = hist[i-1][j];
				vector<int> new_bins(36, 0);
				for(int k = 0; k < 36; k++) {
					new_bins[k] = prev_bins[k] + bins[k];
				}
				histY.push_back(new_bins);
			}
		}
		sumHueX.push_back(hueX);
		sumHueY.push_back(hueY);
		sumSat.push_back(sat);
		sumLum.push_back(lum);
		hist.push_back(histY);
	}
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
	long area = (lr.second - ul.second + 1) * (lr.first - ul.first + 1);
	return area;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
	double hx = 0;
	double hy = 0;
	double s = 0;
	double l = 0;
	for (int i=ul.first; i<=lr.first; i++) {
		for (int j=ul.second; j<=lr.second; j++) {
			hx += sumHueX[i][j];
			hy += sumHueY[i][j];
			s += sumSat[i][j];
			l += sumLum[i][j];
		}
	}
	hx /= rectArea(ul,lr);
	hy /= rectArea(ul,lr);
	s /= rectArea(ul,lr);
	l /= rectArea(ul,lr);
	double h = atan2(hy, hx)*180/PI;
	if(h < 0)
		h += 360;
	return HSLAPixel(h,s,l,1.0);
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
	// vector<int> ret (36,0);
	// for (int i=ul.first; i<=lr.first; i++) {
	// 	for (int j=ul.second; j<=lr.second; j++) {
	// 		for (int k=0; k<36; k++) {
	// 			ret[k] += hist[i][j][k];
	// 		}
	// 	}
	// }
	if(ul.first == 0 && ul.second == 0)
		return hist[lr.first][lr.second];
	// x is 0 and y is not 0
	else if(ul.first == 0) {
		vector<int> ret (36, 0);
		for	(int k = 0; k < 36; k++) {
			ret[k] = hist[lr.first][lr.second][k] - hist[lr.first][ul.second-1][k];
		}
		return ret;
	}
	// x is not 0 and y is 0
	else if(ul.second == 0) {
		vector<int> ret (36, 0);
		for	(int k = 0; k < 36; k++) {
			ret[k] = hist[lr.first][lr.second][k] - hist[ul.first-1][lr.second][k];
		}
		return ret;
	}
	else {
		vector<int> ret (36, 0);
		for	(int k = 0; k < 36; k++) {
			ret[k] = hist[lr.first][lr.second][k] - hist[lr.first][ul.second-1][k] - hist[ul.first-1][lr.second][k] + hist[ul.first-1][ul.second-1][k];
		}
		return ret;
	}
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;
    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }
    return  -1 * entropy;
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
	vector<int> distn = buildHist(ul,lr);
	int area = (int)rectArea(ul,lr);
	double ent = entropy(distn,area);
	return ent;
}
