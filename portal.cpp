#include "portal.h"

Portal::Portal(string id, string destination):
    id(std::move(id)),
    destination(std::move(destination)),
    roomId(),
    x(-1), y(-1),
    bidirectional(false)
{

}

PortalPtr makePortal(const JsonObject& portalObject)
{
    string id = any_cast<string>(portalObject.at("id"));
    string destination = any_cast<string>(portalObject.at("destination"));
    return PortalPtr(new Portal(id, destination));
}
