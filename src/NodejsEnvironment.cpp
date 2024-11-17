#include "NodejsEnvironment.h"

#define NODE_WANT_INTERNALS 1
#include "env.h"

namespace Nodejs {

    NodejsEnvironment::NodejsEnvironment() { initialize(); }

    NodejsEnvironment::~NodejsEnvironment() { shutdown(); }

    void NodejsEnvironment::initialize() {
        // Setup command-line arguments (empty for simplicity).
        std::vector<std::string> args = {"node"};
        std::vector<std::string> exec_args;

        // Add '--experimental-modules' to enable ES modules (for Node.js versions < 13).
        args.push_back("--experimental-modules");

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

            // Expose 'require' to the global context
            v8::Local<v8::String> require_str =
                v8::String::NewFromUtf8(isolate, "require").ToLocalChecked();
            v8::Local<v8::Function> require_func = setup_->env()->builtin_module_require();
            setup_->context()->Global()->Set(setup_->context(), require_str, require_func).Check();
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

    // Define the ResolveCallback function.
    v8::MaybeLocal<v8::Module> NodejsEnvironment::ResolveCallback(
        v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
        v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer
    ) {
        // Module resolution logic goes here.
        // For simplicity, let's return an empty MaybeLocal to indicate module not found.
        return v8::MaybeLocal<v8::Module>();
    }

    // Set the ImportModuleDynamicallyCallback on the isolate.
    void SetImportModuleDynamicallyCallback(v8::Isolate* isolate) {
        isolate->SetHostImportModuleDynamicallyCallback(
            [](v8::Local<v8::Context> context, v8::Local<v8::Data> host_defined_options,
               v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier,
               v8::Local<v8::FixedArray> import_assertions) -> v8::MaybeLocal<v8::Promise> {
                // Dynamic import logic goes here.
                // For simplicity, return an empty MaybeLocal.
                return v8::MaybeLocal<v8::Promise>();
            }
        );
    }

    v8::MaybeLocal<v8::Value> NodejsEnvironment::eval(std::string_view code) {
        v8::Isolate*       isolate = setup_->isolate();
        v8::Locker         locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope    handle_scope(isolate);
        v8::Context::Scope context_scope(setup_->context());

        // Wrap the code in a function to simulate the Node.js module wrapper
        std::string wrapped_code = "(function(exports, require, module, __filename, __dirname) { " +
                                   std::string(code) + "\n})";

        // Convert the wrapped code to a V8 string
        v8::Local<v8::String> source;
        if (!v8::String::NewFromUtf8(isolate, wrapped_code.c_str(), v8::NewStringType::kNormal)
                 .ToLocal(&source)) {
            // Failed to create string.
            return v8::MaybeLocal<v8::Value>();
        }

        // Compile the script
        v8::Local<v8::Script> script;
        if (!v8::Script::Compile(setup_->context(), source).ToLocal(&script)) {
            // Compilation failed.
            return v8::MaybeLocal<v8::Value>();
        }

        // Run the script to get the function
        v8::Local<v8::Value> func_val;
        if (!script->Run(setup_->context()).ToLocal(&func_val)) {
            // Running the script failed.
            return v8::MaybeLocal<v8::Value>();
        }

        // Check if the result is a function
        if (!func_val->IsFunction()) {
            // Not a function.
            return v8::MaybeLocal<v8::Value>();
        }

        v8::Local<v8::Function> func = func_val.As<v8::Function>();

        // Prepare the arguments: exports, require, module, __filename, __dirname
        v8::Local<v8::Object> exports = v8::Object::New(isolate);
        v8::Local<v8::Object> module  = v8::Object::New(isolate);
        module
            ->Set(
                setup_->context(), v8::String::NewFromUtf8(isolate, "exports").ToLocalChecked(),
                exports
            )
            .Check();

        // Get the 'require' function from the global context
        v8::Local<v8::Value> require;
        if (!setup_->context()
                 ->Global()
                 ->Get(
                     setup_->context(), v8::String::NewFromUtf8(isolate, "require").ToLocalChecked()
                 )
                 .ToLocal(&require)) {
            // Failed to get 'require'
            return v8::MaybeLocal<v8::Value>();
        }

        // Prepare filename and dirname
        v8::Local<v8::String> filename =
            v8::String::NewFromUtf8(isolate, "embedded_script.js").ToLocalChecked();
        v8::Local<v8::String> dirname = v8::String::NewFromUtf8(isolate, ".").ToLocalChecked();

        v8::Local<v8::Value> args[] = {exports, require, module, filename, dirname};

        // Call the function
        auto callResult =
            func->Call(setup_->context(), v8::Undefined(isolate), 5, args).ToLocalChecked();

        // Spin the event loop to process any pending events.
        node::SpinEventLoop(setup_->env()).FromJust();

        // Return the module's exports
        return module->Get(
            setup_->context(), v8::String::NewFromUtf8(isolate, "exports").ToLocalChecked()
        );
    }

    v8::Isolate* NodejsEnvironment::getIsolate() { return setup_->isolate(); }

}  // namespace Nodejs