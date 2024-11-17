#include <fstream>
#include <iostream>
#include <sstream>

#include "NodejsEnvironment.h"

int main(int argc, char** argv) {
    std::cout << "Starting node-embed-test" << std::endl;

    // Create a Node.js environment instance.
    auto env = std::make_unique<Nodejs::NodejsEnvironment>();

    // Read the compiled JavaScript code from dist/main.js
    std::ifstream     jsFile("C:/Code/MrowrMisc/EmbeddingNodejs/dist/main.js");
    std::stringstream buffer;
    buffer << jsFile.rdbuf();
    std::string jsCode = buffer.str();

    // Evaluate the JavaScript code.
    std::cout << "Evaluating JavaScript code..." << std::endl << jsCode << std::endl;
    v8::MaybeLocal<v8::Value> result = env->eval(jsCode);

    // Process the result.
    if (!result.IsEmpty()) {
        std::cout << "JavaScript execution succeeded." << std::endl;
        v8::Isolate*         isolate     = env->getIsolate();
        v8::Local<v8::Value> localResult = result.ToLocalChecked();
        if (localResult->IsString()) {
            v8::String::Utf8Value utf8(isolate, localResult);
            std::cout << "JavaScript eval result: " << *utf8 << std::endl;
        } else {
            std::cerr << "JavaScript execution result is not a string." << std::endl;
        }
    } else {
        std::cerr << "JavaScript execution failed." << std::endl;
    }

    std::cout << "Exiting node-embed-test" << std::endl;
    return 0;
}