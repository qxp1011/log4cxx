/***************************************************************************
                          sokectappender.h  -  class SocketAppender
                             -------------------
    begin                : jeu mai 8 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#ifndef _LOG4CXX_NET_SOCKET_APPENDER_H
#define _LOG4CXX_NET_SOCKET_APPENDER_H
 
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/socket.h>
#include <log4cxx/helpers/thread.h>

namespace log4cxx
{
	namespace helpers
	{
		class SocketOutputStream;
		typedef helpers::ObjectPtr<SocketOutputStream> SocketOutputStreamPtr;
	};

	namespace net
	{
		class SocketAppender;
		typedef helpers::ObjectPtr<SocketAppender> SocketAppenderPtr;

        /**
        Sends {@link spi::LoggingEvent LoggingEvent} objects to a remote a log server,
        usually a {@link net::SocketNode SocketNode}.

        <p>The SocketAppender has the following properties:

        <ul>

        <p><li>If sent to a {@link net::SocketNode SocketNode}, remote logging
		is non-intrusive as far as the log event is concerned. In other
        words, the event will be logged with the same time stamp, {@link
        NDC NDC}, location info as if it were logged locally by
        the client.

        <p><li>SocketAppenders do not use a layout. They ship a
        serialized {@link spi::LoggingEvent LoggingEvent} object
		to the server side.

        <p><li>Remote logging uses the TCP protocol. Consequently, if
        the server is reachable, then log events will eventually arrive
        at the server.

        <p><li>If the remote server is down, the logging requests are
        simply dropped. However, if and when the server comes back up,
        then event transmission is resumed transparently. This
        transparent reconneciton is performed by a <em>connector</em>
        thread which periodically attempts to connect to the server.

        <p><li>Logging events are automatically <em>buffered</em> by the
        native TCP implementation. This means that if the link to server
        is slow but still faster than the rate of (log) event production
        by the client, the client will not be affected by the slow
        network connection. However, if the network connection is slower
        then the rate of event production, then the client can only
        progress at the network rate. In particular, if the network link
        to the the server is down, the client will be blocked.

        <p>On the other hand, if the network link is up, but the server
        is down, the client will not be blocked when making log requests
        but the log events will be lost due to server unavailability.

        <p><li>Even if a <code>SocketAppender</code> is no longer
        attached to any category, it will not be garbage collected in
        the presence of a connector thread. A connector thread exists
        only if the connection to the server is down. To avoid this
        garbage collection problem, you should #close the the
        <code>SocketAppender</code> explicitly. See also next item.

        <p>Long lived applications which create/destroy many
        <code>SocketAppender</code> instances should be aware of this
        garbage collection problem. Most other applications can safely
        ignore it.

        <p><li>If the JVM hosting the <code>SocketAppender</code> exits
        before the <code>SocketAppender</code> is closed either
        explicitly or subsequent to garbage collection, then there might
        be untransmitted data in the pipe which might be lost. This is a
        common problem on Windows based systems.

        <p>To avoid lost data, it is usually sufficient to 
        #close the <code>SocketAppender</code> either explicitly or by
        calling the LogManager#shutdown method
        before exiting the application.


        </ul>
        */
        
      	class SocketAppender : public AppenderSkeleton
    	{
		class Connector;
		friend class Connector;
    	public:
    		/**
    		The default port number of remote logging server (4560).
    		*/
    		static int DEFAULT_PORT;

    		/**
    		The default reconnection delay (30000 milliseconds or 30 seconds).
    		*/
    		static int DEFAULT_RECONNECTION_DELAY;

    	protected:
    		/**
    		host name
    		*/
    		tstring remoteHost;

    		/**
    		IP address
    		*/
    		helpers::InetAddress address;

    		int port;
    		helpers::SocketOutputStreamPtr os;
    		int reconnectionDelay;
    		bool locationInfo;

    	public:
    		SocketAppender();
			~SocketAppender();

    		/**
    		Connects to remote server at <code>address</code> and <code>port</code>.
    		*/
    		SocketAppender(unsigned long address, int port);

    		/**
    		Connects to remote server at <code>host</code> and <code>port</code>.
    		*/
    		SocketAppender(const tstring& host, int port);

    		/**
    		Connect to the specified <b>RemoteHost</b> and <b>Port</b>.
    		*/
    		void activateOptions();

		    /**
		    Set options
		    */
			virtual void setOption(const std::string& option, const std::string& value);
			
    		/**
    		* Close this appender.
    		*
    		* <p>This will mark the appender as closed and call then 
    		* #cleanUp method.
    		* */
    		void close();

    		/**
    		* Drop the connection to the remote host and release the underlying
    		* connector thread if it has been created
    		* */
    		void cleanUp();

    		void connect();


    		virtual void append(const spi::LoggingEvent& event);

    		/**
    		* The SocketAppender does not use a layout. Hence, this method
    		* returns <code>false</code>.
    		* */
    		bool requiresLayout()
    			{ return false; }

    		/**
    		* The <b>RemoteHost</b> option takes a string value which should be
    		* the host name of the server where a 
			* {@link net::SocketNode SocketNode} is running.
    		* */
    		inline void setRemoteHost(const tstring& host)
    			{ address = helpers::InetAddress::getByName(host);
    			remoteHost = host; }

    		/**
    		Returns value of the <b>RemoteHost</b> option.
    		*/
    		inline const tstring& getRemoteHost() const
    			{ return remoteHost; }

    		/**
    		The <b>Port</b> option takes a positive integer representing
    		the port where the server is waiting for connections.
    		*/
    		void setPort(int port)
    			{ this->port = port; }

    		/**
    		Returns value of the <b>Port</b> option.
    		*/
    		int getPort() const
    			{ return port; }

    		/**
    		The <b>LocationInfo</b> option takes a boolean value. If true,
    		the information sent to the remote host will include location
    		information. By default no location information is sent to the server.
    		*/
    		void setLocationInfo(bool locationInfo)
    			{ this->locationInfo = locationInfo; }

    		/**
    		Returns value of the <b>LocationInfo</b> option.
    		*/
    		bool getLocationInfo() const
    			{ return locationInfo; }

    		/**
    		The <b>ReconnectionDelay</b> option takes a positive integer
    		representing the number of milliseconds to wait between each
    		failed connection attempt to the server. The default value of
    		this option is 30000 which corresponds to 30 seconds.

    		<p>Setting this option to zero turns off reconnection
    		capability.
    		*/
    		void setReconnectionDelay(int reconnectionDelay)
    			{ this->reconnectionDelay = reconnectionDelay; }

    		/**
    		Returns value of the <b>ReconnectionDelay</b> option.
    		*/
    		int getReconnectionDelay() const
    			{ return reconnectionDelay; }

		    void fireConnector();
       
       private:
		   /**
			The Connector will reconnect when the server becomes available
			again.  It does this by attempting to open a new connection every
			<code>reconnectionDelay</code> milliseconds.

			<p>It stops trying whenever a connection is established. It will
			restart to try reconnect to the server when previpously open
			connection is droppped.
			*/
			class Connector : public helpers::Thread, public helpers::ObjectImpl
			{
			public:
				bool interrupted;
				SocketAppender * socketAppender;

				Connector(SocketAppenderPtr socketAppender);
				virtual void run();
			}; // class Connector

			Connector * connector;
        }; // class SocketAppender
    } // namespace net
}; // namespace log4cxx

#endif // _LOG4CXX_NET_SOCKET_APPENDER_H

