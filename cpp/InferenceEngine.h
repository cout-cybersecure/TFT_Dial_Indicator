#pragma once
#include <onnxruntime_cxx_api.h>
#include <QString>
#include <vector>

class InferenceEngine
{
public:
    InferenceEngine();
    bool loadModel(const QString &modelPath);
    double predict(const std::vector<float> &input);

private:
    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    std::unique_ptr<Ort::Session> session;
};
