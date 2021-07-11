#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS

int main(void)
{
	// AND gate
	// input
	double X[4][2] =
	{
		{0.0, 0.0}, //  0
		{0.0, 1.0}, //  0
		{1.0, 0.0}, //  0
		{1.0, 1.0}  //  1
	};

	// 실제 output
	double Y[4] = { 0.0, 0.0, 0.0, 1.0 };

	double net; //net값
	double x1, x2; //입력값
	double decision[4]; //계산해서 구한 예상값
	int count = 0;
	int forbreak = 0;
	double weight[4]; //가중치
	srand(time(NULL));

	while (1) {
		//weight값 random값으로 초기화
		for (int i = 0; i < 3; i++) {
			weight[i] = ((double)rand() / RAND_MAX) * 2 - 1;
		}

		for (int i = 0; i < 4; i++) {
			//net 구하기
			x1 = X[i][0];
			x2 = X[i][1];
			net = 1 * weight[0] + x1 * weight[1] + x2 * weight[2];

			//theta 값을 0으로 지정후 0보다 작을경우 0울 츌력 그 외의 값에는 1을 출력함.
			if (net < 0) {
				decision[i] = 0;
			}
			else if (net >= 0) {
				decision[i] = 1;
			}

			//예측값과 실제 정답이 다를 경우 틀린 갯수를 카운팅함.
			if (decision[i] != Y[i])
			{
				count++;
			}

			printf("input값: %lf %lf, 가중치: %lf %lf %lf, 예상값: %lf, 정답: %lf, 카운팅: %d\n", x1, x2, weight[0], weight[1], weight[2], decision[i], Y[i], count);
		}
		//예측값과 정답값이 일치하는 횟수를 계산함.
		for (int i = 0; i < 4; i++) {
			if (Y[i] == decision[i]) {
				forbreak++;
			}
		}

		//네가지 input에 대한 output이 모두 일치할 경우 break
		if (forbreak == 4) {
			break;
		}

		//하나라도 일치하지 않을 경우 다시 반복문으로 돌아감.
		if (forbreak != 4) {
			forbreak = 0;
		}
	}

	//탈출했을 때 최종 가중치 값
	printf("최종 가중치: ");
	for (int i = 0; i < 3; i++) {
		printf("%lf ", weight[i]);
	}

	//탈출했을 때 최종 카운팅값
	printf(", 카운팅 값: %d \n", count);

	system("pause");
	return 0;
}