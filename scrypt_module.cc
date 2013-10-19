#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include "scrypt.h"
#include <cstring>

using namespace v8;

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;

  Local<Object> obj = args[0]->ToObject();

  if (!node::Buffer::HasInstance(obj)) {
    ThrowException(
      Exception::TypeError(
        String::New("Argument must be a buffer.")));
 
    return scope.Close(Undefined());
  }
  
  unsigned char* data = static_cast<unsigned char*>(obj->GetIndexedPropertiesExternalArrayData());
  int data_len = node::Buffer::Length(args[0]->ToObject());
  int in_size = 80;
  int out_size = 32;
  char* input = new char[in_size];
  node::Buffer *buff = node::Buffer::New(out_size);

  std::memset(input, 0, in_size*sizeof(*input));
  if (data_len > in_size) { data_len = in_size*sizeof(*input); }
  std::memcpy(input, data, data_len);

  ::scrypt_1024_1_1_256(input, node::Buffer::Data(buff));
  std::free(&input);

  v8::Local<v8::Object> globalObj = v8::Context::GetCurrent()->Global();
  v8::Local<v8::Function> bufferConstructor = v8::Local<v8::Function>::Cast(globalObj->Get(v8::String::New("Buffer")));
  v8::Handle<v8::Value> constructorArgs[3] = { buff->handle_, v8::Integer::New(out_size), v8::Integer::New(0) };
  v8::Local<v8::Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

  return scope.Close(actualBuffer);
}

void init(Handle<Object> target) {
  target->Set(String::NewSymbol("scrypt"), FunctionTemplate::New(Method)->GetFunction());
}
NODE_MODULE(scrypt, init)

