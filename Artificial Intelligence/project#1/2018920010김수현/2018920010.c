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

	// ���� output
	double Y[4] = { 0.0, 0.0, 0.0, 1.0 };

	double net; //net��
	double x1, x2; //�Է°�
	double decision[4]; //����ؼ� ���� ����
	int count = 0;
	int forbreak = 0;
	double weight[4]; //����ġ
	srand(time(NULL));

	while (1) {
		//weight�� random������ �ʱ�ȭ
		for (int i = 0; i < 3; i++) {
			weight[i] = ((double)rand() / RAND_MAX) * 2 - 1;
		}

		for (int i = 0; i < 4; i++) {
			//net ���ϱ�
			x1 = X[i][0];
			x2 = X[i][1];
			net = 1 * weight[0] + x1 * weight[1] + x2 * weight[2];

			//theta ���� 0���� ������ 0���� ������� 0�� ���� �� ���� ������ 1�� �����.
			if (net < 0) {
				decision[i] = 0;
			}
			else if (net >= 0) {
				decision[i] = 1;
			}

			//�������� ���� ������ �ٸ� ��� Ʋ�� ������ ī������.
			if (decision[i] != Y[i])
			{
				count++;
			}

			printf("input��: %lf %lf, ����ġ: %lf %lf %lf, ����: %lf, ����: %lf, ī����: %d\n", x1, x2, weight[0], weight[1], weight[2], decision[i], Y[i], count);
		}
		//�������� ���䰪�� ��ġ�ϴ� Ƚ���� �����.
		for (int i = 0; i < 4; i++) {
			if (Y[i] == decision[i]) {
				forbreak++;
			}
		}

		//�װ��� input�� ���� output�� ��� ��ġ�� ��� break
		if (forbreak == 4) {
			break;
		}

		//�ϳ��� ��ġ���� ���� ��� �ٽ� �ݺ������� ���ư�.
		if (forbreak != 4) {
			forbreak = 0;
		}
	}

	//Ż������ �� ���� ����ġ ��
	printf("���� ����ġ: ");
	for (int i = 0; i < 3; i++) {
		printf("%lf ", weight[i]);
	}

	//Ż������ �� ���� ī���ð�
	printf(", ī���� ��: %d \n", count);

	system("pause");
	return 0;
}