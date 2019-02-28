void onCardiacMuscleContraction()
{
	double time = 0;
	double ForceExt = 0;
	system("rm -rf result");
	system("mkdir result");

	// 파일 12마리 생성
	FILE** fp_out = malloc(sizeof(FILE*) * 12);
	int i;
	char fileName[50];
	// 파일 12마리 오픈
	for (i = 0; i < 12; i++) {
		sprintf(fileName, "result%d.oneD", i);
		fp_out[i] = fopen(fileName, "w+");
	}
	printf("12 files open complete!\n");

	// 파일 12마리 Header 작성
	for (i = 0; i < 12; i += 4) { // 0, 4, 8
	//	fp_out[i + 1] = fopen(fileName, "w");
		if (fp_out[i] != NULL) {
			// 왼쪽 그래프 2개
			fprintf(fp_out[i], "#NumField: 2\n#LabelX: time(mSec), LabelY: Ca(nM)\n#Field1: one, NumPoint:%d\n", 20000);
			fprintf(fp_out[i + 1], "#Field2: two, NumPoint:%d\n", 20000);
			// 오른쪽 그래프 2개
			fprintf(fp_out[i + 2], "#NumField: 2\n#LabelX: time(mSec), LabelY: h(um) \n#Field1: three, NumPoint:%d\n", 20000);
			fprintf(fp_out[i + 3], "#Field2: four, NumPoint:%d\n", 20000);

			// 닫기 !
		}
	}
	printf("12 files header writing complete!\n");

	// 그래프를 그릴 점 4마리
	POINT outPoint[12]; //{ {0,0},{0,0},{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 } };
	for (i = 0; i < 12; i++)
	{
		outPoint[i].x = 0;
		outPoint[i].y = 0;
	}

	// 여기 중요하다
	// 0: Cell Length
	// 1: Force
	// 2: Actin-Myosin

	printf("Pointing work!\n");
	eightelement graph;
	int j = 0;
	for (i = 0; i < 3; i++)
	{
		double basalCa = 100.0e-6;
		// 시간동안 그린다 그래프를 한번에 네 마리
		for (time = 0; time < v.timelength; time += v.timestep)
		{
			// Setup Ca-transient
			if (time < 0.2*v.timelength) v.Cai = basalCa;
			else v.Cai = v.f1*0.5e-3*(exp(-(v.f3*(time - 0.2*v.timelength) / 0.3) / 50.0))*(6.0 - 6.0 * exp(-v.f2*(time - 0.2*v.timelength) / 0.3 / 50.0)) + basalCa;
			// Get mechanical force production
			Myofilament = Contraction(Myofilament, v.TRPNt, v.Cai, ForceExt, v.timestep);

			// calculate real XY-ordinates from value list
			switch (i)
			{
			case 0:
				graph = (eightelement) { time, time, time, time, v.Cai, Myofilament.d, Myofilament.e, Myofilament.d - Myofilament.e };
				break;
			case 1:
				graph = (eightelement) { time, time, v.Cai * 1000000, v.Cai * 1000000, v.Cai, Myofilament.f, Myofilament.g, Myofilament.h };
				break;
			case 2:
				graph = (eightelement) { time, time, time, time, v.Cai, Myofilament.b + Myofilament.c, 1.0 - Myofilament.b - Myofilament.c - Myofilament.a, Myofilament.a };
				break;
			default:
				graph = (eightelement) { time, time, time, time, v.Cai, Myofilament.d, Myofilament.e, Myofilament.d - Myofilament.e };
				break;
				printf("%d\n", i);

			}

			if ((int)time % 5 == 0) {
				// 왼쪽 그래프
				outPoint[j] = (POINT) { graph.a, graph.e * 1000000 };
				outPoint[j + 1] = (POINT) { graph.b, graph.f };
				fprintf(fp_out[j], "%lf	%lf\n", outPoint[j].x, outPoint[j].y);
				fprintf(fp_out[j + 1], "%lf   %lf\n", outPoint[j + 1].x, outPoint[j + 1].y);

				//	printf("%lf %lf %lf %lf \n", outPoint[j].x, outPoint[j].y, outPoint[j+1].x, outPoint[j+1].y);

					// 오른쪽 그래프
				outPoint[j + 2] = (POINT) { graph.c, graph.g };
				outPoint[j + 3] = (POINT) { graph.d, graph.h };
				fprintf(fp_out[j + 2], "%lf    %lf\n", outPoint[j + 2].x, outPoint[j + 2].y);
				fprintf(fp_out[j + 3], "%lf   %lf\n", outPoint[j + 3].x, outPoint[j + 3].y);
			}
		}
		j += 4;
	}
	printf("Pointing work complete!\n");

	// 입력 파일, 출력  파일, [x0:x1], [y0:y1], 두번째 y축 사용여부(0,1), 2_y0, 2_y1, 두번째 입력 파일
	drawGraph("result0.oneD", "result/result0.png", 0, 1000, 0, 2000, 1, 0.8, 1.0, "result1.oneD");
	drawGraph("result2.oneD", "result/result2.png", 0, 1000, 0.003, 0.007, 1, 0, 1, "result3.oneD");

	drawGraph("result4.oneD", "result/result4.png", 0, 1000, 62.5, 887.5, 1, 11.540, 27.492, "result5.oneD");
	drawGraph("result6.oneD", "result/result6.png", 100, 850, 5.766, 13.753, 1, 5.773, 13.741, "result7.oneD");

	drawGraph("result8.oneD", "result/result8.png", 0, 1000, 62.5, 887.5, 1, 0.009, 0.022, "result9.oneD");
	drawGraph("result10.oneD", "result/result10.png", 0, 1000, 0.588, 0.894, 1, 0.097, 0.394, "result11.oneD");



	// 파일 12마리 작업 0, 4, 8
	for (i = 0; i < 12; i += 4) {
		// 파일 합치기
		// 왼쪽 그래프 합치기
		fseek(fp_out[i + 1], 0L, SEEK_SET);
		combine(fp_out[i], fp_out[i + 1]);
		// 오른쪽 그래프 합치기
		fseek(fp_out[i + 3], 0L, SEEK_SET);
		combine(fp_out[i + 2], fp_out[i + 3]);

		// 파일 삭제
		sprintf(fileName, "rm result%d.oneD", i + 1);
		system(fileName);
		printf("\"%s\" success!\n", fileName);
		sprintf(fileName, "rm result%d.oneD", i + 3);
		system(fileName);
		printf("\"%s\" success!\n\n", fileName);

		sprintf(fileName, "mv result%d.oneD result/result%d.oneD", i, i);
		system(fileName);

		sprintf(fileName, "mv result%d.oneD result/result%d.oneD", i + 2, i + 2);
		system(fileName);
		printf("Move files to result folder\n");
	}

	free(fp_out);
	printf("file 12 close !\n");
}
