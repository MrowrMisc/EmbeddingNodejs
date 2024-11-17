#pragma once

#include <memory>
#include <string_view>

#include "node.h"

namespace Nodejs {

    // Define the ResolveCallback functor
    class ResolveCallback {
    public:
        v8::MaybeLocal<v8::Module> operator()(
            v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
            v8::Local<v8::Module> referrer
        ) {
            // Module resolution logic goes here.
            // For simplicity, return an empty MaybeLocal to indicate module not found.
            return v8::MaybeLocal<v8::Module>();
        }
    };

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

        // Define the ResolveCallback function.
        static v8::MaybeLocal<v8::Module> ResolveCallback(
            v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
            v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer
        );

        // Node.js platform and environment setup.
        std::unique_ptr<node::MultiIsolatePlatform>   platform_;
        std::unique_ptr<node::CommonEnvironmentSetup> setup_;

        // Prevent copying.
        NodejsEnvironment(const NodejsEnvironment&)            = delete;
        NodejsEnvironment& operator=(const NodejsEnvironment&) = delete;
    };

}  // namespace Nodejs