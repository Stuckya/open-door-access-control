package can

import (
	"fmt"
	"github.com/brutella/can"
	"github.com/stuckya/door-access-bus-relay/internal/can/messageType"
	"github.com/stuckya/door-access-bus-relay/internal/can/status"
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
	log.Println("Starting the bus...")

	bus := &SocketCan{}

	if err := bus.Open(); err != nil {
		log.Fatal(err)
	}

	go readMessages(bus)

	err := bus.ListenAndServe()

	if err != nil {
		err := bus.Close()

		if err != nil {
			log.Fatal(err)
		}

		log.Fatal(err)
	}
}

func readMessages(bus *SocketCan) {
	for {
		timer := time.NewTimer(10 * time.Second)

		select {
		case frm := <-bus.readChan:
			processCanFrame(frm, bus)
		case <-timer.C:
			log.Println("Timed out while reading message from bus.")
		}
	}
}

func processCanFrame(frm *can.Frame, bus *SocketCan) {

	log.Println("Processing frame...")

	switch frm.Data[0] {
	case messageType.HEARTBEAT:
		evaluateHeartbeat(frm, bus)
	case messageType.CARD_READ:
		fmt.Println(frm.Data)
	}
}

func evaluateHeartbeat(frm *can.Frame, bus *SocketCan) {
	nodeId := frm.ID
	msgStatus := frm.Data[1]

	storedNode, ok := connectedNodes[nodeId]

	if !ok {
		fmt.Println("New node added to map from heartbeat message.")

		// Add to the map, don't want to process a second message twice.
		connectedNodes[nodeId] = NodeStatus{msgStatus, status.READY}

		// TODO: Do we run the check on all statuses? Only pre-operational?
		if doCheck(nodeId) {
			// You are good, we should start you.
			bus.Publish(can.Frame{
				ID: 100,
				Length: 3,
				Flags:  0,
				Res0:   0,
				Res1:   0,
				Data:   [8]uint8{100, messageType.UPDATE_STATUS, status.OPERATIONAL},
			})

			connectedNodes[nodeId] = NodeStatus{msgStatus, status.DONE}

			return
		}

		// You are bad, we should stop you.
		bus.Publish(can.Frame{
			ID: 100,
			Length: 3,
			Flags:  0,
			Res0:   0,
			Res1:   0,
			Data:   [8]uint8{100, messageType.UPDATE_STATUS, status.STOPPED},
		})

		connectedNodes[nodeId] = NodeStatus{msgStatus, status.DONE}

		return
	}

	if storedNode.nodeStatus == msgStatus {
		return
	}

	fmt.Println("Node updated msgStatus in map.")
	// TODO: Do we actually need to change something here?
	// Operational -> Stopped = doNothing
	// Operational -> PreOperational = doNothing? / can this happen?
	// Stopped -> Operational = doNothing
	// Stopped -> PreOperational = doSomething
	connectedNodes[nodeId] = NodeStatus{msgStatus,storedNode.syncStatus}
	// doCheck() and on response update map & bus?
}

func doCheck(id uint32) bool {
	return id == 100
}
