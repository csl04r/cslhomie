#include "homie.hpp"
#include <iostream>
#include <string>

static void introduce_cb(void *arg)
{
    auto pair = (std::pair<std::list<homie::Message> *, int> *)arg;
    auto lst = pair->first;
    auto it = lst->begin();
    for (int i = 0; it != lst->end() && i < pair->second; i++)
    {
        it++;
    }

    if (it == lst->end())
    {
        std::cout << "We done ";
        return;
    }

    int qos = 1;
    bool retain = true;

    std::cout
        //<< "INTRO: "
        << it->topic
        << " "
        << it->payload
        << std::endl;
    // auto res = mgos_mqtt_pub(it->topic.c_str(), it->payload.c_str(), it->payload.length(), qos, retain);
    auto res = 1;
    if (res > 0)
    {
        auto msg = *it;
        it++;
        pair->second++;
        if (false)
        {
            std::cout
                << "  next topic "
                << msg.topic
                << std::endl;
        }
    }
}

static void test_homie()
{
    homie::Device *d = new homie::Device("device123", "1.2.3", "My Device", "192.168.1.69", "aa:bb:cc:dd:ee:ff");
    d->setLocalIp("192.168.1.39");
    d->setMac("feedfacedeadbeef");

    homie::Node *dht22Node = new homie::Node(d, "dht22", "DHT22 Temp/RH Sensor", "DHT22");
    d->addNode(dht22Node);

    auto tempProp = new homie::Property(dht22Node, "tempf", "Temperature in Fahrenheit", homie::FLOAT, false);
    tempProp->setUnit(homie::DEGREE_SYMBOL + "F");
    float tempF = 72.0;
    tempProp->setValue(tempF);
    dht22Node->addProperty(tempProp);

    auto rhProp = new homie::Property(dht22Node, "rh", "Relative Humidity", homie::FLOAT, false);
    rhProp->setUnit("%");
    float rh = 61.0;
    rhProp->setValue(rh);
    dht22Node->addProperty(rhProp);

    auto doorNode = new homie::Node(d, "doora", "South Garage Door", "door");
    d->addNode(doorNode);

    auto openProp = new homie::Property(doorNode, "isopen", "Door Contact", homie::ENUM, false);
    doorNode->addProperty(openProp);
    openProp->setFormat("open,closed");

    auto relayProp = new homie::Property(doorNode, "relay", "Door Activator", homie::INTEGER, true);
    doorNode->addProperty(relayProp);

    for (auto msg : d->introduce())
    {
        std::cout
            //<< "[q:" << msg.qos << ",r:" << msg.retained << "] "
            << msg.topic
            //<< " → "
            << " "
            << msg.payload << std::endl;
    }

    std::cout << "relay cmd topic: " << relayProp->getSubTopic() << std::endl;
    homie::Message lwt = d->getLwt();
    std::cout << "LWT: " << lwt.topic << " " << lwt.payload << std::endl;

    d->setLifecycleState(homie::READY);

    auto msgList = d->introduce();
    auto pair = new std::pair<
        std::list<homie::Message> *, int>;
    pair->first = &msgList;
    pair->second = 0;
    // std::make_pair(intro.begin(), intro.end());

    for (int i = 0; i < 100; i++)
    {
        introduce_cb(pair);
    }

    delete relayProp;
    delete openProp;
    delete rhProp;
    delete tempProp;
    delete dht22Node;
    delete doorNode;
    delete pair;
    delete d;
}
int main(int argc, char **argv)
{
    test_homie();
}