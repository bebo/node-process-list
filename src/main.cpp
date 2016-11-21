#include <nan.h>
#include "snapshot.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(init) {
	Set(target, New<String>("snapshotSync").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(snapshot_sync)).ToLocalChecked());

	Set(target, New<String>("snapshot").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(snapshot_async)).ToLocalChecked());
}

NODE_MODULE(processlist, init);
