//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "trafficlight.h"
#include "veins/modules/application/traci/MyVeinsApp.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
using namespace veins;
Define_Module(veins::trafficlight);

trafficlight::trafficlight() {
    // TODO Auto-generated constructor stub

}

trafficlight::~trafficlight() {
    // TODO Auto-generated destructor stub
}


void trafficlight::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }

}



void trafficlight::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    findHost()->getDisplayString().setTagArg("i", 1, "green");

    if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
    if (!sentMessage) {
        sentMessage = true;
        // repeat the received traffic update once in 2 seconds plus some random delay
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}

void trafficlight::handleSelfMsg(cMessage* msg)
{
    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() + 1);
        if (wsm->getSerial() >= 3) {
            // stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void trafficlight::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    std::string carType = traciVehicle->getTypeId();
    std::string laneId = traciVehicle->getLaneId();
    double carLoc = traciVehicle->getDistanceTravelled();
    double carSpeed = traciVehicle->getSpeed();
    std::string routeId = traciVehicle->getRouteId().c_str();

         if(carType == "emergency"){
             findHost()->getDisplayString().setTagArg("i", 1, "red");

              }

         if(carType == "emergency" && laneId == "14617022#0_0"){
             TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
             populateWSM(wsm);
             wsm->setDemoData(traciVehicle->getLaneId().c_str());
             sendDown(wsm);
             }


             else {
             lastDroveAt = simTime();
             }
         }




