/*
 * local_service.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#include <service_discovery/local_service.h>

namespace servicediscovery { 

static LoggingWrapper logger("LocalService");

LocalService::~LocalService() {
	if (group) {
		avahi_entry_group_free(group);
	}
}

LocalService::LocalService(Client *client,
		AvahiIfIndex interf,
		AvahiProtocol prot,
		AvahiPublishFlags flags,
		std::string name,
		std::string type,
		std::string domain,
		uint16_t port,
		std::list<std::string> list,
		uint32_t ttl,
		bool publish)
	: Service(client, interf, prot, name, type, domain, port, list)
{
	group = NULL;
	this->flags = flags;
	this->ttl = ttl;
	if (publish) {
		int pres = this->publish();
	}
}

LocalService::LocalService(
		Client *client,
		std::string name,
		std::string type,
		uint16_t port,
		std::list<std::string> list,
		uint32_t ttl,
		bool publish)
	: Service(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, name, type, "", port, list)
{
	group = NULL;
	flags = (AvahiPublishFlags) 0;
	this->ttl = ttl;
	if (publish) {
		int pres = this->publish();
	}
}

int LocalService::publish()
{

	if (!getClient()) {
		logger.log(FATAL, "Publish - client pointer is NULL");
		return -1;
	}


	if (group) {
		logger.log(WARN, "Publish - entry group pointer is not null. Service is already published?");
		return -2;
	}

	
#ifdef __CUSTOM_TTL__
	if (!(group = avahi_entry_group_new_custom_ttl(getClient()->getAvahiClient(), entry_group_callback, this, ttl))) {
#else
/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!! merge conflict here please solve !!!!!!!!!!!!!!!!!!!!!!!!!!	
	
	HEAD:src/LocalService.cpp
	std::cerr << "INFO: avahi API is not patched for custom TTL\n";

	logger.log(WARN, "Publish - Avahi API is not patched for custom TTL");
	b8bdb878597d120f139eaaaa4fc13d6c3c527b4d:src/LocalService.cpp
	*/
	if (!(group = avahi_entry_group_new(getClient()->getAvahiClient(), entry_group_callback, this))) {
#endif	
		logger.log(FATAL, "Publish - Failed to create entry group: %s", avahi_strerror(avahi_client_errno(getClient()->getAvahiClient())));
        return -3;
    }
   
	int ret;
        ServiceConfiguration config = getConfiguration();

	if ((ret = avahi_entry_group_add_service_strlst(
				group,
				config.getInterfaceIndex(),
				config.getProtocol(),
				flags,
				config.getName().c_str(),
				config.getType().c_str(),
				config.getDomain().c_str(),
				NULL,
				getPort(),
				getTxt())) < 0) {

		logger.log(FATAL, "Failed to add service to the entry group: %s", avahi_strerror(ret));
		unpublish();
		return -4;
	}
	
    if ((ret = avahi_entry_group_commit(group)) < 0) {
    	logger.log(FATAL, "Failed to commit entry group: %s", avahi_strerror(ret));
    	unpublish();
    	return -5;
    }
    
    return 0;

}

void LocalService::unpublish()
{
	if (group) {
		avahi_entry_group_free(group);
		group = NULL;
	}
}

void LocalService::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata)
{
    LocalService* ls = (LocalService*) userdata;
    ServiceConfiguration config = ls->getConfiguration();
    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
        	logger.log(INFO, "Entry group callback - Service %s is established", config.getName().c_str());
            break;

        case AVAHI_ENTRY_GROUP_COLLISION : {
        	logger.log(FATAL, "Entry group callback - Service collision for %s/%s", config.getType().c_str(), config.getName().c_str());
            break;
        }

        case AVAHI_ENTRY_GROUP_FAILURE :
        	logger.log(FATAL, "Entry group callback - Entry group failed for service %s, reason: %s", config.getName().c_str(), avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}

int LocalService::updateStringList(std::list<std::string> listn) {
	if (!group) {
		logger.log(FATAL, "updateStringList - Entry group not found for updating string list");
		return -1;
	}
	
	AvahiStringList *list = Service::getTxt(listn);
	
        ServiceConfiguration config = getConfiguration();

	int res;
	if ((res = avahi_entry_group_update_service_txt_strlst(
				group,
				config.getInterfaceIndex(),
				config.getProtocol(),
				flags,
				config.getName().c_str(),
				config.getType().c_str(),
				config.getDomain().c_str(),
				list)) < 0) {
		logger.log(FATAL, "updateStringList - Failed to update txt records: %s", avahi_strerror(res));
		return -2;
	}
	
	Service::setTxt(list);
	stringlist = listn;
	
	return 0;
	
}

} // end namespace servicediscovery