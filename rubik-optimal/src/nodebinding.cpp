#include </home/vitor/.nvm/versions/node/v8.11.2/include/node/node.h>
#include </home/vitor/.nvm/versions/node/v8.11.2/include/node/uv.h>
#include "permutation.cpp"
#include "pruningtable.cpp"
#include "solve.cpp"

using namespace v8;

struct Work {
    uv_work_t request;
    Persistent<Function> callback;
    Permutation* scrambled_cube;
    string solution_moves;
};

PruningTable* _get_pruning_table() {
    PruningTable* table = new PruningTable();
    table->allocate();
    table->load_from_file("pruning_table.bin");
    return table;
}

PruningTable* pruning_table = _get_pruning_table();

static void WorkAsync(uv_work_t* req) {
    Work* work = static_cast<Work*>(req->data);
    CubeSolver solver{pruning_table};
    CubeSolution solution = solver.solve(*work->scrambled_cube);
    work->solution_moves = solution.move_names;
}

static void WorkAsyncComplete(uv_work_t* req, int status) {
    Isolate* isolate = Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);

    Work* work = static_cast<Work*>(req->data);
    Local<Value> solution_moves =
        String::NewFromUtf8(isolate, work->solution_moves.data());

    Handle<Value> argv[] = {solution_moves};

    Local<Function>::New(isolate, work->callback)
        ->Call(isolate->GetCurrentContext()->Global(), 1, argv);

    work->callback.Reset();
    
    delete work->scrambled_cube;
    delete work;
}

void SolveCubeAsync(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();

    Work* work = new Work;
    work->request.data = work;

    Permutation* p = new Permutation();
    Local<Object> cube = Local<Object>::Cast(args[0]);
    Local<Array> corners =
        Local<Array>::Cast(cube->Get(String::NewFromUtf8(isolate, "corners")));
    Local<Array> edges =
        Local<Array>::Cast(cube->Get(String::NewFromUtf8(isolate, "edges")));
    // corners
    for (int i = 0; i < 8; i++) {
        p->corners[i].replaced_by =
            Local<Object>::Cast(corners->Get(i))
                ->Get(String::NewFromUtf8(isolate, "replaced_by"))
                ->NumberValue();
        p->corners[i].orientation =
            Local<Object>::Cast(corners->Get(i))
                ->Get(String::NewFromUtf8(isolate, "orientation"))
                ->NumberValue();
    }
    // edges
    for (int i = 0; i < 12; i++) {
        p->edges[i].replaced_by =
            Local<Object>::Cast(edges->Get(i))
                ->Get(String::NewFromUtf8(isolate, "replaced_by"))
                ->NumberValue();
        p->edges[i].orientation =
            Local<Object>::Cast(edges->Get(i))
                ->Get(String::NewFromUtf8(isolate, "orientation"))
                ->NumberValue();
    }


    work->scrambled_cube = p;

    Local<Function> callback = Local<Function>::Cast(args[1]);
    work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &work->request, WorkAsync,
                  WorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
}

void init(Handle<Object> exports, Handle<Object> module) {
    NODE_SET_METHOD(exports, "solve_cube", SolveCubeAsync);
}

NODE_MODULE(solve_cube, init)
