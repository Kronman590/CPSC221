
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 
/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */
	PNG * sub = new PNG(pow(2,k), pow(2,k));
	unsigned int imX = imIn.width()/2 - pow(2,(k-1));
	unsigned int imY = imIn.height()/2 - pow(2,(k-1));
	for (int i=0; i<(pow(2,k)); i++) {
		for (int j=0; j<(pow(2,k)); j++) {
			HSLAPixel * thisPix = sub->getPixel(i,j);
			*thisPix = *(imIn.getPixel(imX+i,imY+j));
		}
	}
	root = buildTree(sub, pow(2,k));
}

int toqutree::size() {
	int k = root->dimension;
	int ret = 0;
	int i = 0;
	while (k>=0) {
		ret += pow(4,i);
		k--;
		i++;
	}
	return ret;
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
// Note that you will want to practice careful memory use
// in this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.
	stats * stat = new stats(*im);
	HSLAPixel av = stat->getAvg(pair<int,int>(0,0),pair<int,int>(k-1,k-1));
	int mX = k/2;
	int mY = k/2;
	if (k>1) {
		double ent = findEntropyInit(*stat,k);
		int start = k/4;
		int end = 3*k/4;
		for (int i=start; i<end; i++) {
			for (int j=start; j<end; j++) {
				double ave = findEntropy(*stat,j,i,k);
				//cout<<i<<", "<<j<<": "<<ave<<endl;
				if (ave < ent) {
					mX = j;
					mY = i;
					ent = ave;
				}
			}
		}
	}
	else {
		mX = 0;
		mY = 0;
	}
	Node * ret = new Node(pair<int,int>(mX,mY),log2(k),av);
	if (k>1) {
		PNG * chNW = new PNG(k/2,k/2);
		PNG * chNE = new PNG(k/2,k/2);
		PNG * chSW = new PNG(k/2,k/2);
		PNG * chSE = new PNG(k/2,k/2);
		makeChild(chNW,chNE,chSW,chSE,*im,mX,mY,k/2);
		ret->NW = buildTree(chNW,k/2);
		ret->NE = buildTree(chNE,k/2);
		ret->SW = buildTree(chSW,k/2);
		ret->SE = buildTree(chSE,k/2);
	}
	delete stat;
	delete im;
	return ret;
}

double toqutree::findEntropyInit(stats stat,int k) {
	double ent = (stat.entropy(pair<int,int>(0,0),pair<int,int>(k/2-1,k/2-1))+
					 stat.entropy(pair<int,int>(k/2,0),pair<int,int>(k-1,k/2-1))+
					 stat.entropy(pair<int,int>(0,k/2),pair<int,int>(k/2-1,k-1))+
					 stat.entropy(pair<int,int>(k/2,k/2),pair<int,int>(k-1,k-1)));
	ent /= 4;
	return ent;
}

double toqutree::findEntropy(stats stat,int x,int y,int m) {
	//double NW = stat.entropy(pair<int,int>(x,y),pair<int,int>(m/2-1+x,m/2-1+y));
	vector<int> histNW;
	vector<int> histNE;
	vector<int> histSW;
	vector<int> histSE;
	if (m==2) {
		histNW = vector<int>(36,0);
		histNE = vector<int>(36,0);
		histSW = vector<int>(36,0);
		histSE = vector<int>(36,0);
		for (int r=0; r<36; r++) {
			histSE[r] = stat.hist[1][1][r];
			histSW[r] = stat.hist[0][1][r];
			histNE[r] = stat.hist[1][0][r];
			histNW[r] = stat.hist[0][0][r];
		}
	}
	else {
		histSE = getHist(stat,x,y,m);
		histSW = getHist(stat,x-m/2,y,m);
		histNE = getHist(stat,x,y-m/2,m);
		histNW = getHist(stat,x-m/2,y-m/2,m);
	}
	double NW = stat.entropy(histNW,(m/2)*(m/2));
	double NE = stat.entropy(histNE,(m/2)*(m/2));
	double SW = stat.entropy(histSW,(m/2)*(m/2));
	double SE = stat.entropy(histSE,(m/2)*(m/2));
	return (NW+NE+SW+SE)/4;
}

vector<int> toqutree::getHist(stats stat,int xLeft,int yUp,int m) {
	int xRight = xLeft+(m/2-1);
	int yBot = yUp+(m/2-1);
	vector<int> retHist(36,0);
	if (0<=xLeft && xRight<m && 0<=yUp && yBot<m)
		retHist = stat.buildHist(pair<int,int>(xLeft,yUp),pair<int,int>(xRight,yBot));
	else if (0<=xLeft && 0>yUp && xRight<m && yBot<m) {
		vector<int> histA(stat.buildHist(pair<int,int>(xLeft,0),pair<int,int>(xRight,yBot)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft,yUp+m),pair<int,int>(xRight,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r];
		}
	}
	else if (0>xLeft && 0<=yUp && xRight<m && yBot<m) {
		vector<int> histA(stat.buildHist(pair<int,int>(0,yUp),pair<int,int>(xRight,yBot)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft+m,yUp),pair<int,int>(m-1,yBot)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r];
		}
	}
	else if (0<=xLeft && 0<=yUp && xRight>=m && yBot<m) {
		vector<int> histA(stat.buildHist(pair<int,int>(0,yUp),pair<int,int>(xRight-m,yBot)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft,yUp),pair<int,int>(m-1,yBot)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r];
		}
	}
	else if (0<=xLeft && 0<=yUp && xRight<m && yBot>=m) {
		vector<int> histA(stat.buildHist(pair<int,int>(xLeft,0),pair<int,int>(xRight,yBot-m)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft,yUp),pair<int,int>(xRight,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r];
		}
	}
	else if (0<=xLeft && xRight>=m && 0<=yUp && yBot>=m) {
		vector<int> histA(stat.buildHist(pair<int,int>(0,0),pair<int,int>(xRight-m,yBot-m)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft,0),pair<int,int>(m-1,yBot-m)));
		vector<int> histC(stat.buildHist(pair<int,int>(0,yUp),pair<int,int>(xRight-m,m-1)));
		vector<int> histD(stat.buildHist(pair<int,int>(xLeft,yUp),pair<int,int>(m-1,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r]+histC[r]+histD[r];
		}
	}		
	else if (0>xLeft && xRight<m && 0>yUp && yBot<m) {
		vector<int> histA(stat.buildHist(pair<int,int>(0,0),pair<int,int>(xRight,yBot)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft+m,0),pair<int,int>(m-1,yBot)));
		vector<int> histC(stat.buildHist(pair<int,int>(0,yUp+m),pair<int,int>(xRight,m-1)));
		vector<int> histD(stat.buildHist(pair<int,int>(xLeft+m,yUp+m),pair<int,int>(m-1,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r]+histC[r]+histD[r];
		}
	}
	else if (0<=xLeft && xRight>=m && 0>yUp && yBot<m) {
		vector<int> histA(stat.buildHist(pair<int,int>(0,0),pair<int,int>(xRight-m,yBot)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft,0),pair<int,int>(m-1,yBot)));
		vector<int> histC(stat.buildHist(pair<int,int>(0,yUp+m),pair<int,int>(xRight-m,m-1)));
		vector<int> histD(stat.buildHist(pair<int,int>(xLeft,yUp+m),pair<int,int>(m-1,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r]+histC[r]+histD[r];
		}
	}
	else  {
		vector<int> histA(stat.buildHist(pair<int,int>(0,0),pair<int,int>(xRight,yBot-m)));
		vector<int> histB(stat.buildHist(pair<int,int>(xLeft+m,0),pair<int,int>(m-1,yBot-m)));
		vector<int> histC(stat.buildHist(pair<int,int>(0,yUp),pair<int,int>(xRight,m-1)));
		vector<int> histD(stat.buildHist(pair<int,int>(xLeft+m,yUp),pair<int,int>(m-1,m-1)));
		for (int r=0; r<36; r++) {
			retHist[r] = histA[r]+histB[r]+histC[r]+histD[r];
		}
	}
	return retHist;
}

void toqutree::makeChild(PNG * NW,PNG * NE,PNG * SW,PNG * SE,PNG org,int x,int y,int sz) {
	for (int i=0; i<sz; i++) {
		for (int j=0; j<sz; j++) {
			*SE->getPixel(j,i) = *org.getPixel(positiveMod(x+j, 2*sz), positiveMod(y+i, 2*sz));
			*SW->getPixel(j,i) = *org.getPixel(positiveMod(x+j-sz, 2*sz), positiveMod(y+i, 2*sz));
			*NE->getPixel(j,i) = *org.getPixel(positiveMod(x+j, 2*sz), positiveMod(y+i-sz, 2*sz));
			*NW->getPixel(j,i) = *org.getPixel(positiveMod(x+j-sz, 2*sz), positiveMod(y+i-sz, 2*sz));
		}
	}
}

HSLAPixel * toqutree::getPix(PNG im,int xCoor,int yCoor,int m) {
	if (xCoor>=m)
		xCoor -= m;
	if (xCoor<0)
		xCoor += m;
	if (yCoor>=m)
		yCoor -= m;
	if (yCoor<0)
		yCoor += m;
	return im.getPixel(xCoor,yCoor);
}

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.
	cout<<"at render"<<endl;
	return renderHelper(root);
}

PNG toqutree::renderHelper(Node *cur_node){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */
	// Base case leaf node (check if one of it's next nodes is null)
	if(cur_node->NW == NULL) {
		// pair<int,int> center = cur_node->center;
		// HSLAPixel *pixel = im->getPixel(center.first, center.second);
		// *pixel = cur_node->avg;
		unsigned int size = pow(2, cur_node->dimension);
		PNG ret_PNG (size, size);
		for(unsigned int i = 0; i < size; i++) {
			for(unsigned int j = 0; j < size; j++) {
				*ret_PNG.getPixel(i, j) = cur_node->avg;
			}
		}
		return ret_PNG;
	}
	else {
		PNG NW_PNG = renderHelper(cur_node->NW);
		PNG NE_PNG = renderHelper(cur_node->NE);
		PNG SE_PNG = renderHelper(cur_node->SE);
		PNG SW_PNG = renderHelper(cur_node->SW);
		return combinePNGs(cur_node, NW_PNG, NE_PNG, SE_PNG, SW_PNG);//combine images
	}
}

PNG toqutree::combinePNGs(Node *cur_node, PNG NW_PNG, PNG NE_PNG, PNG SE_PNG, PNG SW_PNG) {
	unsigned int size = pow(2, cur_node->dimension);
	PNG ret_PNG (size, size);

	/* Node *NW = cur_node->NW;
	Node *NE = cur_node->NE;
	Node *SE = cur_node->SE;
	Node *SW = cur_node->SW; */

	//unsigned int child_size = pow(2, NW->dimension);

	for(unsigned int i = 0; i < size/2; i++) {
		for(unsigned int j = 0; j < size/2; j++) {
			/* *ret_PNG.getPixel(positiveMod(i+NW->center.first, size), positiveMod(j+NW->center.second, size)) = *NW_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i+NE->center.first, size), positiveMod(j+NE->center.second, size)) = *NE_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i+SE->center.first, size), positiveMod(j+SE->center.second, size)) = *SE_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i+SW->center.first, size), positiveMod(j+SW->center.second, size)) = *SW_PNG.getPixel(i, j); */
			*ret_PNG.getPixel(positiveMod(i-size/2+cur_node->center.first, size), positiveMod(j-size/2+cur_node->center.second, size)) = *NW_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i+cur_node->center.first, size), positiveMod(j-size/2+cur_node->center.second, size)) = *NE_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i+cur_node->center.first, size), positiveMod(j+cur_node->center.second, size)) = *SE_PNG.getPixel(i, j);
			*ret_PNG.getPixel(positiveMod(i-size/2+cur_node->center.first, size), positiveMod(j+cur_node->center.second, size)) = *SW_PNG.getPixel(i, j);
		}
	}
	return ret_PNG;
}

int toqutree::positiveMod(int a, int b){
	int ret_val = a%b;
	if(ret_val < 0)
		return ret_val + b;
	return ret_val;
}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){

	pruneHelper(root,tol);

}

void toqutree::pruneHelper(Node* node, double tol){
	if(node->NW == NULL)
		return;
	else {
		if(shouldPrune(node, node, tol)) {
			delete node->NW;
      		delete node->NE;
      		delete node->SE;
      		delete node->SW;
			
			node->NW = NULL;
      		node->NE = NULL;
      		node->SE = NULL;
      		node->SW = NULL;
			return;
		}
		else {
			pruneHelper(node->NW, tol);
			pruneHelper(node->NE, tol);
			pruneHelper(node->SE, tol);
			pruneHelper(node->SW, tol);
		}
	}
}

bool toqutree::shouldPrune(Node *subtree_root, Node *node, double tol){
	if(node->NW == NULL) {
		if(node->avg.dist(subtree_root->avg) <= tol)
			return true;
		else
			return false;
	}

	else {
		return shouldPrune(subtree_root, node->NW, tol) && 
				shouldPrune(subtree_root, node->NE, tol) &&
				shouldPrune(subtree_root, node->SE, tol) &&
				shouldPrune(subtree_root, node->SW, tol);
	}
}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
	// Leaf node
	if(curr->NW == NULL) {
		delete curr;
	}
	else {
		clear(curr->NW);
		clear(curr->NE);
		clear(curr->SE);
		clear(curr->SW);
		delete curr;
	}
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
		// Leaf node
	if(other == NULL) {
		return NULL;
	}
	else if(other->NW == NULL) {
		return new Node(other->center, other->dimension, other->avg);
	}
	else {
		Node *this_node = new Node(other->center, other->dimension, other->avg);
		this_node->NW = copy(other->NW);
		this_node->NE = copy(other->NE);
		this_node->SE = copy(other->SE);
		this_node->SW = copy(other->SW);
		return this_node;
	}
}


