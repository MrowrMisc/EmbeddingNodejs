#include "NodejsEnvironment.h"

namespace Nodejs {

    NodejsEnvironment::NodejsEnvironment() { initialize(); }

    NodejsEnvironment::~NodejsEnvironment() { shutdown(); }

    void NodejsEnvironment::initialize() {
        // Setup command-line arguments (empty for simplicity).
        std::vector<std::string> args = {"node"};
        std::vector<std::string> exec_args;

        // Initialize the Node.js environment.
        node::InitializeOncePerProcess(
            args, {node::ProcessInitializationFlags::kNoInitializeV8,
                   node::ProcessInitializationFlags::kNoInitializeNodeV8Platform}
        );

        // Create the V8 platform.
        platform_ = node::MultiIsolatePlatform::Create(4);
        v8::V8::InitializePlatform(platform_.get());
        v8::V8::Initialize();

        // Setup the Node.js environment.
        std::vector<std::string> errors;
        setup_ = node::CommonEnvironmentSetup::Create(platform_.get(), &errors, args, exec_args);

        if (!setup_) {
            // Handle errors if the setup failed.
            for (const auto& error : errors) {
                fprintf(stderr, "Node.js setup error: %s\n", error.c_str());
            }
            return;
        }

        // Enter the V8 Isolate scope.
        {
            v8::Isolate*       isolate = setup_->isolate();
            v8::Locker         locker(isolate);
            v8::Isolate::Scope isolate_scope(isolate);
            v8::HandleScope    handle_scope(isolate);
            v8::Context::Scope context_scope(setup_->context());

            // Load the environment to initialize built-in modules like 'console'.
            node::LoadEnvironment(setup_->env(), "");

            // Remove the call to StartEnvironment, as it is no longer needed.
            // node::StartEnvironment(setup_->env());
        }
    }

    void NodejsEnvironment::shutdown() {
        // Dispose the Node.js environment.
        if (setup_) {
            node::Stop(setup_->env());
            setup_.reset();
        }

        // Dispose V8 and the platform.
        v8::V8::Dispose();
        v8::V8::DisposePlatform();
        node::TearDownOncePerProcess();
    }

    v8::MaybeLocal<v8::Value> NodejsEnvironment::eval(std::string_view code) {
        v8::Isolate*       isolate = setup_->isolate();
        v8::Locker         locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope    handle_scope(isolate);
        v8::Context::Scope context_scope(setup_->context());

        // Convert the code string to a V8 string.
        v8::Local<v8::String> source;
        if (!v8::String::NewFromUtf8(
                 isolate, code.data(), v8::NewStringType::kNormal, static_cast<int>(code.size())
            )
                 .ToLocal(&source)) {
            // Failed to create string.
            return v8::MaybeLocal<v8::Value>();
        }

        // Compile the script.
        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(setup_->context(), source).ToLocal(&script)) {
            // Compilation failed.
            return v8::MaybeLocal<v8::Value>();
        }

        // Run the script.
        v8::MaybeLocal<v8::Value> result = script->Run(setup_->context());

        // Spin the event loop to process any pending events (e.g., console output).
        node::SpinEventLoop(setup_->env()).FromJust();

        return result;
    }

    v8::Isolate* NodejsEnvironment::getIsolate() { return setup_->isolate(); }

}  // namespace Nodejs