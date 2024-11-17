#pragma once

#include <memory>
#include <string_view>

#include "node.h"
#include "uv.h"

namespace Nodejs {

    // Class representing an active Node.js environment.
    class NodejsEnvironment {
    public:
        // Constructs and initializes the Node.js environment.
        NodejsEnvironment();

        // Destructor shuts down the Node.js environment.
        ~NodejsEnvironment();

        // Evaluates JavaScript code in the Node.js environment.
        // Returns the result as a v8::Local<v8::Value>.
        v8::MaybeLocal<v8::Value> eval(std::string_view code);

        // Returns the V8 isolate.
        v8::Isolate* getIsolate();

    private:
        // Initializes the Node.js environment.
        void initialize();

        // Shuts down the Node.js environment.
        void shutdown();

        // Node.js platform and environment setup.
        std::unique_ptr<node::MultiIsolatePlatform>   platform_;
        std::unique_ptr<node::CommonEnvironmentSetup> setup_;

        // Prevent copying.
        NodejsEnvironment(const NodejsEnvironment&)            = delete;
        NodejsEnvironment& operator=(const NodejsEnvironment&) = delete;
    };

}  // namespace Nodejs