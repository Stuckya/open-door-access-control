package can

import (
	"fmt"
	"github.com/brutella/can"
	"github.com/stuckya/door-access-bus-relay/internal/can/messageType"
	"github.com/stuckya/door-access-bus-relay/internal/can/nodeStatus"
	"github.com/stuckya/door-access-bus-relay/internal/can/syncStatus"
	"log"
	"time"
)

type NodeStatus struct {
	nodeStatus		uint8 // enum?
	syncStatus		uint8 // enum?
	// timestamp?
}

var connectedNodes = map[uint32]NodeStatus{}

func SubscribeToBus() {
	log.Println("Starting bus.")

	bus := &SocketCan{}

	if err := bus.Open(); err != nil {
		log.Fatal(err)
	}

	timer := time.NewTimer(10 * time.Second)

	for {
		select {
		case frm, ok := <-bus.readChan:

			if !ok {

			}

			processCanFrame(frm, bus)
		case <-timer.C:
			log.Println("Timed out while reading bus.")
		}
	}
}

func processCanFrame(frm *can.Frame, bus *SocketCan) {

	log.Println("processing frame...")

	switch frm.Data[0] {
	case messageType.HEARTBEAT:
		evaluateHeartbeat(frm, bus)
	case messageType.CARD_READ:
		fmt.Println(frm.Data)
	}
}

func evaluateHeartbeat(frm *can.Frame, bus *SocketCan) {
	nodeId := frm.ID
	status := frm.Data[1]

	storedNode, ok := connectedNodes[nodeId]

	if !ok {
		fmt.Println("New node added to map from heartbeat message.")

		// Add to the map, don't want to process a second message twice.
		connectedNodes[nodeId] = NodeStatus{status,syncStatus.READY}

		// TODO: Do we run the check on all statuses? Only pre-operational?
		if doCheck(nodeId) {
			// You are good, we should start you.
			bus.Publish(can.Frame{
				ID: 100,
				Length: 3,
				Flags:  0,
				Res0:   0,
				Res1:   0,
				Data:   [8]uint8{100, messageType.UPDATE_STATUS, nodeStatus.OPERATIONAL},
			})

			connectedNodes[nodeId] = NodeStatus{status,syncStatus.DONE}

			return
		}

		// You are bad, we should stop you.
		bus.Publish(can.Frame{
			ID: 100,
			Length: 3,
			Flags:  0,
			Res0:   0,
			Res1:   0,
			Data:   [8]uint8{100, messageType.UPDATE_STATUS, nodeStatus.STOPPED},
		})

		connectedNodes[nodeId] = NodeStatus{status,syncStatus.DONE}

		return
	}

	if storedNode.nodeStatus == status {
		return
	}

	fmt.Println("Node updated status in map.")
	// TODO: Do we actually need to change something here?
	// Operational -> Stopped = doNothing
	// Operational -> PreOperational = doNothing? / can this happen?
	// Stopped -> Operational = doNothing
	// Stopped -> PreOperational = doSomething
	connectedNodes[nodeId] = NodeStatus{status,storedNode.syncStatus}
	// doCheck() and on response update map & bus?
}

func doCheck(id uint32) bool {
	return id == 100
}
