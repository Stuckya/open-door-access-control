package can

import (
	"github.com/brutella/can"
	"github.com/stuckya/door-access-bus-relay/internal/can/frame"
	"time"
)

// Produce repeatedly sends a frame on a bus after a timeout.
// The sending can be stopped by using the returned channel.
func Produce(frame can.Frame, bus *can.Bus, timeout time.Duration) chan <- struct{} {
	stop := make(chan struct{})

	go func() {
		for {
			if err := bus.Publish(frame); err != nil {
				return
			}

			select {
			case <-stop:
				return
			case <-time.After(timeout):
				break
			}
		}
	}()
	return stop
}

// ProduceHeartbeat repeatedly sends a heartbeat frame. test
func ProduceHeartbeat(nodeID uint8, state [8]uint8, bus *can.Bus, timeout time.Duration) chan <- struct{} {
	frame := frame.NewHeartbeatFrame(nodeID, state)
	return Produce(frame, bus, timeout)
}