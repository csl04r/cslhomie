#include "homie.hpp"
#include <iostream>
#include <random>
#include <string>

#include <time.h>

void homie::Device::subscribe(std::string commandTopic) {
  // normally impl logic to subscribe to "${homieBaseTopic}/+/+/+/set"
  // but only if there are settable properties
  // e.g. mgos_mqtt_sub(commandTopic.c_str(), this->subscriptionHandler, this);
  std::cerr << "NO sub logic needed " << commandTopic << std::endl;
}

void homie::Device::publish(Message m) {
  std::cout << "Publish topic=" << m.topic << ": " << m.payload
            << " q=" << m.qos << " r=" << m.retained << std::endl;
}

/*
static void introduce_cb(void *arg) {
  auto pair = (std::pair<std::vector<homie::Message> *, int> *)arg;
  auto lst = pair->first;
  auto it = lst->begin();
  for (int i = 0; it != lst->end() && i < pair->second; i++) {
    it++;
  }

  if (it == lst->end()) {
    std::cout << "We done ";
    return;
  }

  int qos = 1;
  bool retain = true;

  std::cout
      //<< "INTRO: "
      << it->topic << " " << it->payload << std::endl;
  // auto res = mgos_mqtt_pub(it->topic.c_str(), it->payload.c_str(),
  // it->payload.length(), qos, retain);
  auto res = 1;
  if (res > 0) {
    auto msg = *it;
    it++;
    pair->second++;
    if (false) {
      std::cout << "  next topic " << msg.topic << std::endl;
    }
  }
}
*/

int homie::Device::getRssi() { return -58; }

static void test_homie() {
  homie::Device *d =
      new homie::Device(std::string("device123"), std::string("1.2.3"),
                        std::string("My Device"), "holmes");

  homie::Node *dht22Node =
      new homie::Node(d, "dht22", "DHT22 Temp/RH Sensor", "DHT22");

  auto tempProp =
      new homie::Property(dht22Node, "tempf", "Temperature in Fahrenheit",
                          homie::FLOAT, false, []() { return "42.0"; });
  tempProp->setUnit(homie::DEGREE_SYMBOL + "F");

  auto rhProp =
      new homie::Property(dht22Node, "rh", "Relative Humidity", homie::FLOAT,
                          false, []() { return "61.0"; });
  rhProp->setUnit("%");

  auto doorNode = new homie::Node(d, "doora", "South Garage Door", "door");

  auto openProp =
      new homie::Property(doorNode, "isopen", "Door Contact", homie::ENUM, true,
                          []() { return "open"; });
  openProp->setFormat("open,closed");

  auto relayProp =
      new homie::Property(doorNode, "relay", "Door Activator", homie::INTEGER,
                          true, []() { return "false"; });

  d->introduce();

  std::cout << "relay cmd topic: " << relayProp->getSubTopic() << std::endl;
  homie::Message lwt = d->getLwt();
  std::cout << "LWT: " << lwt.topic << " " << lwt.payload << std::endl;

  d->setLifecycleState(homie::READY);

  d->setMac("eeddccbb");
  d->setLocalIp("8.8.8.8");

  d->introduce();

  d->publishWifi();

  std::cout << "This lib version: " << homie::LIB_VERSION << std::endl;

  openProp->setWriterFunc([](std::string s) {
    std::cout << "OPEN derp der der " << s << std::endl;
  });
  homie::Message testMsg(openProp->getPubTopic() + "/set", "open");
  d->onMessage(testMsg);
  delete d;
}

template <typename T> class Wayne {
private:
  T val;

public:
  Wayne<T> &operator<<(T t);
  T getVal() { return val; }

  std::function<T(void)> lambda;

  void call() { *this << lambda(); }
};

template <typename T> Wayne<T> &Wayne<T>::operator<<(T t) {
  this->val = t;
  std::cerr << "setting " << this->val << std::endl;
  return *this;
}

int main() {
  test_homie();
  Wayne<time_t> wdbl;

  wdbl << 12;
  wdbl.lambda = []() { return time(NULL); };

  wdbl.call();
  std::cout << wdbl.getVal() << std::endl;

  std::vector<std::string> res;
  homie::split_string(std::string("a/b/c/d"), "/", res);
  std::cout << "res.len " << res.size() << std::endl;
  for (auto a : res) {
    std::cout << "mem " << a << std::endl;
  }

  std::cout << "Node: " << res[res.size() - 1 - 1]
            << " prop: " << res[res.size() - 1] << std::endl;
}