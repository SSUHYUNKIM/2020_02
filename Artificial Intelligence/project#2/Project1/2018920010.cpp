#include <iostream> //헤더파일 선언문
#include <vector> 
#include <iomanip>
#include <cassert>
#include <initializer_list>
#include <random>
#include <cmath>
#include <algorithm>
#include <stdexcept>

//'이름공간 std에 선언된 모든 것에 대해 이름공간 지정의 생략'을 명령
using namespace std;

class TrainSet {
public:
	TrainSet() { }
	//initializer_list: 각 멤버가 지정된 형식인 요소 배열에 대한 액세스를 제공
		//pair: 두 개체를 단일 개체로 처리하는 기능을 제공하는 구조체
	TrainSet(const std::initializer_list<std::pair<std::vector<double>, double>>& trainingSet) {

		reset(trainingSet);
	}

private:
	vector<std::vector<double>> tinputs;
	vector<double> toutputs;

public:
	void reset(const std::initializer_list<std::pair<std::vector<double>, double>>& trainingSet) {
		//auto:  해당 형식이 선언의 초기화 식에서 추론 되는 변수를 선언
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
		//assign: 벡터 객체에 이전에 있었던 원소들은 모두 삭제하고, 인자로 받은 새로운 내용을 집어 넣음.
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

		// 가중치는 랜덤하게 초기화 한다.
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
		//assert: 벡터 객체에 이전에 있었던 원소들은 모두 삭제하고, 인자로 받은 새로운 내용을 집어 넣음.
		assert(input.size() == tweights.size());
		const auto inputSize = input.size();

		double sum = tbias;
		for (std::size_t i = 0; i < inputSize; ++i) {
			sum += tweights[i] * input[i];
		}

		// 활성화 함수는 시그모이드를 사용한다.
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

			// 각 학습셋에 대해서 학습한다.
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

		/// 이전에 처리한 층. 델타 값이 계산된 층.
		auto* prevLayer = &*it;
		(*prevLayer)[0].setLatestDelta(finalDelta);

		// 가장 마지막 층 이전 층부터 입력층 순서로 델타 계산 및 가중치 조절.
		for (it += 1; it != rEnd; ++it) {
			auto& layer = *it;
			auto neuronCount = layer.size();
			for (std::size_t n = 0; n < neuronCount; ++n) {
				auto& neuron = layer[n];
				double output = neuron.getLatestOutput();
				double deltaSum = 0;

				// 현재 뉴런에 대해 델타를 계산하면서 동시에
				// 출력단에 연결된 뉴런의 가중치 조절.
				for (auto& prevNeuron : *prevLayer) {
					double prevDelta = prevNeuron.getLatestDelta();
					deltaSum += prevNeuron.getWeight(n) * prevDelta;

					// 출력단 뉴런의 가중치 조절.
					prevNeuron.addWeight(n, learningRate * output * prevDelta);
				}

				double delta = deltaSum * output * (1.0 - output);
				neuron.setLatestDelta(delta);

				// 바이어스 조절.
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
	std::cout << "GATE의 종류를 숫자로 입력해주세요.(AND(1), OR(2), XOR(3)): ";
	std::cin >> select;

	Neuron neuron;

	switch (select) {
	case 1: {
		/// 학습 데이터
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 0 },
			{ { 1, 0 }, 0 },
			{ { 1, 1 }, 1 },
		};
		/// 신경망
		Network net;
		// 학습 데이터로 입력층, 출력층은 자동으로 만들어지며
		// 은닉층은 따로 크기를 받아 만든다.
		// 아래 코드의 경우 입력2 -> 은닉3 -> 출력1개가 된다.
		net.reset(trainingSet, { 3 });
		/// 학습 데이터 개수
		auto setCount = trainingSet.getSetCount();
		/// 동시 입력 개수
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
			// 학습.
			net.train(5000, learningRate);
			/// 에러율
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

			// 에러가 일정 수준 미만으로 떨어지면 종료.
			if (errorRate < 0.01) {
				cout << "종료!" << endl;
				cout << "최종 에러값 : " << errorRate * 100.0 << "%" << endl;
				cout << "총 학습횟수 : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}

	case 2: {
		/// 학습 데이터
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 1 },
			{ { 1, 0 }, 1 },
			{ { 1, 1 }, 1 },
		};
		/// 신경망
		Network net;
		net.reset(trainingSet, { 3 });
		/// 학습 데이터 개수
		auto setCount = trainingSet.getSetCount();
		/// 동시 입력 개수
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
			// 학습.
			net.train(5000, learningRate);
			/// 에러율
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

			// 에러가 일정 수준 미만으로 떨어지면 종료.
			if (errorRate < 0.01) {
				cout << "종료!" << endl;
				cout << "최종 에러값 : " << errorRate * 100.0 << "%" << endl;
				cout << "총 학습횟수 : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}

	case 3: {
		/// 학습 데이터
		TrainSet trainingSet = {
			// {{Inputs...}, Output}
			{ { 0, 0 }, 0 },
			{ { 0, 1 }, 1 },
			{ { 1, 0 }, 1 },
			{ { 1, 1 }, 0 },
		};
		/// 신경망
		Network net;
		net.reset(trainingSet, { 3 });
		/// 학습 데이터 개수
		auto setCount = trainingSet.getSetCount();
		/// 동시 입력 개수
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
			// 학습.
			net.train(5000, learningRate);
			/// 에러율
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

			// 에러가 일정 수준 미만으로 떨어지면 종료.
			if (errorRate < 0.01) {
				cout << "종료!" << endl;
				cout << "최종 에러값 : " << errorRate * 100.0 << "%" << endl;
				cout << "총 학습횟수 : " << count << endl;
				break;
			}
		}
		getchar();
		break;
	}
	}
	return 0;
}
