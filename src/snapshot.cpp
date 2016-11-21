#include <nan.h>
#include "snapshot.h"
#include "tasklist.h"

#include <algorithm>
#include <memory>

using namespace v8;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;

NAN_METHOD(snapshot_sync) {

	// get list running processes
	// TODO: exceptions
	std::vector< std::shared_ptr<Process> > snap = tasklist ();
	Local<Array> tasks = Nan::New<Array>(snap.size());

	bool verbose = info[0]->ToBoolean()->Value();

	for (uint32_t i = 0; i < tasks->Length(); ++i) {
		if (verbose) {
			Local<Object> hash = Nan::New<Object>();

			Nan::Set(
				hash,
				Nan::New<v8::String>("name").ToLocalChecked(),
				Nan::New<v8::String>( snap.at(i)->name().c_str() ).ToLocalChecked()
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("pid").ToLocalChecked(),
				Nan::New<v8::Number>( snap.at(i)->pid() )
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("ppid").ToLocalChecked(),
				Nan::New<v8::Number>( snap.at(i)->parentPid() )
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("path").ToLocalChecked(),
				Nan::New<v8::String>( snap.at(i)->path().c_str() ).ToLocalChecked()
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("threads").ToLocalChecked(),
				Nan::New<v8::Number>( snap.at(i)->threads() )
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("owner").ToLocalChecked(),
				Nan::New<v8::String>( snap.at(i)->owner().c_str() ).ToLocalChecked()
			);

			Nan::Set(
				hash,
				Nan::New<v8::String>("priority").ToLocalChecked(),
				Nan::New<v8::Number>( snap.at(i)->priority() )
			);

			tasks->Set(i, hash);
		} else {
			tasks->Set(i, Nan::New<v8::String>( snap.at(i)->name().c_str() ).ToLocalChecked());
		}
	}

	info.GetReturnValue().Set(tasks);
}

class SnapshotWorker : public AsyncWorker {
public:
	SnapshotWorker(Nan::Callback *callback, bool _verbose = false)
	: AsyncWorker(callback)
	{
		verbose = _verbose;
	}

	~SnapshotWorker(){};

	void Execute () {
		tasks = tasklist ();
	}

	void HandleOKCallback () {
		Nan::HandleScope scope;

		Local<Array> jobs = Nan::New<Array>(tasks.size());

		for (uint32_t i = 0; i < jobs->Length(); ++i) {
			if (verbose) {
				Local<Object> hash = Nan::New<Object>();

				Nan::Set(
					hash,
					Nan::New<v8::String>("name").ToLocalChecked(),
					Nan::New<v8::String>( tasks.at(i)->name().c_str() ).ToLocalChecked()
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("pid").ToLocalChecked(),
					Nan::New<v8::Number>( tasks.at(i)->pid() )
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("ppid").ToLocalChecked(),
					Nan::New<v8::Number>( tasks.at(i)->parentPid() )
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("path").ToLocalChecked(),
					Nan::New<v8::String>( tasks.at(i)->path().c_str() ).ToLocalChecked()
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("threads").ToLocalChecked(),
					Nan::New<v8::Number>( tasks.at(i)->threads() )
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("owner").ToLocalChecked(),
					Nan::New<v8::String>( tasks.at(i)->owner().c_str() ).ToLocalChecked()
				);

				Nan::Set(
					hash,
					Nan::New<v8::String>("priority").ToLocalChecked(),
					Nan::New<v8::Number>( tasks.at(i)->priority() )
				);

				jobs->Set(i, hash);
			} else {
				jobs->Set(i, Nan::New<v8::String>( tasks.at(i)->name().c_str() ).ToLocalChecked());
			}
		}

		Local<Value> argv[] = {
			Nan::Null(),
			jobs
		};

		callback->Call(2, argv);
	}

private:
	std::vector< std::shared_ptr<Process> > tasks;
	bool verbose;
};

NAN_METHOD(snapshot_async) {

	bool verbose = info[0]->ToBoolean()->Value();
	Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());

	AsyncQueueWorker(new SnapshotWorker(callback, verbose));
	return;
}
