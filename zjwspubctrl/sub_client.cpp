#include "sub_client.hpp"
#include <jws/json_with_schema.hpp>
#include <zpubctrl/sub_client.hpp>
#include <functional>
#include <memory>
#include <string>

namespace zjwspubctrl {

  struct SubClient::Detail {
    Detail(
      const jws::json& pub_schema,
      const std::string& server_address,
      int pub_port) :
      _client(server_address, pub_port) {
      _pub_validator = jws::load_validator(pub_schema);
    }
    zpubctrl::SubClient _client;
    jws::json_validator _pub_validator;
  };

  SubClient::SubClient(const jws::json& pub_schema, const std::string& server_address, int pub_port) :
    _detail(new Detail(pub_schema, server_address, pub_port)) {
  }

  SubClient::SubClient(const std::string& pub_schema_filename, const std::string& server_address, int pub_port) {
    auto pub_schema = !pub_schema_filename.empty() ? jws::load_json(pub_schema_filename) : jws::json{};
    _detail.reset(new Detail(pub_schema, server_address, pub_port));
  }

  SubClient::SubClient(const char* pub_schema_filename, const std::string& server_address, int pub_port) :
    SubClient(std::string(pub_schema_filename), server_address, pub_port) {
  }

  SubClient::~SubClient() {
  }

  jws::json SubClient::wait_for_data(int timeout_ms) {
    auto data = _detail->_client.wait_for_data(timeout_ms);
    auto data_json = jws::json::parse(data);
    _detail->_pub_validator.validate(data_json);
    return data_json;
  }

}
