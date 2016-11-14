#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib> 
#include <ctime>  
#include "SLIC.h"
#include <fstream>
#include <math.h>
#include <memory>



using namespace cv;
using namespace std;


string str[100];//保存单词的字符串数组
int tag=0;

struct Words{//保存已放入图片的单词的信息，构建链表
	CvSize text_size;
	int baseline;
	int label;
	int pointx, pointy;//单词矩形左下角的坐标
	Words* next;
	double height, width;
	int weight;//权重
	int wordsno;
	int tag=-1;
};

struct BlackPixel{
	int midpointx, midpointy;//中心点横纵坐标
	double weight;//距离图片中心点的距离取反
	int label;
};

void BinaryImage()
{
	IplImage *image;//初始化保存彩色图  
	IplImage  *grayimg;//初始化保存灰度图  
	IplImage  *binaryimg;//初始化保存二值图
	IplImage *imageresize;

	image = cvLoadImage("image5.jpg", -1);//从本地读取彩色图  
	//注意指针变量一定要先初始化才能使用，否则崩溃  
	//灰度转换时通道一定要设置正确  
	//resize(image, OutputArray dst, Size dsize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR)
	imageresize = cvCreateImage(cvSize(300, 300), IPL_DEPTH_8U, 3);
	cvResize(image, imageresize, CV_INTER_LINEAR);
	int channel = 1;//image->nChannels;  
	int depth = image->depth;
	CvSize sz;
	sz.width = imageresize->width;//宽度  
	sz.height = imageresize->height;//高度  

	grayimg = cvCreateImage(sz, depth, channel);//生成image  
	cvCvtColor(imageresize, grayimg, CV_BGR2GRAY);//转换成灰度图  
	cvSaveImage("grayimage.jpg", grayimg);//将灰度图保存到本地

	//生成二值图像
	binaryimg = cvCreateImage(cvGetSize(grayimg), IPL_DEPTH_8U, 1);
	cvThreshold(grayimg, binaryimg, 100, 255, CV_THRESH_BINARY);
	cvSaveImage("binaryimage.jpg", binaryimg);//将二值图保存到本地
}

int Text(){
	
	char str1[1000];
	//char word[100];
	int len;
	int numberofwords;

	fstream outFile;
	outFile.open("heart.txt", ios::in);
	cout << "inFile.txt" << "--- all file is as follows:---" << endl;
	outFile.getline(str1, 999, '\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束  

	cout << str1 << endl;
	outFile.close();
	char *word;//word[num]就是你要的数组
	int i, j = 0, xp = 0, num = 0;
	int x;
	//输入单词，并将每个单词存在字符串数组内

	/*cout << "请输入单词" << endl;
	
	
	//但是这种方法，数组不用以后，必须自己释放

	gets(str1);//输入一组单词，空格分割，回车结束
	*/
	
	len = strlen(str1);
	for (i = 0; i < len; i++){

		if (str1[i] == ' '){
			word = new char[num + 1];//动态分配数组空间
			x = i - num;
			for (j = 0; j < num; j++){
				word[j] = str1[x];
				x++;
			}
			word[j] = '\0';
			str[xp] = word;
			xp++;
			delete[]word; //释放数组
			num = 0;
		}
		else{
			if (i != len - 1)
				num++;
			else{
				word = new char[num + 2];//动态分配数组空间
				x = i - num;
				for (j = 0; j < num + 1; j++){
					word[j] = str1[x];
					x++;
				}
				word[j] = '\0';
				str[xp] = word;
				xp++;
				delete[]word; //释放数组
				num = 0;

			}
		}
	}
	numberofwords = xp;
	cout << "finished" << endl;
	for (; xp >= 0; xp--)
		cout << str[xp] << endl;
	return numberofwords;


}

void strsort(string *s1, string *s2)
{
     string p;
     p = *s1;
     *s1 = *s2;
     *s2 = p;
 }


void Optimizition3(Words* head, Words* tail, int* getlabel, int numberofwords,int imgw,int imgh,int pixellabel[],int labelsize,int maxtextwidth,int maxtextheight,int mintextwidth,int mintextheight){

	//labelsize：画布中的像素数
	int i, j;
	int temp;
	int m, n;
	const char *tp;
	Words* p;
	Words* q;
	int strnum = 0;
	p = tail;
	CvSize size;
	int sbaseline;
	CvFont font;
	

	for (i = 0; i < imgh; i++){
		for (j = 0; j < imgw; j++){			
			if (pixellabel[i*imgw + j] > 0){
				temp = 1;
				for (m = j; m < j + mintextwidth; m++){
					if (m < imgw) {
						if (pixellabel[i*imgw + m] > 0)
							continue;//m未超出画布且没有安排过单词的黑色像素					
						else {
							temp = 0;
							break;
						}
					}
					else{
						temp = 0;
						break;
					}
				}
				for (n = i; n < i + mintextheight; n++){
					if (n < imgh) {
						if (pixellabel[n*imgw + j] > 0)
							continue;
						else {
							temp = 0;
							break;
						}
					}
					else{
						temp = 0;
						break;
					}
				}
			
				if (temp){
					tp = str[strnum].c_str();
					cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 0.5, 0, 1, 8);
					cvGetTextSize(tp, &font, &size, &sbaseline);
					q = new Words;			
					q->wordsno = strnum;
					q->text_size = size;
					q->pointy = i+q->text_size.height;
					//q->label = p->label;
					q->pointx = j;
					q->baseline = sbaseline;					
					q->height = 1;
					q->width = 0.5;
					if (q->pointy >= imgh) q->pointy =  imgh-1;
					else if ((q->pointy - q->text_size.height+1) < 0) q->pointy = q->text_size.height;
					if ((q->pointx + q->text_size.width-1) >= imgw) q->pointx = imgw-q->text_size.width ;
					else if (q->pointx < 0) q->pointx = 0;					
					tail->next = q;
					tail = q;	
					q->tag = tag;
					tag++;				
					q->next = NULL;
					strnum++;
					if (strnum == numberofwords) strnum = 0;
					for (m = (q->pointy-q->text_size.height+1); m <= q->pointy; m++){
						for (n = q->pointx; n < q->pointx + q->text_size.width; n++){
							if (pixellabel[m*imgw+n]>0)
							pixellabel[m*imgw + n] = 0;
						}
					}
					j = j + q->text_size.width;
					
								
			}
			}
						
		}
	}

}

int max(int a, int b){
	if (a > b) return a;
	else return b;
}

int min(int a, int b){
	if (a < b) return a;
	else return b;
}

double OverTheBorderArea(Words* head, int* getlabel, int imgw, int imgh, int pixellabel[]){
	Words *p;
	int overtheborderarea = 0;
	double intheborderarea=0;
	double rate=0;
	int allwordsarea = 0;
	p = head;
	int i, j;
	while (p != NULL){
		overtheborderarea = 0;
		for (i = (p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
			for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
				//if (i < imgh && i>0 && j>0 && j < imgw){
				if (pixellabel[i*imgw + j] < 0) {
					overtheborderarea++;
				}
				//}
				//else continue;
			}
		}
		allwordsarea = allwordsarea + p->pointx*p->pointy;
		intheborderarea =  intheborderarea + (p->text_size.width*p->text_size.height - overtheborderarea);
		
		p = p->next;
	}
    rate = intheborderarea / allwordsarea;
	return (1-rate);

}

double OverlappedArea2(Words* head,int pixellabel[], int imgw, int imgh, int wordtag[],int *getlabel){
		int overlappedarea = 0;
		int overtheborderarea = 0;
		double zeroarea = 0;
		double rate;
		int i = 0;
		int j;
		Words* p;
		
		double wordsarea = 0;
		int allwordsarea = 0;
		p = head;
		while (p != NULL){//全部单词面积
			allwordsarea = allwordsarea + (p->text_size.height)*(p->text_size.width);
			for ((i = p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
				for (j = p->pointx; j < p->pointx + p->text_size.width; j++){
					if (pixellabel[i*imgw + j] == 0) zeroarea++;
				}
			}
			p = p->next;
		}

		for (i = 0; i < imgh; i++){//实际占有的面积
			for (j = 0; j < imgw;j++)
				if (pixellabel[i*imgw+j] == 0) wordsarea++;
		}

		rate =double( wordsarea / zeroarea);
		
		overtheborderarea = OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel);

		overlappedarea = allwordsarea - wordsarea - overtheborderarea;
		
		return (1-rate);
}



double DropTemperature(double Told){//降温函数
	double Tnew;
	Tnew = 0.99*Told;
	return Tnew;
}

Words* changedpoint;

int NewSolution(Words* head, int wordtag[], int pixellabel[], int imgw, int imgh){
	int a, b;
	Words* p;
	int Xold, Yold;
	double oldw, oldh;
	int x, y;
	int i, j;
	int number = 100;
	a = rand() % tag;//随机挑选一个单词，判断有无重叠，有重叠则扰动，否则继续产生随机数
	while (wordtag[a] < 0 && number>0)
	{
		a = rand() % tag;
		number--;
	}
	p = head;
	while (p->tag != a) {
		//cout << "p->tag=" << p->tag << endl;
		p = p->next;
		//找到这一有重叠需要扰动的单词
		if (p == NULL) {
			cout << "Error" << endl;
			//exit(1);
		}
	}


	for (i = (p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
		for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
			//if (i<imgw && j<imgh && i>0 && j>0) {
				if (pixellabel[i*imgw + j]==0)
					pixellabel[i*imgw + j] = 1;//0->1
			//}
		}
	}//移动前将原来区域的pixellabel置为1

	int xs1, xb1, xs2, xb2;
	int ys1, yb1, ys2, yb2;
	int endx, startx, endy, starty;
	double width, height;
	changedpoint = p;
	p = head;
	xs2 = changedpoint->pointx;
	xb2 = changedpoint->pointx + changedpoint->text_size.width-1;
	ys2 = changedpoint->pointy - changedpoint->text_size.height+1;
	yb2 = changedpoint->pointy;

	while (p != NULL){//找出与原区域有重叠的单词，将其pixellabel置为0
		if (p == changedpoint)
		{
			p = p->next;
			continue;
		}
		xs1 = p->pointx;
		xb1 = p->pointx + p->text_size.width-1;
		ys1 = p->pointy - p->text_size.height+1;
		yb1 = p->pointy;
		endx = max(xb1, xb2);
		startx = min(xs1, xs2);
		width = p->text_size.width + changedpoint->text_size.width - (endx - startx);
		endy = max(yb1, yb2);
		starty = min(ys1, ys2);
		height = p->text_size.height + p->text_size.height - (endy - starty);
		if (width > 0 && height > 0){//与原区域有重叠
			for (i = (p->pointy - p->text_size.height+1); i <= p->pointy; i++){
				for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
					//if (i<imgw && j<imgh && i>0 && j>0) {
						if (pixellabel[i*imgw + j] > 0)
							pixellabel[i*imgw + j] = 0;//1->0
					//}
				}

			}
		}
		p = p->next;
	}

	x = changedpoint->pointx;
	y = changedpoint->pointy;

	p = changedpoint;
	number = 100;
	while (true){
		b = rand() % 8;//从8个方向中挑选一个方向进行扰动
		//cout << b << ' ';
		switch (b){
		case 0:{
			x = p->pointx - 1;
			y = p->pointy - 1;
			break;
		}
		case 1:{
			y = p->pointy - 1;
			break;
		}
		case 2:{
			x = p->pointx + 1;
			y = p->pointy - 1;
			break;
		}
		case 3:{
			x = p->pointx + 1;
			break;
		}
		case 4:{
			x = p->pointx + 1;
			y = p->pointy + 1;
			break;
		}
		case 5:{
			y = p->pointy + 1;
			break;
		}
		case 6:{
			x = p->pointx - 1;
			y = p->pointy + 1;
			break;
		}
		default:{
			x = p->pointx - 1;
			break;
		}
		}
		number--;
		if (x >= 0 && (x + p->text_size.width-1) < imgw && (y - p->text_size.height+1) >= 0 && y < imgh && number > 0) {			
			break;//移动后单词仍在画布内,产生新解
		}
		if(number<0){//100次后仍找不到新解，则解不变
			x = p->pointx;
			y = p->pointy;
			break;
		}
	}
	//number--;
	//if (number < 0) break;
	p->pointx = x;
	p->pointy = y;

	for (i = (p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
		for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
			//if (i<imgw && j<imgh && i>0 && j>0) {
				if (pixellabel[i*imgw + j] > 0)
					pixellabel[i*imgw + j] = 0;//1->0
			//}
		}
	}//移动后将新区域的pixellabel置为0

	
	changedpoint = p;
	return b;
}

double BlankArea(Words* head, Words* tail, int wordtag[], int numberofblackpixel,int* getlabel,int imgw,int imgh,int pixellabel[]){
	double wordsarea=0;
	int overlappedarea;
	int blankarea;
	double rate;
	int i, j;
	Words* p;
	p = head;
	for (i = 0; i < imgh; i++){
		for (j = 0; j < imgw; j++){
			if (pixellabel[i*imgw + j] == 0) wordsarea++;
		}
	}//目标图案中已被单词覆盖的面积
	//blankarea = numberofblackpixel - wordsarea;
	rate = double(wordsarea / numberofblackpixel);
	return (1-rate);
}

int calWordsNum(Words *head)
{
	int count = 0;
	Words* p;
	p = head;
	while (p != NULL)
	{
		p = p->next;
		count++;
	}
	return count;
}
void SimulatedAnnealing(Words* head, Words* tail,int wordtag[],int* getlabel,int imgw,int imgh,int numberofblackpixel,int pixellabel[]){
	double T0 = 1000;
	double T;
	int i,j=0;
	int a;
	int n = 0;
	double E[10];
	int number = 1000;
	int number2 = 100;
	double Eold, Enew;
	double Emin, Emax;
	Words* q;
	int xs1, xb1, ys1, yb1;
	int xs2, xb2, ys2, yb2;
	int endx, startx, endy, starty;
	int width, height;
	Words* p;
	//memset(E, 0, sizeof(E));
	int e=0;
	double pp;
	double kb = 1.38*pow(10, -23);
	//cout << kb << endl;
	double r;
	T = T0;
	cout << "simulatedannealing" << endl;
	Eold = Enew = 0.5*OverlappedArea2(head, pixellabel, imgw, imgh, wordtag, getlabel) + 0.5* OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel);
	//Eold = Enew = 0.3*OverlappedArea2(head, pixellabel, imgw, imgh, wordtag, getlabel) + 0.3*OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel) + 0.4* BlankArea(head, tail, wordtag, numberofblackpixel, getlabel, imgw, imgh, pixellabel);
	cout << "Estart=" << Enew << endl;
	//BlankArea(head, tail, wordtag, numberofblackpixel)，OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel) + 
	int errorcount = 0;
	while (T>5){	
		j = 0;	
		Emax = Emin = Enew;
		number = 100;
		while (true){
			//cout << "number :" << number << endl;
			//产生新状态，判断是否接受

				a = NewSolution(head, wordtag,pixellabel,imgw,imgh);

				//cout << "new solution a = "<< a<< endl;
				Enew = 0.5*OverlappedArea2(head, pixellabel, imgw, imgh, wordtag, getlabel) + 0.5*OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel);
				if (Enew < Eold){
					Eold = Enew;
					
					if (Enew > Emax) Emax = Enew;
					if (Enew < Emin) Emin = Enew;
					j++;
					//cout << '1' << ' ';
					
				}
				else{
					pp = exp(-(Enew - Eold) / (T*0.000001));				
					r = double(rand()%100)/100.0;
					//cout << "pp=" << pp << " r="<<r<<endl;
					if (pp > r){//接受
						Eold = Enew;
						//E[j] = Enew;
						if (Enew > Emax) Emax = Enew;
						if (Enew < Emin) Emin = Enew;
						j++;
						//cout << '1' << ' ';
						
					}
					else {//不接受,回退
						//p = changedpoint;
						for (i = (changedpoint->pointy - changedpoint->text_size.height+1); i <= changedpoint->pointy; i++){
							for (j = changedpoint->pointx; j < (changedpoint->pointx + changedpoint->text_size.width); j++){
								//if (i<imgw && j<imgh && i>0 && j>0) {
									if (pixellabel[i*imgw + j] == 0)
										pixellabel[i*imgw + j] = 1;//0->1
								//}
							}
						}//移动前将原来区域的pixellabel置为1

						int xs1, xb1, xs2, xb2;
						int ys1, yb1, ys2, yb2;
						int endx, startx, endy, starty;
						double width, height;
						p = head;
						xs2 = changedpoint->pointx;
						xb2 = changedpoint->pointx + changedpoint->text_size.width-1;
						ys2 = changedpoint->pointy - changedpoint->text_size.height+1;
						yb2 = changedpoint->pointy;

						while (p != NULL){//找出与原区域有重叠的单词，将其pixellabel置为0
							if (p == changedpoint)
							{
								p = p->next;
								continue;
							}
							xs1 = p->pointx;
							xb1 = p->pointx + p->text_size.width-1;
							ys1 = p->pointy - p->text_size.height+1;
							yb1 = p->pointy;
							endx = max(xb1, xb2);
							startx = min(xs1, xs2);
							width = p->text_size.width + changedpoint->text_size.width - (endx - startx);
							endy = max(yb1, yb2);
							starty = min(ys1, ys2);
							height = p->text_size.height + p->text_size.height - (endy - starty);
							if (width > 0 && height > 0){//与原区域有重叠
								for (i = (p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
									for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
										//if (i<imgw && j<imgh && i>0 && j>0) {
											if (pixellabel[i*imgw + j] > 0)
												pixellabel[i*imgw + j] = 0;
										//}
									}

								}
							}
							p = p->next;
						}

						switch (a){
						case 0:{
							changedpoint->pointx = changedpoint->pointx + 1;
							changedpoint->pointy = changedpoint->pointy + 1;
							break;
						}
						case 1:{
							changedpoint->pointy = changedpoint->pointy + 1;
							break;
						}
						case 2:{
							changedpoint->pointx = changedpoint->pointx - 1;
							changedpoint->pointy = changedpoint->pointy + 1;
							break;
						}
						case 3:{
							changedpoint->pointx = changedpoint->pointx - 1;
							break;
						}
						case 4:{
							changedpoint->pointx = changedpoint->pointx - 1;
							changedpoint->pointy = changedpoint->pointy - 1;
							break;
						}
						case 5:{
							changedpoint->pointy = changedpoint->pointy - 1;
							break;
						}
						case 6:{
							changedpoint->pointx = changedpoint->pointx + 1;
							changedpoint->pointy = changedpoint->pointy - 1;
							break;
						}
						case 7:{
							changedpoint->pointx = changedpoint->pointx + 1;
							break;
						}
						}

						for (i = (changedpoint->pointy - changedpoint->text_size.height+1); i <= changedpoint->pointy; i++){
							for (j = changedpoint->pointx; j < (changedpoint->pointx + changedpoint->text_size.width); j++){
								//if (i<imgw && j<imgh && i>0 && j>0) {
									if (pixellabel[i*imgw + j] > 0)
										pixellabel[i*imgw + j] = 0;
								//}
							}
						}//移动后将区域的pixellabel置为0
					}

				}

				//判断单词间的重叠并标记
				for (i = 0; i < tag; i++) wordtag[i] = -1;//没有重叠的标记为-1
				p = head;//统计有重叠情况的单词
				while (p != NULL){
					q = p->next;
					while (q != NULL){
						xs1 = p->pointx;
						xb1 = p->pointx + p->text_size.width - 1;
						ys1 = p->pointy - p->text_size.height + 1;
						yb1 = p->pointy;
						xs2 = q->pointx;
						xb2 = q->pointx + q->text_size.width - 1;
						ys2 = q->pointy - q->text_size.height + 1;
						yb2 = q->pointy;

						endx = max(xb1, xb2);
						startx = min(xs1, xs2);
						width = p->text_size.width + q->text_size.width - (endx - startx);
						endy = max(yb1, yb2);
						starty = min(ys1, ys2);
						height = p->text_size.height + p->text_size.height - (endy - starty);
						if (width > 0 && height > 0){
							//overlappedarea = width*height + overlappedarea;
							wordtag[p->tag] = 1;
							wordtag[q->tag] = 1;

						}

						q = q->next;
					}
					//cout<<
					p = p->next;
					//cout << overlappedarea << ' ';
				}

				if (j == 10){	//每10次判断一次E的变化是否稳定		
					//cout << "Emax=" << Emax << " Emin=" << Emin << endl;
					j = 0;
					if ((Emax - Emin) < 100 || number<0)
						break;
					Emax = Emin = Enew;
				}
				number--;
				if (number < 0) break;
				//cout << "words_number=" << calWordsNum(head) << endl;
				
		}
		//T--;
		T = DropTemperature(T);
		//cout << T << ' ';
	}
	cout << "Eend=" << Eold << endl;
	

}
int main(){
	//转换为二值图像
	BinaryImage();
	int Simgw, Simgh;
	int i, j, m, n;
	int* pixellabel;//每个像素的label,填充部分为正，白色部分为负，已放单词部分为0
	

	IplImage* Simg;
	Simg = cvLoadImage("binaryimage.jpg");
	Simgw = Simg->width;
	Simgh = Simg->height;
	pixellabel = new int[Simgw*Simgh];

	//设置pixellabel，黑色像素为1，白色为-1
	for (i = 0; i < Simgh; i++){
		for (j = 0; j < Simgw; j++){
			if ((uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){
				pixellabel[i*Simgw + j] = 1;
				//cout << pixellabel[i*Simgw + j] << ' ';
			}
			//黑色为1
			else pixellabel[i*Simgw + j] = -1;
		}
	}
	

	//超像素分割
	cv::Mat img, result;
	int* getlabel;
	//int label;
	img = imread("binaryimage.jpg");
	int numSuperpixel = 50;

	SLIC slic;
	slic.GenerateSuperpixels(img, numSuperpixel);
	if (img.channels() == 3)
		result = slic.GetImgWithContours(cv::Scalar(255, 255, 255));
	else
		result = slic.GetImgWithContours(cv::Scalar(128));
	getlabel = slic.GetLabel();
	cv::imwrite("result.jpg", result);


	//输入文字并保存为单词
	int numberofwords;
	numberofwords = Text();
	int strnum = 0;
	
	
	BlackPixel* blackpixel;


	Simg = cvLoadImage("result.jpg");
	Simgw = Simg->width;
	Simgh = Simg->height;
	cout << Simgw << Simgh << endl;

	
	Words *p = NULL;
	Words *q = NULL;
	Words *head = NULL;
	Words *tail = NULL;
	int tiaochu = 0;
	int label;
	int maxtextwidth=0;
	int maxtextheight = 0;
	int mintextwidth = 10000;
	int mintextheight = 10000;
	
	
	const char *tp;
	CvFont font;

	CvPoint pt;
	int blackmidpointx, blackmidpointy;//黑色图像的中点坐标
	int blacktolpointx, blacktolpointy;
	blacktolpointx = 0;
	blacktolpointy = 0;
	blackmidpointx = 0;
	blackmidpointy = 0;
	int numberofblackpixel = 0;
	tp = str[strnum].c_str();
	int tolpointx = 0, tolpointy = 0;
	int numberofblacksuperpixel=0;
	int nx=0;
	for (i = 0; i < Simgh; i++){
		for (j = 0; j < Simgw; j++){
			label = *(getlabel + i*Simgw + j);		
			if (label >= 0 && (uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){//黑色超像素
				for (m = i*Simgw + j; m < Simgh*Simgw; m++){
					if (*(getlabel + m) == label)  *(getlabel + m) = -label - 1;
				}
				numberofblacksuperpixel++;		
			}				
		}
	}

	for (i = 0; i < Simgh; i++){
		for (j = 0; j < Simgw; j++){
			label = *(getlabel + i*Simgw + j);
			if (label < 0){//黑色超像素
				for (m = i*Simgw + j; m < Simgh*Simgw; m++){
					if (*(getlabel + m) == label)  *(getlabel + m) = -label - 1;
				}
			}
		}
	}
	blackpixel = new BlackPixel[numberofblacksuperpixel];
	int num=0;

	for (i = 0; i < Simgh; i++){
		for (j = 0; j < Simgw; j++){			
			label = *(getlabel + i*Simgw + j);
			
			if (label >= 0 && (uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){//某一未放置过单词的黑色超像素
				tolpointx = 0;
				tolpointy = 0;
				nx = 0;
				for (m = i; m < Simgh; m++){//i*Simgw + j找到这一区域的中心
					for (n = 0; n < Simgw; n++){
						if (*(getlabel + m*Simgw + n) == label) {
							if (((uchar*)(Simg->imageData + Simg->widthStep*m))[n * 3] != 255 || ((uchar*)(Simg->imageData + Simg->widthStep*m))[n * 3 + 1] != 255 || ((uchar*)(Simg->imageData + Simg->widthStep*m))[n * 3 + 2] != 255)
							{
								(Simg->imageData + Simg->widthStep*m)[n * 3] = 255;
								(Simg->imageData + Simg->widthStep*m)[n * 3 + 1] = 255;
								(Simg->imageData + Simg->widthStep*m)[n * 3 + 2] = 255;
							}
							tolpointx = tolpointx + n;
							tolpointy = tolpointy + m;
							blacktolpointx = blacktolpointx + n;
							blacktolpointy = blacktolpointy + m;
							nx++;
							numberofblackpixel++;
						}
					}
				}
				for (m = i*Simgw + j; m < Simgh*Simgw; m++){
					if (*(getlabel + m) == label)  *(getlabel + m) = -label - 1;//负
				}
				blackpixel[num].weight= -(pow((Simgw / 2 - tolpointx / nx), 2) + pow((Simgh / 2 - tolpointy / nx), 2));//距离公式，未开根号，取反，距离越远权值越小
				blackpixel[num].midpointx = tolpointx / nx;
				blackpixel[num].midpointy = tolpointy / nx;
				blackpixel[num].label = -label-1;//blackpixellabel为负
				num++;
			}			
		}
	}

	blackmidpointx = blacktolpointx / numberofblackpixel;
	blackmidpointy = blacktolpointy / numberofblackpixel;
	

	double bufferweight;
	int bufferpointx, bufferpointy;
	int bufferlabel; 
	for (i = 0; i < num; i++){//按单词权重按长度由大到小排列
		for (j = 0; j < num - i; j++){
			if (blackpixel[i].weight<blackpixel[j].weight){
				bufferweight = blackpixel[i].weight;
				blackpixel[i].weight = blackpixel[j].weight;
				blackpixel[j].weight = bufferweight;

				bufferpointx = blackpixel[i].midpointx;
				blackpixel[i].midpointx = blackpixel[j].midpointx;
				blackpixel[j].midpointx = bufferpointx;

				bufferpointy = blackpixel[i].midpointy;
				blackpixel[i].midpointy = blackpixel[j].midpointy;
				blackpixel[j].midpointy = bufferpointy;

				bufferlabel = blackpixel[i].label;
				blackpixel[i].label = blackpixel[j].label;
				blackpixel[j].label = bufferlabel;
			}
		}
	}

	int x1, y1;
	int xs = 0, xb = 0, ys = 0, yb = 0;
	//int tolpointx = 0, tolpointy = 0;
	int  ny = 0;
	double textwidth,texthight;
	for (i = 0; i < num; i++){
		textwidth = 0.1;
		texthight = 1;
		tp = str[strnum].c_str();
		//pixellabel = label;
		p = new Words;
		p->label = blackpixel[i].label;
		if (head == NULL) head = tail = p;
		else q->next = p;
		tail = p;
		q = p;
		if (head != NULL) q->next = NULL;
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, texthight, textwidth, 0, 1, 8);
		cvGetTextSize(tp, &font, &p->text_size, &p->baseline);

		x1 = blackpixel[i].midpointx;
		y1 = blackpixel[i].midpointy;
		
		do {
			//不越界
			xs = x1 - p->text_size.width / 2;
			xb = xs + p->text_size.width-1 ;
			yb = y1 + p->text_size.height/2 ;
			ys = yb - p->text_size.height +1;
			
			textwidth = textwidth + 0.1;
			texthight = texthight + 0.5;
			cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, texthight, textwidth, 0, 1, 8);
			cvGetTextSize(tp, &font, &p->text_size, &p->baseline);
		} while (*(getlabel + ys*Simgw + xs) == p->label && *(getlabel + ys*Simgw + xb) == p->label && *(getlabel + yb*Simgw + xs) == p->label&&*(getlabel + yb*Simgw + xb) == p->label);
		
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, texthight-0.5 , textwidth, 0, 1, 8);
		cvGetTextSize(tp, &font, &p->text_size, &p->baseline);
		pt.x = xs; //这样的设置为字体（左右）居中对齐
		pt.y = yb; //这样的设置为字体（上下）居中对齐
		//判定，若超出边界则向内移动
		if ((xs+p->text_size.width-1) > Simgw) pt.x = Simgw - p->text_size.width ;
		else if (pt.x < 0) pt.x = 0;
		if (yb > Simgh) pt.y =  Simgh;
		else if ((yb - (p->text_size.height) +1) < 0) pt.y = p->text_size.height;
		//cout << pt.x << ',' << pt.y << ' ';
		p->wordsno = strnum;
		p->pointx = pt.x;
		p->pointy = pt.y;
		p->height = texthight - 0.5;
		p->width = textwidth;
		p->next = NULL;
		p->tag = tag;
		tag++;
		if (maxtextwidth < p->text_size.width) maxtextwidth = p->text_size.width;
		if (maxtextheight < p->text_size.height) maxtextheight = p->text_size.height;
		if (mintextwidth > p->text_size.width) mintextwidth = p->text_size.width;
		if (mintextheight > p->text_size.height) mintextheight = p->text_size.height;
		strnum++;
		if (strnum == numberofwords) strnum = 0;
	}
	
	
	
	//往空隙中加单词
	int x2, y2,yy;
	p = head;
	CvSize size;
	int sbaseline;
	Words* ptail;
	ptail = tail;
	while (p != ptail){
		yy = -1;
		i = p->pointy;
		//cout << i<<' ';
		while (*(getlabel + i*Simgw + p->pointx) == p->label && i<Simgh) i++;
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 0.5, 0, 1, 8);
		cvGetTextSize(tp, &font, &size, &sbaseline);
		//q = head;
		if (*(getlabel + i*Simgw + p->pointx) < 0 ){
			q = new Words;
			tp = str[strnum].c_str();
			if ((i - size.height) < p->pointy) i = p->pointy + size.height;
			tail->next = q;
			tail = q;
			q->wordsno = strnum;
			q->pointy = i;
			q->label = p->label;
			q->pointx = p->pointx;
			//判断，若超出边界则向内移
			if (q->pointy >= Simgh) q->pointy = Simgh-1;
			else if ((q->pointy - q->text_size.height+1) < 0) q->pointy = q->text_size.height;		
			if ((q->pointx + q->text_size.width - 1) > Simgw) q->pointx = Simgw - q->text_size.width  ;
			else if (q->pointx < 0) q->pointx = 0;
			q->baseline = sbaseline;
			q->text_size = size;
			q->height = 1;
			q->width = 0.5;
			//q->next = NULL;
			q->tag = tag;
			tag++;
			if (maxtextwidth < q->text_size.width) maxtextwidth = q->text_size.width;
			if (maxtextheight < q->text_size.height) maxtextheight = q->text_size.height;
			if (mintextwidth > p->text_size.width) mintextwidth = p->text_size.width;
			if (mintextheight > p->text_size.height) mintextheight = p->text_size.height;
			tail = q;
			strnum++;
			if (strnum == numberofwords) strnum = 0;
			p = p->next;
		}	
		else {
			p = p->next;
			continue;
		}
	}
	q->next = NULL;

	p = head;
	while (p != NULL){
		for (i = (p->pointy - p->text_size.height+1); i <= p->pointy; i++){
			for (j = p->pointx; j < (p->pointx + p->text_size.width); j++){
				//if (i<Simgw && j<Simgh) {
					if (pixellabel[i*Simgw+j]>0)
					 pixellabel[i*Simgw + j] = 0;//目标图案已填充单词的部分置为0,  1->0
				//}
			}
		}
		p = p->next;
	}

	int a,b,c;
	Optimizition3(head, tail, getlabel, numberofwords, Simgw, Simgh, pixellabel,Simgw*Simgh,maxtextwidth,maxtextheight,mintextwidth,mintextheight);
	
	//SimulatedAnnealing(head, tail, wordtag, getlabel, Simgw,Simgh,numberofblackpixel,pixellabel);
    cout << "SA over!" <<endl;
	
	//Optimizition4(head, tail, getlabel, numberofwords, Simgw, Simgh, pixellabel, Simgw*Simgh, maxtextwidth, maxtextheight,mintextwidth,mintextheight);
	
	p = head;
	while (p != NULL){
		//cout << "p->pointx=" << p->pointx << " p->pointy=" << p->pointy << endl;
		//p = p->next;
		if (p->pointx + p->text_size.width < Simgw&&p->pointy < Simgh){
			cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, p->height, p->width, 0, 1, 8);
			//cvGetTextSize(tp, &font, &size, &sbaseline);
			tp = str[p->wordsno].c_str();
			a = rand() % 110;
			cvPutText(Simg, tp, cvPoint(p->pointx, p->pointy), &font, CV_RGB(a, a, a));//插入单词
			p = p->next;
		}
		else p = p->next;
	}
	cout << "finished" << endl;

	//保存图像
	cvSaveImage("ResultImage.jpg", Simg);

	//释放图像
	cvReleaseImage(&Simg);

	return 0;
	
}