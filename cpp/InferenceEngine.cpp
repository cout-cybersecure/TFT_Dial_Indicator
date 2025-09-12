#include "InferenceEngine.h"

InferenceEngine::InferenceEngine()
    : env(ORT_LOGGING_LEVEL_WARNING, "tft_dial"), sessionOptions(), session(nullptr)
{
}

bool InferenceEngine::loadModel(const QString &modelPath)
{
    try {
        session = std::make_unique<Ort::Session>(env, modelPath.toStdString().c_str(), sessionOptions);
        return true;
    } catch (...) {
        return false;
    }
}

double InferenceEngine::predict(const std::vector<float> &input)
{
    if (!session) return 0.0;

    Ort::AllocatorWithDefaultOptions allocator;
    std::vector<int64_t> inputShape = {1, static_cast<int64_t>(input.size())};

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        allocator.GetInfo(),
        const_cast<float*>(input.data()),
        input.size(),
        inputShape.data(),
        inputShape.size()
    );

    auto outputNames = session->GetOutputNameAllocated(0, allocator);
    auto inputNames = session->GetInputNameAllocated(0, allocator);

    const char* inputName = inputNames.get();
    const char* outputName = outputNames.get();

    auto outputTensors = session->Run(Ort::RunOptions{nullptr},
                                      &inputName, &inputTensor, 1,
                                      &outputName, 1);

    float* out = outputTensors.front().GetTensorMutableData<float>();
    return static_cast<double>(out[0]);
}
