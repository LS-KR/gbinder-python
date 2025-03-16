#include <gbinder/gbinder_types.h>
#include <gbinder/gbinder_servicemanager.h>
#include <gbinder/gbinder_buffer.h>
#include <gbinder/gbinder_bridge.h>
#include <gbinder/gbinder_client.h>
#include <gbinder/gbinder_local_object.h>
#include <gbinder/gbinder_local_reply.h>
#include <gbinder/gbinder_local_request.h>
#include <gbinder/gbinder_reader.h>
#include <gbinder/gbinder_remote_object.h>
#include <gbinder/gbinder_remote_reply.h>
#include <gbinder/gbinder_remote_request.h>
#include <gbinder/gbinder_writer.h>

typedef int bint;
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define INT_8_MAX 127
#define UINT_8_MAX 255
#define INT_16_MAX 16383
#define UINT_16_MAX 32767
#define INT_32_MAX 2147483647
#define UINT_32_MAX 4294967295
#define INT_64_MAX 9223372036854775807

typedef bool boolean;

class Bridge;
class RemoteObject;
class RemoteReply;
class Client;
class LocalRequest;
class Ipc;
class LocalReply;
class RemoteRequest;
class LocalObject;
class ServiceManager;
class Buffer;
class Writer;
class Reader;
