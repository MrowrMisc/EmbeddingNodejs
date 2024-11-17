#include <iostream>
#include <string>

#include "NodejsEnvironment.h"

int main(int argc, char** argv) {
    std::cout << "Starting node-embed-test" << std::endl;

    // Create a Node.js environment instance.
    auto env = std::make_unique<Nodejs::NodejsEnvironment>();

    // JavaScript code to execute.
    std::string jsCode = R"(
import fs from 'fs';

    console.log("Hello from Node.js!");

// List the files in the current directory.
fs.readdirSync('.').forEach(file => {
    // Print the file name.
    console.log(file);
});

)";

    // Evaluate the JavaScript code.
    v8::MaybeLocal<v8::Value> result = env->eval(jsCode);

    // Process the result.
    if (!result.IsEmpty()) {
        v8::Isolate*          isolate = env->getIsolate();
        v8::String::Utf8Value utf8(isolate, result.ToLocalChecked());
        std::cout << "JavaScript eval result: " << *utf8 << std::endl;
    } else {
        std::cerr << "JavaScript execution failed." << std::endl;
    }

    std::cout << "Exiting node-embed-test" << std::endl;
    return 0;
}