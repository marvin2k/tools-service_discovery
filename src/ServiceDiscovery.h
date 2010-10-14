#ifndef RIMRES_SERVICEDISCOVERY_CORE_H_
#define RIMRES_SERVICEDISCOVERY_CORE_H_

#include <string>
#include <vector>
#include <semaphore.h>
#include "afAvahiClient.h"
#include "afServiceBrowser.h"
#include "ServiceEvent.h"
#include "ServiceConfiguration.h"
#include "ServicePublisher.h"

namespace dfki { namespace communication {


enum SDException {
	already_started,
	not_configured
};

/**
 * @class ServiceDiscovery
 * @brief
 * A wrapper class for the avahi service discovery
 * @verbatim
 * namespace dc = dfki::communication;
 * void removeCallback(dc::ServiceEvent e)
 * {
 * 
 *    // what to perform when component has been added
 *    dc::ServiceDescription sd = e.getServiceDescription();
 *
 *    std::string serviceName = sd.getName();
 *    std::string labelData = sd.getDescription("my-label");
 *    ...
 * }
 * void addCallback(dc::ServiceEvent e)
 * {
 *   // what to perform when a component has been added
 * }
 * std::string someServiceData;
 * ServiceDiscovery::ServiceDiscovery;
 * std::string serviceName = "ModuleA";
 * std::string serviceType = "_module._tcp"
 * ServiceDiscovery::Configuration conf(someName, serviceType);
 * conf.setDescription("my-label","data-associated-with-label");
 * service.addedComponentConnect(signc::mem_fun(*this, &addCallback));
 * service.removeComponentConnect(signc::mem_fin(*this, &removeCallback));
 * service.start(conf);
 *
 * std::vector<ServiceDescription>
 * service.findServices(SearchPattern("my-component-name"));
 * 
 * @endverbatim
 */
class ServiceDiscovery : public sigc::trackable
{

public: 
	ServiceDiscovery();
	~ServiceDiscovery();

	void start(const ServiceConfiguration& conf);

	void stop();

	/**
	 * name searches in service name and txt for txt records. both are "OR"-ed
	 */
	struct SearchPattern 
	{
		SearchPattern(std::string name) : txt("") {
			this->name = name;
		}

		SearchPattern(std::string name, std::string txt) {
			this->name = name;
			this->txt = txt;
		}

		SearchPattern(std::string name, std::string label, std::string txt)
		{
			this->name = name;
			this->label = label;
			this->txt = txt;
		}

		std::string name;
		std::string label;
		std::string txt;
	};

  /**
   * Abstract base class for ServicePattern. Provides a simple interface for
   * searching via specifice service patterns (position, type, group, state, etc.)
   */
  struct ServicePattern {
    virtual bool matchDescription(const ServiceDescription& service) const = 0;
  };

  struct PropertyPattern : public ServicePattern {
    PropertyPattern() : label("*"), description("*") {}
    PropertyPattern(std::string label, std::string description)
     : label(label), description(description) {}
    ~PropertyPattern() {}

    bool matchDescription(const ServiceDescription& service) const;

   private:
    std::string label, description;
  };

  struct PositionPattern : public ServicePattern {
    PositionPattern(int x, int y, int z, double distance) 
      : x(x), y(y), z(z), distance(distance) {}
    ~PositionPattern() {}

    bool matchDescription(const ServiceDescription& servie) const;

   private:
    int x, y, z;    
    double distance;
  };
		
	std::vector<ServiceDescription> findServices(SearchPattern pattern);

  std::vector<ServiceDescription> findServices(const ServicePattern& pattern);
	
	std::vector<std::string> getServiceNames();

	void addedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		sem_wait(&added_component_sem);
		ServiceAddedSignal.connect(slot);
		sem_post(&added_component_sem);
	}

	void removedComponentConnect(const sigc::slot<void, ServiceEvent>& slot) {
		sem_wait(&removed_component_sem);
		ServiceRemovedSignal.connect(slot);
		sem_post(&removed_component_sem);
	}

private:

	/**
	* Added service
	*/
	void addedService(const afRemoteService& service);

	/**
	* Removed service
	*/
	void removedService(const afRemoteService& service);

	sigc::signal<void, ServiceEvent> ServiceAddedSignal;
	sigc::signal<void, ServiceEvent> ServiceRemovedSignal;

	sem_t added_component_sem;
	sem_t removed_component_sem;

	afList<ServiceDescription> services;
	sem_t services_sem;

	bool started;

	afAvahiClient* client;
	afServiceBrowser* browser;
	
	ServicePublisher* localserv;
	ServiceConfiguration localConfiguration_;

};

}}

#endif