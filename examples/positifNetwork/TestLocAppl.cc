/***************************************************************************
 * file:        TestLocAppl.cc
 *
 * author:      Daniel Willkomm
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it 
 *              and/or modify it under the terms of the GNU General Public 
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later 
 *              version.
 *              For further information see file COPYING 
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 * description: application layer: test class for the application layer
 ***************************************************************************/


#include "TestLocAppl.h"
#include "NetwControlInfo.h"

#include <SimpleAddress.h>

Define_Module_Like(TestLocAppl, BaseApplLayer);


/**
 * First we have to initialize the module from which we derived ours,
 * in this case BasicModule.
 *
 * Then we will set a timer to indicate the first time we will send a
 * message
 *
 **/
void TestLocAppl::initialize(int stage)
{
	BaseApplLayer::initialize(stage);
	delayTimer = NULL;
// 	if (stage == 0) {
// 		delayTimer = new cMessage("delay-timer", SEND_BROADCAST_TIMER);
// 	} else if (stage == 1) {
// 		scheduleAt(simTime() + findHost()->index() + 0.005, delayTimer);
// 	}
}


/**
 * There are two kinds of messages that can arrive at this module: The
 * first (kind = BROADCAST_MESSAGE) is a broadcast packet from a
 * neighbor node to which we have to send a reply. The second (kind =
 * BROADCAST_REPLY_MESSAGE) is a reply to a broadcast packet that we
 * have send and just causes some output before it is deleted
 **/
void TestLocAppl::handleLowerMsg(cMessage * msg)
{
	ApplPkt *m;
	switch (msg->kind()) {
	case BROADCAST_MESSAGE:
		m = static_cast < ApplPkt * >(msg);
		EV << "Received a broadcast packet from host[" << m->
		    getSrcAddr() << "] -> sending reply\n";
		sendReply(m);
		break;
	case BROADCAST_REPLY_MESSAGE:
		m = static_cast < ApplPkt * >(msg);
		EV << "Received reply from host[" << m->
		    getSrcAddr() << "]; delete msg\n";
		delete msg;
		break;
	default:
		EV << "Error! got packet with unknown kind: " << msg->
		    kind() << endl;
		delete msg;
	}
}

/**
 * A timer with kind = SEND_BROADCAST_TIMER indicates that a new
 * broadcast has to be send (@ref sendBroadcast). 
 *
 * There are no other timer implemented for this module.
 *
 * @sa sendBroadcast
 **/
void TestLocAppl::handleSelfMsg(cMessage * msg)
{
	switch (msg->kind()) {
	case SEND_BROADCAST_TIMER:
//              sendBroadcast();
		delete msg;
		delayTimer = NULL;
		break;
	default:
		EV << "Unknown selfmessage! -> delete, kind: " << msg->
		    kind() << endl;
		delete msg;
	}
}

/**
 * This function creates a new broadcast message and sends it down to
 * the network layer
 **/
void TestLocAppl::sendBroadcast()
{
	ApplPkt *pkt = new ApplPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);
	pkt->setDestAddr(-1);
	// we use the host modules index() as a appl address
	pkt->setSrcAddr(myApplAddr());
	pkt->setLength(headerLength);

	// set the control info to tell the network layer the layer 3
	// address;
	pkt->setControlInfo(new NetwControlInfo(L3BROADCAST));

	EV << "Sending broadcast packet!\n";
	sendDown(pkt);
}

void TestLocAppl::sendReply(ApplPkt * msg)
{
	double delay;

	delay = uniform(0, 0.01);

	msg->setDestAddr(msg->getSrcAddr());
	msg->setSrcAddr(myApplAddr());
	msg->setKind(BROADCAST_REPLY_MESSAGE);
	msg->setName("BROADCAST_REPLY_MESSAGE");
	sendDelayedDown(msg, delay);

	EV << "sent message with delay " << delay << endl;

	//NOTE: the NetwControl info was already ste by the network layer
	//and stays the same
}

void TestLocAppl::finish()
{
	BaseApplLayer::finish();
	if (delayTimer)
		cancelAndDelete(delayTimer);
	delayTimer = NULL;
}