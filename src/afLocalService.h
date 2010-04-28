/*
 * afLocalService.h
 *
 *  Created on: Mar 17, 2010
 *      Author: darko
 */

#ifndef AFLOCALSERVICE_H_
#define AFLOCALSERVICE_H_

#include "afAvahiClient.h"
#include "afService.h"
#include <avahi-client/publish.h>
#include <list>
#include <string>
#include <iostream>

namespace dfki {
namespace communication {

/**
 * @brief
 * A class for service objects that are meant to be published
 */
class afLocalService: public afService {
private:
	/**
	 * in this framework every service to be published is connected to one entry group
	 */
	AvahiEntryGroup *group;
	AvahiPublishFlags flags;
	
	uint32_t ttl;
	
	static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata);

public:

	/**
	 * constructor
	 * @param client the related afAvahiClient instance
	 * @param name the name of the service
	 * @param type the type of the service: usually "_sometype._tcp" (must have a ._tcp or ._udp at the end for a protocol type)
	 * @param port the port number at which the service is present
	 * @param list a list of strings as additional information (TODO: avahi api supports map of strings, implement this functionality)
	 * @param ttl time to live of the service in case of network failure
	 * @param publish true if the related entry group is to be immediately commited
	 */
	afLocalService(
			afAvahiClient *client,
			std::string name,
			std::string type,
			uint16_t port,
			std::list<std::string> list,
			uint32_t ttl=0,
			bool publish=true
			);
	/**
	 * constructor
	 * @param client the related afAvahiClient instance
	 * @param interf the network interface at which the service is to be published (above default is all interfaces)
	 * @param prot the protocol ipv4 or ipv6 (above default is unspecified)
	 * @param flags publish flags (above default is no flags)
	 * @param name the name of the service
	 * @param type the type of the service: usually "_sometype._tcp" (must have a ._tcp or ._udp at the end for a protocol type)
	 * @param domain domain of the service (above default is ".local")
	 * @param port the port number at which the service is present
	 * @param list a list of strings as additional information (TODO: avahi api supports map of strings, implement this functionality)
	 * @param ttl time to live of the service in case of network failure
	 * @param publish true if the related entry group is to be immediately commited
	 */
	afLocalService(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			AvahiPublishFlags flags,
			std::string name,
			std::string type,
			std::string domain,
			uint16_t port,
			std::list<std::string> list,
			uint32_t ttl=0,
			bool publish=false
			);
	virtual ~afLocalService();

	/**
	 * create and commit the related entry group
	 * @return int
	 *			0	success (at least for commiting the entry group, also depends on the asynchronous message returned by avahi-daemon TODO: add a signal for this)
	 *			-1	missing client
	 *			-2	entry group pointer not null
	 *			-3	cannot create entry group
	 *			-4	failed to add the service to the entry group
	 *			-5	failed to commit the entry group
	 */
	int publish();
	
	/**
	 * opposite of publish. Unpublish a service.
	 */
	void unpublish();
	
	/**
	 * update the service additional information
	 */
	int updateStringList(std::list<std::string> list);

};

}
}

#endif /* AFLOCALSERVICE_H_ */
