/*
 * UdpController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_
#define LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_
#include "AbstractController.h"
#include <device/UDPSimplePacketComs.hpp>
#include <client/IPacketResponseEvent.h>
#include <client/AbstractPacketType.h>
#include <server/UDPSimplePacket.h>


class UdpController: public AbstractController, public IPacketResponseEvent {
	UDPSimplePacketComs* myconnection;
	AbstractPacketType * readController;
	bool isTimedOutValue=false;
public:
	UdpController(UDPSimplePacketComs* connection);
	virtual ~UdpController();
	/**
	 * Update the controller
	 */
	void loop();
	/**
	 * Returns an array of byte data with each byte representing one controller axis value
	 */
	uint8_t * getData();
	/**
	 * Returns an array of byte data with each byte representing one upstream value
	 */
	uint8_t * getStatus();
	void onResponse(int timeBetweenSendAndRecive);
	void onTimeout(int timeBetweenSendAndRecive);
	void oneShotMode();
	void continousShotMode();
	int getId();
	/**
	 * Get the controller timeout state
	 */
	bool isTimedOut();

};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_ */
