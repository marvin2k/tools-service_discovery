/*
 * afServiceBase.h
 *
 *  Created on: Mar 19, 2010
 *      Author: darko
 */

#ifndef AFSERVICEBASE_H_
#define AFSERVICEBASE_H_


#include <avahi-client/client.h>
#include <string>
#include <iostream>
#include "afAvahiClient.h"

namespace rock {
namespace communication {


/**
 * @brief
 * an abstract base service class used to represent the basic info of a service as received from a service browser callback
 */
class afServiceBase {
public:
private:
	/**
	 * related avahi client instance
	 */
	afAvahiClient *client;
	/** 
	 * network interface connected with this service
	 */
	AvahiIfIndex interface;
	/**
	 * network protocol connected with this service. either ipv4 or ipv6 or both
	 */
	AvahiProtocol protocol;
	/**
	 * name of service
	 */
	std::string name;
	/**
	 * type of the service
	 */
	std::string type;
	/**
	 * domain of the service
	 */
	std::string domain;
public:

	afServiceBase(
			afAvahiClient *client,
			AvahiIfIndex interf,
			AvahiProtocol prot,
			std::string name,
			std::string type,
			std::string domain
			);
	virtual ~afServiceBase();

	bool operator==(const afServiceBase&);

    std::string getDomain() const
    {
        return domain;
    }

    const char* getDomainChar()
	{
    	if (domain.compare("") == 0) {
    		return NULL;
    	} else {
    		return domain.c_str();
    	}
	}


    AvahiIfIndex getInterface() const
    {
        return interface;
    }

    std::string getName() const
    {
        return name;
    }

    AvahiProtocol getProtocol() const
    {
        return protocol;
    }

    std::string getProtocolString() const
    {
        const char *proto = avahi_proto_to_string(protocol);
        return std::string(proto);
    }

    std::string getType() const
    {
        return type;
    }

    afAvahiClient* getClient()
    {
    	return client;
    }
};

}
}

#endif /* AFSERVICEBASE_H_ */
