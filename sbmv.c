#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <stdarg.h>

#include "Calculate.h"
#include "var.h"

typedef struct {
	double a, b, c, d;
}fourelement;

int checkArgs(int argc, char* args[]);
void loadInputDeckFile();
void matchingData();
void Calculate();
void SingleBarrierModelView();
void drawGraph(char* inputFileName, char* outputFileName, double xRange, double xRange2, double yRange, double yRange2, int secondAxis, double y2Range, double y2Range2, char* inputFileName2);
//FILE* combine(FILE* cat1, FILE* cat2);
void combine(FILE *cat1, FILE* cat2);
static FILE *fp_inputdeck;

static Var v;


int main(int argc, char* args[])
{
	double time = 0;
	// ./실행파일명 -inp input.dat
	if (checkArgs(argc, args))
	{

		loadInputDeckFile();
		printf("loadInputDeckFile Complite!!\n");

		SingleBarrierModelView(); // 함수 좌표를  파일로 저장하는거 
		printf("SingleBarrierModelView Complited\n");

	}
	return 0;
}

int checkArgs(int argc, char* argv[])
{
	int count;
	char args_error_flag = 0;
	if (argc > 1) {
		for (count = 1; count < argc; count += 2) {
			if (!strcmp(argv[count], "cmc_inp")) {
				printf("cmc_inp : %s \n", argv[count + 1]);
				fp_inputdeck = fopen(argv[count + 1], "r");
				if (fp_inputdeck == NULL) {
					printf("Error opening %s file. path : %s \n", argv[count], argv[count + 1]);
					exit(1);
				}
				else {
					printf("%s file open! \n", argv[count]);
					return 1;
				}
			}
			else {
				printf("Invalid command option: %s\n", argv[count]);
				args_error_flag = 1;
				return 0;
			}
		}
		if (args_error_flag == 1) {
			printf("Check your comand option\n");
			return 0;
		}
	}
	else {
		printf("Invalid arguments number.\n");
		return 0;
	}
	if (fp_inputdeck == NULL) {
		printf("Error opening fp_inputdeck file \n");
		return 0;
	}
	fclose(fp_inputdeck);
}

void loadInputDeckFile()
{
	matchingData();

	fclose(fp_inputdeck);

}

void matchingData()
{
	char buf_char[256];
	while (1)
	{
		fscanf(fp_inputdeck, "%s", buf_char);
		if (feof(fp_inputdeck)) {
			break;
		}
		if (!strcmp(buf_char, "Cout")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.Cout);
		}
		else if (!strcmp(buf_char, "Cin")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.Cin);
		}
		else if (!strcmp(buf_char, "delta")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.delta);
		}
		else if (!strcmp(buf_char, "RT")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.RT);
		}
		else if (!strcmp(buf_char, "beta")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.beta);
		}
		else if (!strcmp(buf_char, "zF")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.zF);
		}
		else if (!strcmp(buf_char, "delG0")) {
			fscanf(fp_inputdeck, "%*s %lf %*s", &v.delG0);
		}
		else {
			printf("Matching Error Invalid value name :: %s\n", buf_char);
			exit(1);
		}
	}
}
POINT CalculatePoint(double x, double y, fourelement area, fourelement XY)
{
	POINT outP;
	// 좌표산출 방정식
	outP.x = (area.a + (x - XY.a)*(area.c) / (XY.b - XY.a));
	outP.y = (area.b + area.d - (y - XY.c)*(area.d) / (XY.d - XY.c));
	if (outP.y >= area.b + area.d) outP.y = area.b + area.d - 1;
	if (outP.y <= area.b) outP.y = area.b + 1;
	if (outP.x >= area.a + area.c) outP.x = area.a + area.c - 1;
	if (outP.x <= area.a) outP.x = area.a + 1;
	return outP;
}
void SingleBarrierModelView()//q바꿔야함
{
	system("rm -rf result");
	system("mkdir result");

	//create File
	FILE** fp_out = malloc(sizeof(FILE*) * 3);
	int i;
	char fileName[50];

	//File open
	for (i = 0; i < 3; i++)
	{
		sprintf(fileName, "result%d.oneD", i);
		fp_out[i] = fopen(fileName, "w+");
	}
	printf("3 files open!!\n");

	//making header of 3 files
	
		
			fprintf(fp_out[0], "#NumField: 3\n #LabelX: Voltage(mV), LabelY: NetCurrent(A)\n #Field1: EreV, NumPoint:%d\n",1800);
			fprintf(fp_out[1], "#NumField: 3\n #LabelX: Voltage(mV), LabelY: NetCurrent(A)\n #Field2: RED, NumPoint : %d\n", 1800);
			fprintf(fp_out[2], "#NumField: 3\n #LabelX: Voltage(mV), LabelY: NetCurrent(A)\n #Field3: BLUE, NumPoint : %d\n", 1800);
		
	
	printf("writing 3 files header has complited \n");

	//그래프를 그릴 점 3개
	POINT outPoint[3];
	for (i = 0; i < 3; i++)
	{
		outPoint[i].x = 0;
		outPoint[i].y = 0;
	}
	printf("pointing working!\n");

	// 상수 선언
	double RT = 8.314*293.0;						// J / mol
	double beta = 1.0;								// beta: partition coefficient of water-membrane for the ion
	double zF = 1.0*96.480;							// C / mmol
	double delG0 = 100.0e3;							// ex) 100 kJ/mol; standard free energy of activation
	printf("\n\n ======%lf======== \n\n", delG0);
	double delta = 0.5; //double(nPos4)*0.01;		        // factor of asymmetry, delta = 1 when the energy barrier is on the outside margin of the membrane
	double Cout = 100; //double(nPos2)*2.0;
	double Cin = 100; //double(nPos3)*2.0;

	double Vm[1800];
	double ISB[1800];
	//POINT oldPoint = { 0, 0 };
	POINT cPoint = { 0, 0 };


	fourelement GraphXY = { -90, 90, -1.0, 1.0 };
	fourelement GraphArea = { 80, 80, 400, 400 };
	printf("setup var complete!!\n");
	
	
	int j = 0, k = 0, d = 0;
	int ii;
	for (ii = 0; ii < 1800; ii++)	 //Black Line
	{
		Vm[ii] = -90.0 + 0.1*ii;
		ISB[ii] = zF*beta*1.38e-23*293.15 / 6.626e-34*exp(-delG0 / RT)*(Cin*exp(delta*zF*Vm[ii] / RT) - Cout*exp(-(1 - delta)*zF*Vm[ii] / RT));

		cPoint = CalculatePoint(Vm[ii], ISB[ii], GraphArea, GraphXY);
		if (ii > 0)
		{
			if (ISB[ii] < GraphXY.d && ISB[ii] > GraphXY.c)
			{
				//pDC->MoveTo(oldPoint);
				//pDC->LineTo(cPoint);
				outPoint[j] = cPoint;
				fprintf(fp_out[j], "%lf %lf\n", outPoint[j].x, outPoint[j].y*0.001);

			}

		}
		//oldPoint = cPoint;
	}
	j++;
	printf("black line complete!!!!\n");
	
	for ( ii = 0; ii < 1800; ii++)	//Red Line
	{
		Vm[ii] = -90.0 + 0.1*ii;
		ISB[ii] = zF*beta*1.38e-23*293.15 / 6.626e-34*exp(-delG0 / RT)*(Cin*exp(delta*zF*Vm[ii] / RT));

		cPoint = CalculatePoint(Vm[ii], ISB[ii], GraphArea, GraphXY);
		if (ii > 0)
		{
			if (ISB[ii] < GraphXY.d && ISB[ii] > GraphXY.c)
			{
				outPoint[k +1] = cPoint;
				fprintf(fp_out[k +1], "%lf %lf\n", outPoint[k +1].x, outPoint[k +1].y*0.001);
			}

		}
		
	}k++;
	printf("red line complete!!!!\n");

	for (ii = 0; ii < 1800; ii++)	//Blue Line
	{
		Vm[ii] = -90.0 + 0.1*ii;
		ISB[ii] = zF*beta*1.38e-23*293.15 / 6.626e-34*exp(-delG0 / RT)*(-Cout*exp(-(1 - delta)*zF*Vm[ii] / RT));

		cPoint = CalculatePoint(Vm[ii], ISB[ii], GraphArea, GraphXY);
		if (ii > 0)
		{
			if (ISB[ii] < GraphXY.d && ISB[ii] > GraphXY.c)
			{
				outPoint[d + 2] = cPoint;
				fprintf(fp_out[d + 2], "%lf %lf\n", outPoint[d + 2].x, outPoint[d + 2].y*0.001);
			}

		}
		
	}d++;
	printf("blue line complete!!!!\n");

	//*************************************************************//
	// 입력 파일, 출력  파일, [x0:x1], [y0:y1], 두번째 y축 사용여부(0,1), 2_y0, 2_y1, 두번째 입력 파일
	drawGraph("result0.oneD", "result/result0.png", -80, 500, -1, 1, 0, 0, 0, "");
	drawGraph("result1.oneD", "result/result1.png", -80, 500, -1, 1, 0, 0, 0, "result2.oneD");
	drawGraph("result2.oneD", "result/result2.png", -80, 500, -1, 1, 0, 0, 0, "");
	//drawGraph("result1.oneD", "result/result1.png", -90, 90, -1.0, 1.0, 1, 0, 0, "result2.oneD");
	//drawGraph("result2.oneD", "result/result2.png", -90, 90, -1.0, 1.0, 1, 0, 0, "result0.oneD");
	
	//*************************************************************

	// 파일 3마리 합치기
	for (i = 0; i < 3; i += 3) {
		// 파일 합치기 0,1,2
		fseek(fp_out[i + 1], 0L, SEEK_SET);
		combine(fp_out[i], fp_out[i + 1]);

		fseek(fp_out[i + 2], 0L, SEEK_SET);
		combine(fp_out[i], fp_out[i + 2]);
		

		// 파일 삭제 
		//sprintf(fileName, "rm result%d.oneD", i + 1);
		//system(fileName);
		//printf("\"%s\" success!\n", fileName);
		//sprintf(fileName, "rm result%d.oneD", i + 2);
		//system(fileName);
		//printf("\"%s\" success!\n", fileName);


		printf("here is file combine !!\n");

		sprintf(fileName, "mv result%d.oneD result/result%d.oneD", i, i);
		system(fileName);
		sprintf(fileName, "mv result%d.oneD result/result%d.oneD", 1, 1);
		system(fileName);
		//sprintf(fileName, "mv result%d.oneD result/result%d.oneD", i+1, i+1);
		//system(fileName);

		printf("Move files to result folder\n");
	}

	free(fp_out);
	printf("file 3 close !\n");
	
}
void drawGraph(char* inputFileName, char* outputFileName, double xRange, double xRange2, double yRange, double yRange2, int secondAxis, double y2Range, double y2Range2, char* inputFileName2)
{
	FILE* fp_gnuplot = fopen("plot.gnu", "w");
	char set_cmd[500];
	char plot_cmd[500];

	sprintf(set_cmd, "set term png\nset output \"%s\"\nset xrange[%lf:%lf]\nset yrange[%lf:%lf]\n", outputFileName, xRange, xRange2, yRange, yRange2);
	if (!secondAxis)
		sprintf(plot_cmd, "plot '%s'  with lines \n", inputFileName);		//with lines linecolor rgb \"EreV\"\n", outputFileName);		//이부분 쓴다
	else
		sprintf(plot_cmd, "set y2range[%lf:%lf]\nset y2tics\nplot '%s' with lines, \\\n'%s' with lines axes x1y1\n", y2Range, y2Range2, inputFileName, inputFileName2);

	strcat(set_cmd, plot_cmd);
	printf("%s\n\n", set_cmd);
	printf("is it ok? can draw??\n\n");
	fprintf(fp_gnuplot, set_cmd);
	fclose(fp_gnuplot);

	system("gnuplot plot.gnu");
}
void Calculate()
{
}
void combine(FILE *cat1, FILE* cat2)
{
	printf("hello combine\n");
	char buf_char[256];
	FILE* fp_out = cat1;
	if (fp_out) {
		while (fgets(buf_char, 256, cat2) != NULL) {
			//	    printf("combine: %s\n",buf_char);
			fputs(buf_char, fp_out);
		}
	}
	else {
		printf("ERROR!! both files not opened\n");
	}
}



