#include <iostream> //������� ����
#include <vector> 
#include <iomanip>
#include <cassert>
#include <initializer_list>
#include <random>
#include <cmath>
#include <algorithm>
#include <stdexcept>

//'�̸����� std�� ����� ��� �Ϳ� ���� �̸����� ������ ����'�� ���
using namespace std;

class TrainSet {
public:
	TrainSet() { }
	//initializer_list: �� ����� ������ ������ ��� �迭�� ���� �׼����� ����
		//pair: �� ��ü�� ���� ��ü�� ó���ϴ� ����� �����ϴ� ����ü
	TrainSet(const std::initializer_list<std::pair<std::vector<double>, double>>& trainingSet) {

		reset(trainingSet);
	}

private:
	vector<std::vector<double>> tinputs;
	vector<double> toutputs;

public:
	void reset(const std::initializer_list<std::pair<std::vector<double>, double>>& trainingSet) {
		//auto:  �ش� ������ ������ �ʱ�ȭ �Ŀ��� �߷� �Ǵ� ������ ����
		auto inputSize = trainingSet.begin()->first.size();
		tinputs.clear();
		toutputs.clear();

		for (auto& item : trainingSet) {
			tinputs.emplace_back(item.first);
			toutputs.emplace_back(item.second);
		}
	}

public:
	std::size_t getSetCount() const { return toutputs.size(); }

	std::size_t getInputSize() const {
		if (tinputs.size() > 0)
			return tinputs[0].size();
		return 0;
	}

	void copyInput(int indexOfSet, std::vector<double>& outInput) const {
		outInput.clear();
		//assign: ���� ��ü�� ������ �־��� ���ҵ��� ��� �����ϰ�, ���ڷ� ���� ���ο� ������ ���� ����.
		outInput.assign(tinputs[indexOfSet].begin(), tinputs[indexOfSet].end());
	}
	double getOutput(int indexOfSet) const { return toutputs[indexOfSet]; }
};

class Neuron {
public:
	Neuron() : tbias(0), tlatestOutput(0), tlatestDelta(0) { }
	std::vector<double> tweights;
private:
	double tbias;
	double tlatestOutput;
	double tlatestDelta;

public:
	void reset(std::size_t inputSize) {
		tlatestOutput = 0;
		tlatestDelta = 0;
		tweights.clear();
		tweights.resize(inputSize);

		// ����ġ�� �����ϰ� �ʱ�ȭ �Ѵ�.
		std::random_device rd;
		std::mt19937 engine{ rd() };
		std::uniform_real_distribution<> dist{ -1.0, 1.0 };
		tbias = dist(engine);

		for (auto& weight : tweights) {
			weight = dist(engine);
		}
	}

public:
	double calculate(const std::vector<double>& input) {
		//assert: ���� ��ü�� ������ �־��� ���ҵ��� ��� �����ϰ�, ���ڷ� ���� ���ο� ������ ���� ����.
		assert(input.size() == tweights.size());
		const auto inputSize = input.size();

		double sum = tbias;
		for (std::size_t i = 0; i < inputSize; ++i) {
			sum += tweights[i] * input[i];
		}

		// Ȱ��ȭ �Լ��� �ñ׸��̵带 ����Ѵ�.
		tlatestOutput = 1.0 / (1.0 + std::exp(-sum));

		return tlatestOutput;
	}

	double getLatestOutput() const { return tlatestOutput; }

	std::size_t getInputSize() const { return tweights.size(); }

	double getWeight(int index) const {
		return tweights[index];
	}

	void addWeight(int index, double delta) { tweights[index] += delta; }

	double getBias() const { return tbias; }

	void addBias(double delta) { tbias = delta; }

	double getLatestDelta() const { return tlatestDelta; }

	void setLatestDelta(double delta) { tlatestDelta = delta; }
};

class Network {
public:
	Network() { }

private:
	std::vector<std::vector<double>> tinputs;
	std::vector<double> ttargetOutputs;
	vector<double> weightss;

private:
	std::vector<std::vector<Neuron>> m_layers;

public:
	void reset(const TrainSet& trainingSet, const std::initializer_list<std::size_t>& hiddenLayerSizeList = {}) {
		auto setCount = trainingSet.getSetCount();
		auto inputSize = trainingSet.getInputSize();

		tinputs.clear();
		ttargetOutputs.clear();
		tinputs.resize(setCount);

		for (std::size_t i = 0; i < setCount; ++i) {
			trainingSet.copyInput(i, tinputs[i]);
			ttargetOutputs.emplace_back(trainingSet.getOutput(i));
		}

		// Build Network
		m_layers.clear();

		// Input Layer
		m_layers.emplace_back();
		m_layers[0].resize(inputSize);
		std::for_each(m_layers[0].begin(), m_layers[0].end(), [inputSize](Neuron& neuron) {
			neuron.reset(inputSize);
		});

		// Hidden Layer
		std::size_t prevLayerSize = m_layers[0].size();
		for (auto& layerSize : hiddenLayerSizeList) {
			m_layers.emplace_back();
			auto& layer = *(m_layers.end() - 1);

			layer.resize(layerSize);
			std::for_each(layer.begin(), layer.end(), [prevLayerSize](Neuron& neuron) {
				neuron.reset(prevLayerSize);
			});

			prevLayerSize = layer.size();
		}

		// Output Layer
		m_layers.emplace_back();
		auto& outputLayer = *(m_layers.end() - 1);
		outputLayer.resize(1);
		outputLayer[0].reset(prevLayerSize);
	}

public:
	double feed(const std::vector<double>& input) {
		std::vector<double> prevOutput = input;
		std::vector<double> outputs;

		for (auto& layer : m_layers) {
			for (auto& neuron : layer) {
				double result = neuron.calculate(prevOutput);
				outputs.emplace_back(result);
			}

			prevOutput = std::move(outputs);
			outputs.clear();
		}

		if (prevOutput.size() > 0)
			return prevOutput[0];
		return 0;
	}

	void train(std::size_t loopCount, double learningRate) {
		for (std::size_t step = 0; step < loopCount; ++step) {
			auto setCount = tinputs.size();

			// �� �н��¿� ���ؼ� �н��Ѵ�.
			for (std::size_t i = 0; i < setCount; ++i) {
				train(tinputs[i], ttargetOutputs[i], learningRate);
			}
		}
	}

	void train(const std::vector<double>& input, double targetOutput, double learningRate) {
		double finalOutput = feed(input);
		double finalDelta = finalOutput * (1.0 - finalOutput) * (targetOutput - finalOutput);
		auto it = m_layers.rbegin();
		auto rEnd = m_layers.rend();

		/// ������ ó���� ��. ��Ÿ ���� ���� ��.
		auto* prevLayer = &*it;
		(*prevLayer)[0].setLatestDelta(finalDelta);

		// ���� ������ �� ���� ������ �Է��� ������ ��Ÿ ��� �� ����ġ ����.
		for (it += 1; it != rEnd; ++it) {
			auto& layer = *it;
			auto neuronCount = layer.size();
			for (std::size_t n = 0; n < neuronCount; ++n) {
				auto& neuron = layer[n];
				double output = neuron.getLatestOutput();
				double deltaSum = 0;

				// ���� ������ ���� ��Ÿ�� ����ϸ鼭 ���ÿ�
				// ��´ܿ� ����� ������ ����ġ ����.
				for (auto& prevNeuron : *prevLayer) {
					double prevDelta = prevNeuron.getLatestDelta();
					deltaSum += prevNeuron.getWeight(n) * prevDelta;

					// ��´� ������ ����ġ ����.
					prevNeuron.addWeight(n, learningRate * output * prevDelta);
				}

				double delta = deltaSum * output * (1.0 - output);
				neuron.setLatestDelta(delta);

				// ���̾ ����.
				neuron.addBias(learningRate * neuron.getBias() * delta);
			}
			prevLayer = &layer;
		}
	}
};


int main() {
	const double learningRate = 0.4;

	int select;
	int count = 0;
	std::cout << "GATE�� ������ ���ڷ� �Է����ּ���.(AND(1), OR(2), XOR(3)): ";
	std::cin >> select;

	Neuron neuron;

	switch (select) {
	case 1: {
		/// �н� ������
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 0 },
			{ { 1, 0 }, 0 },
			{ { 1, 1 }, 1 },
		};
		/// �Ű��
		Network net;
		// �н� �����ͷ� �Է���, ������� �ڵ����� ���������
		// �������� ���� ũ�⸦ �޾� �����.
		// �Ʒ� �ڵ��� ��� �Է�2 -> ����3 -> ���1���� �ȴ�.
		net.reset(trainingSet, { 3 });
		/// �н� ������ ����
		auto setCount = trainingSet.getSetCount();
		/// ���� �Է� ����
		auto inputSize = trainingSet.getInputSize();

		vector<vector<double>> inputs;
		vector<double> targetOutputs;
		inputs.resize(setCount);

		for (std::size_t i = 0; i < setCount; ++i) {
			trainingSet.copyInput(i, inputs[i]);
			targetOutputs.emplace_back(trainingSet.getOutput(i));
		}
		size_t maxEpoch = 100000;

		for (size_t epoch = 0; epoch < maxEpoch; ++epoch) {
			// �н�.
			net.train(5000, learningRate);
			/// ������
			double errorRate = 0;

			for (size_t i = 0; i < setCount; ++i) {
				for (auto& data : inputs[i]) {
					std::cout << data << " \t";
				}
				double output = net.feed(inputs[i]);
				cout << " -> \t" << output << " => \t" << targetOutputs[i] << endl;
				errorRate += abs(targetOutputs[i] - output);
			}

			cout << endl;
			errorRate /= 2.0;
			cout << setprecision(6) << "Error : " << errorRate * 100.0 << "%" << endl;
			count++;

			// ������ ���� ���� �̸����� �������� ����.
			if (errorRate < 0.01) {
				cout << "����!" << endl;
				cout << "���� ������ : " << errorRate * 100.0 << "%" << endl;
				cout << "�� �н�Ƚ�� : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}

	case 2: {
		/// �н� ������
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 1 },
			{ { 1, 0 }, 1 },
			{ { 1, 1 }, 1 },
		};
		/// �Ű��
		Network net;
		net.reset(trainingSet, { 3 });
		/// �н� ������ ����
		auto setCount = trainingSet.getSetCount();
		/// ���� �Է� ����
		auto inputSize = trainingSet.getInputSize();

		vector<vector<double>> inputs;
		vector<double> targetOutputs;
		inputs.resize(setCount);

		for (std::size_t i = 0; i < setCount; ++i) {
			trainingSet.copyInput(i, inputs[i]);
			targetOutputs.emplace_back(trainingSet.getOutput(i));
		}

		size_t maxEpoch = 100000;

		for (size_t epoch = 0; epoch < maxEpoch; ++epoch) {
			// �н�.
			net.train(5000, learningRate);
			/// ������
			double errorRate = 0;

			for (size_t i = 0; i < setCount; ++i) {
				for (auto& data : inputs[i]) {
					std::cout << data << " \t";
				}
				double output = net.feed(inputs[i]);
				cout << " -> \t" << output << " => \t" << targetOutputs[i] << endl;
				errorRate += abs(targetOutputs[i] - output);
			}

			errorRate /= 2.0;
			cout << setprecision(6) << "Error : " << errorRate * 100.0 << "%" << endl;
			count++;

			// ������ ���� ���� �̸����� �������� ����.
			if (errorRate < 0.01) {
				cout << "����!" << endl;
				cout << "���� ������ : " << errorRate * 100.0 << "%" << endl;
				cout << "�� �н�Ƚ�� : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}

	case 3: {
		/// �н� ������
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 1 },
			{ { 1, 0 }, 1 },
			{ { 1, 1 }, 0 },
		};
		/// �Ű��
		Network net;
		net.reset(trainingSet, { 3 });
		/// �н� ������ ����
		auto setCount = trainingSet.getSetCount();
		/// ���� �Է� ����
		auto inputSize = trainingSet.getInputSize();

		vector<vector<double>> inputs;
		vector<double> targetOutputs;
		inputs.resize(setCount);

		for (std::size_t i = 0; i < setCount; ++i) {
			trainingSet.copyInput(i, inputs[i]);
			targetOutputs.emplace_back(trainingSet.getOutput(i));
		}

		size_t maxEpoch = 100000;

		for (size_t epoch = 0; epoch < maxEpoch; ++epoch) {
			// �н�.
			net.train(5000, learningRate);
			/// ������
			double errorRate = 0;

			for (size_t i = 0; i < setCount; ++i) {
				for (auto& data : inputs[i]) {
					std::cout << data << " \t";
				}
				double output = net.feed(inputs[i]);
				cout << " -> \t" << output << " => \t" << targetOutputs[i] << endl;
				errorRate += abs(targetOutputs[i] - output);
			}

			cout << endl;
			errorRate /= 2.0;
			cout << setprecision(6) << "Error : " << errorRate * 100.0 << "%" << endl;
			count++;

			// ������ ���� ���� �̸����� �������� ����.
			if (errorRate < 0.01) {
				cout << "����!" << endl;
				cout << "���� ������ : " << errorRate * 100.0 << "%" << endl;
				cout << "�� �н�Ƚ�� : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}
	}
	return 0;
}
