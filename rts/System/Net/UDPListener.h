#ifndef _UDPLISTENER 
#define _UDPLISTENER

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

#include "UDPSocket.h"
#include "UDPConnection.h"

namespace netcode
{

class CNet;

/**
@brief Class for handling Connections on an UDPSocket
Use this class if you want to use a UDPSocket to connect to more than one other clients. You can Listen for new connections, initiate new ones and send/recieve data to/from them.
@author Karl-Robert Ernst
*/
class UDPListener : boost::noncopyable
{
public:
	/**
	@brief Open a socket and make it ready for listening
	*/
	UDPListener(int port, CNet* const proto);
	
	/**
	@brief close the socket and DELETE all connections
	*/
	~UDPListener();
	
	/**
	@brief Run this from time to time
	This does: recieve data from the socket and hand it to the associated UDPConnection, or open a new UDPConnection. It also Updates all of its connections
	*/
	void Update();
	
	/**
	@brief Initiate a connection
	Make a new connection to address:port. It will be pushed back in conn
	*/
	boost::shared_ptr<UDPConnection> SpawnConnection(const std::string& address, const unsigned port);
	
	/**
	@brief Set if we are going to accept new connections or drop all data from unconnected addresses
	*/
	void SetWaitingForConnections(const bool state);
	
	/**
	@brief Are we accepting new connections?
	*/
	bool GetWaitingForConnections() const;
	
private:
	/**
	@brief Do we accept packets from unknown sources?
	If true, we will create a new connection, if false, it get dropped
	*/
	bool acceptNewConnections;
	
	/// Our socket
	boost::shared_ptr<UDPSocket> mySocket;
	
	/// all connections
	std::list< boost::weak_ptr< UDPConnection> > conn;
	
	CNet* const net;
};

}

#endif
