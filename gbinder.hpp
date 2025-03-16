#include "cgbinder.h"
#include <cstdlib>
#include <cstring>
#include <functional>
#include <gbinder/gbinder_bridge.h>
#include <gbinder/gbinder_types.h>
#include <vector>
#include <stdexcept>

using namespace std;

vector<uint8> ensure_binary(string& s) {
    vector<uint8> u;
    for (uint8 v : s) {
        u.push_back(v);
    }
    return u;
}

vector<uint8> ensure_binary(vector<uint8>& s) {
    return s;
}

template <typename T>
std::vector<uint8> ensure_binary(const T&) {
    throw std::invalid_argument("not expecting this type");
}

class Ipc {
public:
    GBinderIpc* _ipc;
};

class RemoteObject {
private:
    GBinderRemoteObject* _object;
public:
    function<void()> notify_func;
    ~RemoteObject();
    void set_c_object(GBinderRemoteObject* remote);
    Ipc ipc();
    bool is_dead();
    ulong add_death_handler(function<void()> notify_func);
    void notify_func_callback();
    void remove_handler(ulong id);
};

class RemoteReply {
private:
    GBinderRemoteReply* _reply;
public:
    ~RemoteReply();
    void set_c_reply(GBinderRemoteReply);
    Reader init_reader();
    LocalReply copy_to_local();
    tuple<int, int32> read_int32();
    tuple<int, int64> read_int64();
    tuple<int, uint32> read_uint32();
    tuple<int, uint64> read_uint64();
    tuple<int, const char*> read_string8();
    tuple<int, const char*> read_string16();
    RemoteObject read_object();
};

class Client {
private:
    GBinderClient* _client;
public:
    function<void(RemoteReply, int)> reply_func;
    function<void()> destroy_notif;

    Client(RemoteObject object, vector<string>& ifaces_list);
    ~Client();
    LocalRequest new_request(uint code = 0);
    tuple<RemoteReply, int> transact_sync_reply(uint code, LocalRequest req);
};

class LocalObject {
public:
    function<int(GBinderRemoteRequest*, uint, uint)> handler;
    LocalObject(Ipc ipc, vector<string> ifaces_list = vector<string>(), function<void()> handler = {});
    ~LocalObject();
    void set_c_object(GBinderLocalObject* object);
    void drop();
    LocalReply new_reply();
    int callback(GBinderRemoteRequest* req, uint code, uint flags);
private:
    GBinderLocalObject* _object;
};

class ServiceManager {
public:
    function<void()> func;
    std::function<void()> presence_func;
    std::function<void(const std::vector<std::string>&)> list_func;
    std::function<void(RemoteObject*, int)> get_service_func;
    std::function<void(int)> add_service_func;
    std::function<void(std::string)> registration_func;
    ServiceManager();
    ~ServiceManager();
    LocalObject new_local_object(vector<string> ifaces_list, function<void()> handler);
    bool is_present();
    bool wait(ulong& max_wait_ms);
    int list(std::function<void(const std::vector<std::string>&)> list_func);
    void list_func_callback(vector<string&> services_list);
    vector<string &> list_sync();
    int get_service(string& name, function<void(RemoteObject, int)> get_service_func);
    void get_service_sync_callback(RemoteObject remote, int status);
    void get_service_sync(string& name);
    int add_service(string& name, LocalObject obj, function<void(int)> add_service_sync);
    void add_service_func_callback(int& status);
    int add_service_sync(string& name, LocalObject obj);
    void cancel(ulong& id);
    int add_presence_handler(function<void()> func);
    void func_callback();
    long add_registration_handler(string& name, function<void(string)>& registration_func);
    void registration_func_callback(string& name);
    void remove_handler(ulong id);
    void remove_handlers(vector<ulong>& ids_list);
private:
    GBinderServiceManager* _sm;
};

class Bridge {
public:
    Bridge(string& src_name,
        string& dest_name,
        std::vector<std::string>& ifaces_list,
        ServiceManager& src,
        ServiceManager& dest);

    ~Bridge();
private:
    GBinderBridge* _bridge;
};

void remote_object_local_notify_func(GBinderRemoteObject* object, void* user_data) {
    ((RemoteObject*)user_data)->notify_func_callback();
}

void service_manager_get_service_func(GBinderServiceManager* sm, GBinderRemoteObject* c_object, int status, void* user_data) noexcept {
    RemoteObject remote = RemoteObject();
    remote.set_c_object(c_object);
    ((ServiceManager*)user_data)->get_service_sync_callback(remote, status);
}