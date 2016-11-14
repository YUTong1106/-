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


string str[100];//���浥�ʵ��ַ�������
int tag=0;

struct Words{//�����ѷ���ͼƬ�ĵ��ʵ���Ϣ����������
	CvSize text_size;
	int baseline;
	int label;
	int pointx, pointy;//���ʾ������½ǵ�����
	Words* next;
	double height, width;
	int weight;//Ȩ��
	int wordsno;
	int tag=-1;
};

struct BlackPixel{
	int midpointx, midpointy;//���ĵ��������
	double weight;//����ͼƬ���ĵ�ľ���ȡ��
	int label;
};

void BinaryImage()
{
	IplImage *image;//��ʼ�������ɫͼ  
	IplImage  *grayimg;//��ʼ������Ҷ�ͼ  
	IplImage  *binaryimg;//��ʼ�������ֵͼ
	IplImage *imageresize;

	image = cvLoadImage("image5.jpg", -1);//�ӱ��ض�ȡ��ɫͼ  
	//ע��ָ�����һ��Ҫ�ȳ�ʼ������ʹ�ã��������  
	//�Ҷ�ת��ʱͨ��һ��Ҫ������ȷ  
	//resize(image, OutputArray dst, Size dsize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR)
	imageresize = cvCreateImage(cvSize(300, 300), IPL_DEPTH_8U, 3);
	cvResize(image, imageresize, CV_INTER_LINEAR);
	int channel = 1;//image->nChannels;  
	int depth = image->depth;
	CvSize sz;
	sz.width = imageresize->width;//���  
	sz.height = imageresize->height;//�߶�  

	grayimg = cvCreateImage(sz, depth, channel);//����image  
	cvCvtColor(imageresize, grayimg, CV_BGR2GRAY);//ת���ɻҶ�ͼ  
	cvSaveImage("grayimage.jpg", grayimg);//���Ҷ�ͼ���浽����

	//���ɶ�ֵͼ��
	binaryimg = cvCreateImage(cvGetSize(grayimg), IPL_DEPTH_8U, 1);
	cvThreshold(grayimg, binaryimg, 100, 255, CV_THRESH_BINARY);
	cvSaveImage("binaryimage.jpg", binaryimg);//����ֵͼ���浽����
}

int Text(){
	
	char str1[1000];
	//char word[100];
	int len;
	int numberofwords;

	fstream outFile;
	outFile.open("heart.txt", ios::in);
	cout << "inFile.txt" << "--- all file is as follows:---" << endl;
	outFile.getline(str1, 999, '\n');//getline(char *,int,char) ��ʾ�����ַ��ﵽ256�����������оͽ���  

	cout << str1 << endl;
	outFile.close();
	char *word;//word[num]������Ҫ������
	int i, j = 0, xp = 0, num = 0;
	int x;
	//���뵥�ʣ�����ÿ�����ʴ����ַ���������

	/*cout << "�����뵥��" << endl;
	
	
	//�������ַ��������鲻���Ժ󣬱����Լ��ͷ�

	gets(str1);//����һ�鵥�ʣ��ո�ָ�س�����
	*/
	
	len = strlen(str1);
	for (i = 0; i < len; i++){

		if (str1[i] == ' '){
			word = new char[num + 1];//��̬��������ռ�
			x = i - num;
			for (j = 0; j < num; j++){
				word[j] = str1[x];
				x++;
			}
			word[j] = '\0';
			str[xp] = word;
			xp++;
			delete[]word; //�ͷ�����
			num = 0;
		}
		else{
			if (i != len - 1)
				num++;
			else{
				word = new char[num + 2];//��̬��������ռ�
				x = i - num;
				for (j = 0; j < num + 1; j++){
					word[j] = str1[x];
					x++;
				}
				word[j] = '\0';
				str[xp] = word;
				xp++;
				delete[]word; //�ͷ�����
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

	//labelsize�������е�������
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
							continue;//mδ����������û�а��Ź����ʵĺ�ɫ����					
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
		while (p != NULL){//ȫ���������
			allwordsarea = allwordsarea + (p->text_size.height)*(p->text_size.width);
			for ((i = p->pointy - p->text_size.height + 1); i <= p->pointy; i++){
				for (j = p->pointx; j < p->pointx + p->text_size.width; j++){
					if (pixellabel[i*imgw + j] == 0) zeroarea++;
				}
			}
			p = p->next;
		}

		for (i = 0; i < imgh; i++){//ʵ��ռ�е����
			for (j = 0; j < imgw;j++)
				if (pixellabel[i*imgw+j] == 0) wordsarea++;
		}

		rate =double( wordsarea / zeroarea);
		
		overtheborderarea = OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel);

		overlappedarea = allwordsarea - wordsarea - overtheborderarea;
		
		return (1-rate);
}



double DropTemperature(double Told){//���º���
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
	a = rand() % tag;//�����ѡһ�����ʣ��ж������ص������ص����Ŷ�������������������
	while (wordtag[a] < 0 && number>0)
	{
		a = rand() % tag;
		number--;
	}
	p = head;
	while (p->tag != a) {
		//cout << "p->tag=" << p->tag << endl;
		p = p->next;
		//�ҵ���һ���ص���Ҫ�Ŷ��ĵ���
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
	}//�ƶ�ǰ��ԭ�������pixellabel��Ϊ1

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

	while (p != NULL){//�ҳ���ԭ�������ص��ĵ��ʣ�����pixellabel��Ϊ0
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
		if (width > 0 && height > 0){//��ԭ�������ص�
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
		b = rand() % 8;//��8����������ѡһ����������Ŷ�
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
			break;//�ƶ��󵥴����ڻ�����,�����½�
		}
		if(number<0){//100�κ����Ҳ����½⣬��ⲻ��
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
	}//�ƶ����������pixellabel��Ϊ0

	
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
	}//Ŀ��ͼ�����ѱ����ʸ��ǵ����
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
	//BlankArea(head, tail, wordtag, numberofblackpixel)��OverTheBorderArea(head, getlabel, imgw, imgh, pixellabel) + 
	int errorcount = 0;
	while (T>5){	
		j = 0;	
		Emax = Emin = Enew;
		number = 100;
		while (true){
			//cout << "number :" << number << endl;
			//������״̬���ж��Ƿ����

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
					if (pp > r){//����
						Eold = Enew;
						//E[j] = Enew;
						if (Enew > Emax) Emax = Enew;
						if (Enew < Emin) Emin = Enew;
						j++;
						//cout << '1' << ' ';
						
					}
					else {//������,����
						//p = changedpoint;
						for (i = (changedpoint->pointy - changedpoint->text_size.height+1); i <= changedpoint->pointy; i++){
							for (j = changedpoint->pointx; j < (changedpoint->pointx + changedpoint->text_size.width); j++){
								//if (i<imgw && j<imgh && i>0 && j>0) {
									if (pixellabel[i*imgw + j] == 0)
										pixellabel[i*imgw + j] = 1;//0->1
								//}
							}
						}//�ƶ�ǰ��ԭ�������pixellabel��Ϊ1

						int xs1, xb1, xs2, xb2;
						int ys1, yb1, ys2, yb2;
						int endx, startx, endy, starty;
						double width, height;
						p = head;
						xs2 = changedpoint->pointx;
						xb2 = changedpoint->pointx + changedpoint->text_size.width-1;
						ys2 = changedpoint->pointy - changedpoint->text_size.height+1;
						yb2 = changedpoint->pointy;

						while (p != NULL){//�ҳ���ԭ�������ص��ĵ��ʣ�����pixellabel��Ϊ0
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
							if (width > 0 && height > 0){//��ԭ�������ص�
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
						}//�ƶ��������pixellabel��Ϊ0
					}

				}

				//�жϵ��ʼ���ص������
				for (i = 0; i < tag; i++) wordtag[i] = -1;//û���ص��ı��Ϊ-1
				p = head;//ͳ�����ص�����ĵ���
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

				if (j == 10){	//ÿ10���ж�һ��E�ı仯�Ƿ��ȶ�		
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
	//ת��Ϊ��ֵͼ��
	BinaryImage();
	int Simgw, Simgh;
	int i, j, m, n;
	int* pixellabel;//ÿ�����ص�label,��䲿��Ϊ������ɫ����Ϊ�����ѷŵ��ʲ���Ϊ0
	

	IplImage* Simg;
	Simg = cvLoadImage("binaryimage.jpg");
	Simgw = Simg->width;
	Simgh = Simg->height;
	pixellabel = new int[Simgw*Simgh];

	//����pixellabel����ɫ����Ϊ1����ɫΪ-1
	for (i = 0; i < Simgh; i++){
		for (j = 0; j < Simgw; j++){
			if ((uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){
				pixellabel[i*Simgw + j] = 1;
				//cout << pixellabel[i*Simgw + j] << ' ';
			}
			//��ɫΪ1
			else pixellabel[i*Simgw + j] = -1;
		}
	}
	

	//�����طָ�
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


	//�������ֲ�����Ϊ����
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
	int blackmidpointx, blackmidpointy;//��ɫͼ����е�����
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
			if (label >= 0 && (uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){//��ɫ������
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
			if (label < 0){//��ɫ������
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
			
			if (label >= 0 && (uchar*)(Simg->imageData + Simg->widthStep*i)[j * 3] == 0){//ĳһδ���ù����ʵĺ�ɫ������
				tolpointx = 0;
				tolpointy = 0;
				nx = 0;
				for (m = i; m < Simgh; m++){//i*Simgw + j�ҵ���һ���������
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
					if (*(getlabel + m) == label)  *(getlabel + m) = -label - 1;//��
				}
				blackpixel[num].weight= -(pow((Simgw / 2 - tolpointx / nx), 2) + pow((Simgh / 2 - tolpointy / nx), 2));//���빫ʽ��δ�����ţ�ȡ��������ԽԶȨֵԽС
				blackpixel[num].midpointx = tolpointx / nx;
				blackpixel[num].midpointy = tolpointy / nx;
				blackpixel[num].label = -label-1;//blackpixellabelΪ��
				num++;
			}			
		}
	}

	blackmidpointx = blacktolpointx / numberofblackpixel;
	blackmidpointy = blacktolpointy / numberofblackpixel;
	

	double bufferweight;
	int bufferpointx, bufferpointy;
	int bufferlabel; 
	for (i = 0; i < num; i++){//������Ȩ�ذ������ɴ�С����
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
			//��Խ��
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
		pt.x = xs; //����������Ϊ���壨���ң����ж���
		pt.y = yb; //����������Ϊ���壨���£����ж���
		//�ж����������߽��������ƶ�
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
	
	
	
	//����϶�мӵ���
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
			//�жϣ��������߽���������
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
					 pixellabel[i*Simgw + j] = 0;//Ŀ��ͼ������䵥�ʵĲ�����Ϊ0,  1->0
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
			cvPutText(Simg, tp, cvPoint(p->pointx, p->pointy), &font, CV_RGB(a, a, a));//���뵥��
			p = p->next;
		}
		else p = p->next;
	}
	cout << "finished" << endl;

	//����ͼ��
	cvSaveImage("ResultImage.jpg", Simg);

	//�ͷ�ͼ��
	cvReleaseImage(&Simg);

	return 0;
	
}